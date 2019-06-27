#pragma once

#include "defines.hpp"

class Transform
{
public:
    Transform( );

    void set_parent( Transform* parent );

    void add_child( Transform* child );

    void remove( Transform* child );

    void set_position( const glm::vec3& pos );

    glm::vec3 get_position( ) const;

    void set_pivot_point( const glm::vec3& pivot );

    glm::vec3 get_pivot_point( ) const;

    void set_euler_angles( const glm::vec3& angles );

    void set_scale( const glm::vec3& scale );

    glm::vec3 get_scale( ) const;

    glm::mat4 get_matrix( ) const;

    glm::vec3 get_euler_angles( ) const;

    glm::vec3 get_forward( ) const;

private:
    void update_final_matrix( ) const;

private:
    Transform* m_parent;
    mutable glm::mat4 m_final_mat;
    glm::vec3 m_pos;
    glm::vec3 m_euler_angles;
    glm::vec3 m_pivot_point;
    glm::quat m_quat;
    glm::vec3 m_scale;
    basic::Vector< Transform* > m_children;
};
