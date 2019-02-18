#pragma once

#include "defines.hpp"
#include "resource_storage.hpp"

namespace se
{
struct bakedchar;

class Font : public FileResource
{
public:
    Font( const char* file );
    ~Font( );

	bool load(ResourceStorage*) override;

    bool init( const char* file, ShaderProgram* shader );

    void generate( const char* text, float height, RenderObject& out_object );

	static Font* create(const char* file);

private:
    Texture* m_texture;
    float m_height;
    void* m_cdata;
    ShaderProgram* m_shader;
};
}
