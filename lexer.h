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
        void load(std::string &line);

    protected:

        TOKEN_TYPES token;
        int end_position;
        std::string buffer;
        std::string substr;

        std::string getCurrentTokenStr();
        char currentChr();
        char peek();
        void nextToken();
        void next();
        void backup();
        void save();
        void saveAndNext();
        bool isAlpha();
        bool isDigit();
        bool match(TOKEN_TYPES type);
        void skipWhiteSpace();

    friend class Parser;
};

#endif
