#include <string>
#include <memory>
#include <iostream>
#include <sstream>
#include "jsvalue.h"

JSValue::JSValue() {
    flags = JSVALUE_UNDEFINED;
    locals = std::map<std::string, JSValuePtr>();
}

JSValue::JSValue(JSValuePtr parent) {
    flags = JSVALUE_UNDEFINED;
    locals = std::map<std::string, JSValuePtr>();
    this->parent = parent;
}

JSValue::JSValue(JSValuePtr parent, int data, JSVALUE_FLAGS flags) {
    intData = data;
    this->flags = flags;
    marked = false;
    locals = std::map<std::string, JSValuePtr>();
    this->parent = parent;
}

int JSValue::getInt() {
    return intData;
}

JSValue::JSValue(JSValuePtr parent, float data, JSVALUE_FLAGS flags) {
    floatData = data;
    this->flags = flags;
    marked = false;
    locals = std::map<std::string, JSValuePtr>();
    this->parent = parent;
}

float JSValue::getFloat() {
    return floatData;
}

JSValue::JSValue(JSValuePtr parent, std::string data, JSVALUE_FLAGS flags) {
    this->data = data;
    this->flags = flags;
    marked = false;
    locals = std::map<std::string, JSValuePtr>();
    this->parent = parent;
}

std::string JSValue::getString() {
    return data;
}

JSValue::JSValue(JSValuePtr parent, bool data, JSVALUE_FLAGS flags) {
    intData = data ? 1 : 0;
    this->flags = flags;
    marked = false;
    locals = std::map<std::string, JSValuePtr>();
    this->parent = parent;
}

bool JSValue::getBool() {
    return intData ? true : false;
}

std::string JSValue::str() {
    std::ostringstream out;
    if (isInt()) {
        out << getInt();
    } else if (isFloat()) {
        out << getFloat();
    } else if (isString()) {
        out << getString();
    } else if (isBool()) {
        out << getBool();
    } else if (isFunction()) {
        out << "<function: " << getString() << " | args: ";
        for (auto arg: arguments) {
            out << arg << " ";
        }
        out << ">";
    } else if (isUndefined()) {
        out << "undefined";
    }
    return out.str();
}

template <typename T>
T arithmetic(T a, T b, BINOPS op) {
    switch (op) {
        case PLUS : return a + b;
        case MUL  : return a * b;
        case MIN  : return a - b;
        case DIV  : return a / b;
        default   : throw;
    }
}

JSValuePtr JSValue::binOp(JSValuePtr value, BINOPS op) {
    // Arithmethic
    if (op == MIN || op == PLUS || op == MUL || op == DIV) {
        if (isFloat() or value->isFloat()) {
            float af, bf, cf;
            af = isFloat() ? getFloat() : (float)getInt();
            bf = value->isFloat() ? value->getFloat() : (float)value->getInt();
            cf = arithmetic(af, bf, op);
            return JSValuePtr(new JSValue(value->parent, cf, JSVALUE_FLOAT));
        } else if (isInt() and value->isInt()) {
            int a, b, c;
            a = getInt();
            b = value->getInt();
            c = arithmetic(a, b, op);
            return JSValuePtr(new JSValue(value->parent, c, JSVALUE_INT));
        }
    }

    // TODO: support strings
    unsigned int l, r;
    l = isFloat() ? (int)getFloat() : getInt();
    r = value->isFloat() ? (int)value->getFloat() : value->getInt();

    // Logic operators
    if (op == XOR || op == AND) {
        if (op == XOR)
            l = l ^ r;
        if (op == AND)
            l = l & r;
        if (isFloat())
            return JSValuePtr(new JSValue(value->parent, (float)l, JSVALUE_FLOAT));
        return JSValuePtr(new JSValue(value->parent, (int)l, JSVALUE_INT));

    }

    // Comparrison operators
    auto result = JSValuePtr(new JSValue(value->parent, false, JSVALUE_BOOL));
    switch(op) {
        case LT     : result->setBool(l < r);   break;
        case LT_EQ  : result->setBool(l <= r);  break;
        case GT     : result->setBool(l > r);   break;
        case GT_EQ  : result->setBool(l >= r);  break;
        case NOT_EQ : result->setBool(l != r);  break;
        default     : result = JSValuePtr(new JSValue());
    }
    return result;
}

JSContext::JSContext() {
    JSScopeChain = std::vector<JSValuePtr>();
    JSScopeChain.push_back(JSValuePtr(new JSValue()));
}

void JSContext::pushScope(JSValuePtr value) {
    JSScopeChain.push_back(value);
}

void JSContext::popScope() {
    JSScopeChain.pop_back();
}

JSValuePtr JSContext::getCurrentScope() {
    return JSScopeChain[JSScopeChain.size() - 1];
}

void JSContext::storeValueByName(std::string name, JSValuePtr value) {
    getCurrentScope()->locals[name] = value;
}

void JSContext::overwriteNamedValue(std::string name, JSValuePtr original, JSValuePtr value) {
    JSValuePtr current = getCurrentScope();
    while (current) {
        if (current->locals[name]) {
            current->locals[name] = value;
            break;
        }
        current = current->parent;
    }
}

JSValuePtr JSContext::lookupValueByName(std::string name) {
    JSValuePtr current = getCurrentScope();
    while (current) {
        if (current->locals[name])
            return current->locals[name];
        current = current->parent;
    }
    return NULL;
}
