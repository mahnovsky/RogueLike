#pragma once

#include "defines.hpp"

class ObjectManager
{
public:
    ObjectManager( );
    virtual ~ObjectManager( );

    SharedObject* find( const char* name );

    SharedObject* find( basic::int32 tag );

    SharedObject* find( const char* name, basic::int32 tag );

    friend class SharedObject;

private:
    void add( SharedObject* obj );

    void remove( SharedObject* obj );

private:
    basic::Vector< SharedObject* > m_objects;
};
