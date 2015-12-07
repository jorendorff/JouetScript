#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <string.h>

#include "lexer.h"
#include "jscript.h"
#include "jsvalue.h"

JSValuePtr JScript::digit() {
    JSValuePtr val, zero_val;
    char op; // if we see this, we need to do arithmetic on our value
    if (lexer.match(OPERATOR)) {
        lexer.backup();
        op = lexer.currentChr();
        lexer.next();
        zero_val = JSValuePtr(new JSValue(0, JSVALUE_INT));
        lexer.nextToken();
    }
    if (lexer.match(INT)) {
        val = JSValuePtr(new JSValue(std::stoi(lexer.substr), JSVALUE_INT));
    } else if (lexer.match(FLOAT)) {
        val = JSValuePtr(new JSValue(std::stof(lexer.substr), JSVALUE_FLOAT));
    }
    if (op && val) {
        zero_val = JSValuePtr(new JSValue(0, JSVALUE_INT));
        val = zero_val->arithmetic(val, op);
    }
    return val;
};

JSValuePtr JScript::factor() {
    JSValuePtr r;
    lexer.matchOrFail(L_PAR);
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

JSValuePtr JScript::block() {
    JSValuePtr val;
    lexer.matchOrFail(L_CBRACKET);
    lexer.nextToken();
    if (lexer.match(R_CBRACKET))
        return val;
    cxt.pushScope();
    val = this->base();
    cxt.popScope();
    lexer.nextToken();
    lexer.matchOrFail(R_CBRACKET);
    return val;
};

JSValuePtr JScript::ifStatement() {
    lexer.matchOrFail(IF);
    lexer.nextToken();
    JSValuePtr if_val, val;
    if_val = this->factor();
    lexer.nextToken();
    if (if_val->getBool()) {
        val = this->block();
        lexer.nextToken();
        if (lexer.match(ELSE)) {
            lexer.nextToken();
            lexer.matchOrFail(L_CBRACKET);
            while (!lexer.match(R_CBRACKET))
                lexer.nextToken();
        }
    } else {
        while (!lexer.match(R_CBRACKET))
            lexer.nextToken();
        lexer.nextToken();
        if (lexer.match(ELSE)) {
            lexer.nextToken();
            val = this->block();
        }
    }

    if (!val)
        val = JSValuePtr(new JSValue());
    return val;
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
    int cbrackets = 1;
    while (!lexer.match(_EOF_)) {
        if (lexer.match(L_CBRACKET))
            cbrackets++;
        if (lexer.match(R_CBRACKET))
            cbrackets--;
        if (cbrackets <= 0)
            break;
        if (lexer.match(STRING)) { // add quotes
            function_body = function_body + '"' + lexer.substr + '"';
        } else {
            function_body = function_body + lexer.substr;
        }
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
    JSValuePtr val;

    /* DEFINE FUNCTION */
    if (lexer.match(FUNCTION)) {
        /* we'll either define or return a function depending
         * on whether or not the keyword is followed by an identifier */
        lexer.nextToken();
        if (lexer.match(L_PAR))
            return this->defineLambdaFunction();
        lexer.matchOrFail(IDENTIFIER);
        this->defineFunction();
        return JSValuePtr(new JSValue());
    };

    if (lexer.match(IF)) {
        return this->ifStatement();
    }

    if (lexer.match(VAR)) {
        lexer.nextToken();
        this->assignment();
        return JSValuePtr(new JSValue());
    }

    if (lexer.match(IDENTIFIER)) {
        JSValueHandlePtr tmp = cxt.findChild(lexer.substr);
        // identifiers without a "var" prefix _must_ exist
        if (tmp == NULL)
            lexer.error("'" + lexer.substr + "' is undefined");

        lexer.nextToken();
        if (lexer.match(EQUALS)) {
            lexer.nextToken();
            tmp->value = this->base();
            return JSValuePtr(new JSValue());
        }

        val = tmp->value;

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

    if (lexer.match(INT) or lexer.match(FLOAT) or lexer.match(OPERATOR)) {
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

    if (lexer.match(BOOL)) {
        if (lexer.substr == "true")
            return JSValuePtr(new JSValue(true, JSVALUE_BOOL));
        return JSValuePtr(new JSValue(false, JSVALUE_BOOL));
    }

    lexer.error("invalid symbol (" + lexer.substr + ")");
};

JSValuePtr JScript::execute(std::string line) {
    JSValuePtr result = JSValuePtr(new JSValue());
    lexer.load(line);
    while (true) {
        lexer.nextToken();
        /* IGNORE SEMICOLONS */
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
    JSValuePtr val = tmpJScript.execute(func->getString());
    // kill the function scope
    this->cxt.popScope();
    return val;
};

JScript::JScript(JSContext &cxt) : cxt(cxt) {
    Lexer lexer;
    this->cxt = cxt;
}
