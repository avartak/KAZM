%{

#include <iostream>

#define RETURN(x)    return Token(x, str(), lineno())

%}

%top{
#include <Token.h>
}

D   [0-9]
I   ("0"|[1-9]+{D}*)
E   ([Ee][+-]?{D}+)
S   [ \t]
W   [ \t\v\n\f\r]

%%

{W}+
"//".*

"OPENQASM"{S}+{I}"."{I}{S}*";"      { RETURN(T_HEADER); }

"{"                                 { RETURN('{'); }
"}"                                 { RETURN('}'); }
"["                                 { RETURN('['); }
"]"                                 { RETURN(']'); }
"("                                 { RETURN('('); }
")"                                 { RETURN(')'); }
","                                 { RETURN(','); }
";"                                 { RETURN(';'); }
"+"                                 { RETURN('+'); }
"-"                                 { RETURN('-'); }
"*"                                 { RETURN('*'); }
"/"                                 { RETURN('/'); }
"^"                                 { RETURN('^'); }
"->"                                { RETURN(T_YIELDS); }
"=="                                { RETURN(T_EQUALS); }
"if"                                { RETURN(T_IF); }
"pi"                                { RETURN(T_PI); }
"sin"                               { RETURN(T_SIN); }
"cos"                               { RETURN(T_COS); }
"tan"                               { RETURN(T_TAN); }
"exp"                               { RETURN(T_EXP); }
"ln"                                { RETURN(T_LN); }
"sqrt"                              { RETURN(T_SQRT); }
"qreg"                              { RETURN(T_QREG); }
"creg"                              { RETURN(T_CREG); }
"gate"                              { RETURN(T_GATE); }
"opaque"                            { RETURN(T_OPAQUE); }
"barrier"                           { RETURN(T_BARRIER); }
"measure"                           { RETURN(T_MEASURE); }
"reset"                             { RETURN(T_RESET); }
"U"                                 { RETURN(T_U); }
"CX"                                { RETURN(T_CX); }
"include"                           { RETURN(T_INCLUDE); }

{I}                                 { RETURN(T_NNINTEGER); }
{D}+{E}                             { RETURN(T_REAL); }
{D}+"."{D}*{E}?                     { RETURN(T_REAL); }
{D}*"."{D}+{E}?                     { RETURN(T_REAL); }
[a-z][a-zA-Z_0-9]*                  { RETURN(T_ID); }
\"[^\"]+\"                          { RETURN(T_FILENAME); }

.                                   { RETURN(T_UNDEF); }
<<EOF>>                             { RETURN(0); }

%%

