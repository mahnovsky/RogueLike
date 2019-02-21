#include "line.hpp"

#include "shader.hpp"
#include "render.hpp"
#include "camera.hpp"

Line::Line()
	: m_width(2.f)
	, m_pos0()
	, m_pos1()
	, m_vao_handle(0)
	, m_vbo_handle(0)
	, m_vb()
	, m_color{ 255, 255, 255, 255 }
	, m_shader(nullptr)
{
	
}

Line::~Line()
{
	if (m_shader)
	{
		m_shader->release();
	}
}

void Line::set_coords(const glm::vec3 & p1, const glm::vec3 & p2)
{
	m_pos0 = p1;
	m_pos1 = p2;

	update();
}

void Line::init(ShaderProgram* shader)
{
	ASSERT(shader != nullptr);
	m_shader = shader;
	m_shader->retain();

	glGenVertexArrays(1, &m_vao_handle);
	glBindVertexArray(m_vao_handle);

	glGenBuffers(1, &m_vbo_handle);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_handle);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	size_t offset = sizeof(glm::vec3);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (GLvoid*)offset);
	glEnableVertexAttribArray(1);

	offset = sizeof(glm::vec3) + sizeof(basic::Color);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offset);
	glEnableVertexAttribArray(2);


	glBindVertexArray(0);
}

void Line::draw( IRender* render, ICamera* cam ) const
{
	m_shader->bind();

	glBindVertexArray(m_vao_handle);

	glm::mat4 pv(1.f);
	cam->get_matrix(pv);

	render->push_mvp(pv);

	glDrawArrays(GL_TRIANGLES, 0, m_vb.get_size());

	render->pop_mvp();

	glBindVertexArray(0);

	m_shader->unbind();
}

basic::uint32 Line::get_element_count() const
{
	return basic::uint32();
}

void Line::get_matrix(glm::mat4 & out) const
{
	out = glm::mat4();
}

void Line::update()
{
	glm::vec3 delta = m_pos1 - m_pos0;
	float sq_dist = glm::dot(delta, delta);
	ASSERT(sq_dist > 0.1f);
	ASSERT(m_width > 0.1f);

	glm::vec3 up{ 0.f, 0.f, 1.f };
	glm::vec3 dir = glm::normalize(glm::cross(delta, up));

	const float half_width = m_width / 2;
	glm::vec3 p1 = (dir * half_width) + m_pos0;
	glm::vec3 p2 = (dir * half_width) + m_pos1;
	glm::vec3 p3 = (dir * -half_width) + m_pos0;
	glm::vec3 p4 = (dir * -half_width) + m_pos1;

	m_vb.push({ p1, m_color, glm::vec2() });
	m_vb.push({ p2, m_color, glm::vec2() });
	m_vb.push({ p3, m_color, glm::vec2() });

	m_vb.push({ p4, m_color, glm::vec2() });
	m_vb.push({ p3, m_color, glm::vec2() });
	m_vb.push({ p2, m_color, glm::vec2() });

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_handle);

	glBufferData(
		GL_ARRAY_BUFFER, sizeof(Vertex) * m_vb.get_size(), m_vb.get_raw(), GL_STATIC_DRAW);
}
