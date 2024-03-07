#include "iterator.h"
#include "snappy.h"
#include "tsm.h"

using namespace dt::tsm;

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
SingleData TagSetIterator::next()
{
    // 还有数据
    if (m_data_count != data_num)
    {
        SingleData single_data;
        single_data.m_timestamp = m_timestamps[m_data_count];
        single_data.m_data = m_values[m_data_count];
        ++m_data_count;
        // 直接返回数据
        return single_data;
    }
    else // 数据已经读完
    {
        auto& curr_series_block = m_series_blocks[m_series_count];  // 当前索引块
        // 先判断是否还有块可以加载
        if (curr_series_block.m_index_count != curr_series_block.m_index_entries.size() - 1)
        {
            // 还有块直接加载到内存
            get_curr_tsm_file_data(curr_series_block.m_tsm_name, curr_series_block.m_index_entries[curr_series_block.m_index_count].get_offset());
            ++curr_series_block.m_index_count;
            return next();  // 自己调用自己
        }
        else
        {
            // 再判断是否有新series 可以加载
            if (m_series_count != m_series_blocks.size() - 1)
            {
                
            }
            else
            {

            }
        }
    }

    if (m_is_ploy)
    {
        // 不是聚合操作
        SingleData data_point;
        data_point.m_timestamp = m_timestamps[m_data_count];
        data_point.m_data = m_values[m_data_count];
        ++m_data_count;
    }
    else
    {
        // 聚合操作()
    }
}

/**
 * 获取一个data block中的数据
 */
void TagSetIterator::get_curr_tsm_file_data(const std::string & tsm_file_path, int64_t offset)
{
    auto file = m_tsm_io_manager->m_io_manager.get_file_stream(tsm_file_path, "binary-read");
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
//    std::cout << "curr_offset:" << file->tellg() << std::endl;

    // 读取并解压值数据
    std::string compressed_val(data_block.m_val_snappy_size, '\0');
    file->read(&compressed_val[0], data_block.m_val_snappy_size);
    std::string val_serialize;
    snappy::Uncompress(compressed_val.data(), data_block.m_val_snappy_size, &val_serialize);
    data_block.m_values.clear();
    data_block.m_values = TSM::deserialize_strings(val_serialize);
//    std::cout << "curr_offset:" << file->tellg() << std::endl;

    data_num = m_timestamps.size();
}
/**
 * 检查是否还有更多数据点
 * 如果内部缓冲区为空，尝试从数据源加载更多数据
 */


/**
 * 返回当干数据点，并移动到下一个数据点
 * 如果内部缓冲区用尽，从数据源加载更多数据
 */

/**
 * 从数据源加载数据到内部缓冲区
 */


FieldIterator::FieldIterator(const std::string & field)
{
    m_field = field;
}

/**
 * 对每个SortMergeIterator 执行next(),并根据逻辑合并结果
 */
DataPoint FieldIterator::next()
{

}

/**
 * 执行聚合操作
 */
DataPoint FieldIterator::aggregate()
{
    // 对 m_merges 中的数据进行聚合操作
}

//std::vector<RootIterator::SeriesIndexBlock> RootIterator::m_series_index_blocks = {};

RootIterator::RootIterator()
{
    m_tsm_io_manager = std::make_shared<TsmIOManagerCenter>();
}

/**
 * 遍历m_root, 调用每个RootIterator 中的next 或aggregate()
 * 用于获取数据 然后对数据进行过滤
 */
void RootIterator::next()
{

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

/**
 * 添加字段迭代器
 */
void RootIterator::add_field_iterator(const std::string & field)
{
    m_field_iterators.push_back(std::make_unique<FieldIterator>(field));
}
