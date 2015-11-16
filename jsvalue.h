#ifndef JSOBJECT_H
#define JSOBJECT_H

enum JSVALUE_FLAGS {
    JSVALUE_UNDEFINED   = 0,
    JSVALUE_STRING      = 1,
    JSVALUE_INT         = 2,
    JSVALUE_FLOAT       = 4
};

class JSValue {

    public:
        JSValue(std::string, JSVALUE_FLAGS);
        JSValue(int, JSVALUE_FLAGS);
        JSValue(float, JSVALUE_FLAGS);

        bool isInt() { return (flags & JSVALUE_INT) != 0; };
        int getInt();
        void setInt();

        bool isFloat() { return (flags & JSVALUE_FLOAT) != 0; };
        float getFloat();
        void setFloat();

        bool isString();
        std::string getString();
        void setString();

        std::string str();
        std::unique_ptr<JSValue> arithmetic(std::unique_ptr<JSValue>, char);


    protected:
        int flags;
        std::string data;
        int intData;
        float floatData;
};

#endif
