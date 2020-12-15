#include <SourceFile.h>

namespace kazm {

    SourceFile::SourceFile(const std::string& f) throw (Exception): 
        filename(f),
        file(filename)
    {
        if (!file.is_open()) throw Exception("Unable to open " + filename);
        lexer.in() = file;
    }	
    
    Token SourceFile::scan() {
        return lexer.scan();
    }
}
