#pragma once

#include "defines.hpp"

class IComponent;

class IComponentStorage
{
public:
    virtual ~IComponentStorage( )
    {
    }

    virtual IComponent* create( ) = 0;

    virtual basic::String get_name( ) const = 0;
};

template < class T >
class ComponentStorage : public IComponentStorage
{
public:
    ComponentStorage( const char* name )
        : m_name( name )
        , m_components( )
    {
    }

    IComponent*
    create( ) override
    {
        m_components.emplace( T{m_name.get_cstr( )} );
        return &m_components.back( );
    }

    basic::String
    get_name( ) const override
    {
        return m_name;
    }

private:
    basic::String m_name;
    basic::Vector< T > m_components;
};

class ComponentManager
{
public:
    ComponentManager( );
    ~ComponentManager( );

    IComponent* create_component( const char* name );

    template < class T >
    T*
    create_component( )
    {
        ASSERT( T::_type_id < m_storages.get_size( ) );

        void* vptr = m_storages[ T::_type_id ]->create( );
        T* component = static_cast< T* >( vptr );

        return component;
    }

    template < class T >
    bool
    registry_component( const char* name )
    {
        auto id = T::_type_id;

        if ( m_storages[ id ] != nullptr )
        {
            LOG( "failed register, component already exist" );
            return false;
        }

        m_storages[ id ] = NEW_OBJ( ComponentStorage< T >, name );
        return true;
    }

private:
    basic::Vector< IComponentStorage* > m_storages;
};
