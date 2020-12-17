#include <sstream>

#include <Gate.h>
#include <Instruction.h>
#include <Parameter.h>

namespace kazm {

    Gate::Gate(const std::string& n, const std::vector<std::string>& pn, const std::vector<std::string>& bn):
        name(n),
        nparams(pn.size()),
        nqubits(bn.size())
    {
        for (std::size_t i = 0; i < pn.size(); i++) param_names.push_back(pn[i]);
        for (std::size_t i = 0; i < bn.size(); i++) qubit_names.push_back(bn[i]);
        for (std::size_t i = 0; i < param_names.size(); i++) param_map[param_names[i]] = i;
        for (std::size_t i = 0; i < qubit_names.size(); i++) qubit_map[qubit_names[i]] = i;
        for (std::size_t i = 0; i < param_names.size(); i++) pstack.push_back(std::make_shared<Parameter>(param_names[i]));

    }

    Gate::Gate(const std::string& n, std::size_t p, std::size_t b):
        name(n),
        nparams(p),
        nqubits(b)
    {
    }

    std::string Gate::str() {

        std::stringstream ss;

        ss << name; 
        ss << "(";
        for (std::size_t i = 0; i < param_names.size(); i++) {
            ss << param_names[i];
            if (i != param_names.size()-1) ss << ", ";
        }
        ss << ")";
        for (std::size_t i = 0; i < qubit_names.size(); i++) {
            ss << qubit_names[i];
            if (i != qubit_names.size()-1) ss << ", ";
        }

        if (instructions.size() > 0) {
            ss << " {\n";
            for (std::size_t i = 0; i < instructions.size(); i++) {
                ss << "    ";
                if (instructions[i]->type == instruction_call) {
                    auto call = dynamic_cast<CallInst*>(instructions[i].get());
                    ss << "call gate " << call->gate->name; 
                    ss << "(";
                    for (std::size_t j = 0; j < call->params.size(); j++) {
                        ss << pstack[call->params[j]]->str();
                        if (j != call->params.size()-1) ss << ",";
                        ss << " ";
                    } 
                    ss << ") ";
                    for (std::size_t j = 0; j < call->bits.size(); j++) {
                        ss << qubit_names[call->bits[j]];
                        if (j != call->bits.size()-1) ss << ",";
                        ss << " ";
                    } 
                    ss << "\n";
                }
                else if (instructions[i]->type == instruction_barrier) {
                    auto barr = dynamic_cast<BarrierInst*>(instructions[i].get());
                    ss << "barrier on ";
                    for (std::size_t j = 0; j < barr->bits.size(); j++) {
                        ss << qubit_names[barr->bits[j]];
                        if (j != barr->bits.size()-1) ss << ",";
                        ss << " ";
                    }
                    ss << "\n";
                }
            }
            ss << "}";
        }

        ss << "\n";

        return ss.str();
    }
        
    void Gate::execute(const Program& prog, const std::vector<std::size_t>& p, const std::vector<std::size_t>& b) throw (Exception) {
        if (p.size() != nparams) throw Exception("<Internal error Gate::execute()> Incorrect number of parameters passed");    
        if (b.size() != nqubits) throw Exception("<Internal error Gate::execute()> Incorrect number of qubits passed");

        for (std::size_t i = 0; i < p.size(); i++) {
            auto par = dynamic_cast<Parameter*>(pstack[i].get());
            par->value = prog.pstack[p[i]];
        }
        for (std::size_t i = 0; i < b.size(); i++) bstack.push_back(prog.bstack[b[i]]);

        for (std::size_t i = 0; i < instructions.size(); i++) instructions[i]->execute();

        for (std::size_t i = 0; i < p.size(); i++) {
            auto par = dynamic_cast<Parameter*>(pstack[i].get());
            par->value.reset();
        }
        bstack.clear();
    }

    IdentityGate::IdentityGate():
        Gate("__identity__", 0, 1)
    {
        std::vector<std::string> bn;
        bn.push_back("q0");
        for (std::size_t i = 0; i < bn.size(); i++) qubit_names.push_back(bn[i]);
        for (std::size_t i = 0; i < qubit_names.size(); i++) qubit_map[qubit_names[i]] = i;
    }

    std::string IdentityGate::str() {
        return name + " q0" + "\n";
    }

    void IdentityGate::execute(const Program& prog, const std::vector<std::size_t>& p, const std::vector<std::size_t>& b) throw (Exception) {
        if (p.size() != 0) throw Exception("<Internal error IdentityGate::execute()> Identity gate does not take parameters");
        if (b.size() != 1) throw Exception("<Internal error IdentityGate::execute()> Identity gate takes one qubit as input");

        bstack.push_back(prog.bstack[b[0]]);

        // Do the execution here

        bstack.clear();
    }

    CNOTGate::CNOTGate():
        Gate("__cnot__", 0, 2)
    {
        std::vector<std::string> bn;

        bn.push_back("q0");
        bn.push_back("q1");

        for (std::size_t i = 0; i < bn.size(); i++) qubit_names.push_back(bn[i]);
        for (std::size_t i = 0; i < qubit_names.size(); i++) qubit_map[qubit_names[i]] = i;
    }

    std::string CNOTGate::str() {
        return name + " q0, q1" + "\n";
    }

    void CNOTGate::execute(const Program& prog, const std::vector<std::size_t>& p, const std::vector<std::size_t>& b) throw (Exception) {
        if (p.size() != 0) throw Exception("<Internal error CNOTGate::execute()> CNOT gate does not take parameters");
        if (b.size() != 2) throw Exception("<Internal error CNOTGate::execute()> CNOT gate takes two qubits as input");

        bstack.push_back(prog.bstack[b[0]]);
        bstack.push_back(prog.bstack[b[1]]);

        // Do the execution here

        bstack.clear();
    }

    UGate::UGate():
        Gate("__u__", 3, 1)
    {
        std::vector<std::string> pn;
        std::vector<std::string> bn;

        pn.push_back("theta");
        pn.push_back("phi");
        pn.push_back("lamba");
        bn.push_back("q0");

        for (std::size_t i = 0; i < pn.size(); i++) param_names.push_back(pn[i]);
        for (std::size_t i = 0; i < bn.size(); i++) qubit_names.push_back(bn[i]);
        for (std::size_t i = 0; i < param_names.size(); i++) param_map[param_names[i]] = i;
        for (std::size_t i = 0; i < qubit_names.size(); i++) qubit_map[qubit_names[i]] = i;
        for (std::size_t i = 0; i < param_names.size(); i++) pstack.push_back(std::make_shared<Parameter>(param_names[i]));
    }

    std::string UGate::str() {
        return name + "(theta, phi, lambda) q0\n";
    }

    void UGate::execute(const Program& prog, const std::vector<std::size_t>& p, const std::vector<std::size_t>& b) throw (Exception) {
        if (p.size() != 3) throw Exception("<Internal error UGate::execute()> U gate takes three input parameters");
        if (b.size() != 1) throw Exception("<Internal error UGate::execute()> U gate takes one qubit as input");

        for (std::size_t i = 0; i < p.size(); i++) {
            auto par = dynamic_cast<Parameter*>(pstack[i].get());
            par->value = prog.pstack[p[i]];
        }
        for (std::size_t i = 0; i < b.size(); i++) bstack.push_back(prog.bstack[b[i]]);

        // Do the execution here

        for (std::size_t i = 0; i < p.size(); i++) {
            auto par = dynamic_cast<Parameter*>(pstack[i].get());
            par->value.reset();
        }
        bstack.clear();

    }

}
