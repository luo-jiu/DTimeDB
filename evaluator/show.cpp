#include "evaluator/evaluator.h"
using namespace dt::evaluator;
using namespace dt::execution;

std::shared_ptr<ExecutionPlanNode> Evaluator::eval_show(
        const std::shared_ptr<ast::Show> & node)
{
    std::shared_ptr<ShowNode> i(new ShowNode());
    i->m_type = node->m_type;

    // 这是查看其他信息的
    if (!node->m_curr_db_name.empty())
    {
        // 加载数据库
        m_tab_eng_manager.load_database_metadata(node->m_curr_db_name);
        if (!node->m_tb_name.empty())
        {
            auto method =  m_tab_eng_manager.m_option_map.find(node->m_show_option);
            if (method != m_tab_eng_manager.m_option_map.end())
            {
                (m_tab_eng_manager.*(method->second))(node->m_curr_db_name, node->m_tb_name);  // 利用函数指针回调函数
            }
            return nullptr;
        }
    }
    else if (!node->m_tb_name.empty())  // 说明是 show [table_name] [option]
    {
        if (!m_curr_db_name.empty())
        {
            auto method =  m_tab_eng_manager.m_option_map.find(node->m_show_option);
            if (method != m_tab_eng_manager.m_option_map.end())
            {
               (m_tab_eng_manager.*(method->second))(m_curr_db_name, node->m_tb_name);  // 利用函数指针回调函数
            }
        }
        else
        {
            std::cout << "Database not selected, 'use' command" << std::endl;
        }
        return nullptr;
    }
    return i;
}