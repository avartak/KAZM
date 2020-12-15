#include <cstdlib>
#include <cerrno>
#include <cmath>

#include <Constant.h>

namespace kazm {

    Constant::Constant(const std::string& v):
        value(v)
    {
    }

    double Constant::evaluate() throw (Exception) {
        if (value == "pi") {
            return atan2(1.0, 1.0) * 4.0;
        }
        else {
            double d = strtod(value.c_str(), nullptr);
            if (errno == ERANGE) throw Exception("<Internal error Constant::evaluate()> Unable to parse " + value + " as double precision floating point number");
            return d;
        }
    }

    std::string Constant::str() {
        return value;
    }
}
