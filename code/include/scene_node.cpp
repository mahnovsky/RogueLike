#include "scene_node.h"



SceneNode::~SceneNode()
{
    remove_from_parent();
}

void SceneNode::remove_children()
{
    m_children.clear();
}

void SceneNode::add_child(SceneNode *node)
{
    ASSERT( node != nullptr );

    size_t index = 0;
    if( get_child_index( node, index ) )
    {
        node->retain();

        node->remove_from_parent();

        m_children.push( node );
    }
}

void SceneNode::remove_child(SceneNode *node)
{
    ASSERT(node->get_parent() == this);

    size_t index;
    if( get_child_index( node, index ) )
    {
        m_children.remove_by_index( index );

        node->release();

        return;
    }

    ASSERT_M( false, "Child not found something wrong" );
}

void SceneNode::remove_from_parent()
{
    if( m_parent )
    {
        m_parent->remove_child( this );
    }
}

bool SceneNode::is_contains(SceneNode *child)
{
    ASSERT( child->get_parent() == this );

    return m_children.is_contains( child );
}

bool SceneNode::get_child_index(SceneNode *node, size_t &out_index)
{
    if( node->get_parent() == this )
    {
        return m_children.find_first( out_index, node, 0 );
    }

    return false;
}

SceneNode *SceneNode::get_parent()
{
    return m_parent;
}
