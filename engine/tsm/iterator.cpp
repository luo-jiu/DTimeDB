#include "iterator.h"

#include <utility>
#include "snappy.h"
#include "tsm.h"

using namespace dt::tsm;

TsmIOManagerCenter::TsmIOManagerCenter(std::string db_name, std::string tb_name) : m_io_manager(8), m_db_name(std::move(db_name)), m_tb_name(std::move(tb_name))
{}

void TsmIOManagerCenter::load_tsm_index_entry(const Shard & shard)
{
    std::unique_lock<std::shared_mutex> write_lock(m_mutex);
    for (const auto & tsm_file_name : shard.tsm_file())
    {
        SeriesIndexBlock series_index_block;
        series_index_block.m_tsm_name = tsm_file_name;
        std::string tsm_path = "./../dbs/" + shard.database_name() + "/" + tsm_file_name;
        auto file = m_io_manager.get_file_stream(tsm_path, "binary-read");
        if (!file->is_open())
        {
            std::cerr << "Error: Could not open file for writing" << std::endl;
            return;
        }
        // 读取index entry 偏移量
        file->seekg(-8, std::ios::end);  // 跳转到文件末尾

        int64_t tail = file->tellg();
        int64_t offset = 0;
        file->read(reinterpret_cast<char*>(&offset), sizeof(int64_t));
        file->seekg(offset);

        int64_t curr_pos = file->tellg();
        // 开始读取meta 和index_entry
        while (curr_pos < tail)
        {
            IndexBlockMeta index_meta;
            uint16_t length;
            file->read(reinterpret_cast<char*>(&length), sizeof(uint16_t));  // series key

            char buffer[length + 1];
            file->read(buffer, length);
            buffer[length] = '\0';
            std::string series_key_and_field_name(buffer);
            index_meta.m_key_size = length;
            index_meta.m_key = series_key_and_field_name;
            file->read(reinterpret_cast<char*>(&index_meta.m_type), sizeof(index_meta.m_type));
            file->read(reinterpret_cast<char*>(&index_meta.m_count), sizeof(index_meta.m_count));
            series_index_block.m_meta = index_meta;

            // 清空数据
            series_index_block.m_index_entries.clear();

            // 开始读取index entry
            for(int i = 0; i < index_meta.m_count; ++i)
            {
                dt::tsm::IndexEntry entry;
                std::chrono::system_clock::time_point max;
                std::chrono::system_clock::time_point min;
                file->read(reinterpret_cast<char*>(&max), sizeof(std::chrono::system_clock::time_point));
                file->read(reinterpret_cast<char*>(&min), sizeof(std::chrono::system_clock::time_point));

                entry.set_max_time(max);
                entry.set_min_time(min);

                int64_t get_offset;
                file->read(reinterpret_cast<char*>(&get_offset), sizeof(get_offset));
                entry.set_offset(get_offset);

                int32_t size;
                file->read(reinterpret_cast<char*>(&size), sizeof(size));
                entry.set_size(size);
                series_index_block.m_index_entries.push_back(entry);
            }

            m_tsm_index[shard.shard_id()][series_key_and_field_name].push_back(series_index_block);
            curr_pos = file->tellg();
        }
        m_io_manager.release_file_stream(tsm_path);
    }
}

/**
 * 比较函数，用于基于IndexBlockMeta的m_key_size进行排序
 */
bool TsmIOManagerCenter::compare_index_block_meta(
        const SeriesIndexBlock & a,
        const SeriesIndexBlock & b)
{
    return a.m_meta.get_key_length() < b.m_meta.get_key_length();
}

/**
 * 获取到下一个数据
 */
std::optional<SingleData> TagSetIterator::next()
{
    // 这个块的数据已经加载结束
    if (m_current_data_index >= m_timestamps.size())
    {
        // 是否还有剩余的块
        if (m_current_data_index < m_series_blocks.size())
        {
            // 尝试加载数据
            const auto& block = m_series_blocks[m_current_index++];
            get_curr_tsm_file_data(block.m_tsm_name, block.m_index_entries[0].get_offset());
        }
        else
        {
            return std::nullopt;  // 没有更多的数据块
        }
    }

    if (m_current_data_index < m_timestamps.size())
    {
        // 提供当前数据块的下一个数据点
        SingleData data{m_timestamps[m_current_data_index], m_values[m_current_data_index], m_type};
        ++m_current_data_index;
        return data;
    }

    return std::nullopt;  // 逻辑保护，理论上不应该到达这里
}

bool TagSetIterator::has_next()
{
    return m_current_data_index < m_timestamps.size() || m_current_index < m_series_blocks.size();
}

/**
 * 获取一个data block中的数据
 */
void TagSetIterator::get_curr_tsm_file_data(const std::string & tsm_file_name, int64_t offset)
{
    std::string file_path = "./../dbs/" + m_tsm_io_manager->m_db_name + "/" + tsm_file_name;
    auto file = m_tsm_io_manager->m_io_manager.get_file_stream(file_path, "binary-read");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing" << std::endl;
        return;
    }
    file->seekg(offset);
    dt::tsm::DataBlock data_block;
    file->read(reinterpret_cast<char*>(&data_block.m_type), sizeof(data_block.m_type));
    file->read(reinterpret_cast<char*>(&data_block.m_num), sizeof(data_block.m_num));
    file->read(reinterpret_cast<char*>(&data_block.m_tp_snappy_size), sizeof(data_block.m_tp_snappy_size));
    file->read(reinterpret_cast<char*>(&data_block.m_val_snappy_size), sizeof(data_block.m_val_snappy_size));

    // 读取并解压时间戳数据
    std::string compressed_timestamp(data_block.m_tp_snappy_size, '\0');
    file->read(&compressed_timestamp[0], data_block.m_tp_snappy_size);
    std::string timestamp_serialize;
    snappy::Uncompress(compressed_timestamp.data(), data_block.m_tp_snappy_size, &timestamp_serialize);
    auto timestamps =  TSM::deserialize_differences(timestamp_serialize);
    data_block.m_timestamps.clear();
    data_block.m_timestamps = TSM::restore_timestamps(timestamps);
    m_timestamps.clear();
    m_timestamps = TSM::restore_timestamps_vector(timestamps);
//    std::cout << "curr_offset:" << file->tellg() << std::endl;

    // 读取并解压值数据
    std::string compressed_val(data_block.m_val_snappy_size, '\0');
    file->read(&compressed_val[0], data_block.m_val_snappy_size);
    std::string val_serialize;
    snappy::Uncompress(compressed_val.data(), data_block.m_val_snappy_size, &val_serialize);
    data_block.m_values.clear();
    data_block.m_values = TSM::deserialize_strings(val_serialize);
    m_values.clear();
    m_values = TSM::deserialize_strings_vector(val_serialize);
//    std::cout << "curr_offset:" << file->tellg() << std::endl;

    m_current_data_index = 0;  // 重置计数器

    // 给数据类型附上
    if (data_block.m_type == DataBlock::DATA_STRING)
    {
        m_type = DataType::String;
    }
    else if (data_block.m_type == DataBlock::DATA_INTEGER)
    {
        m_type = DataType::Integer;
    }
    else if (data_block.m_type == DataBlock::DATA_FLOAT)
    {
        m_type = DataType::Float;
    }
    else
    {
        std::cout << "未知数据类型\n";
    }
}

bool ShardIterator::has_next()
{
    // 尝试找到一个有数据的TagSetIterator
    for (size_t i = m_current_tag_set_index; i < m_tag_set_iterators.size(); ++i)
    {
        if (m_tag_set_iterators[i]->has_next())
        {
            return true;
        }
    }
    return false;
}

/**
 * 该next 就需要开始做出判断了
 *
 * 有两种情况:
 * [情况一] 旗下的TagSetIterator 是需要聚合的
 * [情况二] 旗下的TagSetIterator 不需要聚合
 *
 * 处理：
 * 如果需要聚合，则直接对该TagSetIterator 中的数据全获取，进行同一聚合操作
 * 如果不需要聚合，则按照
 *
 */
std::optional<SingleData> ShardIterator::next()
{
    while (m_current_tag_set_index < m_tag_set_iterators.size())
    {
        auto& current_tag_setIterator = m_tag_set_iterators[m_current_tag_set_index];
        // 只要当前tag_set_iterators 有数据，就一直获取
        if (current_tag_setIterator->has_next())
        {
            return current_tag_setIterator->next();
        }
        else
        {
            // 当前TagSetIterator 已经没有数据，移动到下一个
            ++m_current_tag_set_index;
        }
    }

    // 当前shard 已经没有更多的数据了
    return std::nullopt;
}

FieldIterator::FieldIterator(const std::string & field)
{
    m_field = field;
}

/**
 * 要是不是聚合函数，就正常返回一条数据
 */
std::optional<SingleData> FieldIterator::next()
{
    if (m_current_shard_index < m_shard_iterators.size() && m_shard_iterators[m_current_shard_index]->has_next())
    {
        return m_shard_iterators[m_current_shard_index]->next();
    }
    // 没有数据后就直接返回空，RootIterator 会切换到下一个ShardIterator
    return std::nullopt;
}

/**
 * 执行聚合操作
 */
SingleData FieldIterator::aggregate()
{
    // 对 m_merges 中的数据进行聚合操作


}

/**
 * 检查shard 是否还有更多数据
 */
bool FieldIterator::has_next_in_current_shard()
{
    if (m_current_shard_index < m_shard_iterators.size())
    {
        // 返回当前shard 是否还有数据
        return m_shard_iterators[m_current_shard_index]->has_next();
    }
    return false;
}

/**
 * 移动到下一个shard
 */
bool FieldIterator::move_to_next_shard()
{
    if (m_current_shard_index + 1 < m_shard_iterators.size())
    {
        ++m_current_shard_index;
        return true;  // 成功移动到下一个shard
    }

    // 全部遍历结束
    return false;
}

RootIterator::RootIterator(const std::string & db_name, const std::string & tb_name)
{
    m_tsm_io_manager = std::make_shared<TsmIOManagerCenter>(db_name, tb_name);
}

bool compare(
        const std::string & op,
        const DataPoint::Value & field_value,
        const DataPoint::Value & compare_value)
{
    return std::visit([&op](const auto& a, const auto& b) -> bool {
        if constexpr (std::is_same_v<decltype(a), decltype(b)>) {
            if (op == "==")
            {
                return a == b;
            }
            else if (op == "!=")
            {
                return a != b;
            }
            else if constexpr (!std::is_same_v<decltype(a), std::string>)
            {
                // 只有非字符串类型支持<、>等比较
                if (op == ">")
                {
                    return a > b;
                }
                else if (op == "<")
                {
                    return a < b;
                }
                else if (op == ">=")
                {
                    return a >= b;
                }
                else if (op == "<=")
                {
                    return a <= b;
                }
            }
            else
            {
                // 对于字符串，处理特殊情况或输出错误
                std::cerr << "Attempt to compare strings with '>' or '<' not supported.\n";
            }
        }
        else
        {
            std::cerr << "Comparing different types not supported.\n";
        }
        return false; // 如果操作不支持或类型不匹配
    }, field_value, compare_value);
}

std::pair<DataPoint::Value, DataPoint::Value> type_conversion(
        const std::string & type,
        const DataPoint::Value & field_value,
        const DataPoint::Value & compare_value)
{

    // 用于转换的lambda函数
    auto to_int = [](const DataPoint::Value& val) -> DataPoint::Value {
        return std::visit([](auto&& arg) -> DataPoint::Value {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
            {
                return static_cast<int>(arg);
            }
            else
            {
                return arg; // 直接返回整型或字符串类型
            }
        }, val);
    };

    auto to_float = [](const DataPoint::Value& val) -> DataPoint::Value {
        return std::visit([](auto&& arg) -> DataPoint::Value {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>)
            {
                return static_cast<float>(arg);
            }
            else
            {
                return arg; // 直接返回浮点型或字符串类型
            }
        }, val);
    };

    DataPoint::Value converted_field_value = field_value;
    DataPoint::Value converted_compare_value = compare_value;

    if (type == "integer")
    {
        converted_field_value = to_int(field_value);
        converted_compare_value = to_int(compare_value);
    }
    else if (type == "float")
    {
        // 假设浮点型统一使用float表示
        converted_field_value = to_float(field_value);
        converted_compare_value = to_float(compare_value);
    }
    // 对于字符串类型，我们不进行转换

    return {converted_field_value, converted_compare_value};
}

std::pair<DataPoint::Value, DataPoint::Value> type_conversion_based_on_singledata(
        const SingleData& field_data,
        const std::string& compare_value_str)
{
    DataPoint::Value field_value, compare_value;

    switch (field_data.m_type)
    {
        case DataType::String:
        {
            field_value = field_data.m_data;
            compare_value = compare_value_str;
            break;
        }
        case DataType::Integer:
        {
            field_value = std::stoi(field_data.m_data);
            compare_value = std::stoi(compare_value_str);
            break;
        }
        case DataType::Float:
        {
            field_value = std::stof(field_data.m_data);
            compare_value = std::stof(compare_value_str);
            break;
        }
    }

    return {field_value, compare_value};
}

/**
 * 数据过滤函数
 *
 * 会将拼接好的一行数据传入进行过滤
 * 符合所有条件则返回 true,否则false
 */
bool RootIterator::evaluate_expr_node(
        const std::shared_ptr<dt::impl::ExprNode> & node,
        const DataPoint & data_point)
{
    if (!node) return true; // 假设空节点不影响结果

    if (node->m_val == "and")
    {
        return evaluate_expr_node(node->m_left, data_point) && evaluate_expr_node(node->m_right, data_point);
    }
    else if (node->m_val == "or")
    {
        return evaluate_expr_node(node->m_left, data_point) || evaluate_expr_node(node->m_right, data_point);
    }
    else
    {
        // 处理比较操作，假设比较操作节点的左子节点存储字段名，右子节点存储比较值（直接为Value类型）
        const DataPoint::Value & field_value = data_point.get_value(node->m_left->m_val); // 获取字段的值
        DataPoint::Value compare_value = node->m_right->m_val;  // 获取节点的值

        // 获取字段类型
        auto field_type_it = m_field_type.find(node->m_left->m_val);
        if (field_type_it == m_field_type.end())
        {
            return false;
        }

        auto [converted_field_value, converted_compare_value] = type_conversion(field_type_it->second, field_value, compare_value);
        return compare(node->m_val, converted_field_value, converted_compare_value);
    }
}

bool RootIterator::compare_timestamps(
        const std::string & comparison_operator,
        const std::chrono::system_clock::time_point & row_timestamp,
        const std::string & condition_timestamp_str)
{
    // 解析条件时间戳字符串为std::time_t
    std::time_t condition_time = impl::ExprNode::parse_timestamp(condition_timestamp_str);

    // 获取RowNode的时间戳，并转换为std::time_t进行比较
    std::time_t row_time = std::chrono::system_clock::to_time_t(row_timestamp);

    if (comparison_operator == ">")
    {
        return row_time > condition_time;
    }
    else if (comparison_operator == "<")
    {
        return row_time < condition_time;
    }
    else if (comparison_operator == ">=")
    {
        return row_time >= condition_time;
    }
    else if (comparison_operator == "<=")
    {
        return row_time <= condition_time;
    }
    else if (comparison_operator == "==")
    {
        return row_time == condition_time;
    }
    else if (comparison_operator == "!=")
    {
        return row_time != condition_time;
    }

    throw std::invalid_argument("Unsupported comparison operator: " + comparison_operator);
}

/**
 * 用于过滤数据
 */
bool RootIterator::evaluate_expr_node_for_row(
        const std::shared_ptr<dt::impl::ExprNode> & node,
        const RowNode & row_node)
{
    if (!node) return true; // 假设空节点不影响结果

    if (node->m_left && node->m_left->m_val == "time")
    {
        // 对于时间戳的比较
        return compare_timestamps(node->m_val, row_node.m_timestamp, node->m_right->m_val);
    }
    else if (node->m_val == "and")
    {
        return evaluate_expr_node_for_row(node->m_left, row_node) && evaluate_expr_node_for_row(node->m_right, row_node);
    }
    else if (node->m_val == "or")
    {
        return evaluate_expr_node_for_row(node->m_left, row_node) || evaluate_expr_node_for_row(node->m_right, row_node);
    }
    else
    {
        // 处理比较
        auto it = row_node.m_columns.find(node->m_left->m_val);
        if (it == row_node.m_columns.end())
        {
            // 指定的字段不再当前行中
            return false;
        }

        // 从SingleData获取字段值和类型
        const auto& field_data = it->second;
        const auto& compare_value_str = node->m_right->m_val; // 假设这是一个字符串表示的比较值

        // 使用类型信息进行适当的类型转换
        auto [converted_field_value, converted_compare_value] = type_conversion_based_on_singledata(
                field_data, compare_value_str);

        // 使用转换后的值进行比较
        return compare(node->m_val, converted_field_value, converted_compare_value);
    }
}

/*
 * 加载当前shard 中的所有数据
 */
bool RootIterator::load_curr_shard_data()
{
    // 如果已经加载直接返回
    if (m_shard_data_loaded) return true;

    bool has_data = false;
    for (auto& iter : m_field_iterators)
    {
        while (iter->has_next_in_current_shard())
        {
            std::optional<SingleData> dp_opt = iter->next();
            if (dp_opt.has_value())
            {
                const SingleData& dp = *dp_opt;

                // 使用时间戳作为键，更新或插入RowNode
                auto& node = m_row_nodes[dp.m_timestamp];
                node.m_timestamp = dp.m_timestamp;
                node.add_data_point(iter->m_field, dp);

                has_data = true;
            }
            else
            {
                // 没有数据，停止循环
                break;
            }
        }
    }

    // 根据是否成功加载了数据来更新m_shard_data_loaded 状态
    m_shard_data_loaded = has_data;

    return has_data;
}

RowNode RootIterator::next()
{
    if (m_row_nodes.empty() && !load_curr_shard_data())
    {
        if (!move_to_next_shard())  // 尝试移动到下一个shad 并加载数据
        {
            throw std::out_of_range("No more data available.");
        }
    }
    // 获取map中的第一个元素（最早的时间戳对应的RowNode）
    RowNode next_node = m_row_nodes.begin()->second;
    m_row_nodes.erase(m_row_nodes.begin());

    // 不需要检查m_row_nodes是否为空来决定m_shard_data_loaded的值
    // 因为在下一次调用next()时，会自动尝试加载数据或移动到下一个分片

    return next_node;
}

bool RootIterator::has_next()
{
    if (!m_row_nodes.empty()) return true;
    if (m_shard_data_loaded) return false;
    return move_to_next_shard();
}

bool RootIterator::move_to_next_shard()
{
    m_shard_data_loaded = false;  // 重置当前shard 标记
    bool any_shard_left = false;
    for (auto& iter : m_field_iterators)
    {
        if (iter->move_to_next_shard())
        {
            // 如果有数据
            any_shard_left = true;
        }
    }

    if (!any_shard_left)
    {
        // 没有shard 就退出了
        return false;
    }
    return load_curr_shard_data();  // 尝试加载下一个shard 数据
}









