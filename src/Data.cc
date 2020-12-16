#include <Data.h>

namespace kazm {

    bool Data::isBit() {
        return !isReg();
    }

    bool Data::isClassical() {
        return type() == data_classical;
    }

    bool Data::isQuantum() {
        return type() == data_quantum;
    }
}
