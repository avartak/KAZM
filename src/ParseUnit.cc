#include <cstdlib>
#include <cerrno>

#include <Parser.h>

namespace kazm {

    std::size_t Parser::parseHeader(std::size_t it) throw (Exception) {

        if (!parseToken(T_HEADER, it)) return 0;

        std::size_t n = 0;
        auto header_str = tokens[it].value;

        // Clipping the 'OPENQASM' and ';' parts of the header
        header_str = header_str.substr(8, tokens[it].value.length()-8-1);
        // Clipping the whitespace to get the version string
        while (header_str[n] == ' ' || header_str[n] == '\t') n++;
        auto version = header_str.substr(n, header_str.length()-n);

        // Splitting the major and minor version substrings 
        n = 0;
        while (version[n] != '.') n++;
        auto major_str = version.substr(0, n);
        auto minor_str = version.substr(n+1, version.length()-(n+1));

        // Currently the version values are not explicitly checked 
        // This could be added 
        // This parser expects OPENQASM version 2.0   
        uint64_t major_ver = strtoull(major_str.c_str(), nullptr, 0);
        if (errno == ERANGE) throw Exception(files.back()->filename, tokens[it].line, "Major version in the header out of range");
        uint64_t minor_ver = strtoull(minor_str.c_str(), nullptr, 0);
        if (errno == ERANGE) throw Exception(files.back()->filename, tokens[it].line, "Minor version in the header out of range");

        header = std::make_shared<Header>(major_ver, minor_ver);

        return 1;
    }

    std::size_t Parser::parseUnit(std::size_t it) throw (Exception) {

        std::size_t n = 0;

        n = parseInclude(it);
        if (n > 0)  return n;

        n = parseRegDeclaration(it);
        if (n > 0)  return n;

        n = parseOpaqueDeclaration(it);
        if (n > 0)  return n;

        n = parseGateDefinition(it);
        if (n > 0)  return n;

        n = parseProgramStatement(it);
        if (n > 0)  return n;

        throw Exception(files.back()->filename, tokens[it].line, "Unknown statement");
    }

    std::size_t Parser::parseInclude(std::size_t it) throw (Exception) {

        if (!parseToken(T_INCLUDE, it) || !parseToken(T_FILENAME, it+1) || !parseToken(';', it+2)) return 0;

        // Clipping the quotation marks from the file name 'string'
        auto filename = tokens[it+1].value.substr(1, tokens[it+1].value.length()-2);

        parse(filename);

        return 3;

    }

}
