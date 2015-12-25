#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <string.h>
#include <sstream>
#include <vector>

#include "lexer.h"

Lexer::Lexer() {
    end_position = 0;
    prev_position = 0;
}

void Lexer::load(std::string &line) {
    buffer = buffer + line;
};

void Lexer::reset() {
    buffer = "";
    end_position = 0;
    prev_position = 0;
    token = EMPTY;
    prev_token = EMPTY;
    substr = "";
    prev_substr = "";
};


std::string Lexer::getTokenStr(TOKEN_TYPES token) {
    switch (token) {
        case EMPTY      : return "EMPTY";
        case IDENTIFIER : return "IDENTIFIER";
        case FUNCTION   : return "FUNCTION";
        case VAR        : return "VAR";
        case DIGIT      : return "DIGIT";
        case INT        : return "INT";
        case FLOAT      : return "FLOAT";
        case BOOL       : return "BOOL";
        case ALPHA      : return "ALPHA";
        case STRING     : return "STRING";
        case EQUALS     : return "EQUALS";
        case OPERATOR   : return "OPERATOR";
        case SEMICOLON  : return "SEMICOLON";
        case L_PAR      : return "L_PAR";
        case R_PAR      : return "R_PAR";
        case L_CBRACKET : return "L_CBRACKET";
        case R_CBRACKET : return "R_CBRACKET";
        case COMMA      : return "COMMA";
        case IF         : return "IF";
        case ELSE       : return "ELSE";
        case _EOF_      : return "EOF";
    }
    return "";
};

std::string Lexer::getCurrentTokenStr() {
    return getTokenStr(token);
}

void Lexer::next() {
    // allow escaping
    if (currentChr() == '\\')
        end_position++;
    end_position++;
};

void Lexer::backup() {
    end_position--;
};

void Lexer::save() {
    substr = substr + currentChr();
};

void Lexer::saveAndNext() {
    if (currentChr() == '\\')
        end_position++;
    save();
    next();
};

char Lexer::currentChr() {
    return buffer[end_position];
};

char Lexer::peek() {
    return buffer[end_position + 1];
};

void Lexer::skipWhiteSpace() {
    while (currentChr() == ' ' or
            currentChr() == '\t' or
            currentChr() == '\r' or
            currentChr() == '\n')
        next();
};

bool Lexer::isAlpha() {
    if ((currentChr() >= 'a' and currentChr() <= 'z') or
        (currentChr() >= 'A' and currentChr() <= 'Z'))
        return true;
    return false;
}

bool Lexer::isDigit() {
    if (currentChr() >= '0' and currentChr() <= '9')
        return true;
    return false;
}

void Lexer::nextToken() {
    prev_substr = substr;
    prev_position = end_position;
    prev_token = token;
    substr.clear();
    skipWhiteSpace();

    /* ALPHA KEYWORDS AND IDENTIFIERS */
    if (isAlpha()) {
        while (isAlpha()
            or isDigit()
            or currentChr() == '_') {
            saveAndNext();}
        if (substr == "var") {
            token = VAR;
            return;
        } else if (substr == "function") {
            token = FUNCTION;
            return;
        } else if (substr == "true" || substr == "false") {
            token = BOOL;
            return;
        } else if (substr == "if") {
            token = IF;
            return;
        } else if (substr == "else") {
            token = ELSE;
            return;
        } else {
            token = IDENTIFIER;
            return;
        }
    }

    /* INTS AND FLOATS */
    if (isDigit()) {
        while (isDigit())
            saveAndNext();
        if (currentChr() == '.') {
            token = FLOAT;
            saveAndNext();
            while (isDigit())
                saveAndNext();
            return;
        } else {
            token = INT;
            return;
        }
    }

    /* STRINGS */
    if (currentChr() == '"') {
        next();
        while (currentChr() != '"')
            saveAndNext();
       next();
       token = STRING;
       return;
    }

    /* OPERATORS */
    if (currentChr() == '+' or
        currentChr() == '-' or
        currentChr() == '*' or
        currentChr() == '/' or
        currentChr() == '^' or
        currentChr() == '&') {
        token = OPERATOR;
        saveAndNext();
        return;
    }

    /* SINGLE CHAR SYMBOLS */
    switch (currentChr()) {
        case '=' : token = EQUALS;      saveAndNext(); return;
        case '(' : token = L_PAR;       saveAndNext(); return;
        case ')' : token = R_PAR;       saveAndNext(); return;
        case '{' : token = L_CBRACKET;  saveAndNext(); return;
        case '}' : token = R_CBRACKET;  saveAndNext(); return;
        case ';' : token = SEMICOLON;   saveAndNext(); return;
        case ',' : token = COMMA;       saveAndNext(); return;
    }

    while (currentChr() != '\0')
        next();
    token = _EOF_;
    return;
};

void Lexer::prevToken() {
    substr = prev_substr;
    end_position = prev_position;
    token = prev_token;
}

bool Lexer::match(TOKEN_TYPES type) {
    if (token == type)
        return true;
    return false;
};

bool Lexer::matchOrFail(TOKEN_TYPES type) {
    if (token != type)
        throw new LexerException(
            "expected " + getTokenStr(type) + " but found " + getCurrentTokenStr(),
            end_position
        );
    return true;
};

void Lexer::error() {
    throw new LexerException("unexpected " + getCurrentTokenStr(), end_position);
}

void Lexer::error(const std::string msg) {
    throw new LexerException(msg, end_position);
}
