#ifndef DTIMEDB_DATA_POINT_H
#define DTIMEDB_DATA_POINT_H

#include <string>
#include <unordered_map>
#include <variant>
#include <chrono>

namespace dt::tsm
{
    /**
     * 同一时间下同一测量值数据的集合
     */
    class DataPoint
    {
    public:
        using Timestamp = std::chrono::high_resolution_clock::time_point;
        using Value = std::variant<int, float, double, std::string>;
        //                                  field        data
        using ValueMap = std::unordered_map<std::string, Value>;

        DataPoint() {}

        Timestamp get_timestamp() const;
        Value get_value() const;

    public:
        Timestamp m_timestamp;
        ValueMap m_value_map;
        std::unordered_map<std::string, std::string> m_tags;
    };
}

#endif //DTIMEDB_DATA_POINT_H
