#ifndef JSCRIPT_H
#define JSCRIPT_H

#include "jsvalue.h"

class JScript {
    Lexer lexer;

    public:
        JScript();
        std::shared_ptr<JSValue> arithmetic();
        std::shared_ptr<JSValue> execute(std::string line);

    friend Lexer;
};

#endif
