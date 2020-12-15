#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <memory>
#include <string>

#include <Exception.h>

namespace kazm {

    enum UnaryExpType {

        unaryop_nop,
        unaryop_negate,
        unaryop_sin,
        unaryop_cos,
        unaryop_tan,
        unaryop_exp,
        unaryop_ln,
        unaryop_sqrt

    };

    enum BinaryExpType {

        binaryop_add,
        binaryop_subtract,
        binaryop_multiply,
        binaryop_divide,
        binaryop_raise

    };

    struct Expression {

        virtual ~Expression() = default;

        virtual std::string str() = 0;
        virtual double evaluate() throw (Exception) = 0;

    };

    struct UnaryExpression : public Expression {

        UnaryExpType op;
        std::shared_ptr<Expression> ex;

        UnaryExpression(UnaryExpType, const std::shared_ptr<Expression>&);

        std::string str() override;
        double evaluate() throw (Exception) override;

    };

    struct BinaryExpression : public Expression {

        BinaryExpType op;
        std::shared_ptr<Expression> lhs;
        std::shared_ptr<Expression> rhs;

        BinaryExpression(BinaryExpType, const std::shared_ptr<Expression>&, const std::shared_ptr<Expression>&);

        std::string str() override;
        double evaluate() throw (Exception) override;

    };

}

#endif
