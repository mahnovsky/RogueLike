#pragma once

#include "object.hpp"
#include "render_common.hpp"

class StaticMesh : public SharedObject
{
public:
    StaticMesh( ObjectManager* obj_mng, const char* name );
    ~StaticMesh( ) override;

    void initialize( IVertexBuffer* vbo, IIndexBuffer* ibo );

    void bind( );

    void apply_fmt( ) const;

    basic::uint32 get_vertex_count( ) const;

    basic::uint32 get_index_count( ) const;

    static StaticMesh* create( ObjectManager* obj_mng,
                               const char* name,
                               IGpuFactory* factory,
                               basic::Vector< basic::uint8 > data );

private:
    IVertexBuffer* m_vbo;
    IIndexBuffer* m_ibo;
};
