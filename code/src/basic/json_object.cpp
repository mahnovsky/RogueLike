#include "basic/json_object.hpp"

namespace basic
{

JsonObject::JsonObject()
    :m_value()
    ,m_objects()
{
}

bool JsonObject::is_object() const
{
    return m_objects.get_size() > 0;
}

bool JsonObject::to_bool() const
{
    bool res = false;
    bool success = m_value.get(res);
    ASSERT(success);

    return res;
}

int32 JsonObject::to_int() const
{
    int32 res = 0;
    bool success = m_value.get(res);
    ASSERT(success);

    return res;
}

float JsonObject::to_float() const
{
    float res = 0.f;
    bool success = m_value.get(res);
    ASSERT(success);

    return res;
}

String JsonObject::to_string() const
{
    String res;
    bool success = m_value.get(res);
    ASSERT(success);

    return res;
}

const JsonObject *JsonObject::get_object(const char *key) const
{
    const auto it = m_objects.find(key);

    if(it.is_valid())
    {
        return it.value();
    }

    return nullptr;
}

bool JsonObject::get_string(const char *key, String& out) const
{
    const JsonObject* obj = get_object(key);
    if(obj)
    {
        out = obj->to_string();

        return true;
    }
    return false;
}

void JsonObject::set_value(const Variant &t)
{
    m_value = t;
}

void JsonObject::add_object(const char *name, JsonObject *obj)
{
    m_objects.insert( name, obj );
}

}
