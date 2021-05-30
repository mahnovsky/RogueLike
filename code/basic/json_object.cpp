#include "json_object.hpp"

namespace basic
{

JsonObject::JsonObject()
    :m_value()
    ,m_objects()
{
}

bool JsonObject::is_object() const
{
    return m_objects.size() > 0;
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
    return static_cast<int32>(to_float());
}

float JsonObject::to_float() const
{
    float res = 0.f;
    bool success = m_value.get(res);
    ASSERT(success);

    return res;
}

std::string JsonObject::to_string() const
{
    std::string res;
    bool success = m_value.get(res);
    ASSERT(success);

    return res;
}

std::vector<const JsonObject *> JsonObject::to_array() const
{
    std::vector<const JsonObject*> objs;

    if(m_value.get_type() == VariantType::Array)
    {
        std::vector<Variant> tmp;
        m_value.get( tmp );
        if(!tmp.empty())
        {
            for (auto& v : tmp )
            {
                if(v.get_type() == VariantType::Data)
                {
                    void* data = nullptr;
                    v.get(data);
                    objs.push_back( static_cast<const JsonObject*>(data) );
                }
            }
        }
    }

    return objs;
}

const JsonObject *JsonObject::get_object(const char *key) const
{
    const auto it = m_objects.find(key);

    if(it != m_objects.end())
    {
        return it->second;
    }

    return nullptr;
}

bool JsonObject::get(const char *key, float &out) const
{
    const JsonObject* obj = get_object(key);
    if(obj)
    {
        out = obj->to_float();

        return true;
    }
    return false;
}

bool JsonObject::get(const char *key, std::string& out) const
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
    m_objects.insert({ name, obj });
}

}
