#ifndef GATE_H
#define GATE_H

#include <memory>
#include <string>
#include <vector>
#include <map>

#include <Program.h>
#include <Parameter.h>
#include <Argument.h>
#include <Exception.h>

namespace kazm {

    struct Gate : public Program {

        std::string name;
        std::size_t nparams;
        std::size_t nqubits;
        std::vector<std::string> param_names;
        std::vector<std::string> qubit_names;
        std::map<std::string, std::size_t> param_map;
        std::map<std::string, std::size_t> qubit_map;
        std::vector<std::shared_ptr<Parameter> > params;
        std::vector<std::shared_ptr<Argument> > qubits;

        Gate(const std::string&, const std::vector<std::string>& pn, const std::vector<std::string>& bn);

        virtual ~Gate() = default;

        virtual std::string str() override;
        virtual void execute(const std::vector<std::shared_ptr<Expression> >&, const std::vector<std::shared_ptr<Data> >&) throw (Exception);

    };

}

#endif
