#ifndef JSOBJECT_H
#define JSOBJECT_H

enum JSVALUE_FLAGS {
    JSVALUE_UNDEFINED,
    JSVALUE_STRING,
    JSVALUE_INT,
    JSVALUE_FLOAT
};

class JSValue {

    public:
        JSValue(std::string, JSVALUE_FLAGS);
        JSValue(int, JSVALUE_FLAGS);
        JSValue(float, JSVALUE_FLAGS);

        bool isInt() { return flags & JSVALUE_INT ? true : false; };
        int getInt();
        void setInt();

        bool isFloat();
        float getFloat();
        void setFloat();

        bool isString();
        std::string getString();
        void setString();

        std::shared_ptr<JSValue> arithmetic(std::shared_ptr<JSValue>, char);


    protected:
        int flags;
        std::string data;
        int intData;
        float floatData;
};

#endif
