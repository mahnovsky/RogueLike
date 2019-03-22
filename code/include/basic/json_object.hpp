#pragma once

#include "hash_map.hpp"

#include "cJSON/cJSON.h"

namespace basic
{

enum class VariantType
{
    None,
    Bool,
    Int,
    Float,
    String
};

class Variant
{
public:
    Variant()
        :m_type(VariantType::None)
        ,m_value()
    {
    }

#define CONV(type, en) VariantType conv(const type&) const {return VariantType::en; }
    CONV(bool, Bool)
    CONV(int32, Int)
    CONV(float, Float)
    CONV(String, String)
#undef CONV

#define GETTER(type, val) bool get(type& out) const {out = m_value.o.val;return conv(out)==m_type;}
    GETTER(bool, b)
    GETTER(int32, i)
    GETTER(float, f)
    bool get(String& out) const { out = m_value.s; return conv(out) == m_type; }
#undef GETTER

#define SETTER(type, val) void set(const type& in){ m_value.o.val = in; m_type = conv(in); }
    SETTER(bool, b)
    SETTER(int32, i)
    SETTER(float, f)
    void set(const String& s) { m_value.s = s; m_type = conv(s); }
#undef SETTER

private:
    struct Value
    {
        union
        {
            bool b;
            int32 i;
            float f;
        } o;
        String s;
    };

    VariantType m_type;
    Value m_value;
};

class JsonObject
{
public:
    JsonObject();

    bool is_object() const;

    bool to_bool() const;

    basic::int32 to_int( ) const;

    float to_float( ) const;

    String to_string( ) const;

    const JsonObject *get_object( const char* key ) const;

private:
    Variant m_value;
    basic::HashMap<basic::String, JsonObject*> m_objects;
};

}
