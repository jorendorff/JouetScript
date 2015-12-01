#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <string.h>

#include "lexer.h"
#include "jscript.h"
#include "jsvalue.h"

#define MAX_LINE_LENGTH 2048

JSValuePtr JScript::digit() {
    if (lexer.match(INT)) {
        return JSValuePtr(new JSValue(std::stoi(lexer.substr), JSVALUE_INT));
    } else if (lexer.match(FLOAT)) {
        return JSValuePtr(new JSValue(std::stof(lexer.substr), JSVALUE_FLOAT));
    }
    return NULL;
};

JSValuePtr JScript::factor() {
    JSValuePtr r;
    lexer.nextToken();
    r = this->base();
    lexer.nextToken();
    lexer.matchOrFail(R_PAR);
    return r;
};

JSValuePtr JScript::mathExp(JSValuePtr &start) {
    lexer.backup();
    char op = lexer.currentChr();
    lexer.next();
    lexer.nextToken();
    return start->arithmetic(this->base(), op);
};

void JScript::assignment() {
    lexer.matchOrFail(IDENTIFIER);
    std::string name = lexer.substr;

    lexer.nextToken();
    lexer.matchOrFail(EQUALS);

    lexer.nextToken();
    JSValuePtr value = this->base();
    cxt.JSValueCache.push_back(value);
    JSValueHandlePtr symbol = JSValueHandlePtr(new JSValueHandle(value, name));
    cxt.addChild(symbol);
};

JSValuePtr JScript::defineLambdaFunction() {
    std::vector<std::string> arguments;
    lexer.nextToken();
    // collect arguments
    while (!lexer.match(R_PAR)) {
        if (lexer.match(IDENTIFIER)) {
            arguments.push_back(lexer.substr);
            lexer.nextToken();
            if (lexer.match(COMMA))
                lexer.nextToken();
        } else {
            lexer.error("expected IDENTIFIER, COMMA, or R_PAR while parsing function arguments but found " + lexer.getCurrentTokenStr());
        }
    }
    lexer.nextToken();
    lexer.matchOrFail(L_CBRACKET);
    std::string function_body;
    lexer.nextToken();
    while (!lexer.match(R_CBRACKET)) {
        function_body = function_body + lexer.substr;
        if (lexer.currentChr() == ' ')
            function_body = function_body + " ";
        lexer.nextToken();
    }
    JSValuePtr val = JSValuePtr(new JSValue(function_body, JSVALUE_FUNCTION));
    val->arguments = arguments;
    return val;
};

void JScript::defineFunction() {
    std::string funcName = lexer.substr;
    lexer.nextToken();
    lexer.matchOrFail(L_PAR);
    JSValuePtr value = this->defineLambdaFunction();
    cxt.JSValueCache.push_back(value);
    JSValueHandlePtr symbol = JSValueHandlePtr(new JSValueHandle(value, funcName));
    cxt.addChild(symbol);
};

JSValuePtr JScript::base() {
    JSValuePtr val = JSValuePtr(new JSValue());

    /* DEFINE FUNCTION */
    if (lexer.match(FUNCTION)) {
        /* we'll either define or return a function depending
         * on whether or not the keyword is followed by an identifier */
        lexer.nextToken();
        if (lexer.match(L_PAR))
            return this->defineLambdaFunction();
        lexer.matchOrFail(IDENTIFIER);
        this->defineFunction();
        return val;
    };

    if (lexer.match(VAR)) {
        lexer.nextToken();
        this->assignment();
        return val;
    }

    if (lexer.match(IDENTIFIER)) {
        JSValueHandlePtr tmp = cxt.findChild(lexer.substr);
        // identifiers without a "var" prefix _must_ exist
        if (tmp == NULL)
            lexer.error("'" + lexer.substr + "' is undefined");

        lexer.nextToken();
        if (lexer.match(EQUALS)) {
            lexer.prevToken();
            this->assignment();
            return val;
        }
        val = JSValuePtr(tmp->value);

        if (lexer.match(L_PAR)) {
           return this->callFunction(val);
        }

        if (lexer.match(OPERATOR))
            return this->mathExp(val);

        lexer.prevToken();
        return val;
    };

    if (lexer.match(L_PAR)) {
       val = this->factor();
        lexer.nextToken();
        if (lexer.match(OPERATOR)) {
            return this->mathExp(val);
        }
        lexer.prevToken();
        return val;
    }

    if (lexer.match(INT) or lexer.match(FLOAT)) {
        val = this->digit();
        lexer.nextToken();
        if (lexer.match(OPERATOR)) {
            return this->mathExp(val);
        }
        lexer.prevToken();
        return val;
    }

    if (lexer.match(STRING)) {
        val = JSValuePtr(new JSValue(lexer.substr, JSVALUE_STRING));
        return val;
    }

    lexer.error("invalid symbol (" + lexer.substr + ")");
};

JSValuePtr JScript::execute(std::string line) {
    JSValuePtr result = JSValuePtr(new JSValue());
    lexer.load(line);
    while (true) {
        lexer.nextToken();
        while (lexer.token == SEMICOLON) lexer.nextToken();
        if (lexer.token == _EOF_) {
            lexer.reset();
            break;
        }
        result = this->base();
    }
    return result;
}

JSValuePtr JScript::callFunction(JSValuePtr &func) {
    std::vector<JSValuePtr> arguments;
    lexer.nextToken();
    while (!lexer.match(R_PAR)) {
        arguments.push_back(this->base());
        lexer.nextToken();
        if (!lexer.match(COMMA) and !lexer.match(R_PAR))
            lexer.error("expected COMMA, or R_PAR while parsing function arguments but found " + lexer.getCurrentTokenStr());
        if (lexer.match(COMMA))
            lexer.nextToken();
    }
    lexer.nextToken();
    // initialize the local scope and variables
    this->cxt.pushScope();
    for (int i = 0; i < func->arguments.size(); i++) {
        if (i < arguments.size()) {
            this->cxt.addChild(JSValueHandlePtr(new JSValueHandle(arguments[i], func->arguments[i])));
        } else {
            this->cxt.addChild(JSValueHandlePtr(new JSValueHandle(JSValuePtr(), func->arguments[i])));
        }
    };
    JScript tmpJScript(this->cxt);
    JSValuePtr val = this->execute(func->getString());
    // kill the function scope
    this->cxt.popScope();
    return val;
};

JScript::JScript(JSContext &cxt) : cxt(cxt) {
    Lexer lexer;
    this->cxt = cxt;
}

int main() {
    JSContext cxt;
    JScript jscript(cxt);
    for (;;) {
        try {
            /* only load new data if the lexer buffer is empty */
            if (jscript.bytesRemaining() <= 0) {
                char data[MAX_LINE_LENGTH];
                std::cout << "> ";
                std::cin.getline(data, MAX_LINE_LENGTH);
                std::cout << jscript.execute(data)->str() << std::endl;
            } else {
                std::cout << jscript.execute("")->str() << std::endl;
            }
        } catch (LexerException *ex) {
            std::cout << ex->msg << std::endl;
        };
    }
}
