#include <executor/executor.h>
using namespace dt::executor;
using namespace dt::execution;

/**
 * 递归执行 执行计划
 * @param node
 */
void Executor::execute_plan(
        const std::shared_ptr<ExecutionPlanNode> & node) {
    if (node == nullptr) {
        return;  // 结束递归
    }

    // 特殊节点特殊处理 show
    if (node->name() == "show") {
        auto show_node = std::dynamic_pointer_cast<ShowNode>(node);
        if (show_node->m_type == "database") {
            node->show_database();
        }
        else  // table
        {
            // 遍历每个引擎，让他们打印出自己数据库对应的表
            for (auto & engine_it : m_engine_map) {
                node->execute(*engine_it.second);
            }
        }
    }
    else if (node->name() == "use")
    {
        // 遍历引擎
        for (auto & engine_it : m_engine_map) {
            node->execute(*engine_it.second);
        }
    }
    else
    {
        // 执行当前节点的操作(抉择是哪个引擎)
        auto m_tsm = m_engine_map["tsm"];  // 先写死tsm
        node->execute(*m_tsm);
    }

    // 递归执行子节点
    execute_plan(node->get_child());
}