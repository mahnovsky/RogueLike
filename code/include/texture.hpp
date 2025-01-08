#pragma once

#include "defines.hpp"
#include "rect.hpp"
#include "resource_storage.hpp"
#include "type_registration.hpp"

struct TextureRect
{
	Rect rect;
    std::string name;
};

class Texture : public core::FileResource
{
public:

    Texture( const char* file );
    ~Texture( ) override;

    bool load( core::ResourceStorage* ) override;

    static Texture* create( const char* file );

    void init( basic::Image image );

    void init( uint32_t width, uint32_t height, std::vector< uint8_t > image_data, uint32_t cc);

    void init_font( basic::uint32 width,
                    basic::uint32 height,
                    std::vector< uint8_t > image_data );

    void set( basic::uint32 width, basic::uint32 height, basic::uint32 handle, basic::uint32 cc );

    void bind( ) const;

    void unbind( ) const;

    uint32_t get_width() const;

    uint32_t get_height() const;

    bool get_rect( const char* key, TextureRect& out_rect ) const;

    bool get_rect(uint32_t index, TextureRect& out_rect) const;

	uint32_t get_handle() const;

private:
	uint32_t m_texture;
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_components;
    std::vector<TextureRect> m_rects;
};
