#pragma once

#include "defines.hpp"

enum class SharedObjectType;

class ObjectManager
{
public:
    ObjectManager( );
    virtual ~ObjectManager( );

    SharedObject* find( const char* name );

    SharedObject* find( basic::int32 tag );

    SharedObject* find( const char* name, basic::int32 tag );

    SharedObject* find( SharedObjectType type, const char* name );

    basic::Vector< SharedObject* > get_all( SharedObjectType type ) const;

    friend class SharedObject;

private:
    void add( SharedObject* obj );

    void remove( SharedObject* obj );

private:
    basic::Vector< SharedObject* > m_objects;
    basic::Vector< basic::Vector< SharedObject* > > m_typed_objects;
};
