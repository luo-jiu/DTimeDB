#include <engine/tsm/write.h>
using namespace dt::tsm;

void Write::set_file_path_manager(FilePathManager * file_path_manager)
{
    m_file_path = file_path_manager;
}

void Write::write(
        high_resolution_clock::time_point timestamp,
        string & data,
        DataBlock::Type type,
        string & field_name,
        string & db_name,
        string & tb_name,
        TableState & tb_state,
        QueueState & queue_state)
{
    std::shared_ptr<Field> field(new Field());
    // 寻找或创建field
    switch(type)
    {
        case DataBlock::DATA_STRING:
        {
            field = get_field(field_name, "string", tb_state, queue_state);
            break;
        }
        case DataBlock::DATA_INTEGER:
        {
            field = get_field(field_name, "integer", tb_state, queue_state);
            break;
        }
        case DataBlock::DATA_FLOAT:
        {
            field = get_field(field_name, "float", tb_state, queue_state);
            break;
        }
        default:
        {
            std::cerr << "Error : Unknown type " << type << std::endl;
            return;  // 返回
        }
    }

    field->write(timestamp, data, db_name, tb_name);
}

/**
 * 对数据进行刷盘
 * 子线程
 *
 * 一个线程对应一个表
 */
void Write::flush_disk()
{
    std::cout << "Writing" << std::endl;
    while (true)
    {
        {
             std::unique_lock<std::mutex> lock(m_thread_mutex);
             std::cout << "wait()" << std::endl;
             if (!m_is_ready)  // 没有数据, 每一秒激活自己
             {
                 m_cv.wait_for(lock, seconds(1));
             }
             else  // 有数据，启用计数器激活
             {
                 m_cv.wait(lock, [&]{return m_is_ready;});  // 进入等待
             }
        }

        // 激活说明有字段了
        if (!m_field_list.empty())  // 再判断一下
        {
            auto field_name = pop_front_field_list();
            auto field = m_field_map[field_name];  // 通过映射拿到对应field

            /**
             * 现在开始处理消息队列的data_block
             *
             * #注意:
             * 这里是entry 唯一生成的地方，也就是说，这个线程在进行meta 和entry刷盘
             *      的时候，是不会有新的entry 生成的
             * 只会产生新的data_block,但是所有data_block 都会在这里进行处理，固然
             *      不会遗漏任何data_block 和 entry 的刷盘
             *
             * 并且每次循环只刷一个,注意,是一个data_block,因为m_field_list 中的
             *      数量和data_block 数量一一对应，拿一个刷一个，有条不紊
             */
            if (!field->get_data_status())  // 再次确定有 data_block
            {
                // 开始data_block 刷盘
                std::cout << "开始刷写data_block" << std::endl;

                /**
                 * #刷写的时候还需要计算余量空间是否足够
                 *
                 * #公式:
                 *  > 余量: 中间体大小 (默认32MB)
                 *  > 本次data_block 大小和对应entry 大小, 并且同一个field 只会在第
                 *      一次会去减掉meta 大小，其他时候都只会减去data_block 和entry
                 *
                 * 余量(初始值)[已知] -
                 *      <第一次> meta 大小[已知] - data_block 大小[已知] - entry 大小 [已知]
                 *      <其余> data_block 大小[已知] - entry 大小 [已知]
                 *
                 * #注意:
                 * 余量是众多field 共享的
                 *
                 * 每次都从余量减去本次的大小，这样就可以确保都能计算在内,但是千万注意，当
                 *      余量不足的时候，必须将所有的meta 和entry都刷入磁盘，然后再去开
                 *      启新的TSM file 继续写入,所以entry 还是需要维护一个统一队列，用
                 *      来映射map,只不过这次需要全部便利结束
                 */
                auto data_block = field->pop_data_from_deque();  // 获取待写入的data_block

                if (field->get_index_status())  // <第一次>
                {
                    // 去掉meta 大小
                    m_margin -= (12 + m_tb_name.length() + field->m_field_name.length());
                }
                // 在去除data_block 和 entry
                m_margin -= (28 + data_block->m_size);

                // 容量足够
                if (m_margin > 0)
                {
                    // 创建entry [唯一生成]
                    auto entry = m_tsm.create_index_entry(data_block->m_max_timestamp, data_block->m_min_timestamp, m_head_offset, data_block->m_size);
                    field->push_index_to_deque(entry);  // 存入队列

                    auto it = m_index_set.find(field_name);
                    if(it == m_index_set.end())  // 第一次放入块对其初始化计时器
                    {
                        m_index_set.insert(field_name);  // 表示这个字段有entry 未写入
                        field->m_index_last_time = high_resolution_clock::now();
                    }

                    // 刷盘
                    m_tsm.write_data_to_file(data_block, "data.tsm", m_head_offset);

                    // 修改头偏移量
                    m_head_offset += data_block->m_size;
                }
                else  // 容量不足
                {
                    std::cout << "余量不足" << std::endl;

                    // 准备全部刷盘
                    for (string field_name_temp : m_index_set)  // 遍历set
                    {
                        flush_entry_disk(field_name_temp, false);  // 全部去刷盘
                        m_index_set.erase(field_name_temp);  // 移除
                    }

                    /**
                     * 生成新的TSM file 完成刷盘使命(其实就是新文件和新文件路径)
                     *
                     * #注意
                     * 什么是新的TSM file？ 对于这个函数来说，首先就是
                     *      1. 不是同一个文件[写入的文件路径]
                     *
                     * 其实就没有了，唯独要注意的就是，entry 的list 只为一个TSM file 提供服务,
                     *      所以当所有的meta 和entry 都刷盘后，需要保证list 是空的！
                     *      然后就是切换一个新的文件继续重复上述过程罢了
                     * 最好还是有一个文件管理器
                     * 此时field 已经拿出来了,你需要将此次的field写入新文件
                     */

                }
            }
        }

        if (!m_index_set.empty())  // 检验现存的 entry
        {
            std::list<string> flush_complete_field;
            for (auto field_name_temp : m_index_set)  // 遍历set
            {
                auto _field = m_field_map[field_name_temp];  // 通过映射拿到对应field

                /**
                 * 拿到field 先不急，判断meta 到没到刷盘时机
                 *
                 * #原因:
                 * 1. 此时已经有data_block 刷入磁盘但是entry 还在内存，但是不满足数量
                 *     的刷盘条件
                 *
                 * #备注:
                 * 这里添加了一个维护的set，告诉这个线程哪里有还未刷盘的 entry
                 *      目的是最后TSM 不够用时能找到对应的field 进行统一刷盘
                 *
                 * 所以在TSM 容量足够的时候，刷写完entry 后记得将set 元素移除,因为有条件
                 *      _field->get_index_deque_size() > 9；保证数量永远只会低于这个数
                 *      所以刷写完后直接将set 中的元素移除
                 */
                auto current_time = high_resolution_clock::now();
                if (_field->get_index_deque_size() > 9 || (current_time - _field->m_index_last_time >= std::chrono::seconds(10) && !_field->get_index_status()))
                {
                    flush_entry_disk(field_name_temp, false);
                    flush_complete_field.push_back(field_name_temp);
                }
            }

            for (const string& it : flush_complete_field)
            {
                m_index_set.erase(it);
            }
        }

        {
            std::unique_lock<std::mutex> lock(m_thread_mutex);
            if (m_is_ready > 0)
            {
                --m_is_ready;  // 继续等待
            }
        }
    }
}

/**
 * 保证为单线程的，以下的环境都不会有多线程打扰
 * 除开主动访问共享变量外
 */
void Write::field_flush_disk(
        const string & field_name)
{
    while(true)
    {
        auto field_it = m_field_map.find(field_name);
        if (field_it == m_field_map.end())  // 没有这个field,为空
        {
            std::cout << "not find field:'" << field_name << "'" << std::endl;
            break;
        }
        auto field = field_it->second;

        // 再次判断队列里面有没元素
        if(field->get_data_status())
        {
            break;
        }
        else
        {
            // 说明队列有元素
            std::cout << "开始刷写块,field:" << field_name << std::endl;
            auto data_block = field_it->second->pop_data_from_deque();

            if (field->get_index_status())  // 该文件第一次写入
            {
                // 去掉meta 大小
                m_margin -= (12 + m_tb_name.length() + field->m_field_name.length());
            }
            // 在去除data_block 和 entry
            m_margin -= (28 + data_block->m_size);

            if (m_margin > 0)  // 说明容量足够
            {
                // 创建entry [唯一生成]
                auto entry = m_tsm.create_index_entry(data_block->m_max_timestamp, data_block->m_min_timestamp, m_head_offset, data_block->m_size);
                field->push_index_to_deque(entry);  // 存入队列

                auto it = m_index_set.find(field_name);
                if(it == m_index_set.end())  // 第一次放入块对其初始化计时器
                {
                    m_index_set.insert(field_name);  // 表示这个字段有entry 未写入
                    field->m_index_last_time = high_resolution_clock::now();
                }

                // 获取文件用于刷盘
                if (m_curr_file_path.empty())
                {
                    m_curr_file_path = m_file_path->create_file(m_tb_name, m_db_name, "tsm");
                    std::cout << "创建文件:m_curr_file_path:" << m_curr_file_path << "\n";

                    // 第一次创建需要写入头部信息
                    Header header(1, 1);
                    m_tsm.write_header_to_file(header, m_curr_file_path);
                }

                // 刷盘
                m_tsm.write_data_to_file(data_block, m_curr_file_path, m_head_offset);

                // 修改头偏移量
                m_head_offset += data_block->m_size;
            }
            else  // 容量不足
            {
                std::cout << "余量不足" << std::endl;

                // 准备全部刷盘
//                for (string field_name_temp : m_index_set)  // 遍历set
//                {
//                    flush_entry_disk(field_name_temp, false);  // 全部去刷盘
//                    m_index_set.erase(field_name_temp);  // 移除
//                }

                /**
                 * 生成新的TSM file 完成刷盘使命(其实就是新文件和新文件路径)
                 *
                 * #注意
                 * 什么是新的TSM file？ 对于这个函数来说，首先就是
                 *      1. 不是同一个文件[写入的文件路径]
                 *
                 * 其实就没有了，唯独要注意的就是，entry 的list 只为一个TSM file 提供服务,
                 *      所以当所有的meta 和entry 都刷盘后，需要保证list 是空的！
                 *      然后就是切换一个新的文件继续重复上述过程罢了
                 * 最好还是有一个文件管理器
                 * 此时field 已经拿出来了,你需要将此次的field写入新文件
                 */
//                string file_path = m_file_path->create_file(m_tb_name, m_db_name, "tsm");  // 获取一个全新的文件
//                std::cout << file_path << std::endl;
            }
        }
    }
}

void Write::flush_entry_disk(string & field_name, bool is_remove)
{
    auto field = m_field_map[field_name];  // 通过映射拿到对应field

    // 开始meta 和entry 刷盘
    std::cout << "开始刷写meta 和entry" << std::endl;

    /**
     * 生成meta 刷盘
     * 然后entry 刷写到磁盘(拿取的时候就清空了)
     *
     * 此时不会有新的meta 和 entry生成
     * 所以可以直接获取entry 数量
     */
    int entry_size = field->get_index_deque_size();  // 获取entry 数量[还是会小于阈值]
    std::shared_ptr<IndexBlockMeta> meta(new IndexBlockMeta());
    if (field->m_type == DataBlock::DATA_STRING)
    {
        meta = m_tsm.create_index_meta(IndexBlockMeta::Type::DATA_STRING, m_tb_name, field->m_field_name);
    }
    else if (field->m_type == DataBlock::DATA_INTEGER)
    {
        meta = m_tsm.create_index_meta(IndexBlockMeta::Type::DATA_INTEGER, m_tb_name, field->m_field_name);
    }
    else if (field->m_type == DataBlock::Type::DATA_FLOAT)
    {
        meta = m_tsm.create_index_meta(IndexBlockMeta::Type::DATA_FLOAT, m_tb_name, field->m_field_name);
    }
    meta->set_count(entry_size);  // 设置entry 数量

    int index_size = (12 + m_tb_name.length() + field->m_field_name.length()) + entry_size * 28;
    m_tail_offset -= index_size;  // 前移到指定位置开始写入

    m_tsm.write_index_meta_to_file(meta, "data.tsm", m_tail_offset);  // 写入meta
    m_tail_offset += (12 + m_tb_name.length() + field->m_field_name.length());  // 后移指针

    // 如果有 entry[数量只会低于_field->get_index_deque_size() > x] x这个阈值
    while (!field->get_index_status())
    {
        auto entry = field->pop_index_from_deque();
        m_tsm.write_index_entry_to_file(entry, "data.tsm", m_tail_offset);
        m_tail_offset += 28;  // 后移指针
    }
    m_tail_offset -= index_size;  // 写入后指针发生变化还需要重新移位

    // 写了meta 后就需要更新footer 的索引
    Footer _footer(m_tail_offset);
    m_tsm.write_footer_to_file(_footer, "data.tsm");

    if (is_remove)
    {
        // 写完后将m_index_set 中的字段直接移除，表示目前这个字段没有entry 待刷入了
        m_index_set.erase(field_name);
    }

    // 重置刷入时间
    field->m_index_last_time = high_resolution_clock::now();
}

void Write::flush_all_sl()
{

}

std::shared_ptr<Field> Write::get_field(
        string & field_name,
        const string & type,
        TableState & tb_state,
        QueueState & queue_state)
{
    std::lock_guard<std::mutex> lock(m_write_mutex);
    auto it = m_field_map.find(field_name);
    if (it != m_field_map.end())  // 找到field
    {
        if (!it->second->get_status())  // 判断状态
        {
            return it->second;
        }
    }

    std::shared_ptr<Field> field;
    // 没合适的field
    if (type == "string")
    {
        field = std::make_shared<Field>(DataBlock::Type::DATA_STRING, field_name);
    }
    else if (type == "integer")
    {
        field = std::make_shared<Field>(DataBlock::Type::DATA_INTEGER, field_name);
    }
    else if (type == "float")
    {
        field = std::make_shared<Field>(DataBlock::Type::DATA_FLOAT, field_name);
    }
    else
    {
        std::cerr << "Error : Unknown type " << type << std::endl;
        return nullptr;
    }
    // 注册观察者
    field->attach(&queue_state);
    field->get_skip_list().attach(&tb_state);

    m_field_map[field_name] = field;
    return field;
}

bool Write::fields_empty()
{
    std::lock_guard<std::mutex> lock(m_write_mutex);
    return m_field_map.empty();
}

void Write::push_back_field_list(const string & field)
{
    std::lock_guard<std::mutex> lock(m_field_list_mutex);
    m_field_list.push_back(field);
}

string Write::pop_front_field_list()
{
    std::lock_guard<std::mutex> lock(m_field_list_mutex);
    if (m_field_list.empty())
    {
        return {};
    }

    string field = m_field_list.front();
    m_field_list.pop_front();
    return field;
}

bool Write::empty_field_list()
{
    std::lock_guard<std::mutex> lock(m_field_list_mutex);
    return m_field_list.empty();
}

int Write::size_field_list()
{
    std::lock_guard<std::mutex> lock(m_field_list_mutex);
    return m_field_list.size();
}

/**
 * 获取对应跳表的时间戳
 */
high_resolution_clock::time_point Write::get_field_time_point(
        const string & field_name)
{
    std::shared_lock<std::shared_mutex> lock(m_filed_map_mutex);  // 读锁
    auto field_it = m_field_map.find(field_name);
    if (field_it != m_field_map.end())  // 拿到对应Field
    {
        return field_it->second->get_skip_list_time_point();
    }
}