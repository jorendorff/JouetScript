#ifndef JSCRIPT_H
#define JSCRIPT_H

#include "jsvalue.h"

class JScript {
    Lexer lexer;

    public:
        JScript();
        JSValuePtr digit();
        JSValuePtr factor();
        JSValuePtr mathExp(JSValuePtr &start);
        JSValuePtr base();
        JSValuePtr execute(std::string line);

    friend Lexer;
};

#endif
