#pragma once

#include <list>
#include <memory>

#include <token/token.h>
using namespace dt::token;

namespace dt
{
    namespace ast
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
                NODE_SELECT,
                NODE_INSERT,
            };

            Node() {}
            Node(Type type): m_type(type) {}
            virtual ~Node() {}

            Type type() const { return m_type; }
            string name() const;

            virtual Json json() = 0;

        public:
            Type                                m_type;
            Token                               m_token;
            static std::map<Type, string>       m_names;
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
}