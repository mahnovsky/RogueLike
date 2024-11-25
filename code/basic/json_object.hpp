#pragma once

#include <string>
#include <map>
#include <vector>

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
            float f;
            void* d;
        } o = {0.f};
        std::string s;
    };

    Variant()
        :m_type(VariantType::None)
        ,m_value()
        ,m_array()
    {
    }

    Variant& operator = (const Variant& other)
    {
        m_type = other.m_type;
        m_value.o = other.m_value.o;
        m_value.s = other.m_value.s;
        m_array = other.m_array;

        return *this;
    }

    using string = std::string;

#define CONV(type, en) VariantType conv(const type&) const {return VariantType::en; }
    CONV(bool, Bool)
    CONV(float, Float)
    VariantType conv(const std::string&) const { return VariantType::String; }
    VariantType conv(const void*) const {return VariantType::Data; }
#undef CONV

#define GETTER(type, val) bool get(type& out) const {out = m_value.o.val;return conv(out)==m_type;}
    GETTER(bool, b)
    GETTER(float, f)
    GETTER(void*, d)
    bool get(std::string& out) const { out = m_value.s; return conv(out) == m_type; }
    bool get(std::vector<Variant>& out) const { out = m_array; return m_type == VariantType::Array; }
#undef GETTER

#define SETTER(type, val) void set(type in){ m_value.o.val = in; m_type = conv(in); }
    SETTER(bool, b)
    SETTER(float, f)
    SETTER(void*, d)
    void set(const std::string& s) { m_value.s = s; m_type = conv(s); }
    void set(const std::vector<Variant>& v) { m_type = VariantType::Array; m_array = v; }
#undef SETTER

    void add_array_item(const Variant& val)
    {
        m_array.push_back(val);
        m_type = VariantType::Array;
    }

    VariantType get_type() const
    {
        return m_type;
    }

private:
    VariantType m_type;
    Value m_value;
    std::vector<Variant> m_array;
};

class JsonObject
{
public:
    JsonObject();

    bool is_object() const;

    bool to_bool() const;

    basic::int32 to_int( ) const;

    float to_float( ) const;

    std::string to_string( ) const;

    std::vector<const JsonObject *> to_array() const;

    const JsonObject *get_object( const char* key ) const;

    bool get(const char *key, float& out) const;

    bool get(const char *key, std::string& out) const;

    void set_value(const Variant& t);

    void add_object(const char* name, JsonObject* obj);

    friend class JsonDocument;
private:
    Variant m_value;
    std::map<std::string, JsonObject*> m_objects;
};

}
