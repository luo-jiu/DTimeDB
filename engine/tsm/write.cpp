#include <engine/tsm/write.h>
using namespace dt::tsm;

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
    std::shared_ptr<Field> _field(new Field());
    // 寻找或创建field
    switch(type)
    {
        case DataBlock::DATA_STRING:
        {
            _field = get_field(field_name, "string", tb_state, queue_state);
            break;
        }
        case DataBlock::DATA_INTEGER:
        {
            _field = get_field(field_name, "integer", tb_state, queue_state);
            break;
        }
        case DataBlock::DATA_FLOAT:
        {
            _field = get_field(field_name, "float", tb_state, queue_state);
            break;
        }
        default:
        {
            std::cerr << "Error : Unknown type " << type << std::endl;
        }
    }
    auto _str = _field->write(timestamp, data, db_name, tb_name);
//    if (!_str.empty())  // 有field_name 返回,说明已经生成data_block
//    {
//        // 这里就算是多线程出来一定只有一个线程会执行
//        std::unique_lock<std::mutex> lock(m_thread_mutex);
//        push_back_field_list(_str);  // 添加,让其可以映射map
//
//        // 从线程池拿线程去执行写入操作
//        std::cout << "激活" << std::endl;
//        ++m_is_ready;
//        std::cout << "m_is_ready = " << m_is_ready << std::endl;
//        m_cv.notify_one();
//    }
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
            auto _field_name = pop_front_field_list();
            auto _field = m_field_map[_field_name];  // 通过映射拿到对应field

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
            if (!_field->get_data_status())  // 再次确定有 data_block
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
                auto _data_block = _field->pop_data_from_deque();  // 获取待写入的data_block

                if (_field->get_index_status())  // <第一次>
                {
                    // 去掉meta 大小
                    m_margin -= (12 + m_measurement.length() + _field->m_field_name.length());
                }
                // 在去除data_block 和 entry
                m_margin -= (28 + _data_block->m_size);

                if (m_margin > 0)  // 说明容量足够
                {
                    // 创建entry [唯一生成]
                    auto _entry = m_tsm.create_index_entry(_data_block->m_max_timestamp, _data_block->m_min_timestamp, m_head_offset, _data_block->m_size);
                    _field->push_index_to_deque(_entry);  // 存入队列

                    auto it = m_index_set.find(_field_name);
                    if(it == m_index_set.end())  // 第一次放入块对其初始化计时器
                    {
                        m_index_set.insert(_field_name);  // 表示这个字段有entry 未写入
                        _field->m_index_last_time = high_resolution_clock::now();
                    }

                    // 刷盘
                    m_tsm.write_data_to_file(_data_block, "data.tsm", m_head_offset);

                    // 修改头偏移量
                    m_head_offset += _data_block->m_size;
                }
                else  // 容量不足
                {
                    std::cout << "余量不足" << std::endl;

                    // 准备全部刷盘
                    for (string _field_name_temp : m_index_set)  // 遍历set
                    {
                        flush_entry_disk(_field_name_temp, false);  // 全部去刷盘
                        m_index_set.erase(_field_name_temp);  // 移除
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
            for (auto _field_name_temp : m_index_set)  // 遍历set
            {
                auto _field = m_field_map[_field_name_temp];  // 通过映射拿到对应field

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
                    flush_entry_disk(_field_name_temp, false);
                    flush_complete_field.push_back(_field_name_temp);
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

void Write::flush_entry_disk(string & field_name, bool is_remove)
{
    auto _field = m_field_map[field_name];  // 通过映射拿到对应field

    // 开始meta 和entry 刷盘
    std::cout << "开始刷写meta 和entry" << std::endl;

    /**
     * 生成meta 刷盘
     * 然后entry 刷写到磁盘(拿取的时候就清空了)
     *
     * 此时不会有新的meta 和 entry生成
     * 所以可以直接获取entry 数量
     */
    int _entry_size = _field->get_index_deque_size();  // 获取entry 数量[还是会小于阈值]
    std::shared_ptr<IndexBlockMeta> _meta(new IndexBlockMeta());
    if (_field->m_type == DataBlock::DATA_STRING)
    {
        _meta = m_tsm.create_index_meta(IndexBlockMeta::Type::DATA_STRING, m_measurement, _field->m_field_name);
    }
    else if (_field->m_type == DataBlock::DATA_INTEGER)
    {
        _meta = m_tsm.create_index_meta(IndexBlockMeta::Type::DATA_INTEGER, m_measurement, _field->m_field_name);
    }
    else if (_field->m_type == DataBlock::Type::DATA_FLOAT)
    {
        _meta = m_tsm.create_index_meta(IndexBlockMeta::Type::DATA_FLOAT, m_measurement, _field->m_field_name);
    }
    _meta->set_count(_entry_size);  // 设置entry 数量

    int _index_size = (12 + m_measurement.length() + _field->m_field_name.length()) + _entry_size * 28;
    m_tail_offset -= _index_size;  // 前移到指定位置开始写入

    m_tsm.write_index_meta_to_file(_meta, "data.tsm", m_tail_offset);  // 写入meta
    m_tail_offset += (12 + m_measurement.length() + _field->m_field_name.length());  // 后移指针

    // 如果有 entry[数量只会低于_field->get_index_deque_size() > x] x这个阈值
    while (!_field->get_index_status())
    {
        auto entry = _field->pop_index_from_deque();
        m_tsm.write_index_entry_to_file(entry, "data.tsm", m_tail_offset);
        m_tail_offset += 28;  // 后移指针
    }
    m_tail_offset -= _index_size;  // 写入后指针发生变化还需要重新移位

    // 写了meta 后就需要更新footer 的索引
    Footer _footer(m_tail_offset);
    m_tsm.write_footer_to_file(_footer, "data.tsm");

    if (is_remove)
    {
        // 写完后将m_index_set 中的字段直接移除，表示目前这个字段没有entry 待刷入了
        m_index_set.erase(field_name);
    }

    // 重置刷入时间
    _field->m_index_last_time = high_resolution_clock::now();
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

    std::shared_ptr<Field> _field;
    // 没合适的field
    if (type == "string")
    {
        _field = std::make_shared<Field>(DataBlock::Type::DATA_STRING, field_name);
    }
    else if (type == "integer")
    {
        _field = std::make_shared<Field>(DataBlock::Type::DATA_INTEGER, field_name);
    }
    else if (type == "float")
    {
        _field = std::make_shared<Field>(DataBlock::Type::DATA_FLOAT, field_name);
    }
    else
    {
        std::cerr << "Error : Unknown type " << type << std::endl;
        return nullptr;
    }
    // 注册观察者
    _field->attach(&queue_state);
    _field->get_skip_list().attach(&tb_state);

    m_field_map[field_name] = _field;
    return _field;
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

    string _field = m_field_list.front();
    m_field_list.pop_front();
    return _field;
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