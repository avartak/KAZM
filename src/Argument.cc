#include <Argument.h>

namespace kazm {

    Argument::Argument(const std::string& n):
        _name(n)
    {
    }

    Argument::Argument(const std::string& n, const std::shared_ptr<Data>& a):
        _name(n),
        _arg(a)
    {
    }

    std::string Argument::name() {
        if (_arg) return _arg->name();
        return _name;
    }

    DataType Argument::type() throw (Exception) {
        if (!_arg) throw Exception("<Internal error Argument::type()> Argument " + _name + " is null");
        return _arg->type();
    }

    std::size_t Argument::size() throw (Exception) {
        if (!_arg) throw Exception("<Internal error Argument::size()> Argument " + _name + " is null");
        return _arg->size();
    }

    std::size_t Argument::offset() throw (Exception) {
        if (!_arg) throw Exception("<Internal error Argument::offset()> Argument " + _name + " is null");
        return _arg->offset();
    }

    bool Argument::isReg() throw (Exception) {
        if (!_arg) throw Exception("<Internal error Argument::isReg()> Argument " + _name + " is null");
        return _arg->isReg();
    }
    
    std::shared_ptr<Data> Argument::arg() {
        return _arg;
    }

    void Argument::set(const std::shared_ptr<Data>& a) {
        _arg = a;
    }

    void Argument::reset() {
        _arg.reset();
    }

}
