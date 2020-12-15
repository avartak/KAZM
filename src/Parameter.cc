#include <Parameter.h>

namespace kazm {

    Parameter::Parameter():
        name(""),
        value(std::shared_ptr<Expression>())
    {
    }

    Parameter::Parameter(const std::string& n):
        name(n),
        value(std::shared_ptr<Expression>())
    {
    }

    Parameter::Parameter(const std::shared_ptr<Constant>& c):
        name(""),
        value(c)
    {
    }

    Parameter::Parameter(const std::string& n, const std::shared_ptr<Constant>& c):
        name(n),  
        value(c)
    {
    }

    double Parameter::evaluate() throw (Exception) {
        if (!value) throw Exception("<Internal error Parameter::evaluate()> Parameter value not defined");
        return value->evaluate();
    }

    std::string Parameter::str() {
        if (!value) return name;
        return value->str();
    }

}
