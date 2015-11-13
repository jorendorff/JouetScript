#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <string.h>
#include <vector>

#include "lexer.h"
#include "parser.h"

#define MAX_LINE_LENGTH 2048

Parser::Parser() {
    Lexer _lexer;
    lexer = _lexer;
};

std::string Parser::execute(std::string line) {
    std::string _result;

    lexer.load(line);
    while (lexer.token != _EOF_) {
        lexer.nextToken();
        _result = _result + lexer.getCurrentTokenStr();
        _result = _result + " ";
    }
    return _result;
}

int main() {
    Parser parser;
    for (;;) {
        std::cout << "> ";
        char data[MAX_LINE_LENGTH];
        std::cin.getline(data, MAX_LINE_LENGTH);
        std::cout << "> " << parser.execute(data);
    }
}
