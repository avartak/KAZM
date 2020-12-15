#include <ExpOps.h>

namespace kazm {

    UnaryOp::UnaryOp(UnaryExpType o, std::size_t e):
        op(o),
        exp(e)
    {
    }

    bool UnaryOp::isUnary() {
        return true;
    }

    bool UnaryOp::isBinary() {
        return false;
    }

    BinaryOp::BinaryOp(BinaryExpType o, std::size_t l, std::size_t r):
        op(o),
        lhs(l),
        rhs(r)
    {
    }

    bool BinaryOp::isUnary() {
        return false;
    }

    bool BinaryOp::isBinary() {
        return true;
    }

    ConstOp::ConstOp(const std::string& v):
        val(v)
    {
    }

    bool ConstOp::isUnary() {
        return false;
    }

    bool ConstOp::isBinary() {
        return false;
    }

}
