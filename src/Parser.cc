#include <Parser.h>

namespace kazm {

    Parser::Parser():
        clbit_space(0),
        qubit_space(0)
    {
        gates["__identity__"] = std::make_shared<IdentityGate>();
        gates["__cnot__"] = std::make_shared<CNOTGate>();
        gates["__u__"] = std::make_shared<UGate>();
    }

    bool Parser::isReg(const std::string& name) {
        return isCReg(name) || isQReg(name);
    }

    bool Parser::isCReg(const std::string& name) {
        return cregs.find(name) != cregs.end();
    }

    bool Parser::isQReg(const std::string& name) { 
        return qregs.find(name) != qregs.end();
    }

    bool Parser::isGate(const std::string& name) {
        return gates.find(name) != gates.end();
    }

    std::string Parser::str() {
        std::string s = "";
        for (auto it = cregs.begin(); it != cregs.end(); ++it) s += it->second->str();
        for (auto it = qregs.begin(); it != qregs.end(); ++it) s += it->second->str();
        for (auto it = gates.begin(); it != gates.end(); ++it) s += it->second->str();
        s += program.str();
        return s;
    }

    void Parser::parse(const std::string& filename) throw (Exception) {
    
        files.push_back(std::make_shared<SourceFile>(filename));
        
        std::size_t s = tokens.size();
        
        tokens.push_back(files.back()->scan());
        
        if (tokens[s].type == 0) return;
        auto n = parseHeader(s);
        if (n == 0) throw Exception(filename, tokens[s].line, "Missing header");
        tokens.erase(tokens.begin()+s, tokens.begin()+s+n);

        while (tokens[s].type != 0) {
            n = parseUnit(s);
            tokens.erase(tokens.begin()+s, tokens.begin()+s+n);
        }
        
        tokens.pop_back();
        files.pop_back();
    }
    
    bool Parser::parseToken(int t, std::size_t it) {
    
        if (tokens[it].type == t) {
            if (it == tokens.size() - 1) tokens.push_back(files.back()->scan());
            return true;
        }
        return false;
    }

}
