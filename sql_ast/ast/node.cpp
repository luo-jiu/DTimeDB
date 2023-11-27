#include <ast/node.h>
using namespace dt::ast;

/**
 * 节点映射关系
 */
std::map<Node::Type, string> Node::m_names = {
        {NODE_INTEGER,              "Integer"},
        {NODE_INFIX,                "Infix"},
        {NODE_EXPRESSION_STATEMENT, "ExpressionStatement"},
        {NODE_PROGRAM,              "Program"},
        {NODE_PREFIX,               "Prefix"},
        {NODE_FLOAT,                "Float"},
        {NODE_STRING,               "String"},
        {NODE_NULL,                 "Null"},
        {NODE_IDENTIFIER,           "Identifier"},
//        {NODE_FROM,                 "From"},
//        {NODE_WHERE,                "Where"},
        {NODE_SELECT,               "Select"}
};

string Node::name() const
{
    auto it = m_names.find(m_type);
    if (it != m_names.end())
    {
        return it->second;
    }
    return "";
}