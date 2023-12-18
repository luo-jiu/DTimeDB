#include <evaluator/evaluator.h>
using namespace dt::evaluator;

std::shared_ptr<Object> Evaluator::eval_null(
        const std::shared_ptr<ast::Null> & node)
{
    return new_null();
}