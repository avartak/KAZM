#ifndef HEADER_H
#define HEADER_H

#include <cstddef>

namespace kazm {

    struct Header {

        std::size_t major_version;
        std::size_t minor_version;

        Header(std::size_t, std::size_t);

    };

}

#endif
