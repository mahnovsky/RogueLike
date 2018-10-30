#pragma once

#include "transform.hpp"

class World
{
public:
    World( glm::vec3 volume );
    ~World();

    template <class T>
    T* make_actor()
    {
        T* actor = new T;
        TransformPtr transform;

        transform.allocate();
        actor->set_transform( transform ); 

        return actor;
    }

private:
    glm::vec3 m_volume;
    basic::Vector<TransformPtr> m_transforms;
};

