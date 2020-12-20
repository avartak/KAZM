#include <sstream>

#include <Parser.h>
#include <Instruction.h>
#include <Bit.h>

namespace kazm {

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
            if (!parseToken(')', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \')\' at the end of \'if\' condition");
            n++;
            program.bstack.push_back(cregs[cr]);
            condition.first = program.bstack.size() - 1;
            condition.second = num;
            isConditioned = true;
        }

        if (parseToken(T_MEASURE, it+n)) {
            n++;
            std::shared_ptr<Data> qubit;
            std::shared_ptr<Data> clbit;
            std::size_t m = parseQubitReg(it+n, qubit);
            if (m == 0 || !qubit || !qubit->isQuantum()) throw Exception(files.back()->filename, tokens[it+n].line, "Expect a qubit/register after \'measure\'");
            n += m;
            if (!parseToken(T_YIELDS, it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \'->\' after qubit(s) to measure");
            n++;
            m = parseBitReg(it+n, clbit);
            if (m == 0 || !clbit || !clbit->isClassical()) throw Exception(files.back()->filename, tokens[it+n].line, "Expect a bit/register after \'->\'");
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

            inst = std::make_shared<ResetInst>(program, program.bstack.size());
            program.bstack.push_back(qubit);
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

        else if (parseToken(T_ID, it+n) || parseToken(T_U, it+n) || parseToken(T_CX, it+n)) {
            std::string gname = tokens[it+n].value;
            std::string gate_name = gname;
            if (parseToken(T_U, it+n)) gate_name = "__u__";
            if (parseToken(T_CX, it+n)) gate_name = "__cnot__";
            n++;
            if (!isGate(gate_name)) throw Exception(files.back()->filename, tokens[it+n].line, gname + " is not a gate");
            auto gate = gates[gate_name];
            std::vector<std::size_t> expv;
            std::vector<std::size_t> qidxv;
            if (gate->nparams == 0) {
                if (gate_name == "CX") {
                    if (parseToken('(', it+n)) {
                        throw Exception(files.back()->filename, tokens[it+n].line, "Cannot use \'(\' when calling CX gate");
                    }
                } 
                else {
                    if (parseToken('(', it+n)) {
                        n++;
                        if (!parseToken(')', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \')\' after \'(\' when calling gate " + gname);
                        n++;
                    }
                }
            }
            else {
                if (!parseToken('(', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \'(\' for parameter list when calling gate " + gname);
                n++;
                std::size_t m = parseExpList(it+n, program, expv);
                if (expv.size() != gate->nparams) {
                    std::stringstream ss;
                    ss << gate->name << " gate expects " << gate->nparams << " parameters, " << expv.size() << " parameters provided";
                    throw Exception(files.back()->filename, tokens[it+n].line, ss.str());
                }
                n += m;
                if (!parseToken(')', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \')\' after parameter list for gate " + gname);
                n++;
            }
            std::size_t m = parseQubitRegList(it+n, qidxv);
            if (qidxv.size() != gate->nqubits) {
                std::stringstream ss;
                ss << gate->name << " gate expects " << gate->nqubits << " qubits/registers, " << qidxv.size() << " provided";
                throw Exception(files.back()->filename, tokens[it+n].line, ss.str());
            }
            n += m;
            if (!parseToken(';', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \';\' at the end of call to gate " + gname);
            n++;
            inst = std::make_shared<CallInst>(program, gate, expv, qidxv);
        }

        if ( isConditioned && !inst) throw Exception(files.back()->filename, tokens[it+n].line, "if statement not followed by a valid instruction");
        if (!isConditioned && !inst) return 0;
        if (!isConditioned &&  inst) program.instructions.push_back(inst);
        if ( isConditioned &&  inst) program.instructions.push_back(std::make_shared<IfInst>(program, condition.first, condition.second, inst));

        return n;

    }

    std::size_t Parser::parseQubitReg(std::size_t it, std::shared_ptr<Data>& data) throw (Exception) {

        std::size_t n = 0;

        if (!parseToken(T_ID, it)) return 0;
        const std::string& reg = tokens[it].value;
        if (!isQReg(reg)) return 0;
        n++;

        if (parseToken('[', it+n)) {
            n++;
            if (!parseToken(T_NNINTEGER, it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect an index after \'[\'");
            uint64_t idx = strtoull(tokens[it+n].value.c_str(), nullptr, 0);
            if (errno == ERANGE) throw Exception(files.back()->filename, tokens[it].line, "Index of register " + reg + " is out of range");
            if (idx >= qregs[reg]->size()) throw Exception(files.back()->filename, tokens[it].line, "Index of register " + reg + " is out of bounds");
            n++;
            if (!parseToken(']', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \']\' after register index");
            n++;
            data = std::make_shared<Bit>(qregs[reg], idx);
            return n;
        }

        data = qregs[reg];
        return n;

    }

    std::size_t Parser::parseBitReg(std::size_t it, std::shared_ptr<Data>& data) throw (Exception) {

        std::size_t n = 0;

        if (!parseToken(T_ID, it)) return 0;
        const std::string& reg = tokens[it].value;
        if (!isCReg(reg)) return 0;
        n++;

        if (parseToken('[', it+n)) {
            n++;
            if (!parseToken(T_NNINTEGER, it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect an index after \'[\'");
            uint64_t idx = strtoull(tokens[it+n].value.c_str(), nullptr, 0);
            if (errno == ERANGE) throw Exception(files.back()->filename, tokens[it].line, "Index of register " + reg + " is out of range");
            if (idx >= cregs[reg]->size()) throw Exception(files.back()->filename, tokens[it].line, "Index of register " + reg + " is out of bounds");
            n++;
            if (!parseToken(']', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \']\' after register index");
            n++;
            data = std::make_shared<Bit>(cregs[reg], idx);
            return n;
        }

        data = cregs[reg];
        return n;

    }

    std::size_t Parser::parseQubitRegList(std::size_t it, std::vector<std::size_t>& qidxv) throw (Exception) {

        std::size_t n = 0;

        std::shared_ptr<Data> data;
        std::size_t m = parseQubitReg(it+n, data);
        if (m == 0) return 0;
        qidxv.push_back(program.bstack.size());
        program.bstack.push_back(data);
        n += m;

        while (true) {

            if (!parseToken(',', it+n)) break; 
            n++;
            std::shared_ptr<Data> data;
            m = parseQubitReg(it+n, data);
            if (m == 0) throw Exception(files.back()->filename, tokens[it+n].line, "Expect a qubit/register after \',\'");
            qidxv.push_back(program.bstack.size());
            program.bstack.push_back(data);
            n += m;

        }

        std::vector<std::shared_ptr<Data> > regs;
        std::vector<std::shared_ptr<Data> > bits;

        for (std::size_t i = 0; i < qidxv.size(); i++) {
            auto data = program.bstack[qidxv[i]];
            if (data->isReg()) regs.push_back(data);
            else bits.push_back(data);
        }

        std::size_t reg_size = 0;
        if (regs.size() > 0) reg_size = regs[0]->size();
        for (std::size_t i = 1; i < regs.size(); i++) {
            if (regs[i]->size() != reg_size) throw Exception(files.back()->filename, tokens[it].line, "Register arguments must have the same size");
        }

        for (std::size_t i = 0; i < regs.size(); i++) {
            for (std::size_t j = i+1; j < regs.size(); j++) {
                if (regs[i]->offset() == regs[j]->offset()) throw Exception(files.back()->filename, tokens[it].line, "Registers used in arguments must be unique");
            }
        }

        for (std::size_t i = 0; i < bits.size(); i++) {
            for (std::size_t j = i+1; j < bits.size(); j++) {
                if (bits[i]->offset() == bits[j]->offset()) throw Exception(files.back()->filename, tokens[it].line, "Qubit arguments must be unique");
            }
        }

        for (std::size_t i = 0; i < bits.size(); i++) {
            for (std::size_t j = 0; j < regs.size(); j++) {
                if (bits[i]->name() == regs[j]->name()) throw Exception(files.back()->filename, tokens[it].line, "Register " + regs[j]->name() + " overlaps with a qubit argument");
            }
        }

        return n;

    }

}
