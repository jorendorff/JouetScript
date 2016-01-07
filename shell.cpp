#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>

#include "lexer.h"
#include "jscript.h"
#include "jsvalue.h"

#define MAX_LINE_LENGTH 2048

int main(int argc, char *argv[]) {
    JSContext cxt;
    JScript jscript(cxt);
    if (argc > 1) {
        try {
            std::ifstream t(argv[1]);
            std::stringstream buffer;
            buffer << t.rdbuf();
            std::cout << jscript.execute(buffer.str())->str() << std::endl;
        } catch (LexerException *ex) {
            std::cout << ex->msg << std::endl;
            return 1;
        }
        return 0;
    }
    for (;;) {
        try {
            /* only load new data if the lexer buffer is empty */
            if (jscript.bytesRemaining() <= 0) {
                char data[MAX_LINE_LENGTH];
                std::cout << "JouetScript$ ";
                if (!std::cin.getline(data, MAX_LINE_LENGTH)) {
                    std::cout << std::endl;
                    break;
                }
                std::cout << jscript.execute(data)->str() << std::endl;
            } else {
                std::cout << jscript.execute("")->str() << std::endl;
            }
        } catch (LexerException *ex) {
            std::cout << ex->msg << std::endl;
        }
    }
}
