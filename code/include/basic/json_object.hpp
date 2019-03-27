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
    Data,
    String,
    Array
};

class Variant
{
public:
    struct Value
    {
        union
        {
            bool b;
            int32 i;
            float f;
            void* d;
        } o;
        String s;
    };

    Variant()
        :m_type(VariantType::None)
        ,m_value()
    {
    }

    Variant& operator = (const Variant& other)
    {
        m_type = other.m_type;
        m_value.o = other.m_value.o;
        m_value.s = other.m_value.s;

        return *this;
    }

#define CONV(type, en) VariantType conv(const type&) const {return VariantType::en; }
    CONV(bool, Bool)
    CONV(int32, Int)
    CONV(float, Float)
    CONV(String, String)
    CONV(void*, Data)
#undef CONV

#define GETTER(type, val) bool get(type& out) const {out = m_value.o.val;return conv(out)==m_type;}
    GETTER(bool, b)
    GETTER(int32, i)
    GETTER(float, f)
    GETTER(void*, d)
    bool get(String& out) const { out = m_value.s; return conv(out) == m_type; }
    bool get(Vector<Variant>& out) const { out = m_array; return m_type == VariantType::Array; }
#undef GETTER

#define SETTER(type, val) void set(type in){ m_value.o.val = in; m_type = conv(in); }
    SETTER(bool, b)
    SETTER(int32, i)
    SETTER(float, f)
    SETTER(void*, d)
    void set(const String& s) { m_value.s = s; m_type = conv(s); }
    void set(const Vector<Variant>& v) { m_type = VariantType::Array; m_array = v; }
#undef SETTER

    void add_array_item(const Variant& val)
    {
        m_array.push(val);
    }

private:
    VariantType m_type;
    Value m_value;
    Vector<Variant> m_array;
};

class JsonObject
{
    JsonObject();
public:
    bool is_object() const;

    bool to_bool() const;

    basic::int32 to_int( ) const;

    float to_float( ) const;

    String to_string( ) const;

    const JsonObject *get_object( const char* key ) const;

    bool get_string(const char *key, String& out) const;

    void set_value(const Variant& t);

    void add_object(const char* name, JsonObject* obj);

    friend class JsonDocument;
private:
    Variant m_value;
    basic::HashMap<basic::String, JsonObject*> m_objects;
};

}
