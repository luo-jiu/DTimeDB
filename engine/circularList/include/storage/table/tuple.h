//
// Created by illumwang on 23-12-24.
//

#ifndef DTIMEDB_TUPLE_H
#define DTIMEDB_TUPLE_H

#include <vector>
#include "engine/circularList/include/type/value.h"
#include "engine/circularList/include/catalog/schema.h"

namespace ctl{
    //一行数据
    class Tuple{
        friend class TablePage;
    public:
        Tuple()=default;
        //根据定义的表结构Schema构造行数据
        Tuple(std::vector<Value> values,const Schema *schema);
        Tuple(const Tuple &other);
    };
}
#endif //DTIMEDB_TUPLE_H
