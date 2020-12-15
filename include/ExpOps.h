#ifndef EXPOPS_H
#define EXPOPS_H

#include <string>

#include <Expression.h>

namespace kazm {

    struct ExpOps {

        virtual ~ExpOps() = default;        

        virtual bool isUnary() = 0;
        virtual bool isBinary() = 0;

    };

    struct UnaryOp : public ExpOps {

        UnaryExpType op;
        std::size_t exp;

        UnaryOp(UnaryExpType, std::size_t);

        bool isUnary() override;
        bool isBinary() override;

    };

    struct BinaryOp : public ExpOps {

        BinaryExpType op;
        std::size_t lhs;
        std::size_t rhs;

        BinaryOp(BinaryExpType, std::size_t, std::size_t);

        bool isUnary() override;
        bool isBinary() override;

    };

    struct ConstOp : public ExpOps {

        std::string val;

        ConstOp(const std::string&);

        bool isUnary() override;
        bool isBinary() override;

    };

}

#endif
