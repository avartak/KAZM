#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <memory>
#include <map>

#include <Exception.h>
#include <SourceFile.h>
#include <Token.h>
#include <Data.h>
#include <Register.h>
#include <Gate.h>
#include <Program.h>

namespace kazm {

    struct Parser {
	
        std::vector<std::shared_ptr<SourceFile> > files;
        std::vector<Token> tokens;

        std::map<std::string, std::shared_ptr<Register> > cregs;
        std::map<std::string, std::shared_ptr<Register> > qregs;
        std::map<std::string, std::shared_ptr<Gate> > gates;

        std::size_t clbit_space;
        std::size_t qubit_space;
        
        Program program;

        Parser();

        bool isReg(const std::string&);
        bool isQReg(const std::string&);
        bool isCReg(const std::string&);
        bool isGate(const std::string&);
        
        void parse(const std::string&) throw (Exception);

        bool parseToken(int, std::size_t);

        std::size_t parseHeader(std::size_t) throw (Exception);
        std::size_t parseUnit(std::size_t) throw (Exception);
        std::size_t parseInclude(std::size_t) throw (Exception);
        std::size_t parseRegDeclaration(std::size_t) throw (Exception);
        std::size_t parseOpaqueDeclaration(std::size_t) throw (Exception);
        std::size_t parseProgramStatement(std::size_t) throw (Exception);

        std::size_t parseQubitReg(std::size_t, std::shared_ptr<Data>&) throw (Exception);
        std::size_t parseBitReg(std::size_t, std::shared_ptr<Data>&) throw (Exception);
        std::size_t parseQubitRegList(std::size_t, std::vector<std::size_t>&) throw (Exception);
        std::size_t parseExp(std::size_t, std::shared_ptr<Expression>&) throw (Exception);
        std::size_t parseExpList(std::size_t, std::vector<std::size_t>&) throw (Exception);

        bool checkQubitRegList(const std::vector<std::size_t>&);
    };

}

#endif