#pragma once

#include "defines.hpp"

#include "resource_storage.hpp"

struct Frame
{
    float x;
    float y;
    float w;
    float h;
    basic::String name;
};

class Frames : public FileResource
{
public:
    Frames(ObjectManager* manager, const char* file);
    ~Frames() override;

    bool load( ResourceStorage* ) override;

    static Frames* create(ObjectManager *manager, const char* file );

    basic::uint32 get_count() const;

    void get(basic::uint32 index, Frame &frame) const;

    bool get(const basic::String& name, Frame &frame) const;

    void add(const Frame& f);

private:
    basic::Vector<Frame> m_frames;
};
