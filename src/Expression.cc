#include <cmath>

#include <Expression.h>

namespace kazm {

    std::map<std::string, UnaryExpType> UnaryExpression::type = { 
        {"+"   , unaryop_nop},
        {"-"   , unaryop_negate},
        {"sin" , unaryop_sin},
        {"cos" , unaryop_cos},
        {"tan" , unaryop_tan},
        {"exp" , unaryop_exp},
        {"ln"  , unaryop_ln},
        {"sqrt", unaryop_sqrt}
    };

    UnaryExpression::UnaryExpression(UnaryExpType o, const std::shared_ptr<Expression>& e):
        op(o),
        ex(e)
    {
    }

    double UnaryExpression::evaluate() throw (Exception) {
        if (op == unaryop_negate) return -ex->evaluate();
        else if (op == unaryop_sin) return sin(ex->evaluate());
        else if (op == unaryop_cos) return cos(ex->evaluate());
        else if (op == unaryop_tan) return tan(ex->evaluate());
        else if (op == unaryop_exp) return exp(ex->evaluate());
        else if (op == unaryop_ln) return log(ex->evaluate());
        else if (op == unaryop_sqrt) return sqrt(ex->evaluate());
        else return ex->evaluate();
    }

    std::string UnaryExpression::str() {
        if (op == unaryop_negate) return "-" + ex->str();
        else if (op == unaryop_sin)  return "sin("  + ex->str() + ")";
        else if (op == unaryop_cos)  return "cos("  + ex->str() + ")";
        else if (op == unaryop_tan)  return "tan("  + ex->str() + ")";
        else if (op == unaryop_exp)  return "exp("  + ex->str() + ")";
        else if (op == unaryop_ln)   return "log("  + ex->str() + ")";
        else if (op == unaryop_sqrt) return "sqrt(" + ex->str() + ")";
        else return "(" + ex->str() + ")";
    }

    UnaryExpType UnaryExpression::GetType(const std::string& s) {

        return type[s];

    }

    std::map<std::string, BinaryExpType> BinaryExpression::type = { 
        {"+", binaryop_add},
        {"-", binaryop_subtract},
        {"*", binaryop_multiply},
        {"/", binaryop_divide},
        {"^", binaryop_raise}
    };
    std::map<std::string, std::size_t> BinaryExpression::precedence = {
        {"+", 100},
        {"-", 100},
        {"*", 200},
        {"/", 200},
        {"^", 400}
    };

    BinaryExpression::BinaryExpression(BinaryExpType o, const std::shared_ptr<Expression>& l, const std::shared_ptr<Expression>& r):
        op(o),
        lhs(l),
        rhs(r)
    {
    }

    double BinaryExpression::evaluate() throw (Exception) {
        if (op == binaryop_add) return lhs->evaluate() + rhs->evaluate();
        else if (op == binaryop_subtract) return lhs->evaluate() - rhs->evaluate();
        else if (op == binaryop_multiply) return lhs->evaluate() * rhs->evaluate();
        else if (op == binaryop_divide) return lhs->evaluate() / rhs->evaluate();
        else return pow(lhs->evaluate(), rhs->evaluate());
    }

    std::string BinaryExpression::str() {
        if (op == binaryop_add) return lhs->str() + " + " + rhs->str();
        else if (op == binaryop_subtract) return lhs->str() + " - " + rhs->str();
        else if (op == binaryop_multiply) return lhs->str() + " * " + rhs->str();
        else if (op == binaryop_divide) return lhs->str() + " / " + rhs->str();
        else return lhs->str() + " ^ " + rhs->str();
    }

    BinaryExpType BinaryExpression::GetType(const std::string& s) {

        return type[s];

    }

    std::size_t BinaryExpression::GetPrecedence(const std::string& s) {

        return precedence[s];

    }

}
