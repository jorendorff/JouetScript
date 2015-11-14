#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <string.h>
#include <vector>

#include "lexer.h"

Lexer::Lexer() {
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
    // allow escaping
    if (this->currentChr() == '\\')
        end_position++;
    end_position++;
};

void Lexer::save() {
    substr = substr + this->currentChr();
};

void Lexer::saveAndNext() {
    this->save();
    this->next();
};

char Lexer::currentChr() {
    return buffer[end_position];
};

char Lexer::peek() {
    return buffer[end_position + 1];
};

void Lexer::skipWhiteSpace() {
    while (this->currentChr() == ' ' or
            this->currentChr() == '\t' or
            this->currentChr() == '\n')
        this->next();
};

void Lexer::backup() {
    end_position--;
};

bool Lexer::isAlpha() {
    if ((this->currentChr() >= 'a' and this->currentChr() <= 'z') or
        (this->currentChr() >= 'A' and this->currentChr() <= 'Z'))
        return true;
    return false;
}

bool Lexer::isDigit() {
    if (this->currentChr() >= '0' and this->currentChr() <= '9')
        return true;
    return false;
}

void Lexer::nextToken() {
    substr.clear();
    this->skipWhiteSpace();

    /* ALPHA KEYWORDS AND IDENTIFIERS */
    if (this->isAlpha()) {
        while (this->isAlpha() or this->isDigit()) {
            this->saveAndNext();}
        if (substr == "var") {
            token = VAR;
            return;
        } else {
            token = IDENTIFIER;
            return;
        }
    }

    /* INTS AND FLOATS */
    if (this->isDigit()) {
        while (this->isDigit())
            this->saveAndNext();
        if (this->currentChr() == '.') {
            token = FLOAT;
            this->saveAndNext();
            while (this->isDigit())
                this->saveAndNext();
            return;
        } else {
            token = INT;
            return;
        }
    }

    /* OPERATORS */
    if (this->currentChr() == '+' or
        this->currentChr() == '-' or
        this->currentChr() == '*' or
        this->currentChr() == '/') {
        token = OPERATOR;
        this->saveAndNext();
        return;
    }

    /* EQUALS */
    if (this->currentChr() == '=') {
        token = EQUALS;
        this->saveAndNext();
        return;
    }

    while (this->currentChr() != '\0')
        this->next();
    token = _EOF_;
    return;
};

bool Lexer::match(TOKEN_TYPES type) {
    if (token == type)
        return true;
    return false;
};

void Lexer::load(std::string &line) {
    buffer = buffer + line;
};
