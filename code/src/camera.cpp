#include "camera.hpp"

Camera::Camera()
    : m_distance( 10.f )
    ,m_view()
{
}

Camera::~Camera()
{
}

void Camera::init_perspective( float fov, float aspect, float near, float far )
{
    m_view = glm::lookAt(
        glm::vec3(4,3,3), // Камера находится в мировых координатах (4,3,3)
        glm::vec3(0,0,0), // И направлена в начало координат
        glm::vec3(0,1,0)  // "Голова" находится сверху
        );

    m_projection = glm::perspective( glm::radians(fov), aspect, near, far );
} 

