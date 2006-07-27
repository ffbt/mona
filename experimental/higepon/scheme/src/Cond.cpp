#include "Cond.h"

using namespace monash;

Cond::Cond(Clauses* clauses, Objects* elseActions) : clauses_(clauses), elseActions_(elseActions)
{
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    for (Clauses::iterator it = clauses->begin(); it != clauses->end(); it++)
    {
        printf("[%s]\n", (*it)->first->toString().c_str());
    }
}

Cond::~Cond()
{
}

std::string Cond::toString()
{
    return std::string("cond");
}

int Cond::type() const
{
    return Object::COND;
}

Object* Cond::expand()
{
    return expandInternal(clauses_->begin());
}

Object* Cond::expandInternal(Clauses::iterator it)
{
    if (it == clauses_->end())
    {
        if (NULL != elseActions_)
        {
            return new Begin(elseActions_);
        }
        else
        {
            return new Number(0); // fix me
        }
    }
    Clause* clause = *it;
    return new SpecialIf(clause->first, new Begin(clause->second), expandInternal(++it));
}

Object* Cond::eval(Environment* env)
{
    Object* specialif = expand();
    printf(specialif->toString().c_str());
    return specialif->eval(env);
}
