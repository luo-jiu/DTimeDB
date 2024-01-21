#include <execution_plan/node.h>
#include <execution_plan/integer.h>
#include <execution_plan/float.h>
#include <execution_plan/error.h>
#include <execution_plan/string.h>
#include <execution_plan/null.h>
#include <cstdarg>
using namespace dt::execution;

#include <filesystem>
namespace fs = std::filesystem;

string ExecutionPlanNode::m_current_db = "";

std::map<ExecutionPlanNode::Type, string> ExecutionPlanNode::m_names = {
        {OBJECT_USE,         "use"},
        {OBJECT_SHOW,        "show"},
        {OBJECT_ERROR,       "error"},
        {OBJECT_STRING,      "string"},
        {OBJECT_INTEGER,     "integer"},
        {OBJECT_FLOAT,       "float"},
        {OBJECT_NULL,        "null"},
};

string ExecutionPlanNode::name() const
{
    auto it = m_names.find(m_type);
    if (it != m_names.end())
    {
        return it->second;
    }
    return "";
}

std::shared_ptr<ExecutionPlanNode> ExecutionPlanNode::new_error(const char * format, ...)
{
    char buf[1024] = {0};
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(buf, sizeof(buf), format, arg_ptr);
    va_end(arg_ptr);

    std::shared_ptr<Error> obj = std::make_shared<Error>(buf);
    return obj;
}

std::shared_ptr<ExecutionPlanNode> ExecutionPlanNode::new_string(string & value)
{
    return std::make_shared<String>(value);
}

std::shared_ptr<ExecutionPlanNode> ExecutionPlanNode::new_integer(int64_t value)
{
    return std::make_shared<Integer>(value);
}

std::shared_ptr<ExecutionPlanNode> ExecutionPlanNode::new_float(float value)
{
    return std::make_shared<Float>(value);
}

std::shared_ptr<ExecutionPlanNode> ExecutionPlanNode::new_null()
{
    return std::make_shared<Null>();
}

/**
 * 这个命令用的少且较为特殊直接写在这里了
 */
void ExecutionPlanNode::show_database()
{
    for (const auto & entry : fs::directory_iterator("./../dbs"))
    {
        if (entry.is_directory())
        {
            std::cout << entry.path().filename().string() << std::endl;
        }
    }
}