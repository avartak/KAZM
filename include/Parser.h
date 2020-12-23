#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <utility>

#include <Exception.h>
#include <Scanner.h>
#include <Token.h>
#include <Data.h>
#include <Register.h>
#include <Gate.h>
#include <Program.h>

namespace kazm {

    struct Parser {
	
        std::vector<std::shared_ptr<Scanner> > files;
        std::vector<Token> tokens;

        std::shared_ptr<std::pair<std::size_t, std::size_t> > qasm_version;

        std::size_t clbit_space;
        std::size_t qubit_space;
        
        std::map<std::string, std::shared_ptr<Register> > cregs;
        std::map<std::string, std::shared_ptr<Register> > qregs;
        std::map<std::string, std::shared_ptr<Gate> > gates;

        Program program;

        Parser();

        bool isQReg(const std::string&);
        bool isCReg(const std::string&);
        bool isGate(const std::string&);

        std::string str();
        
        void parse(const std::string&) throw (Exception);

        bool parseToken(int, std::size_t);

        std::size_t parseHeader(std::size_t) throw (Exception);
        std::size_t parseUnit(std::size_t) throw (Exception);
        std::size_t parseInclude(std::size_t) throw (Exception);

        std::size_t parseReg(std::size_t) throw (Exception);
        std::size_t parseGate(std::size_t) throw (Exception);
        std::size_t parseQubitList(std::size_t, const Gate&, std::vector<std::shared_ptr<Data> >&) throw (Exception);

        std::size_t parseProgramStatement(std::size_t) throw (Exception);
        std::size_t parseQubitReg(std::size_t, std::shared_ptr<Data>&) throw (Exception);
        std::size_t parseBitReg(std::size_t, std::shared_ptr<Data>&) throw (Exception);
        std::size_t parseQubitRegList(std::size_t, std::vector<std::shared_ptr<Data> >&) throw (Exception);

        std::size_t parseExpList(std::size_t, const Program&, std::vector<std::shared_ptr<Expression> >&) throw (Exception);
        std::size_t parseExp(std::size_t, const Program&, std::shared_ptr<Expression>&) throw (Exception);
        std::size_t parseUnary(std::size_t, const Program&, std::shared_ptr<Expression>&) throw (Exception);
        std::size_t parseBinaryRHS(std::size_t, const Program&, const std::string&, std::shared_ptr<Expression>&) throw (Exception);
        
    };

}

#endif
