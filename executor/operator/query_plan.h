#ifndef DTIMEDB_QUERY_PLAN_H
#define DTIMEDB_QUERY_PLAN_H

#include <iostream>
#include <utility>
#include <vector>
#include <functional>
#include <memory>
#include <string>
using std::string;

/**
 * ScanOperator      扫描操作符: 从数据源(比如表格) 读取数据，是查询计划的起点
 * SelectOperator    选择操作符: 基于给定的条件(谓词) 过滤数据，它检查每行数据
 *      只有满足条件的数据才会被传递到下一个操作符
 * ProjectOperator   投影操作符: 选择特定的列返回，它根据查询的需要从每行数据中
 *      抽取指定的字段
 * JoinOperator      连接操作符: 将两个数据源基于某些条件结合在一起(不用做)
 * GroupByOperator   分组操作符: 根据某个或某些字段将数据进行分组，通常用于聚合
 *      操作(不用做)
 * AggregateOperator 聚合操作符: 对一组值执行计算并返回单个值，如求和，平均，最
 *      大，最小值等
 * SortOperator      排序操作符: 更具字段进行排序
 * LimitOperator     限制操作符: 限制返回发数据数量，常见于分页查询
 */
namespace dt::evaluator
{
    class Row
    {
    public:
        int         m_id;
        string      m_name;
        int         m_age;
    };

    /**
     * 简化的操作符接口
     */
    class Operator
    {
    public:
        virtual ~Operator() {}
        virtual bool get_next(Row & row) = 0;
    };

    /**
     * 投影操作符
     */
    class ProjectOperator : public Operator
    {
    public:
        ProjectOperator(Operator* input, std::function<void(const Row&)> projector)
            : input(input), projector(std::move(projector)){}

        bool get_next(Row & row) override
        {
            if (input->get_next(row))
            {
                projector(row);  // 应用投影
                return true;
            }
            return false;
        }

    private:
        Operator* input;
        std::function<void(const Row&)> projector;

    };

    /**
     * 选择操作符
     */
    class SelectOperator : public Operator
    {
    public:
        SelectOperator(Operator * input, std::function<bool(const Row &)> predicate)
            : input(input), predicate(predicate) {}

        bool get_next(Row & row) override
        {
            while (input->get_next(row))  // 当数据没有返回 false 退出循环
            {
                if (predicate(row))
                {
                    return true;
                }
            }
            return false;
        }

    private:
        Operator * input;
        std::function<bool(const Row &)> predicate;
    };

    /**
     * 扫描操作符
     */
    class ScanOperator : public Operator
    {
    public:
        ScanOperator(std::list<string>  data) : m_data(std::move(data)), m_current_index(0) {}

        bool get_next(Row & row) override
        {
//            return m_engine->get_next_data(m_data,);

//            if (m_current_index < m_data.size())
//            {
//                row = m_data[m_current_index++];
//                return true;
//            }
//            return false;
        }

    private:
        std::shared_ptr<IEngine>        m_engine;  // 引擎操作接口
        std::list<string>               m_data;

//        std::vector<Row>    m_data;
        size_t              m_current_index;
    };

    /**
     * 混合查询
     */
    class IndexedScanOperator : public Operator
    {
    public:
        // 使用 begin_indexed_scan() 和 get_next_row() 实现索引扫描

    private:
        std::shared_ptr<IEngine> m_engine;
        high_resolution_clock::time_point m_index;  // 时间索引

    };
}

#endif //DTIMEDB_QUERY_PLAN_H
