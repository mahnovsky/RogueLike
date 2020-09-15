#include "sprite.hpp"

#include "render_common.hpp"
#include "transform.hpp"
#include "texture.hpp"

Sprite::Sprite( )
    : m_color{255, 255, 255, 255}
    , m_size( 1.f, 1.f, 0.f )
    , m_anchor( 0.5f, 0.5f )
    , m_angle(0.f)
{
}

Sprite::~Sprite()
{
}

void
Sprite::init( Texture* texture )
{
    ASSERT(texture);

    VertexBufferT vb;

    TextureRect rect;
}

void
Sprite::draw( ICamera* camera, IRender* render )
{
    
}

void
Sprite::set_position( const glm::vec3& pos )
{
}

glm::vec3
Sprite::get_position( ) const
{
    

    return glm::vec3();
}

void
Sprite::set_color( basic::uint8 r, basic::uint8 g, basic::uint8 b, basic::uint8 a )
{
    m_color = {r, g, b, a};
}

void Sprite::update_frame()
{

}

void
Sprite::set_size( float width, float height )
{

}

void
Sprite::set_anchor( float x, float y )
{
    m_anchor = {x, y};

    VertexBufferT vb;
}

void Sprite::set_angle(float angle)
{
    m_angle = angle;
}

float Sprite::get_angle() const
{
    return m_angle;
}

void Sprite::set_frame_name(const char *name)
{
    m_frame_name = name;

    update_frame();
}
