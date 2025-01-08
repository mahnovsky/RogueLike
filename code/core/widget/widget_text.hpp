#pragma once

#include "defines.hpp"

#include "widget.hpp"

struct TextData
{
	std::string text;
	std::string font_name;
	int32_t font_size;
};

class WidgetText : public Widget
{
public:
    WidgetText(core::WidgetSystem* root);

    ~WidgetText() override = default;

    void initialize();

    void set_text(const std::string_view& text);

    void set_color(const basic::Color& color);

    basic::Color get_color( ) const;
	
private:
    void update();

    void apply_align();

	void draw(IRender* render) override;

private:
	core::FTransform m_text_transform;
    std::string m_font_name;
    std::shared_ptr<se::Font> m_font;
	std::string m_text;
    IRenderObject* m_text_render;
    glm::vec2 m_text_size;
	basic::Color m_color;
};
