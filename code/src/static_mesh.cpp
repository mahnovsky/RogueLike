#include "static_mesh.hpp"

#include "render_common.hpp"

StaticMesh::StaticMesh( ObjectManager* obj_mng, const char* name )
    : FileResource( obj_mng, SharedObjectType::StaticMesh, name )
    , m_vbo( nullptr )
    , m_ibo( nullptr )
	, m_vbo_u(0)
	, m_vib_u(0)
	, m_vertex_count(0)
	, m_index_count(0)
{
}

StaticMesh::~StaticMesh( )
{
    DELETE_OBJ( m_vbo );
    DELETE_OBJ( m_ibo );
}

bool StaticMesh::load(ResourceStorage* rs)
{
	return false;
}

void
StaticMesh::initialize( IVertexBuffer* vbo, IIndexBuffer* ibo )
{
    if ( m_vbo )
    {
        DELETE_OBJ( m_vbo );
    }

    m_vbo = vbo;

    if ( m_ibo )
    {
        DELETE_OBJ( m_ibo );
    }
    m_ibo = ibo;
}

void
StaticMesh::bind( )
{
    ASSERT( m_vbo );
    m_vbo->bind( true );
    if ( m_ibo )
    {
        m_ibo->bind( true );
    }
}

void
StaticMesh::apply_fmt( ) const
{
    ASSERT( m_vbo );
    m_vbo->apply_fmt( );
}

basic::uint32
StaticMesh::get_vertex_count( ) const
{
    return m_index_count;
}

basic::uint32
StaticMesh::get_index_count( ) const
{
    return m_index_count;
}

basic::uint32 StaticMesh::get_vbo() const
{
	return m_vbo_u;
}

basic::uint32 StaticMesh::get_ibo() const
{
	return m_vib_u;
}

const basic::Vector<VertexFMT>& StaticMesh::get_fmt_list() const
{
	return m_fmt_list;
}

StaticMesh* StaticMesh::create(ObjectManager* obj_mng, const char* name)
{
	ASSERT(name);

	MeshData mesh_data;
	auto data = basic::get_file_content(name);
	if (!data.is_empty() && load_mesh(std::move(data), mesh_data) && !mesh_data.vb.is_empty())
	{
		auto static_mesh = NEW_OBJ(StaticMesh, obj_mng, name);

		static_mesh->m_vbo_u = create_buffer(GL_ARRAY_BUFFER,
			GL_STATIC_DRAW,
			mesh_data.vb.get_raw(),
			mesh_data.vb.get_size() * sizeof(Vertex));

		static_mesh->m_vertex_count = mesh_data.vb.get_size();

		if (!mesh_data.ib.is_empty())
		{
			static_mesh->m_vib_u = create_buffer(GL_ELEMENT_ARRAY_BUFFER,
				GL_STATIC_DRAW,
				mesh_data.ib.get_raw(),
				mesh_data.ib.get_size() * sizeof(basic::uint16));

			static_mesh->m_index_count = mesh_data.ib.get_size();
		}

		Vertex v;
		static_mesh->m_fmt_list = ::get_fmt_list(&v);

		return static_mesh;
	}

	return nullptr;
}

StaticMesh*
StaticMesh::create( ObjectManager* obj_mng,
                    const char* name,
                    IGpuFactory* factory,
                    basic::Vector< basic::uint8 > data )
{
    ASSERT( name );
    ASSERT( !data.is_empty( ) );

    MeshData mesh_data;
    if ( load_mesh( std::move( data ), mesh_data ) && !mesh_data.vb.is_empty( ) )
    {
        auto vbo = factory->create_vertex_buffer( );
        vbo->load( std::move( mesh_data.vb ) );

        IIndexBuffer* ibo = nullptr;
        if ( !mesh_data.ib.is_empty( ) )
        {
            ibo = factory->create_index_buffer( );
            ibo->load( std::move( mesh_data.ib ) );
        }

        auto static_mesh = NEW_OBJ( StaticMesh, obj_mng, name );

        static_mesh->initialize( vbo, ibo );

        return static_mesh;
    }

    return nullptr;
}
