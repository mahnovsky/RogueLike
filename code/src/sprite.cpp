#include "sprite.hpp"

#include "engine.hpp"
#include "iglobal_context.hpp"
#include "render.hpp"
#include "render_common.hpp"
#include "system_manager.hpp"
#include "transform.hpp"
#include "texture.hpp"
#include "resource_storage.hpp"

Sprite::Sprite( )
    : m_color{255, 255, 255, 255}
    , m_size( 1.f, 1.f, 0.f )
    , m_anchor( 0.5f, 0.5f )
    , m_angle(0.f)
	, m_render_object(nullptr)
{
}

void Sprite::init(const std::string& texture_name, const Rect& texture_rect)
{
    if(const auto gloabal_instance = core::IGlobalContext::GetInstance()) 
    {
		const auto sys_manager = gloabal_instance->get_system_manager();

        const auto storage = sys_manager->get_system<core::ResourceStorage>();

        m_render_object = gloabal_instance->get_engine()->get_render()->create_object();
		m_render_object->set_vertex_draw_mode(VertexDrawMode::TriangleFan);
        if(const auto texture = storage->get_resource<Texture>(texture_name.c_str())) {
			m_render_object->set_resource(RenderResourceType::Texture, texture_name);
			m_render_object->set_resource(RenderResourceType::ShaderProgram, "texture");
        }


		VertexBufferT vb;
		vb.emplace_back(Vertex_T{ { -0.5f, -0.5f, 0.f }, { 0.f, 0.f } });
		vb.emplace_back(Vertex_T { { -0.5f, 0.5f, 0.f }, { 0.f, 0.f } });
		vb.emplace_back(Vertex_T { { 0.5f, 0.5f, 0.f }, { 0.f, 0.f } });
		vb.emplace_back(Vertex_T { { 0.5f, -0.5f, 0.f }, { 0.f, 0.f } });

        for (auto& v : vb) {
			v.pos *= m_size;
        }

		auto& mesh_data = m_render_object->get_mesh_data();
		setup_vertices(mesh_data.vertex_data, vb);
    }

}

void Sprite::draw( IRender* render )
{
	render->add_to_frame(m_render_object);
}

void Sprite::set_position( const glm::vec3& pos )
{
}

glm::vec3 Sprite::get_position( ) const
{
    return {};
}

void Sprite::set_color( basic::uint8 r, basic::uint8 g, basic::uint8 b, basic::uint8 a )
{
    m_color = {r, g, b, a};
}

void Sprite::update_frame()
{

}

void Sprite::set_size( float width, float height )
{

}

void Sprite::set_anchor( float x, float y )
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
