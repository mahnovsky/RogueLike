#pragma once

#include "render_common.hpp"

using vertex_update = void (*)( Vertex*, void* );

class RenderObject
{
public:
    RenderObject();

    void init();

    void update( vertex_update callback, void* user_data );

    bool is_initialized() const;

    void set_vertex_buffer( VertexBuffer buffer ); 

    void set_index_buffer( IndexBuffer buffer );

    void bind() const;
    
    void unbind() const;

    size_t get_element_count() const { return m_ib.get_size(); }

private:
    void init_vertex_buffer( );

    void init_index_buffer( );

    void bind_array_object( bool on ) const; 

    VertexBuffer m_vb;
    IndexBuffer m_ib;

    basic::uint32 m_array_object;    
    basic::uint32 m_index_object;    
    basic::uint32 m_vertex_object;    
}; 
