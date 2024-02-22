#include "iterator.h"
using namespace dt::tsm;

/**
 * 检查是否还有更多数据点
 * 如果内部缓冲区为空，尝试从数据源加载更多数据
 */
bool SortMergeIterator::has_next()
{

}

/**
 * 返回当干数据点，并移动到下一个数据点
 * 如果内部缓冲区用尽，从数据源加载更多数据
 * @return
 */
DataPoint SortMergeIterator::next()
{

}

/**
 * 从数据源加载数据到内部缓冲区
 */
void SortMergeIterator::load_more_data()
{
    
}

/**
 * 对每个SortMergeIterator 执行next(),并根据逻辑合并结果
 */
DataPoint RootIterator::next()
{

}

/**
 * 执行聚合操作
 */
DataPoint RootIterator::aggregate()
{
    // 对 m_merges 中的数据进行聚合操作
}

/**
 * 遍历m_root, 调用每个RootIterator 中的next 或aggregate()
 */
void Iterator::next()
{

}