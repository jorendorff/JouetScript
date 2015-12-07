#include <string>
#include <memory>
#include <iostream>
#include <sstream>
#include "jsvalue.h"

JSValue::JSValue() {
    flags = JSVALUE_UNDEFINED;
};

JSValue::JSValue(int data, JSVALUE_FLAGS flags) {
    intData = data;
    flags = flags;
    marked = false;
};

int JSValue::getInt() {
    return intData;
}

JSValue::JSValue(float data, JSVALUE_FLAGS flags) {
    floatData = data;
    flags = flags;
    marked = false;
};

float JSValue::getFloat() {
    return floatData;
}

JSValue::JSValue(std::string data, JSVALUE_FLAGS flags) {
    data = data;
    flags = flags;
    marked = false;
};

std::string JSValue::getString() {
    return data;
}

JSValue::JSValue(bool data, JSVALUE_FLAGS flags) {
    intData = data ? 1 : 0;
    flags = flags;
    marked = false;
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
        out << getBool() ? "true" : "false";
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
        return JSValuePtr(new JSValue(cf, JSVALUE_FLOAT));
    } else if (isInt() and value->isInt()) {
        int a, b, c;
        a = getInt();
        b = value->getInt();
        c = mathOp(a, b, op);
        return JSValuePtr(new JSValue(c, JSVALUE_INT));
    }
    throw;
};

JSValueHandle::JSValueHandle(JSValuePtr value, std::string name) {
    name = name;
    value = value;
}

JSContext::JSContext() {
    JSValueCache = std::vector<JSValuePtr>();
    JSScopeChain = std::vector<std::vector<JSValueHandlePtr>>();
    JSScopeChain.push_back(std::vector<JSValueHandlePtr>());
};

void JSContext::pushScope() {
    JSScopeChain.push_back(std::vector<JSValueHandlePtr>());
};

void JSContext::popScope() {
    JSScopeChain.pop_back();
};

void JSContext::addChild(JSValueHandlePtr value) {
    JSScopeChain[JSScopeChain.size() - 1].push_back(value);
};

JSValueHandlePtr JSContext::findChild(std::string name) {
    for (int i = JSScopeChain.size() - 1; i >= 0; i--) {
        for (int j = JSScopeChain[i].size() - 1; j >= 0; j--) {
            if (JSScopeChain[i][j]->name == name)
                return JSScopeChain[i][j];
        }
    }
    return NULL;
};
