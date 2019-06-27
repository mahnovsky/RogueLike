#include "image_rect_set.hpp"

#include "config.hpp"

#define FILE_PATH "textures/"

ImageRectSet::ImageRectSet( ObjectManager* manager, const char* file )
    : FileResource( manager, SharedObjectType::ImageRectSet, file )
    , m_frames( )
{
}

ImageRectSet::~ImageRectSet( )
{
}

bool
ImageRectSet::load( ResourceStorage* storage )
{
    basic::String file = FILE_PATH + get_name( ) + ".conf";
    Config* conf = storage->get_resorce< Config >( file.get_cstr( ) );
    if ( conf )
    {
        const basic::JsonObject* obj = conf->get_values( "rects" );

        auto frames = obj->to_array( );
        for ( auto& frame : frames )
        {
            ImageRect f;
            bool res = frame->get( "x", f.x ) && frame->get( "y", f.y ) && frame->get( "w", f.w )
                       && frame->get( "h", f.h ) && frame->get( "name", f.name );

            if ( res )
            {
                m_frames.push( f );
            }
        }
    }

    return !m_frames.is_empty( );
}

ImageRectSet*
ImageRectSet::create( ObjectManager* manager, const char* file )
{
    return NEW_OBJ( ImageRectSet, manager, file );
}

basic::uint32
ImageRectSet::get_count( ) const
{
    return m_frames.get_size( );
}

void
ImageRectSet::get( basic::uint32 index, ImageRect& frame ) const
{
    ASSERT( index < m_frames.get_size( ) );

    frame = m_frames[ index ];
}

bool
ImageRectSet::get( const basic::String& name, ImageRect& frame ) const
{
    ASSERT( !name.is_empty( ) );
    for ( auto& f : m_frames )
    {
        if ( f.name == name )
        {
            frame = f;
            return true;
        }
    }
    return false;
}

void
ImageRectSet::add( const ImageRect& f )
{
    m_frames.push( f );
}
