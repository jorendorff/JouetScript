#ifndef JSCRIPT_H
#define JSCRIPT_H

#include "jsvalue.h"

class JScript {
    Lexer lexer;

    public:
        JScript();
        std::unique_ptr<JSValue> arithmetic();
        std::unique_ptr<JSValue> execute(std::string line);

    friend Lexer;
};

#endif
