#ifndef JSOBJECT_H
#define JSOBJECT_H

#include <vector>

enum JSVALUE_FLAGS {
    JSVALUE_UNDEFINED   = 0,
    JSVALUE_STRING      = 1,
    JSVALUE_INT         = 2,
    JSVALUE_FLOAT       = 4,
    JSVALUE_FUNCTION    = 8,
};

class JSValue;

typedef std::shared_ptr<JSValue> JSValuePtr;

class JSValue {

    public:
        JSValue();
        JSValue(std::string, JSVALUE_FLAGS);
        JSValue(int, JSVALUE_FLAGS);
        JSValue(float, JSVALUE_FLAGS);

        int getInt();
        float getFloat();
        std::string getString();

        bool isInt()        { return (flags & JSVALUE_INT) != 0; };
        bool isFloat()      { return (flags & JSVALUE_FLOAT) != 0; };
        bool isString()     { return (flags & JSVALUE_STRING) != 0; };
        bool isUndefined()  { return (flags & JSVALUE_UNDEFINED) != 0; };
        bool isFunction()   { return (flags & JSVALUE_FUNCTION) != 0; };

        std::string str();
        JSValuePtr arithmetic(JSValuePtr, char);
        /* execute data as a function */
        JSValuePtr execute();
        /* named arguments for functions */
        std::vector<std::string> arguments;


    protected:
        int flags;
        std::string data;
        int intData;
        float floatData;
        bool marked;
};


class JSValueHandle {

    public:
        JSValueHandle(JSValuePtr, std::string);
        JSValuePtr value;
        std::string name;
};

typedef std::shared_ptr<JSValueHandle> JSValueHandlePtr;

class JSContext {

    public:
        JSContext();
        /* Adds a child to the _current_ scope */
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
