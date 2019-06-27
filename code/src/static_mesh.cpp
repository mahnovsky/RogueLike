#include "static_mesh.hpp"

StaticMesh::StaticMesh( ObjectManager* obj_mng, const char* name )
    : SharedObject( obj_mng, SharedObjectType::StaticMesh, name )
    , m_vbo( nullptr )
    , m_ibo( nullptr )
{
}

StaticMesh::~StaticMesh( )
{
    DELETE_OBJ( m_vbo );
    DELETE_OBJ( m_ibo );
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
    if ( m_vbo )
    {
        return m_vbo->count( );
    }
    return 0;
}

basic::uint32
StaticMesh::get_index_count( ) const
{
    if ( m_ibo )
    {
        return m_ibo->count( );
    }
    return 0;
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
