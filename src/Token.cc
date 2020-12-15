#include <Token.h>

namespace kazm {

    Token::Token():
        type(T_UNDEF),
        value(""),
        line(0)
    {
    }
    
    Token::Token(int t, const std::string& v, int l):
        type(t),
        value(v),
        line(l)
    {
    }

}
