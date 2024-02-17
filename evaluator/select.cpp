#include "evaluator/evaluator.h"
using namespace dt::evaluator;
using namespace dt::execution;

std::shared_ptr<ExecutionPlanNode> Evaluator::eval_clt_select(
        const std::shared_ptr<ast::Select> & node,
        const std::shared_ptr<ExecutionPlanNode> & where,
        const std::shared_ptr<ExecutionPlanNode> & root)
{
    auto temp = root;

    // 处理为 root -> ScanNode
    std::shared_ptr<ScanNode> scan(new ScanNode);
    // 没有tag 和timestamp 就全表扫描
    // 有的话就走索引扫描
    // ...
    scan->m_scan_type = ScanNode::SCAN_FULL_TABLE;
    auto table_name = std::dynamic_pointer_cast<ast::String>(node->m_from);
    scan->m_table_name = table_name->m_value;
    temp->set_child(scan);
    temp = temp->get_child();

    // 开始制作过滤操作符
    // 处理为 root -> ScanNode -> FilterNode 过滤节点
    std::shared_ptr<FilterNode> filter(new FilterNode);
    if (!node->m_fields.empty())  // 配置field
    {
        if (node->m_fields.size() == 1)
        {
            auto it = node->m_fields.begin();
            std::advance(it, 0);
            filter->m_fields.emplace_back(*it);
        }
        else
        {
            // 遍历字段(主要是做语法检查)
            for (auto & field : node->m_fields)
            {
                if (field == "*")
                {
                    return nullptr;
                }
                filter->m_fields.emplace_back(field);
            }
        }
    }

    if (node->m_where)  // 配置where
    {

    }
    temp->set_child(filter);
    return root;
}

std::shared_ptr<ExecutionPlanNode> Evaluator::eval_tsm_select(
        const std::shared_ptr<ast::Select> & node,
        const std::shared_ptr<ExecutionPlanNode> & where,
        const std::shared_ptr<ExecutionPlanNode> & root)
{

}