#pragma once

#include "object.hpp"
#include "resource_storage.hpp"
#include "render_common.hpp"

class StaticMesh : public FileResource
{
public:
    StaticMesh( ObjectManager* obj_mng, const char* name );
    ~StaticMesh( ) override;

	bool load(ResourceStorage* rs) override;

    void initialize( IVertexBuffer* vbo, IIndexBuffer* ibo );

    void bind( );

    void apply_fmt( ) const;

    basic::uint32 get_vertex_count( ) const;

    basic::uint32 get_index_count( ) const;

	basic::uint32 get_vbo() const;

	basic::uint32 get_ibo() const;

	const basic::Vector<VertexFMT>& get_fmt_list() const;

	static StaticMesh* create( ObjectManager* obj_mng,
							   const char* name );

    static StaticMesh* create( ObjectManager* obj_mng,
                               const char* name,
                               IGpuFactory* factory,
                               basic::Vector< basic::uint8 > data );

private:
    IVertexBuffer* m_vbo;
    IIndexBuffer* m_ibo;

	basic::uint32 m_vbo_u;
	basic::uint32 m_vib_u;

	basic::uint32 m_vertex_count;
	basic::uint32 m_index_count;

	basic::Vector<VertexFMT> m_fmt_list;
};
