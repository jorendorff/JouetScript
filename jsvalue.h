#ifndef JSOBJECT_H
#define JSOBJECT_H

#include <vector>

enum JSVALUE_FLAGS {
    JSVALUE_UNDEFINED   = 0,
    JSVALUE_STRING      = 1,
    JSVALUE_INT         = 2,
    JSVALUE_FLOAT       = 4,
    JSVALUE_FUNCTION    = 8,
    JSVALUE_BOOL        = 16,
};

class JSValue;

typedef std::shared_ptr<JSValue> JSValuePtr;

class JSValue {

    public:
        JSValue();
        bool isUndefined()  { return (flags & JSVALUE_UNDEFINED) != 0; };

        JSValue(int, JSVALUE_FLAGS);
        int getInt();
        bool isInt()        { return (flags & JSVALUE_INT) != 0; };

        JSValue(float, JSVALUE_FLAGS);
        float getFloat();
        bool isFloat()      { return (flags & JSVALUE_FLOAT) != 0; };

        JSValue(std::string, JSVALUE_FLAGS);
        std::string getString();
        bool isString()     { return (flags & JSVALUE_STRING) != 0; };

        JSValue(bool, JSVALUE_FLAGS);
        bool getBool();
        bool isBool()       { return (flags & JSVALUE_BOOL) != 0; };

        std::string str();
        JSValuePtr arithmetic(JSValuePtr, char);

        bool isFunction()   { return (flags & JSVALUE_FUNCTION) != 0; };
        /* named arguments for functions */
        std::vector<std::string> arguments;


    protected:
        int flags;
        std::string data;
        int intData;
        float floatData;
        bool marked;
};


/**
 * Named pointers to JSValues, i.e. variables
 **/
class JSValueHandle {

    public:
        JSValueHandle(JSValuePtr value, std::string name);
        JSValuePtr value;
        std::string name;
};

typedef std::shared_ptr<JSValueHandle> JSValueHandlePtr;

/**
 * Manages the memory space of one or more execution environments.
 **/
class JSContext {

    public:
        JSContext();
        void pushScope();
        void popScope();
        /* Adds a child to the _current_ scope */
        void addChild(JSValueHandlePtr value);
        /* Find a child by name */
        JSValueHandlePtr findChild(const std::string name);

    protected:
        std::vector<JSValuePtr> JSValueCache;
        std::vector<std::vector<JSValueHandlePtr>> JSScopeChain;

    friend class JScript;
};

#endif
