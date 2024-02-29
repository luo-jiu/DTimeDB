#include "evaluator/evaluator.h"
using namespace dt::evaluator;
using namespace dt::execution;

std::shared_ptr<ExecutionPlanNode> eval_clt_select(
        const std::shared_ptr<dt::ast::Select> & node,
        const std::shared_ptr<ExprNode> & where,
        const std::shared_ptr<ExecutionPlanNode> & root)
{
    auto temp = root;

    // 处理为 root -> ScanNode
    std::shared_ptr<ScanNode> scan(new ScanNode);
    // 没有tag 和timestamp 就全表扫描
    // 有的话就走索引扫描
    // ...
    scan->m_scan_type = ScanNode::SCAN_FULL_TABLE;
    auto table_name = std::dynamic_pointer_cast<dt::ast::String>(node->m_from);
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
    // 配置where
    filter->m_where = where;

    temp->set_child(filter);
    return root;
}

std::shared_ptr<ExecutionPlanNode> eval_tsm_select(
        const std::shared_ptr<dt::ast::Select> & node,
        const std::shared_ptr<ExprNode> & where)
{
    std::shared_ptr<QueryTSMNode> tsm_node(new QueryTSMNode);
    tsm_node->m_expr_tree = where;
//    for (const auto & field : node->m_fields)
//    {
//        tsm_node->m_fields.push_back(field);
//    }
    tsm_node->m_reduced_fields = node->m_reduced_fields;
    tsm_node->m_measurement = std::dynamic_pointer_cast<dt::ast::String>(node->m_from)->m_value;
    tsm_node->m_db_name = node->m_curr_db_name;
    return tsm_node;
}

std::shared_ptr<ExecutionPlanNode> Evaluator::eval_select(
        const std::shared_ptr<ast::Select> & select,
        const std::shared_ptr<ExecutionPlanNode> & root)
{
    // 先将类型转换为中缀表达式
    auto e = std::dynamic_pointer_cast<ast::Infix>(select->m_where);
    // 将where 的ast 树 转换为表达式树
    auto where = convert_to_new_tree(e);
    if (!select->m_curr_db_name.empty())
    {
        m_tab_eng_manager.load_database_metadata(select->m_curr_db_name);
    }
    std::string db_name = m_curr_db_name;
    if (m_curr_db_name.empty())
    {
        if (select->m_curr_db_name.empty())
        {
            return nullptr;
        }
        else
        {
            db_name = select->m_curr_db_name;
        }
    }
    if (m_tab_eng_manager.table_engine_is(db_name, std::dynamic_pointer_cast<ast::String>(select->m_from)->m_value, "tsm"))
    {
        auto child = eval_tsm_select(select, where);
        root->set_child(child);
        return child;
    }
    else if (m_tab_eng_manager.table_engine_is(db_name, std::dynamic_pointer_cast<ast::String>(select->m_from)->m_value, "clt"))
    {
        return eval_clt_select(select, where, root);
    }
    return nullptr;
}