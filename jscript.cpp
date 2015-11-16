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

JScript::JScript() {
    Lexer _lexer;
    lexer = _lexer;
};

std::unique_ptr<JSValue> JScript::arithmetic() {
    std::unique_ptr<JSValue> r;
    if (lexer.match(INT)) {
        r = std::unique_ptr<JSValue>(new JSValue(std::stoi(lexer.substr), JSVALUE_INT));
    } else if (lexer.match(FLOAT)) {
        r = std::unique_ptr<JSValue>(new JSValue(std::stof(lexer.substr), JSVALUE_FLOAT));

    }
    lexer.nextToken();
    if (lexer.match(OPERATOR)) {
        lexer.backup();
        char op = lexer.currentChr();
        lexer.next();
        lexer.nextToken();
        if (lexer.match(INT) or lexer.match(FLOAT)) {
            r = r->arithmetic(this->arithmetic(), op);
        }
    }
    return r;
};

std::unique_ptr<JSValue> JScript::execute(std::string line) {
    std::unique_ptr<JSValue> result;
    lexer.load(line);
    lexer.token = EMPTY;
    while (lexer.token != _EOF_) {
        lexer.nextToken();
        if (lexer.match(INT) or lexer.match(FLOAT)) {
            result = this->arithmetic();
            continue;
        }
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
