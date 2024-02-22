#ifndef DTIMEDB_ITERATOR_H
#define DTIMEDB_ITERATOR_H

#include "data_point.h"

#include <vector>
#include <memory>

namespace dt::tsm
{
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
    class RootIterator
    {
    public:
        DataPoint next();
        DataPoint aggregate();



    public:
        std::vector<std::shared_ptr<SortMergeIterator>> m_merges;
    };

    class Iterator
    {
    public:
        void next();

    public:
        std::vector<std::shared_ptr<RootIterator>> m_root;
    };
}


#endif //DTIMEDB_ITERATOR_H
