#pragma once

#include "debug.hpp"
#include "file.hpp"
#include "hash_map.hpp"
#include "image.hpp"
#include "json_document.hpp"
#include "memory.hpp"
#include "pool.hpp"
#include "string.hpp"
#include "time.hpp"
#include "vector.hpp"

#define GENERATE_COMP( name, type, field_type, field ) \
    struct name                                        \
    {                                                  \
        const field_type& field;                       \
        name( const field_type& ft )                   \
            : field( ft )                              \
        {                                              \
        }                                              \
        bool                                           \
        operator( )( const type& t ) const             \
        {                                              \
            return t.field == field;                   \
        }                                              \
    };

#define GENERATE_GREATER_COMP_PTR( name, type, field )      \
    struct name                                             \
    {                                                       \
        bool                                                \
        operator( )( const type* t1, const type* t2 ) const \
        {                                                   \
            return t1->field > t2->field;                   \
        }                                                   \
    };
