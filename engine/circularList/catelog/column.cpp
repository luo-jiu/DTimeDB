#include <sstream>
#include "engine/circularList/include/catalog/column.h"
#include "engine/circularList/include/type/type.h"
namespace ctl{
    std::string Column::ToString() const {
        std::ostringstream os;
        os<<"Column["<<column_name_<<","<<Type::TypeIdToString(column_type_)<<","
        <<"Offset:"<<column_offset_<<",";
        return (os.str());
    }
}