#pragma once

#include "defines.hpp"
#include "resource_storage.hpp"

namespace se
{
struct bakedchar;

class Font : public FileResource
{
public:
    Font(ObjectManager *manager, const char* file );
    ~Font( ) override;

	bool load(ResourceStorage*) override;

    void update( const char* text, float height, RenderObject& out_object );

    RenderNode* create_text_node( );

    void update(const char* text, RenderNode* out_object );

    static Font* create(ObjectManager *manager, const char* file);

private:
    ShaderProgram* m_shader;
    Texture* m_texture;
    float m_height;
    void* m_cdata;
};
}
