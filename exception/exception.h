#include <stdexcept>
#include <iostream>
namespace exception{
    enum class ExceptionType{
        /**非法的异常类型*/
        INVALID=0,
        /** 找不到对应数据类型*/
        UNKOWN_TYPE=1,
        /** 方法接口未实现*/
        NOT_IMPLEMENTDE=2,
        /**值超出范围。*/
        OUT_OF_RANGE=3
    };
    class Exception :public std::runtime_error{
    public:
        ExceptionType GetType()const{return type_;}
        explicit Exception(const std::string &message): std::runtime_error(message),type_(ExceptionType::INVALID){
            std::string exception_message="Message"+message+"\n";
            std::cerr<<exception_message;
        }
        Exception(ExceptionType type,const std::string &message)
                : std::runtime_error(message),type_(type){
            std::string  exception_message=
                    "\nException Type:" +ExceptionTypeToString(type_)+"\nMessage: "+message+"\n";
        }
        static std::string ExceptionTypeToString(ExceptionType type){
            switch (type) {
                case ExceptionType::INVALID:
                    return "Invalid";
                case ExceptionType::UNKOWN_TYPE:
                    return "Unknown Type";
                case ExceptionType::NOT_IMPLEMENTDE:
                    return "Not implemented";
                case ExceptionType::OUT_OF_RANGE:
                    return "Out of Range";
                default:
                    return "Unknown";
            }
        }
    private:
        ExceptionType                   type_;
    };

}
