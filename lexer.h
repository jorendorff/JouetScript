#ifndef LEXER_H
#define LEXER_H

enum TOKEN_TYPES {
    EMPTY,
    IDENTIFIER,
    VAR,
    DIGIT,
    INT,
    FLOAT,
    ALPHA,
    STRING,
    EQUALS,
    OPERATOR,
    SEMICOLON,
    _EOF_
};

class LexerException : std::exception {
    std::string _msg;
    public:
    LexerException(std::string _msg) { this->_msg = _msg; }
    const char* what() const noexcept {return _msg.c_str();}
};

class Lexer {

   public:

        Lexer();
        std::string getCurrentTokenStr();
        void load(std::string &line);
        void next();
        char currentChr();
        char peek();
        void match(TOKEN_TYPES);
        void nextToken();

    protected:

        TOKEN_TYPES token;
        int start_position;
        int end_position;
        std::string buffer;

    friend class Parser;
};

#endif
