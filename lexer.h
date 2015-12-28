#ifndef LEXER_H
#define LEXER_H

enum TOKEN_TYPES {
    EMPTY,
    IDENTIFIER,
    FUNCTION,
    RETURN,
    VAR,
    DIGIT,
    INT,
    FLOAT,
    BOOL,
    ALPHA,
    STRING,
    EQUALS,
    BINOP,
    SEMICOLON,
    L_PAR,
    R_PAR,
    L_CBRACKET,
    R_CBRACKET,
    COMMA,
    IF,
    ELSE,
    WHILE,
    _EOF_
};

enum BINOPS {
    _BINOP_NOT_FOUND_,
    PLUS,
    PLUS_EQ,
    MIN,
    MIN_EQ,
    MUL,
    MUL_EQ,
    DIV,
    DIV_EQ,
    XOR,
    XOR_EQ,
    AND,
    AND_EQ,
    // all ops with a triple equals form go below
    LT,
    LT_EQ,
    //LT_EQ_EQ,
    GT,
    GT_EQ,
    //GT_EQ_EQ,
    EQ_EQ,
    //EQ_EQ_EQ,
    NOT_EQ,
    //NOT_EQ_EQ,
};

// TODO: Stop being lazy and add precedences

class LexerException : std::exception
{
    public:
        std::string msg;
        LexerException(const std::string _msg, int column) {
            msg = _msg + " : at column " + std::to_string(column);
        }
};

struct LexerState {
    TOKEN_TYPES token;
    TOKEN_TYPES prev_token;
    int end_position;
    int prev_position;
    std::string substr;
    std::string prev_substr;
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
        struct LexerState dumpLexerState();
        void loadLexerState(struct LexerState state);
        void next();
        void backup();
        void save();
        void saveAndNext();
        char currentChr();
        char peek();
        void skipWhiteSpace();
        void skipComments();
        bool isAlpha();
        bool isDigit();
        void nextToken();
        void prevToken();
        BINOPS binOp();
        bool match(TOKEN_TYPES type);
        bool matchOrFail(TOKEN_TYPES type);
        void error();
        void error(const std::string);

    friend class JScript;
};

#endif
