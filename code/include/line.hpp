#pragma once

#include "defines.hpp"
#include "render_object.hpp"

class Line : public IRenderObject
{
public:
	Line();
	~Line() override;

    void set_color( const basic::Color& color );

    void set_pos0( const glm::vec3& p0 );

    void set_pos1( const glm::vec3& p1 );

    void set_coords(const glm::vec3& p0, const glm::vec3& p1);

    void set_width( float width );

	void init(ShaderProgram* shader);

	void draw(IRender* render, ICamera* cam) const override;
	
	basic::uint32 get_element_count() const override;

	void get_matrix(glm::mat4& out) const override;

private:
    void update( VertexBufferT& vb );

private:
	float m_width;
	glm::vec3 m_pos0;
    glm::vec3 m_pos1;
    basic::Color m_color;
    RenderNode* m_render_node;
};
