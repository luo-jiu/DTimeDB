#include <engine/tsm/write.h>
using namespace dt::tsm;

void Write::write(
        high_resolution_clock::time_point timestamp,
        string & data,
        const string & file_path,
        DataBlock::Type type,
        string & field_name)
{
    std::shared_ptr<Field> _field(new Field());
    switch(type)
    {
        case DataBlock::DATA_STRING:
        {
            _field = get_field(field_name, "string");
            break;
        }
        case DataBlock::DATA_INTEGER:
        {
            _field = get_field(field_name, "integer");
            break;
        }
        case DataBlock::DATA_FLOAT:
        {
            _field = get_field(field_name, "float");
            break;
        }
        default:
        {
            std::cerr << "Error : Unknown type " << type << std::endl;
        }
    }
    _field->write(timestamp, data);
}

/**
 * 对数据进行刷盘
 */
void Write::flush_disk()
{
    std::cout << "Writing" << std::endl;
    /**
     * 判断有没有满足要求需要刷盘的数据
     * 1. data队列里面有块就可以刷写了
     */
     while (true)
     {
         if (fields_empty())  // 如果有字段
         {
             for(auto it = m_fields.begin(); it != m_fields.end(); ++it)  // 遍历每个字段
             {
                 // 先判断meta 数量是否达标
                 int _entry_size = it->second->get_index_deque_size();
                 if (_entry_size >= 10)
                 {
                     /**
                      * 生成meta 刷盘
                      * 然后entry 刷写到磁盘(拿取的时候就清空了)
                      */
                     std::shared_ptr<IndexBlockMeta> _meta(new IndexBlockMeta());
                     if (it->second->m_type == DataBlock::DATA_STRING)
                     {
                         _meta = m_tsm.create_index_meta(IndexBlockMeta::Type::DATA_STRING, m_measurement, it->second->m_field_name);
                     }
                     else if (it->second->m_type == DataBlock::DATA_INTEGER)
                     {
                         _meta = m_tsm.create_index_meta(IndexBlockMeta::Type::DATA_INTEGER, m_measurement, it->second->m_field_name);
                     }
                     else if (it->second->m_type == DataBlock::Type::DATA_FLOAT)
                     {
                         _meta = m_tsm.create_index_meta(IndexBlockMeta::Type::DATA_FLOAT, m_measurement, it->second->m_field_name);
                     }
                     _meta->set_count(10);

                     int _index_size = (12 + m_measurement.length() + it->second->m_field_name.length()) + _entry_size * 28;
                     m_tail_offset -= _index_size;  // 前移到指定位置开始写入

                     while (it->second->get_index_status())  // 如果有 entry
                     {
                         auto _entry = it->second->pop_index_from_deque();
                         m_tsm.write_index_entry_to_file(_entry, "data.tsm", m_tail_offset);
                         m_tail_offset += 28;  // 后移指针
                     }
                     m_tsm.write_index_meta_to_file(_meta, "data.tsm", m_tail_offset);  // 写入meta

                     m_tail_offset -= _index_size;  // 写入后指针发生变化还需要重新移位
                 }

                 while (it->second->get_data_status())  // 有data block，准备刷入磁盘
                 {
                     auto data_block = it->second->pop_data_from_deque();  // 获取块

                     /**
                      * 判断该TSM 是否还能写入
                      * 余量(head - tail)[已知] -
                      *    index_block(entry_size * entry_num[数量已知&动态不固定] + meta)[已知] >
                      *    data_block.size[已知]
                      */
                     if (m_tail_offset - m_head_offset - _entry_size * 28 + m_measurement.length() + it->second->m_field_name.length() > data_block->m_size)
                     {
                         /**
                          * 余量足够先生成entry
                          * 然后将data_block 刷写到磁盘
                          */

                         // 创建entry
                         auto _entry = m_tsm.create_index_entry(data_block->m_max_timestamp, data_block->m_min_timestamp, m_head_offset, data_block->m_size);
                         it->second->push_index_to_deque(_entry);  // 存入队列(数量肯定不达标的，达标早写入了)

                         // 刷盘
                         m_tsm.write_data_to_file(data_block, "data.tsm", m_head_offset);

                         // 修改头偏移量
                         m_head_offset += data_block->m_size;
                     }
                     else
                     {
                         /**
                          * 余量不足先把之前的meta, entry写入
                          * 然后开启一个新的TSM File
                          */


                     }
                 }
             }
         }
     }
}

std::shared_ptr<Field> Write::get_field(
        string & field_name,
        const string & type)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_fields.find(field_name);
    if (it != m_fields.end())  // 找到field
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
    m_fields[field_name] = _field;
    return _field;
}

bool Write::fields_empty()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_fields.empty();
}