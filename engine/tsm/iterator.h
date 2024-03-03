#ifndef DTIMEDB_ITERATOR_H
#define DTIMEDB_ITERATOR_H

#include "data_point.h"
#include "tsm_ingredient.h"
#include "file_manager/file_io_manager.h"

#include <vector>
#include <memory>
#include <set>
#include <shared_mutex>

namespace dt::tsm
{
    /**
     * 需要依赖注入到TagSetIterator
     */
    class TsmIOManagerCenter
    {
    public:
        TsmIOManagerCenter() : m_io_manager(8){}

        // 加载tsm文件中的索引
        void load_tsm_index_entry(const std::string & tsm_path);

    public:
        mutable std::shared_mutex                                   m_mutex;                 // 读写锁保证m_index_entry 安全
        dt::file::FileIOManager                                     m_io_manager;
        // tsm文件索引
        //                 field                           shard_id
        std::unordered_map<std::string, std::unordered_map<std::string, std::vector<dt::tsm::IndexEntry>>> m_index_entry;
    };


    /**
     * 查询迭代器
     *
     * 底层迭代器体系负责单个shard 上满足条件的某一列值(shard)等查找或者单机聚合
     * eg: where tag1 = value1
     * 查询包含该tag 的所有series,再为每个series 构建一个TagSetIterator 去查找对应
     * 列值,TagSetIterator 会将查找指针至于最小的列值处
     *
     * [利用倒排索引定位到所有满足条件的series key,求交集后,再为每一个series key构建一个
     *      TagSetIterator 用于查找列值数据(field data),TagSetUterator是正规体系中唯
     *      一一个干活的Iterator]
     */
    class TagSetIterator
    {
    public:



    public:
        std::string m_series_key;
        std::shared_ptr<TsmIOManagerCenter> m_tsm_io_manager;
    };

    /**
     * Shard 迭代器
     * 因为查询到列值可能分布在不同的Shard 上, 需要根据TimeRange 决定给定时间段
     * 在哪些shard 上，并为每一个shard 构建Iterator, 雇佣这个逻辑Iterator 负责
     * 查询这个shard 上对应的列值
     *
     * [为每个shard 构建ShardIterator, 让ShardIterator 查询该shard 上的对应列值(field)的数据]
     */
    class ShardIterator
    {
    public:

    public:
        std::string m_shard_id;
        std::vector<std::shared_ptr<TagSetIterator>> m_tag_set_iterators;
    };

    /**
     * 字段迭代器
     *
     * 可以根据是否需要聚合分为VarRefIterator 和CallIterator,前者表示列值可以直接
     * 返回，不许要聚合,后者表示查询列需要执行某些聚合操作
     * CallIterator 实现需要两步,首先通过VarRefIterator 把对应列值查询到，再通过对
     * 应的Reduce 函数进行相应的聚合
     *
     * [负责整个字段(field)的全数据查询(VarRefIterator),需要聚合再Reduce(CallIterator)]
     */
    class FieldIterator
    {
    public:
        explicit FieldIterator(const std::string & field);

        DataPoint next();
        DataPoint aggregate();

    public:
        std::string m_field;  // 字段名
        std::vector<std::shared_ptr<ShardIterator>> m_shard_iterators;
    };


    /**
     * Root 迭代器
     * 会为所有查询的field 构造一个FieldIterator, FieldIterator 表示每个查询
     * 列都会创建一个Iterator(ExprIterator),基于列分别构建Iterator 方便执行查
     * 询聚合操作,比如sum(field1)等等
     */
    class RootIterator
    {
    public:
        RootIterator();

        void next();
        void load_tsm_index_entry(const std::string & tsm_path);
        void add_field_iterator(const std::string & field);

    public:
        std::vector<std::shared_ptr<FieldIterator>> m_field_iterators;
        std::vector<DataPoint> m_data_points;  // 数据 集合
        std::set<std::string> m_load_tsm_finished;

        std::shared_ptr<TsmIOManagerCenter> m_tsm_io_manager;  // 文件管理器
    };
}


#endif //DTIMEDB_ITERATOR_H
