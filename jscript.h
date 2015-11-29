#ifndef JSCRIPT_H
#define JSCRIPT_H

#include "jsvalue.h"

class JScript {
    Lexer lexer;
    JSContext cxt;

    public:
        JSValuePtr digit();
        JSValuePtr factor();
        JSValuePtr mathExp(JSValuePtr &start);
        void assignment();
        JSValuePtr base();
        JSValuePtr execute(std::string line);
};

#endif
