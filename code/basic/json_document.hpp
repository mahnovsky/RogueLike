#pragma once

#include "json_object.hpp"

namespace basic
{

class JsonDocument
{
public:
    JsonDocument();
    ~JsonDocument();

    const JsonObject* get_root() const;

    bool load(const char* string);

    JsonObject* create();

private:
    JsonObject* m_root;
    Vector<JsonObject*> m_objects;
};

}
