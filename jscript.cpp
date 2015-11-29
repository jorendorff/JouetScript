#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <string.h>

#include "lexer.h"
#include "jscript.h"
#include "jsvalue.h"

#define MAX_LINE_LENGTH 2048

JSValuePtr JScript::digit() {
    if (lexer.match(INT)) {
        return JSValuePtr(new JSValue(std::stoi(lexer.substr), JSVALUE_INT));
    } else if (lexer.match(FLOAT)) {
        return JSValuePtr(new JSValue(std::stof(lexer.substr), JSVALUE_FLOAT));
    }
    return NULL;
};

JSValuePtr JScript::factor() {
    JSValuePtr r;
    lexer.nextToken();
    r = this->base();
    lexer.nextToken();
    if (!lexer.match(R_PAR))
        throw;
    return r;
};

JSValuePtr JScript::mathExp(JSValuePtr &start) {
    lexer.backup();
    char op = lexer.currentChr();
    lexer.next();
    lexer.nextToken();
    return start->arithmetic(this->base(), op);
};

void JScript::assignment() {
    if (!lexer.match(IDENTIFIER))
        throw;

    std::string name = lexer.substr;

    lexer.nextToken();
    if (!lexer.match(EQUALS))
        throw;

    lexer.nextToken();
    JSValuePtr value = this->base();
    cxt.JSValueCache.push_back(value);
    JSValueHandlePtr symbol = JSValueHandlePtr(new JSValueHandle(&*value, name));
    cxt.addChild(symbol);
};

JSValuePtr JScript::base() {
    JSValuePtr val = JSValuePtr(new JSValue());

    if (lexer.match(VAR)) {
        lexer.nextToken();
        this->assignment();
        return val;
    }

    if (lexer.match(IDENTIFIER)) {
        JSValueHandlePtr tmp = cxt.findChild(lexer.substr);
        // identifiers without a "var" prefix _must_ exist
        if (tmp == NULL)
            throw;

        lexer.nextToken();
        if (lexer.match(EQUALS)) {
            lexer.prevToken();
            this->assignment();
            return val;
        }

        val = JSValuePtr(tmp->value);
        if (lexer.match(OPERATOR))
            return this->mathExp(val);

        lexer.prevToken();
        return val;
    };

    if (lexer.match(L_PAR)) {
       val = this->factor();
        lexer.nextToken();
        if (lexer.match(OPERATOR)) {
            return this->mathExp(val);
        }
        lexer.prevToken();
        return val;
    }

    if (lexer.match(INT) or lexer.match(FLOAT)) {
        val = this->digit();
        lexer.nextToken();
        if (lexer.match(OPERATOR)) {
            return this->mathExp(val);
        }
        lexer.prevToken();
        return val;
    }
    throw;
};

JSValuePtr JScript::execute(std::string line) {
    JSValuePtr result = JSValuePtr(new JSValue());
    lexer.load(line);
    lexer.token = EMPTY;
    while (true) {
        lexer.nextToken();
        if (lexer.token == _EOF_) break;
        result = this->base();
    }
    return result;
}

int main() {
    JScript jscript;
    for (;;) {
        std::cout << "> ";
        char data[MAX_LINE_LENGTH];
        std::cin.getline(data, MAX_LINE_LENGTH);
        try {
            std::cout << "> " << jscript.execute(data)->str() << std::endl;
        } catch (LexerException *ex) {
            std::cerr << ex->what() << std::endl;
        };
    }
}
