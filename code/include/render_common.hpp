#pragma once

extern "C"
{
#define GLEW_STATIC
#include <GL/glew.h>
}

#include <GL/gl.h>

#include <cstddef>
#include <memory>
#include <vector>

#include "basic/util_functions.hpp"

using byte = unsigned char;
using word = unsigned short;

struct Vertex
{
    float x, y, z;
    byte r, g, b, a;
};

template < GLenum type, class DataType >
class RenderBuffer
{
public:
    void
    init( DataType* data, GLuint count )
    {
        glGenBuffers( 1, &m_buffer_id );

        update( data, count );
    }

    void
    update( DataType* data, GLuint count )
    {
        glBindBuffer( type, m_buffer_id );

        glBufferData( type, sizeof( DataType ) * count, data, GL_STATIC_DRAW );
    }

    GLuint
    get_buffer_id( ) const
    {
        return m_buffer_id;
    }

private:
    GLuint m_buffer_id;
};

class IRenderData
{
public:
    virtual unsigned int get_buffer_array( ) const = 0;

    virtual void set_vetices( const std::vector< Vertex >& vertices ) = 0;

    virtual void set_indices( const std::vector< word >& indices ) = 0;

    virtual size_t get_element_count( ) const = 0;

    virtual void init( ) = 0;

    virtual void update( ) = 0;
};

class RenderData : public IRenderData
{
public:
    RenderData( )
        : m_vao( 0 )
        , m_vetices( )
        , m_indices( )
        , m_vbo( )
        , m_ibo( )
    {
    }

    void
    init( ) override
    {
        glGenVertexArrays( 1, &m_vao );
        glBindVertexArray( m_vao );

        m_vbo.init( m_vetices.data( ), m_vetices.size( ) );

        m_ibo.init( m_indices.data( ), m_indices.size( ) );

        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (GLvoid*)0 );
        glEnableVertexAttribArray( 0 );

        size_t offset = sizeof( float ) * 3;
        glVertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof( Vertex ), (GLvoid*)offset );
        glEnableVertexAttribArray( 1 );

        glBindVertexArray( 0 );
    }

    void
    update( )
    {
        glBindVertexArray( m_vao );

        m_vbo.update( m_vetices.data( ), m_vetices.size( ) );
        m_ibo.update( m_indices.data( ), m_indices.size( ) );

        glBindVertexArray( 0 );
    }

    unsigned int
    get_buffer_array( ) const override
    {
        return m_vao;
    }

    void
    set_vetices( const std::vector< Vertex >& vertices ) override
    {
        m_vetices.clear( );
        m_vetices.insert( m_vetices.begin( ), vertices.begin( ), vertices.end( ) );
    }

    void
    set_indices( const std::vector< word >& indices )
    {
        m_indices.clear( );
        m_indices.insert( m_indices.begin( ), indices.begin( ), indices.end( ) );
    }

    size_t
    get_element_count( ) const override
    {
        return m_indices.size( );
    }

private:
    GLuint m_vao;

    std::vector< Vertex > m_vetices;
    std::vector< word > m_indices;

    RenderBuffer< GL_ARRAY_BUFFER, Vertex > m_vbo;
    RenderBuffer< GL_ELEMENT_ARRAY_BUFFER, word > m_ibo;
};
