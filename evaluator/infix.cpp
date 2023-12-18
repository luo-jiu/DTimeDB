#include <evaluator/evaluator.h>
using namespace dt::evaluator;

/**
 * 中缀表达式求值
 */
std::shared_ptr<Object> Evaluator::eval_infix(
        const string & op,
        const std::shared_ptr<Object> & left,
        const std::shared_ptr<Object> & right)
{
    switch (left->type())
    {
        case Object::OBJECT_STRING:  // 左边类型: string
        {
            if (right->type() == Object::OBJECT_STRING)  //
            {

            }
        }
    }
}