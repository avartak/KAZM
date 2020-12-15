#ifndef TOKEN_H
#define TOKEN_H

#include <string>

namespace kazm {

    enum TokenType {
        T_HEADER = 258,
        T_ID,
        T_REAL,
        T_NNINTEGER,
        T_FILENAME,
        T_YIELDS,
        T_EQUALS,
        T_IF,
        T_PI,
        T_SIN,
        T_COS,
        T_TAN,
        T_EXP,
        T_LN,
        T_SQRT,
        T_QREG,
        T_CREG,
        T_GATE,
        T_OPAQUE,
        T_BARRIER,
        T_MEASURE,
        T_RESET,
        T_U,
        T_CX,
        T_INCLUDE,
        T_UNDEF
    };
    
    struct Token {
    
        int type;
        std::string value;
        int line;

        Token();

        Token(int, const std::string&, int);
    };

}

#endif
