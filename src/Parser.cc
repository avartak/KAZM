#include <sstream>

#include <Parser.h>
#include <Instruction.h>

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

    std::size_t Parser::parseHeader(std::size_t it) throw (Exception) {

        if (!parseToken(T_HEADER, it)) return 0;

        std::size_t n = 0;
        auto header = tokens[it].value;

        // Clipping the 'OPENQASM' and ';' parts of the header
        header = header.substr(8, tokens[it].value.length()-8-1);
        // Clipping the whitespace to get the version string
        while (header[n] == ' ' || header[n] == '\t') n++;
        auto version = header.substr(n, header.length()-n);

        // Splitting the major and minor version substrings 
        n = 0;
        while (version[n] != '.') n++;
        auto major_str = version.substr(0, n);
        auto minor_str = version.substr(n+1, version.length()-(n+1));

        // Currently the version values are not explicitly checked 
        // This could be added 
        // This parser expects OPENQASM version 2.0   
        uint64_t major_ver = strtoull(major_str.c_str(), nullptr, 0);
        if (errno == ERANGE) throw Exception(files.back()->filename, tokens[it].line, "Major version in the header out of range");
        uint64_t minor_ver = strtoull(minor_str.c_str(), nullptr, 0);
        if (errno == ERANGE) throw Exception(files.back()->filename, tokens[it].line, "Minor version in the header out of range");

        files.back()->major_version = major_ver;
        files.back()->minor_version = minor_ver;

        return 1;
    }

    std::size_t Parser::parseUnit(std::size_t it) throw (Exception) {

        std::size_t n = 0;

        n = parseInclude(it);
        if (n > 0)  return n;

        n = parseRegDeclaration(it);
        if (n > 0)  return n;

        n = parseOpaqueDeclaration(it);
        if (n > 0)  return n;

        n = parseGateDefinition(it);
        if (n > 0)  return n;

        n = parseProgramStatement(it);
        if (n > 0)  return n;

        throw Exception(files.back()->filename, tokens[it].line, "Unknown statement");
    }

    std::size_t Parser::parseInclude(std::size_t it) throw (Exception) {

        if (!parseToken(T_INCLUDE, it) || !parseToken(T_FILENAME, it+1) || !parseToken(';', it+2)) return 0;

        // Clipping the quotation marks from the file name 'string'
        auto filename = tokens[it+1].value.substr(1, tokens[it+1].value.length()-2);
        parse(filename);

        return 3;

    }

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

    std::size_t Parser::parseGateDefinition(std::size_t it) throw (Exception) {

        std::size_t n = 0;

        if (!parseToken(T_GATE, it)) return 0;
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

        auto gate = std::make_shared<Gate>(gate_name, param_list, qubit_list);

        std::size_t param_current = param_list.size();

        if (!parseToken('{', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \'{\'");
        n++;

        while (true) {

            if (parseToken(T_BARRIER, it+n)) {
                n++;
                std::vector<std::size_t> qidxv;
                std::size_t m = parseQubitList(it+n, *gate, qidxv);
                if (m == 0 || qidxv.size() == 0) throw Exception(files.back()->filename, tokens[it+n].line, "Expect one or more qubits after \'barrier\'");
                n += m;
                if (!parseToken(';', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \';\' at the end of barrier statement");
                n++;
                gate->instructions.push_back(std::make_shared<BarrierInst>(*gate, qidxv));
            }

            else if (parseToken(T_U, it+n)) {
                n++;
                if (!parseToken('(', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \'(\' \'U\'");
                n++;
                std::vector<std::size_t> expv;
                std::vector<std::size_t> qidxv;
                std::size_t m = parseExpList(it+n, *gate, expv);
                if (expv.size() != 3) throw Exception(files.back()->filename, tokens[it+n].line, "Expect three arguments \'U\'");
                n += m;
                if (!parseToken(')', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \')\' \'U\' arguments");
                n++;
                m = parseQubitList(it+n, *gate, qidxv);
                if (m == 0) throw Exception(files.back()->filename, tokens[it+n].line, "Expect a qubit argument for \'U\'");
                if (qidxv.size() != 1) throw Exception(files.back()->filename, tokens[it+n].line, "Expect one qubit argument for \'U\'");
                n += m;
                if (!parseToken(';', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \';\' at the end of U gate call");
                n++;
                gate->instructions.push_back(std::make_shared<CallInst>(program, gates["__u__"], expv, qidxv));
            }

            else if (parseToken(T_CX, it+n)) {
                n++;
                std::vector<std::size_t> qidxv;
                std::size_t m = parseQubitList(it+n, *gate, qidxv);
                if (qidxv.size() != 2) throw Exception(files.back()->filename, tokens[it+n].line, "Expect two qubits for \'CX\'");
                n += m;
                if (!parseToken(';', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \';\' at the end of CX gate call");
                n++;
                gate->instructions.push_back(std::make_shared<CallInst>(*gate, gates["__cnot__"], qidxv));
            }

            else if (parseToken(T_ID, it+n)) {
                std::string gt_name = tokens[it+n].value;
                n++;
                if (!isGate(gt_name)) throw Exception(files.back()->filename, tokens[it+n].line, gt_name + " is not a gate");
                auto gt = gates[gt_name];
                std::vector<std::size_t> expv;
                std::vector<std::size_t> qidxv;
                if (gt->nparams == 0) {
                    if (parseToken('(', it+n)) {
                        n++;
                        if (!parseToken(')', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \'(\' after \')\' when calling gate " + gt_name);
                        n++;
                    }
                }
                else {
                    if (!parseToken('(', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \'(\' for parameter list when calling gate " + gt_name);
                    n++;
                    std::size_t m = parseExpList(it+n, *gate, expv);
                    if (expv.size() != gt->nparams) {
                        std::stringstream ss;
                        ss << gt->name << " gate expects " << gt->nparams << " parameters, " << expv.size() << " parameters provided";
                        throw Exception(files.back()->filename, tokens[it+n].line, ss.str());
                    }
                    n += m;
                    if (!parseToken(')', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \')\' after parameter list for gate " + gt_name);
                    n++;
                }
                std::size_t m = parseQubitList(it+n, *gate, qidxv);
                if (qidxv.size() != gt->nqubits) {
                    std::stringstream ss;
                    ss << gt->name << " gate expects " << gt->nparams << " qubits, " << qidxv.size() << " provided";
                    throw Exception(files.back()->filename, tokens[it+n].line, ss.str());
                }
                n += m;
                if (!parseToken(';', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \';\' at the end of call to gate " + gt_name);
                n++;
                gate->instructions.push_back(std::make_shared<CallInst>(*gate, gt, expv, qidxv));
            }

            else break;
        }

        if (!parseToken('}', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \'}\'");
        n++;

        if (gate->instructions.size() == 0) {
            for (std::size_t i = 0; i < gate->nqubits; i++) {
                std::vector<std::size_t> expv;
                std::vector<std::size_t> qidxv;
                qidxv.push_back(i);
                gate->instructions.push_back(std::make_shared<CallInst>(*gate, gates["__identity__"], expv, qidxv));
            }
        }

        gates[gate_name] = gate;

        return n;

    }

    std::size_t Parser::parseProgramStatement(std::size_t it) throw (Exception) {

        std::size_t n = 0;

        std::shared_ptr<Instruction> inst;

        std::pair<std::size_t, std::string> condition;
        bool isConditioned = false;
        if (parseToken(T_IF, it+n)) {
            n++;
            if (!parseToken('(', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \'(\' after \'if\'");
            n++;
            if (!parseToken(T_ID, it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect identifier' after \'if (\'");
            auto cr = tokens[it+n].value;
            if (!isCReg(cr)) throw Exception(files.back()->filename, tokens[it+n].line, cr + " is not defined as a classical register");
            n++;
            if (!parseToken(T_EQUALS, it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect == after \'if (" + cr + "\'");
            n++;
            if (!parseToken(T_NNINTEGER, it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect integer after \'if (" + cr + " == \'");
            std::string num = tokens[it+n].value;
            n++;
            program.bstack.push_back(cregs[cr]);
            condition.first = program.bstack.size() - 1;
            condition.second = num;
            isConditioned = true;
        }

        else if (parseToken(T_MEASURE, it+n)) {
            n++;
            std::shared_ptr<Data> qubit;
            std::shared_ptr<Data> clbit;
            std::size_t m = parseQubitReg(it+n, qubit);
            if (m == 0 || !qubit || !qubit->isQuantum()) throw Exception(files.back()->filename, tokens[it+n].line, "Expect a qubit/register after \'measure\'");
            n += m;
            if (!parseToken(T_YIELDS, it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect integer after \'->\' after qubit(s) to measure");
            m = parseBitReg(it+n, clbit);
            if (m == 0 || !clbit == !clbit->isClassical()) throw Exception(files.back()->filename, tokens[it+n].line, "Expect a bit/register after \'->\'");
            n += m;
            if (!parseToken(';', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \';\' at the end of measure statement");
            n++;

            std::size_t nb = program.bstack.size();
            program.bstack.push_back(qubit);
            program.bstack.push_back(clbit);
            inst = std::make_shared<MeasureInst>(program, nb, nb+1);
        }

        else if (parseToken(T_RESET, it+n)) {
            n++;
            std::shared_ptr<Data> qubit;
            std::size_t m = parseQubitReg(it+n, qubit);
            if (m == 0 || !qubit || !qubit->isQuantum()) throw Exception(files.back()->filename, tokens[it+n].line, "Expect a qubit/register after \'reset\'");
            n += m;
            if (!parseToken(';', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \';\' at the end of measure statement");
            n++;

            program.bstack.push_back(qubit);
            inst = std::make_shared<ResetInst>(program, program.bstack.size());
        }

        else if (parseToken(T_BARRIER, it+n)) {
            n++;
            std::vector<std::size_t> qidxv;
            std::size_t m = parseQubitRegList(it+n, qidxv);
            if (m == 0 || qidxv.size() == 0) throw Exception(files.back()->filename, tokens[it+n].line, "Expect one or more qubit/registers after \'barrier\'");
            n += m;
            if (!parseToken(';', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \';\' at the end of barrier statement");
            n++;
            inst = std::make_shared<BarrierInst>(program, qidxv);
        }

        else if (parseToken(T_U, it+n)) {
            n++;
            if (!parseToken('(', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \'(\' \'U\'");
            n++;
            std::vector<std::size_t> expv;
            std::vector<std::size_t> qidxv;
            std::size_t m = parseExpList(it+n, expv);
            if (expv.size() != 3) throw Exception(files.back()->filename, tokens[it+n].line, "Expect three arguments \'U\'");
            n += m;
            if (!parseToken(')', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \')\' \'U\' arguments");
            n++;
            std::shared_ptr<Data> qubit;
            m = parseQubitReg(it+n, qubit);
            if (m == 0 || !qubit || !qubit->isQuantum()) throw Exception(files.back()->filename, tokens[it+n].line, "Expect a qubit/register for \'U\'");
            n += m;
            qidxv.push_back(program.bstack.size());
            program.bstack.push_back(qubit);
            if (!parseToken(';', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \';\' at the end of U gate call");
            n++;
            inst = std::make_shared<CallInst>(program, gates["__u__"], expv, qidxv);
        }

        else if (parseToken(T_CX, it+n)) {
            n++;
            std::vector<std::size_t> qidxv;
            std::size_t m = parseQubitRegList(it+n, qidxv);
            if (qidxv.size() != 2) throw Exception(files.back()->filename, tokens[it+n].line, "Expect two qubits/registers for \'CX\'");
            if (!checkQubitRegList(qidxv)) throw Exception(files.back()->filename, tokens[it+n].line, "Overlapping qubits for \'CX\'");
            n += m;
            if (!parseToken(';', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \';\' at the end of CX gate call");
            n++;
            inst = std::make_shared<CallInst>(program, gates["__cnot__"], qidxv);
        }

        else if (parseToken(T_ID, it+n)) {
            std::string gate_name = tokens[it+n].value;
            n++;
            if (!isGate(gate_name)) throw Exception(files.back()->filename, tokens[it+n].line, gate_name + " is not a gate");
            auto gate = gates[gate_name];
            std::vector<std::size_t> expv;
            std::vector<std::size_t> qidxv;
            if (gate->nparams == 0) {
                if (parseToken('(', it+n)) {
                    n++;
                    if (!parseToken(')', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \'(\' after \')\' when calling gate " + gate_name);
                    n++;
                }
            }
            else {
                if (!parseToken('(', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \'(\' for parameter list when calling gate " + gate_name);
                n++;
                std::size_t m = parseExpList(it+n, expv);
                if (expv.size() != gate->nparams) {
                    std::stringstream ss;
                    ss << gate->name << " gate expects " << gate->nparams << " parameters, " << expv.size() << " parameters provided";
                    throw Exception(files.back()->filename, tokens[it+n].line, ss.str());
                }
                n += m;
                if (!parseToken(')', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \')\' after parameter list for gate " + gate_name);
                n++;
            }
            std::size_t m = parseQubitRegList(it+n, qidxv);
            if (qidxv.size() != gate->nqubits) {
                std::stringstream ss;
                ss << gate->name << " gate expects " << gate->nparams << " qubits/registers, " << qidxv.size() << " provided";
                throw Exception(files.back()->filename, tokens[it+n].line, ss.str());
            }
            if (!checkQubitRegList(qidxv)) throw Exception(files.back()->filename, tokens[it+n].line, "Overlapping qubits for gate " + gate_name);
            n += m;
            if (!parseToken(';', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \';\' at the end of call to gate " + gate_name);
            n++;
            inst = std::make_shared<CallInst>(program, gate, expv, qidxv);
        }

        if ( isConditioned && !inst) throw Exception(files.back()->filename, tokens[it+n].line, "if statement not followed by a valid instruction");
        if (!isConditioned && !inst) return 0;
        if (!isConditioned &&  inst) program.instructions.push_back(inst);
        if ( isConditioned &&  inst) program.instructions.push_back(std::make_shared<IfInst>(program, condition.first, condition.second, inst));

        return n;

    }

    std::size_t Parser::parseQubitReg(std::size_t, std::shared_ptr<Data>&) throw (Exception) {

        return 0;

    }

    std::size_t Parser::parseBitReg(std::size_t, std::shared_ptr<Data>&) throw (Exception) {

        return 0;

    }

    std::size_t Parser::parseQubitRegList(std::size_t, std::vector<std::size_t>&) throw (Exception) {

        return 0;

    }

    std::size_t Parser::parseQubitList(std::size_t, const Gate&, std::vector<std::size_t>&) throw (Exception) {

        // Check for duplicates

        return 0;

    }

    std::size_t Parser::parseExpList(std::size_t it, std::vector<std::size_t>& ev) throw (Exception) {

        return 0;

    }

    std::size_t Parser::parseExpList(std::size_t it, Gate& gate, std::vector<std::size_t>& ev) throw (Exception) {

        return 0;

    }

    bool Parser::checkQubitRegList(const std::vector<std::size_t>&) {

        return false;

    }
}
