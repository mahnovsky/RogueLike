#include "basic/json_document.hpp"

#include "cJSON/cJSON.h"

namespace basic
{
static void*
alloc( size_t sz )
{
    return basic::mem_alloc( static_cast< basic::uint32 >( sz ) );
}

static void
free_mem( void* ptr )
{
    basic::mem_free( ptr );
}

JsonDocument::JsonDocument( )
    : m_root( nullptr )
    , m_objects( )
{
    static cJSON_Hooks hoocks;
    if ( !hoocks.malloc_fn )
    {
        hoocks.malloc_fn = alloc;
        hoocks.free_fn = free_mem;
        cJSON_InitHooks( &hoocks );
    }
}

JsonDocument::~JsonDocument( )
{
    for ( JsonObject* obj : m_objects )
    {
        DELETE_OBJ( obj );
    }
    m_objects.clear( );
}

const JsonObject*
JsonDocument::get_root( ) const
{
    return m_root;
}

void recursive_apply( JsonDocument* doc, JsonObject* root, const cJSON* node );

static void
apply_value( JsonDocument* doc, Variant& v, const cJSON* node )
{
    if ( cJSON_IsBool( node ) )
    {
        v.set( static_cast< bool >( cJSON_IsTrue( node ) ) );
    }
    else if ( cJSON_IsNumber( node ) )
    {
        v.set( static_cast< float >( node->valuedouble ) );
    }
    else if ( cJSON_IsString( node ) )
    {
        std::string s = node->valuestring;
        v.set( s );
    }
    else if ( cJSON_IsArray( node ) )
    {
        const cJSON* item = nullptr;
        cJSON_ArrayForEach( item, node )
        {
            Variant nv;
            if ( !cJSON_IsObject( item ) )
            {
                apply_value( doc, nv, item );
            }
            else
            {
                JsonObject* obj = doc->create( );

                recursive_apply( doc, obj, item );

                nv.set( obj );
            }

            v.add_array_item( nv );
        }
    }
}

void
recursive_apply( JsonDocument* doc, JsonObject* root, const cJSON* node )
{
    const cJSON* child = node->child;

    while ( child )
    {
        JsonObject* obj = doc->create( );

        if ( cJSON_IsObject( child ) )
        {
            recursive_apply( doc, obj, child );
        }
        else
        {
            Variant v;
            apply_value( doc, v, child );

            obj->set_value( v );
        }

        root->add_object( child->string, obj );

        child = child->next;
    }
}

bool
JsonDocument::load( const char* string )
{
    cJSON* json = cJSON_Parse( string );

    if ( json )
    {
        m_root = create( );

        recursive_apply( this, m_root, json );

        cJSON_Delete( json );

        return true;
    }

    return false;
}

JsonObject*
JsonDocument::create( )
{
    JsonObject* obj = NEW_OBJ( JsonObject );

    m_objects.push( obj );

    return obj;
}
}
