#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <string.h>
#include <vector>

#include "lexer.h"

Lexer::Lexer() {
    start_position = 0;
    end_position = 0;
}

std::string Lexer::getCurrentTokenStr() {
    switch (token) {
        case EMPTY : return "EMPTY";
        case IDENTIFIER : return "IDENTIFIER";
        case VAR : return "VAR";
        case DIGIT : return "DIGIT";
        case INT : return "INT";
        case FLOAT : return "FLOAT";
        case ALPHA : return "ALPHA";
        case STRING : return "STRING";
        case EQUALS : return "EQUALS";
        case OPERATOR : return "OPERATOR";
        case SEMICOLON : return "SEMICOLON";
        case _EOF_ : return "EOF";
        default : throw new LexerException("Unknown Symbol");
    }
}

void Lexer::next() {
    end_position++;
};

char Lexer::currentChr() {

};

void Lexer::nextToken() {
};

void Lexer::load(std::string &line) {
    buffer = line;
}
