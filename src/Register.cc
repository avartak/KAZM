#include <Register.h>

namespace kazm {

    Register::Register(DataType t, const std::string& n, std::size_t s, std::size_t o):
        _type(t),
        _name(n),
        _size(s),
        _offset(o)
    {
    }

    std::string Register::name() {
        return _name;
    }

    DataType Register::type() {
        return _type;
    }

    std::size_t Register::size() {
        return _size;
    }

    std::size_t Register::offset() {
        return _offset;
    }

    bool Register::isReg() {
        return true;
    }

}
