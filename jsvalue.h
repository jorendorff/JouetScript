#ifndef JSOBJECT_H
#define JSOBJECT_H

#include <vector>

enum JSVALUE_FLAGS {
    JSVALUE_UNDEFINED   = 0,
    JSVALUE_STRING      = 1,
    JSVALUE_INT         = 2,
    JSVALUE_FLOAT       = 4
};

class JSValue;

typedef std::shared_ptr<JSValue> JSValuePtr;

class JSValue {

    public:
        JSValue();
        JSValue(std::string, JSVALUE_FLAGS);
        JSValue(int, JSVALUE_FLAGS);
        JSValue(float, JSVALUE_FLAGS);

        bool isInt() { return (flags & JSVALUE_INT) != 0; };
        int getInt();
        void setInt();

        bool isFloat() { return (flags & JSVALUE_FLOAT) != 0; };
        float getFloat();
        void setFloat();

        bool isString() { return (flags & JSVALUE_STRING) != 0; };
        std::string getString();
        void setString();

        bool isUndefined() { return (flags & JSVALUE_UNDEFINED) != 0; };

        std::string str();
        JSValuePtr arithmetic(JSValuePtr, char);


    protected:
        int flags;
        std::string data;
        int intData;
        float floatData;
        bool marked;
};


class JSValueHandle {

    public:
        JSValueHandle(JSValue *value, std::string name);
        JSValuePtr value;
        std::string name;
};

typedef std::shared_ptr<JSValueHandle> JSValueHandlePtr;

class JSContext {

    public:
        JSContext();
        /* Adds a child to the current scope */
        void addChild(JSValueHandlePtr value);
        JSValueHandlePtr findChild(const std::string name);
        void pushScope();
        void popScope();

    protected:
        std::vector<JSValuePtr> JSValueCache;
        std::vector<std::vector<JSValueHandlePtr>> JSScopeChain;

    friend class JScript;
};

#endif
