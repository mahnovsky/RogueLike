#pragma once

#include "defines.hpp"

class ICamera
{
public:
    virtual ~ICamera();

    virtual void init( const glm::vec3& pos, 
                  const glm::vec3& dir, 
                  const glm::vec3& up ) = 0;

    virtual void get_matrix( glm::mat4& mat ) const = 0;

    virtual void set_position( const glm::vec3& pos ) = 0;

    virtual void set_direction( const glm::vec3& dir ) = 0;

    virtual void set_up( const glm::vec3& up ) = 0;
};

class PerspectiveCamera : public ICamera
{
public:
    PerspectiveCamera(float fov, float aspect, float near, float far);
    ~PerspectiveCamera() override;

    void init( const glm::vec3& pos, 
               const glm::vec3& dir, 
               const glm::vec3& up ) override; 

    void set_position( const glm::vec3& pos ) override;

    void set_direction( const glm::vec3& dir ) override;

    void set_up( const glm::vec3& up ) override;

    void get_matrix( glm::mat4& out ) const override;

private:
    void update();

private:
    glm::vec3 m_position;
    glm::vec3 m_direction;
    glm::vec3 m_up;

    glm::mat4 m_projection;
    glm::mat4 m_view;
};


class OrthoCamera : public ICamera
{
public:
    OrthoCamera( float width, float height, float near, float far );

    void init( const glm::vec3& pos, 
               const glm::vec3& dir, 
               const glm::vec3& up );

    void get_matrix( glm::mat4& mat ) const;

    void set_position( const glm::vec3& pos );

    void set_direction( const glm::vec3& dir );

    void set_up( const glm::vec3& up );

private:
    void update();

private:
    glm::mat4 m_projection;

    float m_width;
    float m_height;
    float m_near;
    float m_far;
    glm::vec3 m_position;
};

