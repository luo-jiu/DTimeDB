#include "engine/tsm/controller.h"
#include <algorithm>
using namespace dt::tsm;
using namespace dt::impl;
using namespace std::chrono;
using std::string;

Controller::Controller(): m_producer_thread_pool(8), m_consumer_thread_pool(6), m_running(true), m_file("tsm")
{
    init();
    // 设置回调函数
    m_field_state.set_skip_condition_callback(
            [this](const std::string & db_name, const std::string & tb_name, const std::pair<std::string, std::string> & shard_id_and_series_info, const std::string & field_name) {
                // skip_list 数据监控 回调处理函数
                return is_ready_disk_write(db_name, tb_name, shard_id_and_series_info, field_name);
            });
    m_field_state.set_index_condition_callback(
            [this](const std::string & db_name, const std::string & tb_name, const std::string & shard_id, const std::string & field_name) {
                // index queue监控 回调处理函数
                return is_ready_index_write(db_name, tb_name, shard_id, field_name);
            });
    m_table_state.set_condition_callback(
            [this](const std::string & db_name, const std::string & tb_name) {
                // data block queue监控 回调处理函数
                return disk_write(db_name, tb_name);
            });
    m_shard_state.set_condition_callback(
            [this](){
                return flush_shard_meta();
            });
}

Controller::~Controller()
{
    stop_monitoring_thread();
    if (m_monitor_thread.joinable())
    {
        m_monitor_thread.join();
    }
}

/**
 * 初始化函数
 *
 * 主要目标是启动写入线程，监控线程
 */
void Controller::init()
{
    //  开启监控线程
    load_meta_file();
    m_monitor_thread = std::thread([this]{ this->monitoring_thread(); });
}

/**
 * 创建数据库
 *
 * 其实就是在真实的文件系统上创建了一个文件夹
 */
bool Controller::create_database(
        string & db_name)
{
    auto path = m_file.create_database(db_name);
    if (!path.empty())
    {
        return true;
    }
    return false;
}

/**
 * 当要操作一个表的时候才会调用
 */
bool Controller::load_database(
        string & db_name,
        std::vector<std::string> & tables)
{
    std::unique_lock<std::shared_mutex> lock(m_fields_map_mutex);
    for (auto& tb_name : tables)
    {
        bool exist = false;
        auto db_it = m_db_mea_map.find(db_name);
        if (db_it != m_db_mea_map.end())
        {
            auto tb_it = db_it->second.m_mea_fields.find(tb_name);
            if (tb_it != db_it->second.m_mea_fields.end())
            {
                exist = true;
            }
        }

        if (!exist)
        {
            // 不存在,为这个表加载字段信息
            m_db_mea_map[db_name].m_mea_fields[tb_name] = m_file.load_mea_fields(db_name, tb_name);
        }
    }
    lock.unlock();

    return m_file.load_database(db_name, tables);
}

void Controller::show_table(
        string & db_name)
{
    m_file.show_data(db_name);
}

/**
 * 创建表
 * @param tb_name
 * @return
 */
bool Controller::create_table(
        string & tb_name,
        string & db_name)
{
    // 先判断表是否已经存在
    if (m_file.exists_table(db_name, tb_name, false))
    {
        // 表已经存在
        std::cout << tb_name << " exists" << std::endl;
        return false;
    }

    // 创建表
    return m_file.create_table(tb_name, db_name);
}

/**
 * 插入数据操作
 * @param file_path 文件路径
 * @return 操作是否成功
 */
bool Controller::insert(
        high_resolution_clock::time_point timestamp,
        string & tags_str,
        string value,
        Type type,
        string & field_name,
        string & tb_name,
        string & db_name,
        bool is_monitor_thread)
{
    // 从线程池分配任务
    m_producer_thread_pool.enqueue(&Controller::insert_thread, this, timestamp, tags_str, value, type, field_name, tb_name, db_name, is_monitor_thread);
    return true;
}

void Controller::insert_thread(
        high_resolution_clock::time_point timestamp,
        const string & tags_str,
        string value,
        Type type,
        string & field_name,
        string & tb_name,
        string & db_name,
        bool is_monitor_thread)
{
    std::unique_lock<std::shared_mutex> write_lock(m_mutex);
    std::cout << "tsm调用线程池处理插入任务..." << std::endl;

    // 先看文件管理器里面有没有
    if (m_file.exists_table(db_name, tb_name, true))
    {
        if (!exists_table(db_name, tb_name))
        {
            // 不存在需要初始化
            std::map<string, Table> table;
            auto write = std::make_shared<Write>(db_name, tb_name);
            write->set_file_path_manager(&m_file);  // 依赖注入
            write->shard_attach(&m_shard_state);  // 注册观察者
            table[tb_name] = Table{write};
            m_map[db_name] = Database{"", table};

            // 为这个表加载字段信息
            m_db_mea_map[db_name].m_mea_fields[tb_name] = m_file.load_mea_fields(db_name, tb_name);
        }
    }
    else
    {
        std::cout << "the '"<< tb_name << "' table does not exist.";
        return;
    }

    // 拿取出对应表的writer (这里一定是读取)
    auto & writer = m_map[db_name].m_table_map[tb_name].m_writer;
    write_lock.unlock();
    // 类型转换
    DataBlock::Type type_temp;
    if (type == IEngine::Type::DATA_STRING)
    {
        type_temp = DataBlock::DATA_STRING;
    }
    else if (type == IEngine::Type::DATA_INTEGER)
    {
        type_temp = DataBlock::DATA_INTEGER;
    }
    else if (type == IEngine::Type::DATA_FLOAT)
    {
        type_temp = DataBlock::DATA_FLOAT;
    }
    else
    {
        std::cerr << "Error : unknown type " << type << std::endl;
        return;
    }

    // 判断field 是否已经存在
    if (!mea_field_exist(db_name, tb_name, field_name))
    {
        // 不存在
        std::cout << "字段不存在，写入表元数据,field " << field_name << std::endl;
        m_file.insert_field_to_file(db_name, tb_name, field_name);
        add_field(db_name, tb_name, field_name);
    }

    // 拼接seriesKey
    string series_key;
    if (!is_monitor_thread)
    {
        // 不是监控线程则需要拼接
        series_key = tb_name + tags_str;  // measurement + tags
    }
    writer->write(timestamp, series_key, value, type_temp, field_name, db_name, tb_name, m_field_state, m_table_state, is_monitor_thread);
}

/**
 * 创建索引
 */
bool Controller::create_index(
        string & measurement,
        std::list<string> & tags)
{
    // 从线程池分配任务
    m_producer_thread_pool.enqueue(&Controller::create_index_thread, this, measurement, tags);
    return true;
}

void Controller::create_index_thread(
        string & measurement,
        std::list<string> & tags)
{
    m_index.create_index(measurement, tags);
}

//bool Controller::get_next_data(string & data)
//{
//
//}
//
//void Controller::begin_indexed_scan(
//        const high_resolution_clock::time_point & timestamp,
//        string & data)
//{
//
//}
//
//bool Controller::get_range_data(
//        const high_resolution_clock::time_point & start,
//        const high_resolution_clock::time_point & end,
//        std::vector<string> & data)
//{
//
//}

/**
 * 查询数据
 * @param measurement 测量值
 * @param field 字段集合
 * @param tags 纬度值
 * @param expr_node 表达式树
 */
bool Controller::get_range_data(
        const string & db_name,
        const string & measurement,
        std::vector<std::pair<std::string, std::string>> reduce_fields,
        std::shared_ptr<ExprNode> expr_node)
{
    m_producer_thread_pool.enqueue(&Controller::analytic_expr_tree, this, db_name, measurement, reduce_fields, expr_node);
    return true;
}

void print_row_node(const RowNode & row)
{
    // 将时间戳转换为易读的字符串格式
    auto timestamp = std::chrono::system_clock::to_time_t(row.m_timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %H:%M:%S");

    std::cout << "Timestamp: " << ss.str() << std::endl;

    // 遍历并打印每个字段的名称和值
    for (const auto& [field_name, single_data] : row.m_columns) {
        std::cout << "Field: " << field_name << ", Value: " << single_data.m_data << "\n";
    }
}

/**
 * 分离出tag 以及where
 */
void Controller::analytic_expr_tree(
        const string & db_name,
        const string & measurement,
        const std::vector<std::pair<string, string>> & reduce_fields,
        const std::shared_ptr<impl::ExprNode> & expr_node)
{
    // 解析表达式树(摘除掉tag 的部分)
    std::vector<std::pair<std::string, std::string>> tags;
    auto new_expr_tree = rebuild_tree_without_tags(measurement, expr_node, tags);

    // 获取该measurement 下所有的分片ID(shard id),需要结合where 条件过滤额掉不需要的分片(new_expr_tree)
    auto shard_ids = filter_shards(db_name, measurement, new_expr_tree);
    std::sort(shard_ids.begin(), shard_ids.end());  // 排序

    // 获取所有的字段

    // 在此需要判断哪个字段需要聚合
    // ...

    std::vector<string> fields;
    if (reduce_fields.size() == 1 && reduce_fields[0].first == "*")
    {
        auto fields_set = m_file.load_mea_fields(db_name, measurement);
        std::copy(fields_set.begin(), fields_set.end(), std::back_inserter(fields));
    }
    else
    {
        for (const auto& field : reduce_fields)
        {
            fields.push_back(field.first);
        }
    }

    // 构建迭代器树
    auto root_iterator = create_iterator_tree(db_name, measurement, fields, shard_ids, tags, new_expr_tree);

    // 开始运行迭代树
    m_row_nodes.clear();
    while (root_iterator.has_next())
    {
        RowNode row = root_iterator.next();
        if (root_iterator.evaluate_expr_node_for_row(new_expr_tree, row))
        {
            // 该行数据符合条件
            m_row_nodes.push_back(row);
        }
        // 不符合则跳过该行...
    }

    // 输出查询结果
    for (const auto& row : m_row_nodes)
    {
        print_row_node(row);
    }
}

/**
 * 构建迭代器树
 */
RootIterator Controller::create_iterator_tree(
        const string & db_name,
        const string & measurement,
        std::vector<std::string> & fields,
        std::vector<std::string> & shard_ids,
        std::vector<std::pair<std::string, std::string>> & tags,
        const std::shared_ptr<impl::ExprNode> & expr_node)
{
    // 利用倒排索引查询series key,并求所有tag 对应series key 的交集
    std::set<string> accumulated_intersection;
    bool first = true;
    for (const auto& tag : tags)
    {
        auto current_set = m_index.get_series_key_by_tag(measurement, tag);
        if (first)
        {
            accumulated_intersection = current_set;
            first = false;
            continue;
        }

        std::set<string> temp_intersection;
        std::set_intersection(accumulated_intersection.begin(), accumulated_intersection.end(),
                              current_set.begin(), current_set.end(),
                              std::inserter(temp_intersection, temp_intersection.begin()));
        accumulated_intersection = std::move(temp_intersection);
    }

    // 在此需要判断哪个字段是需要聚合的
    // 暂时不做

    std::unique_lock<std::shared_mutex> write_lock(m_mutex);  // 加锁
    RootIterator root_iterator(db_name, measurement);
    root_iterator.m_expr_node = expr_node;  // 配置表达式树

    // 加载索引到内存
    for (const auto& shard_id : shard_ids)
    {
        // 加载shard [该时间段内全部的data block索引]，按照series key + field_name 区分
        auto shard = *(m_map[db_name].m_table_map[measurement].m_writer->get_shard(shard_id));
        root_iterator.m_tsm_io_manager->load_tsm_index_entry(shard);
    }

    // 遍历字段构建字段迭代器
    for (const auto& field : fields)  // 构建第一层[FieldIterator]
    {
        auto field_iterator = std::make_shared<FieldIterator>(field);

        for (const auto& shard_id : shard_ids)  // 构建第二层[ShardIterator]
        {
            auto shard_iterator = std::make_shared<ShardIterator>();

            // std::set本就是有序的直接遍历
            for (const auto& series_key : accumulated_intersection)  // 构建第三层[TagSetIterator]
            {
                auto tag_set_iterator = std::make_shared<TagSetIterator>();
                tag_set_iterator->m_series_key = series_key;
                tag_set_iterator->m_series_blocks = root_iterator.m_tsm_io_manager->m_tsm_index[shard_id][series_key + field];  // 传递索引

                // 依赖注入
                tag_set_iterator->m_tsm_io_manager = root_iterator.m_tsm_io_manager;
                shard_iterator->m_tag_set_iterators.push_back(tag_set_iterator);
            }
            field_iterator->m_shard_iterators.push_back(shard_iterator);
        }
        root_iterator.m_field_iterators.push_back(field_iterator);
    }
    return root_iterator;
}

/**
 * 用于遍历表达式树来判断哪个shard 符合条件
 */
bool Controller::evaluate_condition(
        const std::string & shard_id,
        std::shared_ptr<impl::ExprNode> & expr_node)
{
    if (!expr_node) return true;
    if (expr_node->is_comparison())  // 如果是比较操作符
    {
        return expr_node->evaluate_shard(shard_id);
    }
    else if (expr_node->is_logical())  // 如果是逻辑操作符
    {
        // 处理逻辑操作符
        bool left_result = evaluate_condition(shard_id, expr_node->m_left);
        bool right_result = evaluate_condition(shard_id, expr_node->m_right);

        if (expr_node->m_val == "and")
        {
            return left_result && right_result;
        }
        else if (expr_node->m_val == "or")
        {
            return left_result || right_result;
        }
    }
    return false;
}

/**
 * 过滤筛选出符合的shard_id
 */
std::vector<string> Controller::filter_shards(
        const string & db_name,
        const string & measurement,
        std::shared_ptr<impl::ExprNode> & expr_node)
{
    std::vector<string> result_shard_id;
    std::vector<string> filter_shards;
    // 获取该字段下所有的shard id
    std::unique_lock<std::shared_mutex> write_lock(m_mutex);
    auto database_it = m_map.find(db_name);
    if (database_it != m_map.end())
    {
        auto table_it = database_it->second.m_table_map.find(measurement);
        if (table_it != database_it->second.m_table_map.end())
        {
            result_shard_id = table_it->second.m_writer->get_shard_ids();
        }
    }
    for(const auto& shard_id : result_shard_id)
    {
        if (evaluate_condition(shard_id, expr_node))
        {
            filter_shards.push_back(shard_id);
            std::cout << "符合的shard id: " << shard_id << "\n";
        }
    }
    // 记得排序shard
    return filter_shards;
}

/**
 * 重构表达式树
 */
std::shared_ptr<ExprNode> Controller::rebuild_tree_without_tags(
        const std::string & measurement,
        const std::shared_ptr<ExprNode> & expr_node,
        std::vector<std::pair<std::string, std::string>> & tags)
{
    if (!expr_node)
    {
        return nullptr;
    }
    // 如果当前比较属于纬度比较，返回空，移除该节点
    if (is_tag_comparison(measurement, expr_node))
    {
        tags.emplace_back(expr_node->m_left->m_val, expr_node->m_right->m_val);
        return nullptr;
    }
    auto left = rebuild_tree_without_tags(measurement, expr_node->m_left, tags);
    auto right = rebuild_tree_without_tags(measurement, expr_node->m_right, tags);

    if ((expr_node->m_val == "and" || expr_node->m_val == "or") && (!left || !right))
    {
        return left ? left : right;
    }
    auto new_node = std::make_shared<ExprNode>(*expr_node);
    new_node->m_left = left;
    new_node->m_right = right;
    return new_node;
}

bool Controller::is_tag_comparison(
        const std::string & measurement,
        const std::shared_ptr<ExprNode> & node)
{
    if (node && node->m_val == "=")
    {
        return m_index.whether_tag(measurement, node->m_left->m_val);
    }
    return false;
}

bool Controller::sys_show_file(
        string & db_name,
        string & tb_name)
{
    // 先看文件管理器里面有没有
    if (m_file.exists_table(db_name, tb_name, true))
    {
        return m_file.sys_show_file(db_name, tb_name);
    }
    return false;
}

bool Controller::sys_update_file(
        string & db_name,
        string & tb_name,
        const string & where)
{
    // 先看文件管理器里面有没有
    if (m_file.exists_table(db_name, tb_name, true))
    {
        size_t pos = where.find('=');
        string key, value;
        if (pos != std::string::npos) {
            key = where.substr(0, pos);
            value = where.substr(pos + 1);
        }
        if (key == "head_offset")
        {
            return m_file.update_system_file_offset(db_name, tb_name, "head", std::stoi(value));
        }
        if (key == "tail_offset")
        {
            return m_file.update_system_file_offset(db_name, tb_name, "tail", std::stoi(value));
        }
        else if (key == "margin")
        {
            return m_file.update_system_file_margin(db_name, tb_name, std::stoi(value));
        }
    }
    return false;
}

bool Controller::sys_clear_file(
        string & db_name,
        string & tb_name)
{
    // 先看文件管理器里面有没有
    if (m_file.exists_table(db_name, tb_name, true))
    {
        return m_file.sys_clear_file(db_name, tb_name);
    }
    return false;
}

/**
 * 监控线程需要执行到函数
 *
 * 用于不断去检查跳表中是否有残余数据还未刷写
 */
void Controller::monitoring_thread()
{
    while(m_running.load())
    {
        m_field_state.iterate_map(false);  // 监控skip list
        m_field_state.iterate_map(true);   // 监控index entry
        m_table_state.iterate_map();       // 监控data deque
        m_shard_state.iterate_map();       // 监控shard
        // 等待一段时间再次检查
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

/**
 * 结束监控线程
 */
void Controller::stop_monitoring_thread()
{
    m_running.store(false);
}

/**
 * 保证调用这个方法的方法已经有读写锁了
 */
bool Controller::exists_table(
        string & db_name,
        string & tb_name)
{
    auto db_it = m_map.find(db_name);
    if (db_it != m_map.end())
    {
        auto tb_it = db_it->second.m_table_map.find(tb_name);
        if (tb_it != db_it->second.m_table_map.end())
        {
            return true;
        }
    }
    return false;
}

/**
 * 加载meta.tsm
 */
void Controller::load_meta_file()
{
    // 锁住整个表开始加载元数据
    std::unique_lock<std::shared_mutex> write_lock(m_mutex);
    std::ifstream file("./../metadata/meta.tsm", std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file for reading." << std::endl;
        return;
    }
    while (!file.eof())
    {
        auto shard = std::make_unique<Shard>();
        uint16_t length;
        file.read(reinterpret_cast<char*>(&length), sizeof(uint16_t));
        if (file.eof()) break; // 避免处理文件末尾的额外读取

        std::vector<char> buffer(length);
        file.read(buffer.data(), length);

        // 反序列化
        string data(buffer.begin(), buffer.end());
        shard->ParseFromString(data);
        std::cout << "读取shard:" << shard->DebugString() << std::endl;

        auto db_name = shard->database_name();
        auto tb_name = shard->measurement();
        auto shard_id = shard->shard_id();

        // 判断write 是否存在
        bool write_exist = false;
        auto db_it = m_map.find(db_name);
        if (db_it != m_map.end())
        {
            auto tb_it = db_it->second.m_table_map.find(tb_name);
            if (tb_it != db_it->second.m_table_map.end())
            {
                // 说明write 存在
                tb_it->second.m_writer->add_shard(shard_id, (std::unique_ptr<Shard> &) std::move(shard));
                write_exist = true;
            }
        }

        // 不存在则创建
        if (!write_exist)
        {
            // 为其创建write 并添加到其中
            auto write = std::make_shared<Write>(db_name, tb_name);

            // 不存在需要初始化
            std::map<string, Table> table;
            write->set_file_path_manager(&m_file);  // 依赖注入
            write->shard_attach(&m_shard_state);  // 注册观察者
            table[tb_name] = Table{write};
            m_map[db_name] = Database{"", table};

            write->add_shard(shard_id, (std::unique_ptr<Shard> &) std::move(shard));
        }
    }
    file.close();
}

void Controller::add_field(
        const std::string & db_name,
        const std::string & tb_name,
        const std::string & field)
{
    std::unique_lock<std::shared_mutex> write_lock(m_fields_map_mutex);
    m_db_mea_map[db_name].m_mea_fields[tb_name].insert(field);
}

/**
 * 该字段是否存在
 */
bool Controller::mea_field_exist(
        const std::string & db_name,
        const std::string & tb_name,
        const std::string & field)
{
    std::shared_lock<std::shared_mutex> read_lock(m_fields_map_mutex);

    // 首先检查数据库是否存在
    auto db_it = m_db_mea_map.find(db_name);
    if (db_it == m_db_mea_map.end()) {
        // 数据库不存在
        return false;
    }

    // 然后检查表是否存在
    auto &mea_fields = db_it->second.m_mea_fields;
    auto tb_it = mea_fields.find(tb_name);
    if (tb_it == mea_fields.end()) {
        // 表不存在
        return false;
    }

    // 最后检查字段是否存在
    return tb_it->second.find(field) != tb_it->second.end();
}

/**
 * 残余数据刷盘
 */
bool Controller::is_ready_disk_write(
        const string & db_name,
        const string & tb_name,
        const std::pair<std::string, std::string> & shard_id_and_series_info,
        const string & field_name)
{
    std::shared_lock<std::shared_mutex> read_lock(m_mutex);
    auto db_it = m_map.find(db_name);
    if (db_it != m_map.end())
    {
        auto tb_it = db_it->second.m_table_map.find(tb_name);
        if (tb_it != db_it->second.m_table_map.end())
        {
            auto writer = tb_it->second.m_writer;
            if (writer)
            {
                if (writer->skip_need_flush_data_block(shard_id_and_series_info.first + shard_id_and_series_info.second, field_name))
                {
                    std::cout << "---满足刷块\n";
                    // 刷跳表(传入空数据激活即可) 获取对应字段类型
                    auto fd = field_name;
                    auto tb = tb_name;
                    auto db = db_name;
                    // 利用插入数据语句,进入对应跳表激活判断逻辑引发刷块
                    m_producer_thread_pool.enqueue(&Controller::insert_thread,
                         // shard_id_and_series_info.first 是shard_id  |shard_id_and_series_info.second 是series_key + field_name
                         this, system_clock::from_time_t(Write::shard_key_to_timestamp(shard_id_and_series_info.first)), "", shard_id_and_series_info.second, Type::DATA_STRING, fd, tb, db, true);
                    return true;
                }
                else
                {
                    std::cout << "---本轮不满足刷块\n";
                }
            }
        }
    }
    return false;
}

/**
 * 监控是否需要将index entry刷盘
 */
bool Controller::is_ready_index_write(
        const string & db_name,
        const string & tb_name,
        const string & shard_id,
        const string & field_name)
{
    std::shared_lock<std::shared_mutex> read_lock(m_mutex);
    auto db_it = m_map.find(db_name);
    if (db_it != m_map.end())
    {
        auto tb_it = db_it->second.m_table_map.find(tb_name);
        if (tb_it != db_it->second.m_table_map.end())
        {
            auto writer = tb_it->second.m_writer;
            if (writer)
            {
//                std::cout << "---判断是否需要将index entry 刷写到磁盘\n";
                if (writer->should_flush_index(field_name))
                {
                    std::cout << "--监控线程触发写入index entry";
                    // 需要刷盘, 注册事件队列然后触发刷盘函数即可
                    writer->push_task(field_name);
                    // 线程池触发index block 刷盘
                    m_consumer_thread_pool.enqueue(&Controller::disk_write_thread, this, db_name, tb_name);
                    return true;
                }
                else
                {
                    std::cout << "---本轮不满足index entry 刷盘\n";
                }
            }
        }
    }
    return false;
}

/**
 * 开启刷盘线程
 */
bool Controller::disk_write(
        const string & db_name,
        const string & tb_name)
{
    // 从线程池拿取线程进行刷盘操作
    m_consumer_thread_pool.enqueue(&Controller::disk_write_thread, this, db_name, tb_name);
    return true;
}

void Controller::disk_write_thread(
        const string & db_name,
        const string & tb_name)
{
    std::cout << "================disk_write_thread==============" << std::endl;
    // 拿到对应Writer
    auto writer = m_map[db_name].m_table_map[tb_name].m_writer;

    if (writer)
    {
        // 调用其刷盘函数
        writer->queue_flush_disk();
    }
}

bool Controller::flush_shard_meta()
{
    m_consumer_thread_pool.enqueue(&Controller::flush_shard_meta_thread, this);
    return true;
}

/**
 * 达到目标将给所有的shard 刷盘
 */
bool Controller::flush_shard_meta_thread()
{
    // 锁住整个表开始刷写元数据
    std::unique_lock<std::shared_mutex> write_lock(m_mutex);
    auto stream = std::make_shared<std::fstream>("./../metadata/meta.tsm", std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
    if (!stream->is_open())
    {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return false; // 确保文件打开成功
    }

    std::cout << "开始刷写shard\n";
    for (auto& write : m_map)
    {
        for(auto& table : write.second.m_table_map)
        {
            auto shard_list = table.second.m_writer->get_shard_in_meta();
            for (const auto& shard_serialize : shard_list)
            {
                uint16_t length = shard_serialize.size();
                stream->write(reinterpret_cast<const char*>(&length), sizeof(uint16_t));
                stream->write(shard_serialize.c_str(), length);  // 写字符串
            }
            std::cout << "刷写了一个measurement 全部shard";
        }
    }
    std::cout << "shard刷写完毕\n";
    stream->flush();
    stream->close();
    return true;
}
