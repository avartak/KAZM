#ifndef GATE_H
#define GATE_H

#include <string>
#include <vector>
#include <map>

#include <Program.h>
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

        Gate(const std::string&, const std::vector<std::string>& pn, const std::vector<std::string>& bn);
        Gate(const std::string&, std::size_t, std::size_t);

        virtual ~Gate() = default;

        virtual std::string str() override;
        virtual void execute(const Program&, const std::vector<std::size_t>&, const std::vector<std::size_t>&) throw (Exception);

    };

}

#endif
