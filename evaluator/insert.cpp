#include "evaluator/evaluator.h"
using namespace dt::evaluator;
using namespace dt::execution;
using std::string;

std::shared_ptr<ExecutionPlanNode> Evaluator::eval_insert(
        const std::shared_ptr<ast::Insert> & node)
{
    std::shared_ptr<InsertNode> i(new InsertNode());
    i->set_table(node->m_from);

    for (string tag : node->m_tags)
    {
        i->add_tag(tag);
    }
    while (!node->m_fields.empty())
    {
        string f = node->m_fields.front();
        node->m_fields.pop_front();
        string v = node->m_values.front();
        node->m_values.pop_front();

        string fv = f + "=" + v;
        i->add_field(fv);
    }
    i->set_timestamp(node->m_timestamp);

    return i;
}