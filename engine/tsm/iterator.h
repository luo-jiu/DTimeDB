#ifndef DTIMEDB_ITERATOR_H
#define DTIMEDB_ITERATOR_H

#include "data_point.h"

#include <vector>
#include <memory>

namespace dt::tsm
{

    class ShardIterator
    {

    };

    /**
     * 查询迭代器
     */
    class SortMergeIterator
    {
    public:
        bool has_next();
        DataPoint next();
        void load_more_data();

    public:


    };

    /**
     * 字段迭代器
     */
    class FieldIterator
    {
    public:
        DataPoint next();
        DataPoint aggregate();



    public:
        std::vector<std::shared_ptr<SortMergeIterator>> m_merges;
    };

    class RootIterator
    {
    public:
        void next();

    public:
        std::vector<std::shared_ptr<FieldIterator>> m_root;
    };
}


#endif //DTIMEDB_ITERATOR_H
