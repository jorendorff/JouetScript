#ifndef JSCRIPT_H
#define JSCRIPT_H

#include "jsvalue.h"

class JScript {
    Lexer lexer;

    public:
        JSContext &cxt;

        /* determine whether our buffer is empty */
        JScript(JSContext&);
        int bytesRemaining() { return lexer.buffer.size(); }
        JSValuePtr callFunction(JSValuePtr&);

        /* parsing functions */
        JSValuePtr digit();
        JSValuePtr factor();
        JSValuePtr mathExp(JSValuePtr &start);
        void assignment();
        void defineFunction();
        JSValuePtr defineLambdaFunction();
        JSValuePtr base();
        JSValuePtr execute(std::string line);
};

#endif
