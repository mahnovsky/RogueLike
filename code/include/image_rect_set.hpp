#pragma once

#include "defines.hpp"

#include "resource_storage.hpp"

struct ImageRect
{
    float x;
    float y;
    float w;
    float h;
    basic::String name;
};

class ImageRectSet : public FileResource
{
public:
    ImageRectSet(ObjectManager* manager, const char* file);
    ~ImageRectSet() override;

    bool load( ResourceStorage* ) override;

    static ImageRectSet* create(ObjectManager *manager, const char* file );

    basic::uint32 get_count() const;

    void get(basic::uint32 index, ImageRect &frame) const;

    bool get(const basic::String& name, ImageRect &frame) const;

    void add(const ImageRect& f);

private:
    basic::Vector<ImageRect> m_frames;
};
