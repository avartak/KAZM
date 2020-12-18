#include <sstream>

#include <Gate.h>
#include <Instruction.h>
#include <Parameter.h>
#include <Argument.h>

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
        for (std::size_t i = 0; i < qubit_names.size(); i++) bstack.push_back(std::make_shared<Argument>(qubit_names[i]));

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
            ss << " " << qubit_names[i];
            if (i != qubit_names.size()-1) ss << ",";
        }

        if (instructions.size() > 0) {
            ss << " {\n";
            for (std::size_t i = 0; i < instructions.size(); i++) {
                ss << "    " << instructions[i]->str() << std::endl;
            }
            ss << "}";
        }

        ss << "\n";

        return ss.str();
    }
        
    void Gate::execute(const Program& prog, const std::vector<std::size_t>& p, const std::vector<std::size_t>& b) throw (Exception) {
        if (p.size() != nparams) throw Exception("<Internal error Gate::execute()> Incorrect number of parameters passed to gate " + name);    
        if (b.size() != nqubits) throw Exception("<Internal error Gate::execute()> Incorrect number of qubits passed to gate " + name);

        for (std::size_t i = 0; i < p.size(); i++) {
            auto par = dynamic_cast<Parameter*>(pstack[i].get());
            par->value = prog.pstack[p[i]];
        }
        for (std::size_t i = 0; i < b.size(); i++) {
            auto arg = dynamic_cast<Argument*>(bstack[i].get());
            auto caller_arg = dynamic_cast<Argument*>(prog.bstack[b[i]].get());
            if (caller_arg == nullptr) arg->set(prog.bstack[b[i]]);
            arg->set(caller_arg->arg());
        }

        for (std::size_t i = 0; i < instructions.size(); i++) instructions[i]->execute();

        for (std::size_t i = 0; i < p.size(); i++) {
            auto par = dynamic_cast<Parameter*>(pstack[i].get());
            par->value.reset();
        }
        for (std::size_t i = 0; i < b.size(); i++) {
            auto arg = dynamic_cast<Argument*>(bstack[i].get());
            arg->reset();
        }
    }

}
