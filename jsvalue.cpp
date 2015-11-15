#include <string>
#include <memory>
#include <iostream>
#include "jsvalue.h"

JSValue::JSValue(int data, JSVALUE_FLAGS flags) {
    this->intData = data;
    this->flags = flags;
};

int JSValue::getInt() {
    return intData;
}

std::shared_ptr<JSValue> JSValue::arithmetic(std::shared_ptr<JSValue> value, char op) {
    int a, b, c;
    a = this->getInt();
    b = value->getInt();
    // can't use a switch because it has a special scope
    if (op == '+')
        c = a + b;
    if (op == '*')
        c = a * b;
    if (op == '-')
        c = a - b;
    if (op == '/')
        c = a / b;
    return std::shared_ptr<JSValue>(new JSValue(c, JSVALUE_INT));
}
