#ifndef PARAMETER_H
#define PARAMETER_H

#include <memory>
#include <string>

#include <Expression.h>
#include <Constant.h>
#include <Exception.h>

namespace kazm {

    struct Parameter : public Expression {

        std::string name;
        std::shared_ptr<Expression> value;

        Parameter();
        Parameter(const std::string&);
        Parameter(const std::shared_ptr<Constant>&);
        Parameter(const std::string&, const std::shared_ptr<Constant>&);

        std::string str() override;
        double evaluate() throw (Exception) override;

    };

}

#endif
