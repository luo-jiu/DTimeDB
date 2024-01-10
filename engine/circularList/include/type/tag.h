/**
 * tag标签类，是附加在tuple上的索引
 */
#include <string>

namespace ctl{
    class Tag{
        friend class Type;
    public:
        void AddTag();
    private:
        //std::unordered_map<std::string,std::string>         tags_;
    };

}