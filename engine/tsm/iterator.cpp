#include "iterator.h"

#include <utility>
using namespace dt::tsm;

/**
 * 检查是否还有更多数据点
 * 如果内部缓冲区为空，尝试从数据源加载更多数据
 */


/**
 * 返回当干数据点，并移动到下一个数据点
 * 如果内部缓冲区用尽，从数据源加载更多数据
 * @return
 */

/**
 * 从数据源加载数据到内部缓冲区
 */


FieldIterator::FieldIterator(const std::string & field)
{
    m_field = field;
}

/**
 * 对每个SortMergeIterator 执行next(),并根据逻辑合并结果
 */
DataPoint FieldIterator::next()
{

}

/**
 * 执行聚合操作
 */
DataPoint FieldIterator::aggregate()
{
    // 对 m_merges 中的数据进行聚合操作
}

/**
 * 遍历m_root, 调用每个RootIterator 中的next 或aggregate()
 */
void RootIterator::next()
{

}

/**
 * 添加字段迭代器
 */
void RootIterator::add_field_iterator(const std::string & field)
{
    m_field_iterators.push_back(std::make_unique<FieldIterator>(field));
}
