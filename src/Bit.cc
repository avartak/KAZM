#include <Bit.h>

namespace kazm {

    Bit::Bit(const std::shared_ptr<Register>& r, std::size_t i):
        _reg(r),
        _index(i)
    {
    }

    std::string Bit::name() {
        return _reg->name();
    }

    DataType Bit::type() throw (Exception) {
        return _reg->type();
    }

    std::size_t Bit::size() throw (Exception) {
        return 1;
    }

    std::size_t Bit::offset() throw (Exception) {
        return _reg->offset() + _index;
    }

    bool Bit::isReg() throw (Exception) {
        return false;
    }

    std::size_t Bit::index() {
        return _index;
    }

    std::shared_ptr<Register> Bit::reg() {
        return _reg;
    }

}
