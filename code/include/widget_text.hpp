#pragma once

#include "defines.hpp"

#include "widget.hpp"

class WidgetText : public Widget
{
public:
    WidgetText(ObjectManager *manager,
               const glm::vec2 &size);

    ~WidgetText() override;

    void init(ResourceStorage* storage) override;

    void set_text(const basic::String& text);

    void set_align(AlignH horizontal);

    void set_align(AlignV vertical);
private:
    void update();

    void apply_align();

private:
    basic::String m_font_name;
    se::Font* m_font;
    basic::String m_text;
    RenderNode* m_text_render;
    glm::vec2 m_text_size;
};
