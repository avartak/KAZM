#ifndef CONSTANT_H
#define CONSTANT_H

#include <string>

#include <Expression.h>
#include <Exception.h>

namespace kazm {

    struct Constant : public Expression {

        std::string value;

        Constant(const std::string&);

        std::string str() override;
        double evaluate() throw (Exception) override;

    };

}

#endif
