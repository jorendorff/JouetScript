#ifndef JSCRIPT_H
#define JSCRIPT_H

#include "jsvalue.h"

class JScript {
    Lexer lexer;
    JSContext cxt;

    public:
        /* determine whether our buffer is empty */
        int bytesRemaining() { return lexer.buffer.size(); }

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
