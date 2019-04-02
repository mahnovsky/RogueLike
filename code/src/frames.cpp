#include "frames.hpp"

#include "config.hpp"

#define FILE_PATH "textures/"

Frames::Frames(ObjectManager* manager, const char* file)
    : FileResource (manager, file)
    , m_frames()
{
}

Frames::~Frames()
{
}

bool Frames::load(ResourceStorage * storage)
{
    basic::String file = FILE_PATH + get_name() + ".conf";
    Config* conf = storage->get_resorce<Config>(file.get_cstr());
    if(conf)
    {
        const basic::JsonObject* obj = conf->get_values("frames");

        auto frames = obj->to_array();
        for(auto& frame : frames)
        {
            Frame f;
            bool res = frame->get("x", f.x) &&
                    frame->get("y", f.y) &&
                    frame->get("w", f.w) &&
                    frame->get("h", f.h) &&
                    frame->get("name", f.name);

            if(res)
            {
                m_frames.push(f);
            }
        }
    }

    return !m_frames.is_empty();
}

Frames *Frames::create(ObjectManager *manager, const char *file)
{
    return NEW_OBJ(Frames, manager, file);
}

basic::uint32 Frames::get_count() const
{
    return m_frames.get_size();
}

void Frames::get(basic::uint32 index, Frame &frame) const
{
    ASSERT(index < m_frames.get_size());

    frame = m_frames[index];
}

bool Frames::get(const basic::String &name, Frame &frame) const
{
    ASSERT(!name.is_empty());
    for(auto& f : m_frames)
    {
        if(f.name == name)
        {
            frame = f;
            return true;
        }
    }
    return false;
}

void Frames::add(const Frame &f)
{
    m_frames.push(f);
}
