#pragma once

#include "token/token.h"

#include <list>
#include <memory>

namespace dt::ast
{
    /**
     * Ast 树节点定义
     */
    class Node
    {
    public:
        enum Type  // 节点类型
        {
            NODE_INTEGER = 0,
            NODE_FLOAT,
            NODE_STRING,
            NODE_NULL,
            NODE_IDENTIFIER,
            NODE_PREFIX,  // 前缀
            NODE_INFIX,   // 中缀
            NODE_EXPRESSION_STATEMENT,
            NODE_PROGRAM,

            NODE_SHOW,
            NODE_USE,
            NODE_CREATE,
            NODE_SELECT,  // 对应ScanNode
            NODE_INSERT,
            NODE_UPDATE,

            NODE_WHERE,  // 对应FilterNode

            NODE_SYS,
        };

        Node() {}
        Node(Type type): m_type(type) {}
        virtual ~Node() {}

        Type type() const { return m_type; }
        std::string name() const;

        virtual Json json() = 0;

    public:
        Type                                    m_type;
        token::Token                            m_token;
        static std::map<Type, std::string>      m_names;
    };

    /**
     * 表达式
     */
    class Expression : public Node
    {
    public:
        Expression(): Node() {}
        Expression(Type type): Node(type) {}
        ~Expression() {}
    };

    /**
     * 语句
     */
    class Statement : public Node
    {
    public:
        Statement(): Node() {}
        Statement(Type type): Node(type) {}
        ~Statement() {}
    };
}
