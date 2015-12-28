#include <string>
#include <memory>
#include <iostream>
#include <sstream>
#include "jsvalue.h"

JSValue::JSValue() {
    flags = JSVALUE_UNDEFINED;
    locals = std::map<std::string, JSValuePtr>();
};

JSValue::JSValue(JSValuePtr parent) {
    flags = JSVALUE_UNDEFINED;
    locals = std::map<std::string, JSValuePtr>();
    this->parent = parent;
};

JSValue::JSValue(JSValuePtr parent, int data, JSVALUE_FLAGS flags) {
    intData = data;
    this->flags = flags;
    marked = false;
    locals = std::map<std::string, JSValuePtr>();
    this->parent = parent;
};

int JSValue::getInt() {
    return intData;
}

JSValue::JSValue(JSValuePtr parent, float data, JSVALUE_FLAGS flags) {
    floatData = data;
    this->flags = flags;
    marked = false;
    locals = std::map<std::string, JSValuePtr>();
    this->parent = parent;
};

float JSValue::getFloat() {
    return floatData;
}

JSValue::JSValue(JSValuePtr parent, std::string data, JSVALUE_FLAGS flags) {
    this->data = data;
    this->flags = flags;
    marked = false;
    locals = std::map<std::string, JSValuePtr>();
    this->parent = parent;
};

std::string JSValue::getString() {
    return data;
}

JSValue::JSValue(JSValuePtr parent, bool data, JSVALUE_FLAGS flags) {
    intData = data ? 1 : 0;
    this->flags = flags;
    marked = false;
    locals = std::map<std::string, JSValuePtr>();
    this->parent = parent;
};

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
T mathOp(T a, T b, char op) {
    switch (op) {
        case '+' : return a + b;
        case '*' : return a * b;
        case '-' : return a - b;
        case '/' : return a / b;
        case '^' : return (char)a ^ (char)b;
        case '&' : return (char)a & (char)b;
    }
    throw;
};

JSValuePtr JSValue::arithmetic(JSValuePtr value, char op) {
    if (isFloat() or value->isFloat()) {
        float af, bf, cf;
        af = isFloat() ? getFloat() : (float)getInt();
        bf = value->isFloat() ? value->getFloat() : (float)value->getInt();
        cf = mathOp(af, bf, op);
        return JSValuePtr(new JSValue(value->parent, cf, JSVALUE_FLOAT));
    } else if (isInt() and value->isInt()) {
        int a, b, c;
        a = getInt();
        b = value->getInt();
        c = mathOp(a, b, op);
        return JSValuePtr(new JSValue(value->parent, c, JSVALUE_INT));
    }
    throw;
};

JSContext::JSContext() {
    JSScopeChain = std::vector<JSValuePtr>();
    JSScopeChain.push_back(JSValuePtr(new JSValue()));
};

void JSContext::pushScope(JSValuePtr value) {
    JSScopeChain.push_back(value);
};

void JSContext::popScope() {
    JSScopeChain.pop_back();
};

JSValuePtr JSContext::getCurrentScope() {
    return JSScopeChain[JSScopeChain.size() - 1];
};

void JSContext::storeValueByName(std::string name, JSValuePtr value) {
    getCurrentScope()->locals[name] = value;
};

void JSContext::overwriteNamedValue(std::string name, JSValuePtr original, JSValuePtr value) {
    JSValuePtr current = getCurrentScope();
    while (current) {
        if (current->locals[name]) {
            current->locals[name] = value;
            break;
        }
        current = current->parent;
    }
};

JSValuePtr JSContext::lookupValueByName(std::string name) {
    JSValuePtr current = getCurrentScope();
    while (current) {
        if (current->locals[name])
            return current->locals[name];
        current = current->parent;
    }
    return NULL;
};
