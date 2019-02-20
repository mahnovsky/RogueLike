#pragma once

#include "defines.hpp"
#include "render_object.hpp"

class Line : public IRenderObject
{
public:
	Line();
	~Line() override;

	void set_coords(const glm::vec3& p1, const glm::vec3& p2);

	void init(ShaderProgram* shader);

	void draw(IRender* render, ICamera* cam) const override;
	
	basic::uint32 get_element_count() const override;

	void get_matrix(glm::mat4& out) const override;

private:
	void update();

private:
	float m_width;
	glm::vec3 m_pos0;
	glm::vec3 m_pos1;
	basic::uint32 m_vao_handle;
	basic::uint32 m_vbo_handle;
	VertexBuffer m_vb;
	basic::Color m_color;
	ShaderProgram* m_shader;
};
