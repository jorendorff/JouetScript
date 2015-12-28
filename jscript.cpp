#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <string.h>

#include "lexer.h"
#include "jscript.h"
#include "jsvalue.h"

JScript::JScript(JSContext &cxt) : cxt(cxt) {
    Lexer lexer;
    this->cxt = cxt;
}

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
        result = base();
    }
    return result;
};

JSValuePtr JScript::callFunction(JSValuePtr func) {
    std::vector<JSValuePtr> arguments;
    lexer.nextToken();
    while (!lexer.match(R_PAR)) {
        arguments.push_back(base());
        lexer.nextToken();
        if (lexer.match(COMMA))
            lexer.nextToken();
    }
    // initialize the local scope and variables
    cxt.pushScope(JSValuePtr(new JSValue(func->parent)));
    for (unsigned int i = 0; i < func->arguments.size(); i++) {
        if (i < arguments.size()) {
            cxt.storeValueByName(func->arguments[i], arguments[i]);
        } else {
            cxt.storeValueByName(func->arguments[i], JSValuePtr());
        }
    };
    JScript tmpJScript(cxt);
    JSValuePtr val = tmpJScript.execute(func->getString());
    // kill the function scope
    cxt.popScope();
    return val;
};

JSValuePtr JScript::digit() {
    JSValuePtr val, zero_val;
    char op = 0; // if we see this, we need to do arithmetic on our value
    if (lexer.match(OPERATOR)) {
        lexer.backup();
        op = lexer.currentChr();
        lexer.next();
        zero_val = JSValuePtr(new JSValue(cxt.getCurrentScope(), 0, JSVALUE_INT));
        lexer.nextToken();
    }
    if (lexer.match(INT)) {
        val = JSValuePtr(new JSValue(cxt.getCurrentScope(), std::stoi(lexer.substr), JSVALUE_INT));
    } else if (lexer.match(FLOAT)) {
        val = JSValuePtr(new JSValue(cxt.getCurrentScope(), std::stof(lexer.substr), JSVALUE_FLOAT));
    }
    if (op && val) {
        zero_val = JSValuePtr(new JSValue(cxt.getCurrentScope(), 0, JSVALUE_INT));
        val = zero_val->arithmetic(val, op);
    }
    return val;
};

JSValuePtr JScript::factor() {
    JSValuePtr r;
    lexer.matchOrFail(L_PAR);
    lexer.nextToken();
    r = base();
    lexer.nextToken();
    lexer.matchOrFail(R_PAR);
    return r;
};

JSValuePtr JScript::mathExp(JSValuePtr &start) {
    lexer.backup();
    char op = lexer.currentChr();
    lexer.next();
    lexer.nextToken();
    return start->arithmetic(base(), op);
};

void JScript::assignment() {
    lexer.matchOrFail(IDENTIFIER);
    std::string name = lexer.substr;

    lexer.nextToken();
    lexer.matchOrFail(EQUALS);

    lexer.nextToken();
    JSValuePtr value = base();
    cxt.storeValueByName(name, value);
};

JSValuePtr JScript::block() {
    JSValuePtr val;
    lexer.matchOrFail(L_CBRACKET);
    int cbrackets = 1;
    while (!lexer.match(_EOF_)) {
        lexer.nextToken();
        if (lexer.match(L_CBRACKET))
            cbrackets++;
        if (lexer.match(R_CBRACKET))
            cbrackets--;
        if (cbrackets <= 0)
            break;
        val = base();
    }
    return val;
};

JSValuePtr JScript::ifStatement() {
    lexer.matchOrFail(IF);
    lexer.nextToken();
    JSValuePtr if_val, val;
    if_val = factor();
    lexer.nextToken();
    cxt.pushScope(JSValuePtr(new JSValue(cxt.getCurrentScope())));
    if (if_val->getBool()) {
        val = block();
        if (!lexer.match(_EOF_)) {
            lexer.nextToken();
            if (lexer.match(ELSE)) {
                lexer.nextToken();
                lexer.matchOrFail(L_CBRACKET);
                int cbrackets = 1;
                while (cbrackets > 0) {
                    lexer.nextToken();
                    if (lexer.match(L_CBRACKET))
                        cbrackets++;
                    if (lexer.match(R_CBRACKET))
                        cbrackets--;
                }
            }
        }
    } else {
        while (!lexer.match(R_CBRACKET))
            lexer.nextToken();
        lexer.nextToken();
        if (lexer.match(ELSE)) {
            lexer.nextToken();
            val = block();
        }
    }
    cxt.popScope();
    if (!val)
        val = JSValuePtr(new JSValue());
    return val;
};

JSValuePtr JScript::whileLoop() {
    lexer.matchOrFail(WHILE);
    lexer.nextToken();
    // we'll save our state here for looping
    auto state = lexer.dumpLexerState();
    JSValuePtr if_val, val;
    cxt.pushScope(JSValuePtr(new JSValue(cxt.getCurrentScope())));
    if_val = factor();
    while (if_val->getBool()) {
        lexer.nextToken();
        val = block();
        lexer.loadLexerState(state);
        if_val = factor();
    }
    cxt.popScope();
    // finally skip over the whole block
    lexer.nextToken();
    lexer.matchOrFail(L_CBRACKET);
    int cbrackets = 1;
    while (cbrackets > 0) {
        lexer.nextToken();
        if (lexer.match(L_CBRACKET))
            cbrackets++;
        if (lexer.match(R_CBRACKET))
            cbrackets--;
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
    JSValuePtr val = JSValuePtr(new JSValue(cxt.getCurrentScope(), function_body, JSVALUE_FUNCTION));
    val->arguments = arguments;
    return val;
};

void JScript::defineFunction() {
    std::string funcName = lexer.substr;
    lexer.nextToken();
    lexer.matchOrFail(L_PAR);
    JSValuePtr value = defineLambdaFunction();
    cxt.storeValueByName(funcName, value);
};

JSValuePtr JScript::base() {
    JSValuePtr val;

    /* DEFINE FUNCTION */
    if (lexer.match(FUNCTION) && !val) {
        /* we'll either define or return a function depending
         * on whether or not the keyword is followed by an identifier */
        lexer.nextToken();
        if (lexer.match(L_PAR)) {
            val = defineLambdaFunction();
        } else {
            lexer.matchOrFail(IDENTIFIER);
            defineFunction();
            val = JSValuePtr(new JSValue());
        }
    }

    if (lexer.match(RETURN) && !val) {
        lexer.nextToken();
        val = base();
        // Skip any remaining code
        while (!lexer.match(_EOF_))
            lexer.nextToken();
    }

    if (lexer.match(IF) && !val) {
        val = ifStatement();
    }

    if (lexer.match(WHILE) && !val) {
        val = whileLoop();
    }

    if (lexer.match(VAR) && !val) {
        lexer.nextToken();
        assignment();
        val = JSValuePtr(new JSValue());
    }

    if (lexer.match(IDENTIFIER) && !val) {
        std::string name = lexer.substr;
        JSValuePtr tmp = cxt.lookupValueByName(lexer.substr);
        // identifiers without a "var" prefix _must_ exist
        if (tmp == NULL)
            lexer.error("'" + lexer.substr + "' is undefined");

        val = tmp;
        lexer.nextToken();
        if (lexer.match(EQUALS)) {
            lexer.nextToken();
            tmp = base();
            cxt.overwriteNamedValue(name, val, tmp);
            val = JSValuePtr(new JSValue());
        } else if (lexer.match(L_PAR)) {
            val = callFunction(val);
        } else if (lexer.match(OPERATOR)) {
            val = mathExp(val);
        } else {
            lexer.prevToken();
        }
    }

    if (lexer.match(L_PAR) && !val) {
        val = factor();
        lexer.nextToken();
        if (lexer.match(OPERATOR)) {
            val = mathExp(val);
        } else {
            lexer.prevToken();
        }
    }

    if ((lexer.match(INT) or lexer.match(FLOAT) or lexer.match(OPERATOR)) && !val) {
        val = digit();
        lexer.nextToken();
        if (lexer.match(OPERATOR)) {
            val = mathExp(val);
        } else {
            lexer.prevToken();
        }
    }

    if (lexer.match(STRING) && !val) {
        val = JSValuePtr(new JSValue(cxt.getCurrentScope(), lexer.substr, JSVALUE_STRING));
        return val;
    }

    if (lexer.match(BOOL) && !val) {
        if (lexer.substr == "true") {
            val = JSValuePtr(new JSValue(cxt.getCurrentScope(), true, JSVALUE_BOOL));
        } else {
            val = JSValuePtr(new JSValue(cxt.getCurrentScope(), false, JSVALUE_BOOL));
        }
    }

    if (!val)
        lexer.error("invalid symbol (" + lexer.substr + ")");

    // skip semicolons
    lexer.nextToken();
    if (lexer.token != SEMICOLON)
        lexer.prevToken();

    return val;
};

