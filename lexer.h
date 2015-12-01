#ifndef LEXER_H
#define LEXER_H

enum TOKEN_TYPES {
    EMPTY,
    IDENTIFIER,
    FUNCTION,
    VAR,
    DIGIT,
    INT,
    FLOAT,
    ALPHA,
    STRING,
    EQUALS,
    OPERATOR,
    SEMICOLON,
    L_PAR,
    R_PAR,
    L_CBRACKET,
    R_CBRACKET,
    COMMA,
    _EOF_
};

class LexerException : std::exception
{
    public:
        std::string msg;
        LexerException(const std::string _msg, int column) {
            msg = _msg + " : at column " + std::to_string(column);
        }
};

class Lexer {

   public:

        Lexer();
        void load(std::string &line);
        void reset();

    protected:

        TOKEN_TYPES token;
        TOKEN_TYPES prev_token;
        int end_position;
        int prev_position;
        std::string buffer;
        std::string substr;
        std::string prev_substr;

        std::string getCurrentTokenStr();
        char currentChr();
        char peek();
        void nextToken();
        void prevToken();
        void next();
        void backup();
        void save();
        void saveAndNext();
        bool isAlpha();
        bool isDigit();
        bool match(TOKEN_TYPES type);
        void skipWhiteSpace();
        void error();
        void error(const std::string);

    friend class JScript;
};

#endif
