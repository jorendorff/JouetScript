#include <string>
#include <memory>
#include <iostream>
#include <sstream>
#include "jsvalue.h"

JSValue::JSValue() {
    this->flags = JSVALUE_UNDEFINED;
};

JSValue::JSValue(int data, JSVALUE_FLAGS flags) {
    this->intData = data;
    this->flags = flags;
    this->marked = false;
};

int JSValue::getInt() {
    return intData;
}

JSValue::JSValue(float data, JSVALUE_FLAGS flags) {
    this->floatData = data;
    this->flags = flags;
    this->marked = false;
};

float JSValue::getFloat() {
    return floatData;
}

JSValue::JSValue(std::string data, JSVALUE_FLAGS flags) {
    this->data = data;
    this->flags = flags;
    this->marked = false;
};

std::string JSValue::getString() {
    return data;
}

std::string JSValue::str() {
    std::ostringstream out;
    if (this->isInt()) {
        out << this->getInt();
    } else if (this->isFloat()) {
        out << this->getFloat();
    } else if (this->isString()) {
        out << this->getString();
    } else if (this->isFunction()) {
        out << "<function: " << this->getString() << " | args: ";
        for (auto arg: this->arguments) {
            out << arg << " ";
        }
        out << ">";
    } else if (this->isUndefined()) {
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
    }
    throw;
};

JSValuePtr JSValue::arithmetic(JSValuePtr value, char op) {
    if (this->isFloat() or value->isFloat()) {
        float af, bf, cf;
        af = this->isFloat() ? this->getFloat() : (float)this->getInt();
        bf = value->isFloat() ? value->getFloat() : (float)value->getInt();
        cf = mathOp(af, bf, op);
        return JSValuePtr(new JSValue(cf, JSVALUE_FLOAT));
    } else if (this->isInt() and value->isInt()) {
        int a, b, c;
        a = this->getInt();
        b = value->getInt();
        c = mathOp(a, b, op);
        return JSValuePtr(new JSValue(c, JSVALUE_INT));
    }
    throw;
};

JSValueHandle::JSValueHandle(JSValuePtr value, std::string name) {
    this->name = name;
    this->value = value;
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
