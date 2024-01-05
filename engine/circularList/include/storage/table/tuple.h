#include <vector>
#include "engine/circularList/include/type/field.h"
#include "engine/circularList/include/catalog/schema.h"

namespace ctl{
    //一行数据
    class Tuple{
        friend class Table;
    public:
        Tuple()=default;
        //根据定义的表结构Schema构造行数据
        Tuple(std::vector<Field> values, const Schema *schema);
        Tuple(const Tuple &other);
    };
}

