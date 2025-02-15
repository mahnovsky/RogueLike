#pragma once

#include "defines.hpp"
#include "resource_storage.hpp"

#include "opengl/resource_shader.hpp"


namespace se
{
struct bakedchar;

class Font : public core::FileResource
{
public:
    Font( const char* file );
    ~Font( ) override;

	bool load(core::ResourceStorage*) ;

    void update(const char* text, uint32_t count, IRenderObject* out_object, glm::vec2& size);

    static Font* create( const char* file);

private:
	std::shared_ptr<ogl::ShaderProgram> m_shader;
	std::shared_ptr<Texture> m_texture;
    float m_height;
    void* m_cdata;
	std::string m_texture_name;
};
}
