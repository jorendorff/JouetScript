#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <string.h>

#include "lexer.h"
#include "parser.h"

#define MAX_LINE_LENGTH 2048

Parser::Parser() {
    Lexer _lexer;
    lexer = _lexer;
};

// this should return a code object....
int Parser::arithmetic() {
    int i = std::stoi(lexer.substr);
    lexer.nextToken();
    if (lexer.match(OPERATOR)) {
        lexer.backup();
        char op = lexer.currentChr();
        lexer.next();

        lexer.nextToken();
        if (!lexer.match(INT))
            throw LexerException("Expected a digit.");

        int j = std::stoi(lexer.substr);
        switch (op) {
            case '+' : return i + j;
            case '*' : return i * j;
            case '-' : return i - j;
            case '/' : return i / j;
        }
    } else if (!lexer.match(_EOF_)) {
        // yes yes this is crappy, must fix the message
        throw LexerException("Expected a digit.");
    }
    return i;
};

std::string Parser::execute(std::string line) {
    std::string _result;
    lexer.load(line);
    lexer.token = EMPTY;
    lexer.nextToken();
    if (lexer.match(INT)) {
        _result = std::to_string(this->arithmetic());
    };

    return _result;
}

int main() {
    Parser parser;
    for (;;) {
        std::cout << "> ";
        char data[MAX_LINE_LENGTH];
        std::cin.getline(data, MAX_LINE_LENGTH);
        try {
            std::cout << "> " << parser.execute(data) << std::endl;
        } catch (LexerException *ex) {
            std::cerr << ex->what() << std::endl;
        };
    }
}
