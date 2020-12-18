#ifndef DATA_H
#define DATA_H

#include <string>

#include <Exception.h>

namespace kazm {

    enum DataType {
        data_classical,
        data_quantum
    };

    struct Data {

        virtual ~Data() = default;

        virtual std::string name() = 0;
        virtual DataType type() throw (Exception) = 0;
        virtual std::size_t size() throw (Exception)= 0;
        virtual std::size_t offset() throw (Exception) = 0;

        virtual bool isReg() throw (Exception) = 0;
        virtual bool isBit() throw (Exception);
        virtual bool isClassical() throw (Exception);
        virtual bool isQuantum() throw (Exception);
    };

}

#endif
