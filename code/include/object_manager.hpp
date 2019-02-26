#pragma once

#include "defines.hpp"

class ObjectManager
{
public:
    ObjectManager();
    virtual ~ObjectManager();

    Object* find( const char* name );

    Object* find( basic::int32 tag );

    Object* find( const char* name, basic::int32 tag );

    friend class Object;

private:
    void add( Object* obj );

    void remove( Object* obj );

private:
    basic::Vector<Object*> m_objects;
};
