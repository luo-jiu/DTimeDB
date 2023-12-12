#include <engine/tsm/write.h>
using namespace dt::tsm;

void Write::write(
        high_resolution_clock::time_point timestamp,
        string & data,
        const string & file_path,
        DataBlock::Type type)
{
    std::shared_ptr<Field> _field(new Field());
    switch(type)
    {
        case DataBlock::DATA_STRING:
        {
            _field = get_field("string");
            break;
        }
        case DataBlock::DATA_INTEGER:
        {
            _field = get_field("integer");
            break;
        }
        case DataBlock::DATA_FLOAT:
        {
            _field = get_field("float");
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

}

std::shared_ptr<Field> Write::get_field(
        const string & type)
{
    auto it = m_files.find(type);
    if (it != m_files.end())  // 找到field
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
        _field = std::make_shared<Field>(DataBlock::Type::DATA_STRING);
    }
    else if (type == "integer")
    {
        _field = std::make_shared<Field>(DataBlock::Type::DATA_INTEGER);
    }
    else if (type == "float")
    {
        _field = std::make_shared<Field>(DataBlock::Type::DATA_FLOAT);
    }
    else
    {
        std::cerr << "Error : Unknown type " << type << std::endl;
        return nullptr;
    }
    m_files[type] = _field;
    return _field;
}