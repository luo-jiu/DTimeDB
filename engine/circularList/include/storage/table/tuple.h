#include <vector>
#include "engine/circularList/include/type/field.h"
#include "engine/circularList/include/catalog/schema.h"
/**
 * 一行数据存储在表页内的格式
 */
namespace ctl{
    //一行数据
    class Tuple{
        friend class Table;
    public:
        Tuple()=default;
        //根据定义的表结构Schema构造行数据
        Tuple(std::vector<Field> values, const Schema *schema);
        Tuple(const Tuple &other);
    private:

    };
}

