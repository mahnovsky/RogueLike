#pragma once

#include "defines.hpp"

#include "widget.hpp"

class WidgetText : public Widget
{
public:
    WidgetText(RootWidget* root);

    ~WidgetText() override;

    void init(ResourceStorage* storage) override;

    void set_text(const std::string& text);

    void set_color(const basic::Color& color);

    basic::Color get_color( ) const;

    void set_align(AlignH horizontal);

    void set_align(AlignV vertical);
private:
    void update();

    void apply_align();

	void draw(IRender* render) override;

private:
    std::string m_font_name;
    std::shared_ptr<se::Font> m_font;
	std::string m_text;
    IRenderObject* m_text_render;
    glm::vec2 m_text_size;
};
