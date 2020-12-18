#include <sstream>

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

    DataType Register::type() throw (Exception) {
        return _type;
    }

    std::size_t Register::size() throw (Exception) {
        return _size;
    }

    std::size_t Register::offset() throw (Exception) {
        return _offset;
    }

    bool Register::isReg() throw (Exception) {
        return true;
    }

    std::string Register::str() {
        std::stringstream ss;
        ss << (isClassical() ? "Classical register " : "Quantum register "); 
        ss << name() << "(size : " << size() << ", offset : " << offset() << ")" << std::endl;
        return ss.str();
    }

}
