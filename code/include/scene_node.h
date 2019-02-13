#pragma once

#include "basic/vector.hpp"

#include "object.hpp"
#include "transform.hpp"

class SceneNode : public Object
{
public:
    SceneNode();
    virtual ~SceneNode();

    void remove_children();

    void add_child( SceneNode* node );

    void remove_child(SceneNode* node);

    void remove_from_parent( );

    bool is_contains( SceneNode* child );

    bool get_child_index(SceneNode* node, basic::uint32 &out_index );

    SceneNode* get_parent();

public:
    Transform transform;

private:
    SceneNode* m_parent;
    basic::Vector<SceneNode*> m_children;
};
