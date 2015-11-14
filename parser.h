#ifndef PARSER_H
#define PARSER_H

class Parser {
    Lexer lexer;

    public:
        Parser();
        std::string execute(std::string line);
        int arithmetic();

    friend Lexer;
};

#endif
