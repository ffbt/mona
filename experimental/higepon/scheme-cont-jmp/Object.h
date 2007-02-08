#ifndef __OBJECT_H__
#define __OBJECT_H__

#ifdef USE_BOEHM_GC
#define GC_NOT_DLL
#include "gc_cpp.h"
#endif
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "Assert.h"
#include "Error.h"
#include <iostream>
#include <string>
#include <vector>

namespace monash {

class Environment;
class Object;
typedef std::vector<Object*> Objects;
#ifdef USE_BOEHM_GC
class Object : public gc_cleanup
//    class Object : public gc
#else
class Object
#endif
{
public:
    Object();
    virtual ~Object();

public:
    virtual std::string toString()         = 0;
    virtual std::string toStringValue() { return toString(); }
    virtual bool eqv(Object* o) { return false; }
    virtual bool eq(Object* o) { return o == this; }
    virtual int type() const               = 0;
    virtual uint32_t lineno() const        = 0;
    virtual Object* eval(Environment* env) = 0;
    bool isNumber() const { return type() == NUMBER; }
    bool isString() const { return type() == STRING; }
    bool isCharcter() const { return type() == CHARCTER; }
    bool isVector() const { return type () == VECTOR; }
    bool isQuote() const { return type() == QUOTE; }
    bool isPair() const { return type() == PAIR; }
    bool isTrue() const { return type() == TRUE; }
    bool isFalse() const { return type() == FALSE; }
    bool isBoolean() const { isTrue() || isFalse(); }
    bool isUndef() const { return type() == UNDEF; }
    bool isCompoundProcedure() const { return type() == PROCEDURE; }
    bool isProcedure() const { return isCompoundProcedure() || isPrimitiveProcedure(); }
    bool isPrimitiveProcedure() const { return type() == PRIMITIVE_PROCEDURE; }
    bool isApplication() const { return type() == APPLICATION; }
    bool isBegin() const { return type() == BEGIN; }
    bool isLambda() const { return type() == LAMBDA; }
    bool isEnvironment() const { return type() == ENVIRONMENT; }
    bool isContinuation() const { return type() == CONTINUATION; }
public:
    Object* parent;

    enum
    {
        NUMBER,
        STRING,
        CHARCTER,
        QUOTE,
        VECTOR,
        CONTINUATION,
        VARIABLE,
        ASSIGNMENT,
        IF,
        LAMBDA,
        PROCEDURE,
        ENVIRONMENT,
        BEGIN,
        DEFINITION,
        COND,
        EVAL,
        PAIR,
        APPLICATION,
        PRIMITIVE_PROCEDURE,
        AND,
        OR,
        LET,
        LET_ASTERISK,
        TRUE,
        FALSE,
        UNDEF,
    };
};

}; // namespace monash

#endif // __OBJECT_H__
