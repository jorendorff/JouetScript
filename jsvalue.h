#ifndef JSOBJECT_H
#define JSOBJECT_H

#include <vector>
#include <map>

enum JSVALUE_FLAGS {
    JSVALUE_UNDEFINED   = 0,
    JSVALUE_STRING      = 1,
    JSVALUE_INT         = 2,
    JSVALUE_FLOAT       = 4,
    JSVALUE_FUNCTION    = 8,
    JSVALUE_BOOL        = 16,
};

class JSValue;
class JSContext;

typedef std::shared_ptr<JSValue> JSValuePtr;

class JSValue {

    public:
        JSValue();
        bool isUndefined()  { return (flags & JSVALUE_UNDEFINED) != 0; };

        JSValue(JSValuePtr parent, int data, JSVALUE_FLAGS flags);
        int getInt();
        bool isInt()        { return (flags & JSVALUE_INT) != 0; };

        JSValue(JSValuePtr parent, float data, JSVALUE_FLAGS flags);
        float getFloat();
        bool isFloat()      { return (flags & JSVALUE_FLOAT) != 0; };

        JSValue(JSValuePtr parent, std::string data, JSVALUE_FLAGS flags);
        std::string getString();
        bool isString()     { return (flags & JSVALUE_STRING) != 0; };

        JSValue(JSValuePtr parent, bool data, JSVALUE_FLAGS flags);
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
        JSValuePtr parent;
        std::map<std::string, JSValuePtr> locals;

   friend JSContext;
};


/**
 * Manages the memory space of one or more execution environments.
 **/
class JSContext {

    public:
        JSContext();
        void pushScope(JSValuePtr);
        void popScope();
        /* Adds a child to the _current_ scope */
        void addChild(std::string name, JSValuePtr value);
        /* Find a child by name */
        JSValuePtr findChild(const std::string name);
        /* Return the top-most scope */
        JSValuePtr getCurrentScope();

    protected:
        std::vector<JSValuePtr> JSScopeChain;

    friend class JScript;
};

#endif
