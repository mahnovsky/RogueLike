#pragma once

#include "type_registration.hpp"
#include "defines.hpp"
#include "component.hpp"


class Transform : public Component
{
public:
	GENERIC_OBJECT_IMPL(Transform, NS_COMPONENT_TYPE);

    Transform( Entity* ent );
	~Transform() override;

    void set_position( const glm::vec3& pos );

    glm::vec3 get_position( ) const;

    void set_pivot_point( const glm::vec3& pivot );

    glm::vec3 get_pivot_point( ) const;

    void set_euler_angles( const glm::vec3& angles );

    glm::quat get_quaternion() const;

    void set_quaternion(glm::quat rotation);

    void set_scale( const glm::vec3& scale );

    glm::vec3 get_scale( ) const;

    glm::mat4 get_matrix( ) const;

    glm::vec3 get_euler_angles( ) const;

    glm::vec3 get_forward( ) const;

private:
    void update_final_matrix() const;

	mutable glm::mat4 m_final_mat;
    
    glm::vec3 m_pos;
    glm::vec3 m_euler_angles;
    glm::vec3 m_pivot_point;
    glm::quat m_quat;
    glm::vec3 m_scale;

public:
	mutable bool is_changed = true;
};
