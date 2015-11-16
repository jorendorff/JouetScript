#include <string>
#include <memory>
#include <iostream>
#include <sstream>
#include "jsvalue.h"

JSValue::JSValue(int data, JSVALUE_FLAGS flags) {
    this->intData = data;
    this->flags = flags;
};

int JSValue::getInt() {
    return intData;
}

JSValue::JSValue(float data, JSVALUE_FLAGS flags) {
    this->floatData = data;
    this->flags = flags;
};

float JSValue::getFloat() {
    return floatData;
}

std::string JSValue::str() {
    std::ostringstream out;
    if (this->isInt()) {
        out << this->getInt();
    } else if (this->isFloat()) {
        out << this->getFloat();
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

std::unique_ptr<JSValue> JSValue::arithmetic(std::unique_ptr<JSValue> value, char op) {
    if (this->isFloat() or value->isFloat()) {
        float af, bf, cf;
        af = this->isFloat() ? this->getFloat() : (float)this->getInt();
        bf = value->isFloat() ? value->getFloat() : (float)value->getInt();
        cf = mathOp(af, bf, op);
        return std::unique_ptr<JSValue>(new JSValue(cf, JSVALUE_FLOAT));
    } else if (this->isInt() and value->isInt()) {
        int a, b, c;
        a = this->getInt();
        b = value->getInt();
        c = mathOp(a, b, op);
        return std::unique_ptr<JSValue>(new JSValue(c, JSVALUE_INT));
    }
    throw;
}
