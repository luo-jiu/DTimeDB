#include <cstring>
#include <climits>
#include "engine/circularList/include/type/field.h"
#include "exception/exception.h"
using namespace exception;
namespace ctl{
    //定义为浮点类型最小值
    static constexpr double DBL_LOWEST = std::numeric_limits<double>::lowest();
    //复制构造函数的实现
    Field::Field(const ctl::Field &value) {
        type_id_=value.type_id_;
        size_=value.size_;
        manage_data_=value.manage_data_;
        value_=value.value_;
        switch (type_id_) {
            case TypeId::VARCHAR:
                if (size_.len_==UINT_MAX){
                    value_.varlen_= nullptr;
                } else{
                    if (manage_data_){
                        value_.varlen_=new char [size_.len_];
                        memcpy(value_.varlen_,value.value_.varlen_,size_.len_);
                    } else{
                        value_=value.value_;
                    }
                }
                break;
            default:
                value_=value.value_;
        }
    }

    //Boolean
    Field::Field(ctl::TypeId type, int8_t i): Field(type){
        switch (type) {
            case TypeId::BOOLEAN:
                value_.boolean_=i;
                size_.len_=(value_.boolean_==SCHAR_MIN?UINT_MAX:0);
                break;
            case TypeId::INTEGER:
                value_.integer_=i;
                size_.len_=(value_.integer_==INT_MIN?UINT_MAX:0);
                break;
            default:
                throw Exception(ExceptionType::UNKOWN_TYPE,"Unknown type");
        }
    }
    //Double
    Field::Field(ctl::TypeId type, double i) : Field(type){
        switch (type) {
            case TypeId::DECIMAL:
                value_.decimal_=i;
                size_.len_=(value_.decimal_==DBL_LOWEST?UINT_MAX:0);
                break;
            default:
                throw Exception(ExceptionType::UNKOWN_TYPE,"Unknown type");
        }
    }
    //Integer
    Field::Field(ctl::TypeId type, int32_t i) : Field(type){
        switch (type) {
            case TypeId::BOOLEAN:
                value_.boolean_=i;
                size_.len_=(value_.boolean_==SCHAR_MIN?UINT_MAX:0);
                break;
            case TypeId::INTEGER:
                value_.integer_=i;
                size_.len_=(value_.integer_==INT_MIN?UINT_MAX:0);
                break;
            case TypeId::TIMESTAMP:
                value_.timestamp_=i;
                size_.len_=(value_.timestamp_==ULLONG_MAX?UINT_MAX:0);
            default:
                throw Exception(ExceptionType::UNKOWN_TYPE,"Unknown type");
        }
    }
    //TimeStamp
    Field::Field(ctl::TypeId type, uint64_t i) : Field(type) {
        switch (type) {
            case TypeId::BOOLEAN:
                value_.boolean_=i;
                size_.len_=(value_.boolean_==SCHAR_MIN?UINT_MAX:0);
                break;
            case TypeId::INTEGER:
                value_.integer_=i;
                size_.len_=(value_.integer_==INT_MIN?UINT_MAX:0);
                break;
            case TypeId::TIMESTAMP:
                value_.timestamp_=i;
                size_.len_=(value_.timestamp_==ULLONG_MAX?UINT_MAX:0);
            default:
                throw Exception(ExceptionType::UNKOWN_TYPE,"Unknown type");
        }
    }
    //varchar类型
    Field::Field(ctl::TypeId type, const char *data, uint32_t len, bool manage_data) {

    }
    Field::Field(ctl::TypeId type, const std::string &data) {

    }
    //delete
    Field::~Field() {
        switch (type_id_) {
            case VARCHAR:
                if (manage_data_){
                    delete[] value_.varlen_;
                }
                break;
            default:
                break;
        }
    }
}