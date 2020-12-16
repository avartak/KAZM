#ifndef DATA_H
#define DATA_H

#include <string>

namespace kazm {

    enum DataType {
        data_classical,
        data_quantum
    };

    struct Data {

        virtual ~Data() = default;

        virtual std::string name() = 0;
        virtual DataType type() = 0;
        virtual std::size_t size() = 0;
        virtual std::size_t offset() = 0;

        virtual bool isReg() = 0;
        virtual bool isBit();
        virtual bool isClassical();
        virtual bool isQuantum();
    };

}

#endif
