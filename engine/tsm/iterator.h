#ifndef DTIMEDB_ITERATOR_H
#define DTIMEDB_ITERATOR_H

#include "proto/Shard.pb.h"
#include "data_point.h"
#include "tsm_ingredient.h"
#include "file_manager/file_io_manager.h"
#include "engine/impl/itsm.h"

#include <vector>
#include <memory>
#include <set>
#include <shared_mutex>
#include <queue>
#include <optional>

namespace dt::tsm
{
    enum class DataType {Integer, Float, String};

    struct SingleData
    {
        std::chrono::system_clock::time_point m_timestamp;
        std::string m_data;
        DataType m_type{DataType::String};  // 默认string
    };

    struct SeriesIndexBlock
    {
        std::string                         m_tsm_name;
        std::vector<IndexEntry>             m_index_entries;
        int64_t                             m_index_count{0};
        IndexBlockMeta                      m_meta;
    };

    struct RowNode
    {
        std::chrono::system_clock::time_point m_timestamp;
        std::map<std::string, SingleData> m_columns;  // 字段名映射到数据点

        // 添加数据点到行节点
        void add_data_point(const std::string & field_name, const SingleData & dp)
        {
            m_columns[field_name] = dp;
        }

        // 检查行节点是否包含指定的字段
        bool containsField(const std::string& field_name) const
        {
            return m_columns.find(field_name) != m_columns.end();
        }
    };

    /**
     * 需要依赖注入到TagSetIterator
     */
    class TsmIOManagerCenter
    {
    public:
        TsmIOManagerCenter(std::string  db_name, std::string  tb_name);

        // 加载tsm文件中的索引
        void load_tsm_index_entry(const Shard & shard);

        // 让每个TagSet存储对应的SeriesIndexBlock
        bool compare_index_block_meta(const SeriesIndexBlock & a, const SeriesIndexBlock & b);

    public:
        std::string m_db_name;
        std::string m_tb_name;

        mutable std::shared_mutex               m_mutex;       // 读写锁保证m_index_entry 安全
        dt::file::FileIOManager                 m_io_manager;
        //                 shard_id                  series_key + field_name  sort(series index block[通过series key 排序])
        std::unordered_map<std::string, std::unordered_map<std::string, std::vector<SeriesIndexBlock>>> m_tsm_index;
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
        std::optional<SingleData> next();

        bool has_next();

        // 获取当前tsm 文件中属于该series key的数据
        void get_curr_tsm_file_data(const std::string & tsm_file_name, int64_t offset);

    public:
        DataType                                                 m_type;  // 数据类型

        std::string                                              m_ploy;  // 聚合类型
        std::string                                              m_measurement;

        // 内存中的数据
        std::vector<std::chrono::system_clock::time_point>       m_timestamps;
        std::vector<std::string>                                 m_values;

        size_t                                                   m_current_data_index = 0;

        Shard                                                    m_shard;
        std::string                                              m_series_key;
        std::shared_ptr<TsmIOManagerCenter>                      m_tsm_io_manager;  // 依赖注入

        // 需要加载的全部索引
        std::vector<SeriesIndexBlock>                            m_series_blocks;
        int64_t                                                  m_current_index = 0;
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
        bool has_next();
        std::optional<SingleData> next();

    public:
        Shard                                               m_shard;
        // 需要存储有多少
        std::vector<std::shared_ptr<TagSetIterator>>        m_tag_set_iterators;
        size_t                                              m_current_tag_set_index = 0;
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

        using FieldMap = std::unordered_map<std::string, int>;
        using TimeSeriesData = std::map<int, FieldMap>;

        std::optional<SingleData> next();
        SingleData aggregate();

        bool has_next_in_current_shard();
        bool move_to_next_shard();

    public:
        std::string m_field;  // 字段名
        std::string m_type;  // 字段类型
        bool m_is_aggregate;  // 是否聚合
        bool m_data_empty{false};  // 该列是否为空

        // 需要排序shard,让shard有序
        std::vector<std::shared_ptr<ShardIterator>> m_shard_iterators;
        size_t m_current_shard_index = -1;  // 正在处理的FieldIterator 对应的字段名
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
        RootIterator(const std::string & db_name, const std::string & tb_name);

        RowNode next();
        bool evaluate_expr_node(const std::shared_ptr<dt::impl::ExprNode> & node, const DataPoint & data_point);
        bool compare_timestamps(const std::string & comparison_operator, const std::chrono::system_clock::time_point & row_timestamp, const std::string & condition_timestamp_str);
        bool evaluate_expr_node_for_row(const std::shared_ptr<dt::impl::ExprNode> & node, const RowNode & row_node);

        bool load_curr_shard_data();
        bool move_to_next_shard();
        bool has_next();

    public:
        std::vector<std::shared_ptr<FieldIterator>> m_field_iterators;
        std::map<std::chrono::system_clock::time_point, RowNode> m_row_nodes;
        bool m_shard_data_loaded = false; // 表示是否已加载当前shard的数据

        std::vector<DataPoint> m_data_points;  // 数据 集合
        std::set<std::string> m_load_tsm_finished;
        std::unordered_map<std::string, std::string> m_field_type;  // 字段类型

        struct SeriesIndexBlock
        {
            std::vector<IndexEntry>      m_index_entry;
            IndexBlockMeta               m_meta;
        };

        // 存储加载tsm_index_entry
        std::vector<SeriesIndexBlock> m_series_index_blocks;
        std::shared_ptr<impl::ExprNode> m_expr_node;  // 存储表达式树
        std::shared_ptr<TsmIOManagerCenter> m_tsm_io_manager;  // 文件管理器
    };
}


#endif //DTIMEDB_ITERATOR_H
