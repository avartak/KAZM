#include <Token.h>

namespace kazm {

    Token::Token(int t, const std::string& v, int l):
        type(t),
        value(v),
        line(l)
    {
    }

}
