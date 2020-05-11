#pragma once

#include "defines.hpp"
#include "resource_storage.hpp"

namespace se
{
struct bakedchar;

class Font : public FileResource
{
public:
    GENERIC_OBJECT_IMPL(Font, NS_RESOURCE_TYPE);

    Font(GenericObjectManager *manager, const char* file );
    ~Font( ) override;

	bool load(ResourceStorage*) ;

    RenderNode* create_text_node( );

    void update(const char* text, RenderNode* out_object, glm::vec2& size);

    static Font* create(GenericObjectManager* manager, const char* file);

private:
    GenericObjectManager* m_object_manager;
    ShaderProgram* m_shader;
    Texture* m_texture;
    float m_height;
    void* m_cdata;
};
}
