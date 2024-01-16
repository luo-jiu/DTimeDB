
/**
 * 处理不同数据类型
 */
#pragma once
#include <cstdint>
#include <string>
#include "type_id.h"
#ifndef TYPE_H
#define TYPE_H
/**
 *Type是所有数据类型的抽象类
 */
namespace ctl {
    class Field;

    class Type {
    public:
        explicit Type(TypeId type_id) : type_id_(type_id) {}


        //type会被派生类调用，所以设计虚拟析构函数直接点用派生类的析构函数删除
        virtual ~Type() = default;

        //获取数据类型的大小
        static uint64_t GetTypeSize(TypeId type_id);

        //转换为字符串
        static std::string TypeIdToString(TypeId type_id);

        // 获取特定类型的最小值
        static Field GetMinValue(TypeId type_id);

        // 获取特定类型的最大值
        static Field GetMaxValue(TypeId type_id);

        // 获取类型标识
        inline TypeId GetTypeId() const { return type_id_; }

        // 获取类型的实例
        inline static Type *GetInstance(TypeId type_id) { return k_types[type_id]; }

        //虚函数，获取边长字符串的长度
        virtual uint32_t GetLength(const Field &value) const;

        //获取变长字符串的值
        virtual char *GetData(const Field &value) const;

        //转换为字符串
        virtual std::string ToString(const Field &val) const;

    protected:
        //实际的类型标识
        TypeId type_id_;
        //由所有实例对象共享的指针数组
        static Type *k_types[14];
    };
}
#endif // TYPE_H