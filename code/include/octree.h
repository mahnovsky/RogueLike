#pragma once

#include <vector>
#include <memory>
#include <array>

#include "type_registration.hpp"
#include "generic/generic_object.hpp"
#include "pool.hpp"
#include "render_system.hpp"

struct ViewFrustum;

struct Vec3
{
	float x, y, z;

	Vec3() = default;

	float sq_len() const
	{
		return x * x + y * y + z * z;
	}

	static const Vec3 Zero;
};

inline Vec3 operator + (const Vec3& v1, const Vec3& v2) noexcept
{
	return Vec3{ v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

inline Vec3 operator - (const Vec3& v1, const Vec3& v2) noexcept
{
	return Vec3{ v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

inline bool operator > (const Vec3& v1, const Vec3& v2) noexcept
{
	return v1.sq_len() > v2.sq_len();
}

inline bool operator < (const Vec3& v1, const Vec3& v2) noexcept
{
	return v1.sq_len() < v2.sq_len();
}

inline Vec3 operator / (const Vec3& v1, float k) noexcept
{
	return Vec3{ v1.x / k, v1.y / k, v1.z / k };
}

struct Sphere
{
	glm::vec3 pos;
	float radius;

	Sphere() = default;
};

struct Box
{
	glm::vec3 min;
	glm::vec3 max;

	Box() = default;

	bool is_contain(const glm::vec3& point) const
	{
		return point.x > min.x&& point.y > min.y && point.z > min.z&&
			point.x < max.x&& point.y < max.y && point.z < max.z;
	}

	bool is_contain(const Box& box) const
	{
		return is_contain(box.min) && is_contain(box.max);
	}

	bool is_contain(const Sphere& s) const
	{
		glm::vec3 ss = size();
		glm::vec3 delta = s.pos - min;
		return is_contain(s.pos) && delta.x < ss.x && delta.y < ss.y && delta.z < ss.z;
	}

	glm::vec3 size() const { return max - min; }
};


bool hit_test(const Sphere& s, const Box& b);
bool hit_test(const Sphere& s, const Sphere& b);

class Octree;
class OctreeNode;

enum class ShapeType : short
{
	Sphere,
	Box
};

class OctreeObject : public Component
{
public:
	GENERIC_OBJECT_IMPL(OctreeObject, NS_COMPONENT_TYPE);

	OctreeObject(Entity* ent, const Box& box);

	OctreeObject(Entity* ent, const Sphere& box);

	~OctreeObject();

	void reset(const Box& b);

	void reset(const Sphere& s);

	void set_owner(OctreeNode* node);

	OctreeNode* get_owner() const;

	bool is_inside(const Box& enclosing_box) const;

	bool hit_test(const Sphere& s) const;

	bool hit_test(const ViewFrustum& frustum) const;

	void set_position(const glm::vec3& pos);

	void set_radius(float radius);

	void set_box_size(const glm::vec3& size);

	bool is_exist(ShapeType type) const;

	const Box& get_box() const;

	const Sphere& get_sphere() const;

private:
	class OctreeNode* m_owner;

	union ShapeSelector
	{
		Box box;
		Sphere sphere;

		ShapeSelector(const Box& b) : box(b) {}
		ShapeSelector(const Sphere& s) : sphere(s) {}
	}
	m_shape;
	ShapeType m_shape_type;
};

class OctreeNode
{
public:
	OctreeNode(Octree* tree, const Box& enclosing_box, uint16_t index, OctreeNode* parent);
	~OctreeNode();

	bool add_to_child(OctreeObject* object);

	bool add(OctreeObject* object);

	void update(OctreeObject* object);

	void find(const Sphere& s, std::vector<OctreeObject*>& objects);

	void find(const Sphere& s, const ViewFrustum& frustum, std::vector<OctreeObject*>& objects);

	bool hit_test(const Sphere& s) const;

	void remove_object(OctreeObject* obj);

	void remove_child(OctreeNode* node, uint16_t index);

private:
	void fill_octant(std::array<Box, 8>& octant) const;

	static constexpr float MIN_SIZE = 5.f;

	Octree* m_tree;
	OctreeNode* m_parent;
	Box m_region;
	uint16_t m_index;
	uint16_t m_child_count;
	OctreeNode* m_children[8] = {nullptr};
	std::vector<OctreeObject*> m_objects;
};

class Octree : public IGenericObject
{
public:
	GENERIC_OBJECT_IMPL(Octree, NS_SYSTEM_TYPE);

	Octree(EcsManager* manager, const Box& b);
	~Octree() override = default;

	OctreeNode* create_node(const Box& enclosing_box, OctreeNode* parent, int layer);

	void free_node(OctreeNode* node);

	bool add_object(OctreeObject* object);

	void free_object(OctreeObject* object);

	void find_objects(const Sphere& s, std::vector<OctreeObject*>& objects) const;

	void find_objects(const Sphere& s, const ViewFrustum& frustum, std::vector<OctreeObject*>& objects) const;

	uint32_t bytes_total_size() const;

	void on_object_out(OctreeObject* object);

private:
	EcsManager* m_manager;
	Pool<OctreeNode> m_node_pool;
	Pool<OctreeObject> m_object_pool;
	std::vector<OctreeObject*> m_out_objects;
	OctreeNode* m_root;
};