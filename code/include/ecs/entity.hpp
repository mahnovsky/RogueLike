#pragma once

#include "component.hpp"

class TransformComponent : public Component< TransformComponent >
{
public:
    glm::vec3 pos;
};

class Entity
{
public:
    Entity( );
    virtual ~Entity( );

    virtual void initialize( );

    void add_component( IComponent* component );

    template < class T >
    T*
    get_component( )
    {
        IComponent* comp = find_component( T::_type_id );
        if ( comp )
        {
            void* vptr = static_cast< void* >( comp );
            return static_cast< T* >( comp );
        }
        return nullptr;
    }

    IComponent* find_component( ComponentTypeID id );

private:
    basic::Vector< IComponent* > m_components;
};
