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
    BOOL,
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
    IF,
    ELSE,
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

        std::string getTokenStr(TOKEN_TYPES token);
        std::string getCurrentTokenStr();
        void next();
        void backup();
        void save();
        void saveAndNext();
        char currentChr();
        char peek();
        void skipWhiteSpace();
        bool isAlpha();
        bool isDigit();
        void nextToken();
        void prevToken();
        bool match(TOKEN_TYPES type);
        bool matchOrFail(TOKEN_TYPES type);
        void error();
        void error(const std::string);

    friend class JScript;
};

#endif
