#ifndef SOURCEFILE_H
#define SOURCEFILE_H

#include <string>
#include <fstream>

#include <Exception.h>
#include <Scanner.h>
#include <Token.h>

namespace kazm {

    struct SourceFile {
    
        std::string filename;
        std::ifstream file;
        Scanner lexer;
        
        SourceFile(const std::string&) throw (Exception);
        
        Token scan();
    };

}

#endif
