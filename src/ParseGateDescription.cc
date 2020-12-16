#include <sstream>

#include <Parser.h>
#include <Instruction.h>

namespace kazm {

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

            else if (parseToken(T_ID, it+n) || parseToken(T_U, it+n) || parseToken(T_CX, it+n)) {
                std::string gname = tokens[it+n].value;
                std::string gt_name = gname;
                if (parseToken(T_U, it+n)) gt_name = "__u__";
                if (parseToken(T_CX, it+n)) gt_name = "__cnot__";
                n++;
                if (!isGate(gt_name)) throw Exception(files.back()->filename, tokens[it+n].line, gname + " is not a gate");
                auto gt = gates[gt_name];
                std::vector<std::size_t> expv;
                std::vector<std::size_t> qidxv;
                if (gt->nparams == 0) {
                    if (gate_name == "CX") {
                        if (parseToken('(', it+n)) {
                            throw Exception(files.back()->filename, tokens[it+n].line, "Cannot use \'(\' when calling CX gate");
                        }
                    }
                    else { 
                        if (parseToken('(', it+n)) {
                            n++;
                            if (!parseToken(')', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \'(\' after \')\' when calling gate " + gname);
                            n++;
                        }
                    }
                }
                else {
                    if (!parseToken('(', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \'(\' for parameter list when calling gate " + gname);
                    n++;
                    std::size_t m = parseExpList(it+n, *gate, expv);
                    if (expv.size() != gt->nparams) {
                        std::stringstream ss;
                        ss << gt->name << " gate expects " << gt->nparams << " parameters, " << expv.size() << " parameters provided";
                        throw Exception(files.back()->filename, tokens[it+n].line, ss.str());
                    }
                    n += m;
                    if (!parseToken(')', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \')\' after parameter list for gate " + gname);
                    n++;
                }
                std::size_t m = parseQubitList(it+n, *gate, qidxv);
                if (qidxv.size() != gt->nqubits) {
                    std::stringstream ss;
                    ss << gt->name << " gate expects " << gt->nparams << " qubits, " << qidxv.size() << " provided";
                    throw Exception(files.back()->filename, tokens[it+n].line, ss.str());
                }
                n += m;
                if (!parseToken(';', it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect \';\' at the end of call to gate " + gname);
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

    std::size_t Parser::parseQubitList(std::size_t it, const Gate& gate, std::vector<std::size_t>& qidxv) throw (Exception) {

        std::size_t n = 0;
        auto qmap = gate.qubit_map;

        if (!parseToken(T_ID, it)) return 0;
        auto qid = tokens[it].value;
        if (qmap.find(qid) != qmap.end()) return 0; 
        qidxv.push_back(qmap[qid]);
        n++;

        while (true) {

            if (!parseToken(',', it+n)) return n;
            n++;
            if (!parseToken(T_ID, it+n)) throw Exception(files.back()->filename, tokens[it+n].line, "Expect a qubit after \',\'");
            if (qmap.find(qid) != qmap.end()) throw Exception(files.back()->filename, tokens[it+n].line, "Unknown qubit argument");
            for (std::size_t i = 0; i < qidxv.size(); i++) {
                if (qidxv[i] == qmap[qid]) throw Exception(files.back()->filename, tokens[it+n].line, "Qubit argument " + qid + " is repeated");
            }
            qidxv.push_back(qmap[qid]);
            n++;

        }

    }

}
