#pragma once

#include "defines.hpp"
#include "resource_storage.hpp"

class Texture : public FileResource
{
public:
	
    Texture( const char* file );
    ~Texture( ) override;

	bool load(ResourceStorage*) override;

	static Texture* create(const char* file);

    void init( basic::Image image );

    void init( basic::uint32 width,
               basic::uint32 height,
               basic::Vector<basic::uint8> image_data,
               basic::uint32 cc );

	void init_font(basic::uint32 width,
		basic::uint32 height,
		basic::Vector<basic::uint8> image_data);

	void set(basic::uint32 width,
		basic::uint32 height,
		basic::uint32 handle,
		basic::uint32 cc);

    void bind( ) const;

    void unbind( ) const;

    basic::uint32 get_width( ) const;

    basic::uint32 get_height( ) const;

private:
    basic::uint32 m_texture;
    basic::uint32 m_width;
    basic::uint32 m_height;
    basic::uint32 m_components;
};
