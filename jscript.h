#ifndef JSCRIPT_H
#define JSCRIPT_H

#include "jsvalue.h"

class JScript {
    Lexer lexer;

    public:
        JSContext &cxt;

        JScript(JSContext&);
        JSValuePtr execute(std::string line);
        JSValuePtr callFunction(JSValuePtr);
        int bytesRemaining() { return lexer.buffer.size(); }

        /* parsing functions */
        JSValuePtr digit();
        JSValuePtr block();
        JSValuePtr factor();
        JSValuePtr mathExp(JSValuePtr &start);
        void assignment();
        JSValuePtr ifStatement();
        JSValuePtr whileLoop();
        void defineFunction();
        JSValuePtr defineLambdaFunction();
        JSValuePtr base();
};

#endif
