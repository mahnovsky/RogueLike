#include "render_common.hpp"
#include "basic/file.hpp"
#include "basic/string.hpp"

static basic::uint16
convert( basic::String str )
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
};

bool
load_mesh( const char* file, Mesh& out_mesh )
{
    basic::Vector< basic::uint8 > data{basic::get_file_content( file )};

    if ( data.is_empty( ) )
    {
        return false;
    }

    basic::Vector< glm::vec3 > vert_coords;
    basic::Vector< glm::vec2 > tex_coords;
    basic::Vector< MeshVertex > vertexes;

    basic::uint32 offset = 0;
	int line_counter = 0;

    while ( offset < data.get_size( ) )
    {
		++line_counter;

        basic::String line
                = basic::String::read_line( reinterpret_cast<basic::char_t*>(data.get_raw( ) + offset),
                                            static_cast<basic::uint32>(data.get_size( ) - offset) );

        offset += line.get_size( );
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
            float x = static_cast<float>( atof( items[ 1 ].get_cstr( ) ) );
            float y = static_cast<float>( atof( items[ 2 ].get_cstr( ) ) );
            float z = static_cast<float>( atof( items[ 3 ].get_cstr( ) ) );

            vert_coords.push( {x, y, z} );
        }
        else if ( items.front( ) == "vt" && items.get_size( ) > 2 )
        {
            float tx = static_cast<float>( atof( items[ 1 ].get_cstr( ) ) );
            float ty = static_cast<float>( atof( items[ 2 ].get_cstr( ) ) );

            tex_coords.push( {tx, ty} );
        }
        else if ( items.front( ) == "f" && items.get_size( ) > 3 )
        {
            basic::Vector< basic::uint16 > faces;
            items[ 1 ].split_to< basic::uint16 >( faces, '/', convert );
            items[ 2 ].split_to< basic::uint16 >( faces, '/', convert );
            items[ 3 ].split_to< basic::uint16 >( faces, '/', convert );

            basic::uint32 step = faces.get_size( ) >= 6 ? 2 : 1;
            for ( basic::uint32 i = 0; i < faces.get_size( ); i += step )
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
                for ( basic::uint32 i = 0; i < vertexes.get_size( ); ++i )
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
    for ( basic::uint32 i = 0; i < vertexes.get_size( ); ++i )
    {
        auto& vv = vertexes[ i ];

        out_mesh.vb.push( {vv.pos, {255, 255, 255, 255}, vv.uv} );
    }

    return true;
}
