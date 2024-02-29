#ifndef DTIMEDB_INDEX_H
#define DTIMEDB_INDEX_H

#include "engine/tsm/tsi_ingredient.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <shared_mutex>
#include <set>
#include <iostream>

namespace dt::tsm
{
    /**
     * [序列化策略]
     * 1. 先构建SeriesBlock,先将map<tag_key, map<tag_value, list<series_key>>
     * 中的所有的series_key 持久化到SeriesKeyChunk
     *
     * [查询策略]
     * 1. 通过measurement 拿tag_block, 里面有双map, 然后通过tag_key 定位内部map,再利用tag_value 定位所有的由该tag组成过的series_key
     * 2. 多组tag可以分组查询聚合
     */
    class Index
    {
    public:
        void create_index(const std::string & measurement, std::list<std::string> & tags);
        bool whether_tag(const std::string & measurement, std::string & target);
        std::vector<std::string> search(const std::string & measurement, std::list<std::string> & tags);
        bool series_key_empty(std::string & series_key);

        std::set<std::string> get_series_key_by_tag(const std::string & measurement, const std::pair<std::string, std::string>& tag);
    private:
        mutable std::shared_mutex m_mutex;  // 读写锁保证线程安全

        /**
         * 主要作用是判断series_key 是否已经存在
         */
        struct SeriesBlock
        {
            /**
             * map<measurement, list<series_key>>
             * 注意这里结构是set 红黑树 + hash index,influxdb采用的是b+ Tree和hash index
             * 进行定位SeriesKey
             */
            std::set<SeriesKey> m_series_key_chunks;
        };

        struct TagBlock
        {
            /**
             * 碎片tag 寻找seriesKey
             * map<tag_key, map<tag_value, list<series_key>>
             * 需要排序,目的是可以进行范围查询
             */
            std::unordered_map<std::string, std::unordered_map<std::string, std::set<std::string>>> m_series_by_tag_key_value;
        };

        /**
         * 就是Measurement Block 的块
         * 作用是查询到对应measurement,可以找到对应的Tag Block
         * 需要排序,目的是可以实现范围查询
         */
        std::unordered_map<std::string, TagBlock>                   m_mea_tag_keys;

        /**
         * 记录该measurement 中有多少的纬度值
         * map<measurement, list<tag_key>>
         */
        std::unordered_map<std::string, std::set<std::string>>      m_mea_tags;
        SeriesBlock                                                 m_series_block;
    };
}

#endif //DTIMEDB_INDEX_H
