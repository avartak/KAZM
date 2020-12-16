#include <Parser.h>

namespace kazm {

    std::size_t Parser::parseRegDeclaration(std::size_t it) throw (Exception) {

        DataType rt;
        if (parseToken(T_QREG, it)) rt = data_classical;
        else if (parseToken(T_CREG, it)) rt = data_quantum;
        else return 0;

        if (!parseToken(T_ID, it+1) || !parseToken('[', it+2) || !parseToken(T_NNINTEGER, it+3) || !parseToken(']', it+4) || !parseToken(';', it+5)) return 0;

        const std::string& name = tokens[it+1].value;
        if (isCReg(name)) throw Exception(files.back()->filename, tokens[it+1].line, tokens[it+1].value + " has been previously defined as a classical register");
        if (isQReg(name)) throw Exception(files.back()->filename, tokens[it+1].line, tokens[it+1].value + " has been previously defined as a quantum register");
        if (isGate(name)) throw Exception(files.back()->filename, tokens[it+1].line, tokens[it+1].value + " has been previously defined as a gate");

        uint64_t sz = strtoull(tokens[it+3].value.c_str(), nullptr, 0);
        if (errno == ERANGE) throw Exception(files.back()->filename, tokens[it+3].line, "Register size out of range");
        if (sz == 0) throw Exception(files.back()->filename, tokens[it+3].line, "Register size cannot be 0");

        if (rt == data_classical) {
            if (clbit_space + sz < clbit_space) throw Exception(files.back()->filename, tokens[it+3].line, "Total size of bit space exceeds limit");
            cregs[name] = std::make_shared<Register>(data_classical, name, sz, clbit_space);
            clbit_space += sz;
        }
        else {
            if (qubit_space + sz < qubit_space) throw Exception(files.back()->filename, tokens[it+3].line, "Total size of qubit space exceeds limit");
            qregs[name] = std::make_shared<Register>(data_classical, name, sz, qubit_space);
            qubit_space += sz;
        }

        return 6;

    }

    std::size_t Parser::parseOpaqueDeclaration(std::size_t it) throw (Exception) {

        std::size_t n = 0;

        if (!parseToken(T_OPAQUE, it)) return 0;
        n++;

        if (!parseToken(T_ID, it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect identifier after opaque keyword");
        auto gate_name = tokens[it+n].value;
        if (isGate(gate_name)) throw Exception(files.back()->filename, tokens[it+n].line, "Gate " + gate_name + " is already declared");
        n++;

        std::vector<std::string> param_list;
        if (parseToken('(', it+n)) {
            n++;
           
            while (true) {
                if (parseToken(T_ID, it+n)) {
                    std::string id = tokens[it+n].value;
                    for (std::size_t i = 0; i < param_list.size(); i++) {
                        if (id == param_list[i]) throw Exception(files.back()->filename, tokens[it+n].line, "Parameter names must be unique");
                    }
                    n++;
                    param_list.push_back(id);
                    if (parseToken(',', it+n)) {
                        n++;
                        continue;
                    }
                }
                break;
            }

            if (!parseToken(')', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \')\'");
            n++;
        }

        std::vector<std::string> qubit_list;
        while (true) {
            if (parseToken(T_ID, it+n)) {
                std::string id = tokens[it+n].value;
                for (std::size_t i = 0; i < param_list.size(); i++) {
                    if (id == param_list[i]) throw Exception(files.back()->filename, tokens[it+n].line, "Qubit names must be different than parameter names");
                }
                for (std::size_t i = 0; i < qubit_list.size(); i++) {
                    if (id == qubit_list[i]) throw Exception(files.back()->filename, tokens[it+n].line, "Qubit names must be unique");
                }
                n++;
                qubit_list.push_back(id);
                if (parseToken(',', it+n)) {
                    n++;
                    continue;
                }
            }
            break;
        }
        if (qubit_list.size() == 0) throw Exception(files.back()->filename, tokens[it+n].line, "Expect at least one qubit argument");

        if (!parseToken(';', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \';\' at the end of opaque declaration");
        n++;

        gates[gate_name] = std::make_shared<Gate>(gate_name, param_list, qubit_list);

        return n;
    }

}
