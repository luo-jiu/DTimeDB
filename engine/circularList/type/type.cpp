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
            case VARCHAR:
                return "VARCHAR";
            case TIMESTAMP:
                return "TIMESTAMP";
            default:
                return "INVALID";
        }
    }

}

