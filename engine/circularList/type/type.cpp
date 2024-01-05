#include "engine/circularList/include/type/type.h"
#include "exception/exception.h"

using namespace exception;
namespace ctl{
    //
    Type *Type::k_types[]={
            new Type(TypeId::INVALID)
    };

    uint64_t Type::GetTypeSize(ctl::TypeId type_id) {
        switch (type_id) {
            case BOOLEAN:
            case TINYINT:
                return 1;
            case INTEGER:
                return 4;
            case VARCHAR:
                return 0;
            case TIMESTAMP:
                return 8;
            default:
                break;
        }
        throw Exception(ExceptionType::UNKOWN_TYPE,"Unknown type");
    }
    std::string Type::TypeIdToString(ctl::TypeId type_id) {
        switch (type_id) {
            case INVALID:
                return "INVALID";
            case BOOLEAN:
                return "BOOLEAN";
            case INTEGER:
                return "INTEGER";
            case TINYINT:
                return "TINYINT";
            case VARCHAR:
                return "VARCHAR";
            case TIMESTAMP:
                return "TIMESTAMP";
            default:
                return "INVALID";
        }
    }

    Field Type::GetMinValue(TypeId type_id){
        switch (type_id) {
            case BOOLEAN:
                return Field(type_id, 0);
            case TINYINT:
                return Field(type_id, -127);
        }
    }

    Field Type::GetMaxValue(ctl::TypeId type_id) {

    }
    Type::Type() {}

}