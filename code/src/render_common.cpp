#include "render_common.hpp"
#include "basic/file.hpp"
#include "basic/string.hpp"

static basic::uint16
convert( const basic::String& str )
{
    return static_cast< basic::uint16 >( atoi( str.get_cstr( ) ) );
}

struct MeshVertex
{
    glm::vec3 pos;
    glm::vec2 uv;
    basic::uint16 index;
    basic::uint16 vi;
    basic::uint16 ti;

    MeshVertex()
        :pos()
        ,uv()
        ,vi(0)
        ,ti(0)
    {}
};

bool
load_mesh( const char* file, Mesh& out_mesh )
{
    basic::Vector< char > data{std::move( basic::get_file_content( file ) )};

    if ( data.is_empty( ) )
    {
        return false;
    }

    basic::Vector< glm::vec3 > vert_coords;
    basic::Vector< glm::vec2 > tex_coords;
    basic::Vector< MeshVertex > vertexes;

    size_t offset = 0;

    while ( offset < data.get_size( ) )
    {
        basic::String line
                = basic::String::read_line( data.get_raw( ) + offset, data.get_size( ) - offset );

        offset += line.get_size( ) + 1;
        if ( offset >= data.get_size( ) )
        {
            break;
        }

        if ( line.front( ) == '#' || line.is_empty( ) )
        {
            continue;
        }

        basic::Vector< basic::String > items;
        line.split( items, ' ' );

        if( items.is_empty() )
        {
            continue;
        }

        if ( items.front( ) == "v" && items.get_size( ) > 3 )
        {
            float x = atof( items[ 1 ].get_cstr( ) );
            float y = atof( items[ 2 ].get_cstr( ) );
            float z = atof( items[ 3 ].get_cstr( ) );

            vert_coords.push( {x, y, z} );
        }
        else if ( items.front( ) == "vt" && items.get_size( ) > 2 )
        {
            float tx = atof( items[ 1 ].get_cstr( ) );
            float ty = atof( items[ 2 ].get_cstr( ) );

            tex_coords.push( {tx, ty} );
        }
        else if ( items.front( ) == "f" && items.get_size( ) > 3 )
        {
            basic::Vector< basic::uint16 > faces;
            items[ 1 ].split_to< basic::uint16 >( faces, '/', convert );
            items[ 2 ].split_to< basic::uint16 >( faces, '/', convert );
            items[ 3 ].split_to< basic::uint16 >( faces, '/', convert );

            size_t step = faces.get_size( ) >= 6 ? 2 : 1;
            for ( size_t i = 0; i < faces.get_size( ); i += step )
            {
                MeshVertex v;
                v.vi = faces[ i ];
                v.pos = vert_coords.get( v.vi - 1 );

                if ( step > 1 && !tex_coords.is_empty() )
                {
                    v.ti = faces[ i + 1 ];
                    v.uv = tex_coords.get( faces[ v.ti - 1 ] );
                }

                int index = -1;
                bool has_vertex = false;
                for ( size_t i = 0; i < vertexes.get_size( ); ++i )
                {
                    auto& vv = vertexes[ i ];
                    has_vertex = vv.vi == v.vi && vv.ti == v.ti;
                    if ( has_vertex )
                    {
                        index = vv.index;
                        break;
                    }
                }
                if ( index < 0 )
                {
                    index = vertexes.get_size( );
                    v.index = index;
                    vertexes.push( v );
                }

                out_mesh.ib.push( index );
            }
        }
    }

    out_mesh.vb.reserve( vertexes.get_size( ) );
    for ( size_t i = 0; i < vertexes.get_size( ); ++i )
    {
        auto& vv = vertexes[ i ];

        out_mesh.vb.push( {vv.pos, {255, 255, 255, 255}, vv.uv} );
    }

    return true;
}
