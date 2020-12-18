#include <Data.h>

namespace kazm {

    bool Data::isBit() throw (Exception) {
        return !isReg();
    }

    bool Data::isClassical() throw (Exception) {
        return type() == data_classical;
    }

    bool Data::isQuantum() throw (Exception) {
        return type() == data_quantum;
    }
}
