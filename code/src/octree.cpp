#include "octree.h"
#include "frustum.hpp"

bool hit_test(const Sphere& s, const Box& b)
{
	const glm::vec3 closet {
	 std::max(b.min.x, std::min(s.pos.x, b.max.x)),
	 std::max(b.min.y, std::min(s.pos.y, b.max.y)),
	 std::max(b.min.z, std::min(s.pos.z, b.max.z))
	};
	
	return glm::length2(closet - s.pos) < (s.radius*s.radius);
}

bool hit_test(const Sphere& s, const Sphere& b)
{
	const auto dist = s.pos - b.pos;
	const float max_dist = (s.radius + b.radius);

	return glm::length2(dist) < (max_dist * max_dist);
}

OctreeObject::OctreeObject(Entity* ent, const Box& box)
	: Component(ent)
	, m_owner(nullptr)
    , m_shape(box)
    , m_shape_type(ShapeType::Box)
{
	add_listen_component<Transform>();
}

OctreeObject::OctreeObject(Entity* ent, const Sphere& s)
	: Component(ent)
	, m_owner(nullptr)
    , m_shape(s)
    , m_shape_type(ShapeType::Sphere)
{
	add_listen_component<Transform>();
}

OctreeObject::~OctreeObject()
{
	if (m_owner)
	{
		m_owner->remove_object(this);
		m_owner = nullptr;
	}
}

void OctreeObject::reset(const Box& b)
{
	m_shape_type = ShapeType::Box;
	m_shape.box = b;

	m_owner->update(this);
}

void OctreeObject::reset(const Sphere& s)
{
	m_shape_type = ShapeType::Sphere;
	m_shape.sphere = s;

	m_owner->update(this);
}

void OctreeObject::set_owner(OctreeNode* node)
{
	m_owner = node;
}

OctreeNode* OctreeObject::get_owner() const
{
	return m_owner;
}

bool OctreeObject::is_inside(const Box& enclosing_box) const
{
	if (m_shape_type == ShapeType::Box)
	{
		return enclosing_box.is_contain(m_shape.box);
	}
	return enclosing_box.is_contain(m_shape.sphere);
}

bool OctreeObject::hit_test(const Sphere& s) const
{
	if (m_shape_type == ShapeType::Box)
	{
		return ::hit_test(s, m_shape.box);
	}

	return ::hit_test(s, m_shape.sphere);
}

bool OctreeObject::hit_test(const ViewFrustum& frustum) const
{
	if (m_shape_type == ShapeType::Box)
	{
		return get_box_visibility(frustum, m_shape.box) != BoxVisibility::Invisible;
	}

	return is_sphere_visible(frustum, m_shape.sphere);
}

void OctreeObject::set_position(const glm::vec3& pos)
{
	bool was_changed = false;

	if (m_shape_type == ShapeType::Box)
	{
		auto halfs = m_shape.box.size() / 2.f;

		glm::vec3 prev_min = m_shape.box.min;
		glm::vec3 prev_max = m_shape.box.max;

		m_shape.box.min = pos - halfs;
		m_shape.box.max = pos + halfs;

		was_changed = glm::all(glm::notEqual(m_shape.box.min, prev_min)) &&
			glm::all(glm::notEqual(m_shape.box.max, prev_max));
	}
	else
	{
		was_changed = glm::all(glm::notEqual(m_shape.sphere.pos, pos));
		m_shape.sphere.pos = pos;
	}

	if (was_changed && m_owner)
	{
		m_owner->update(this);
	}
}

void OctreeObject::set_radius(float radius)
{
	assert(m_shape_type == ShapeType::Sphere);

	m_shape.sphere.radius = radius;
	m_owner->update(this);
}

void OctreeObject::set_box_size(const glm::vec3& size)
{
	assert(m_shape_type == ShapeType::Sphere);

	m_shape.box.max = m_shape.box.min + size;
	m_owner->update(this);
}

bool OctreeObject::is_exist(ShapeType type) const
{
	return m_shape_type == type;
}

const Box& OctreeObject::get_box() const
{
	return m_shape.box;
}

const Sphere& OctreeObject::get_sphere() const
{
	return m_shape.sphere;
}

void OctreeObject::on_event(Component* sender, ComponentEvent event_type)
{
	if (sender->type_index() == TypeInfo<Transform, ComponentType>::type_index &&
		event_type == ComponentEvent::Updated)
	{
		auto transform = fast_cast<Transform, ComponentType>(sender);

		set_position(transform->get_position());
	}
}

OctreeNode::OctreeNode(Octree* tree, const Box& enclosing_box, uint16_t index, OctreeNode* parent)
	: m_tree(tree)
	, m_parent(parent)
	, m_region(enclosing_box)
	, m_index(index)
	, m_child_count(0)
{}

OctreeNode::~OctreeNode()
{
	for (auto obj : m_objects)
	{
		if (obj->get_owner() == this)
		{
			obj->set_owner(nullptr);
		}
	}
	if (m_parent)
	{
		for (size_t i = 0; i < 8; ++i)
		{
			if (this == m_parent->m_children[i])
			{
				m_parent->m_children[i] = nullptr;
				m_parent->m_child_count -= 1;
				m_parent = nullptr;

				break;
			}
		}
	}
}

bool OctreeNode::add_to_child(OctreeObject* object)
{
	std::array<Box, 8> octant = {};
	fill_octant(octant);

	if( octant[0].size().x < MIN_SIZE && 
		octant[0].size().y < MIN_SIZE && 
		octant[0].size().z < MIN_SIZE)
	{
		return false;	
	}

	for (size_t i = 0; i < octant.size(); ++i)
	{
		if (object->is_inside(octant[i]))
		{
			if (!m_children[i])
			{
				m_children[i] = m_tree->create_node(octant[i], this,  i);
				++m_child_count;
			}

			m_children[i]->add(object);

			return true;
		}
	}

	return false;
}

bool OctreeNode::add(OctreeObject* object)
{
	if (object->is_inside(m_region))
	{
		if (add_to_child(object))
		{
			return true;
		}

		m_objects.push_back(object);
		object->set_owner(this);

		return true;
	}

	return false;
}

void OctreeNode::update(OctreeObject* object)
{
	if(object->is_inside(m_region))
	{
		if (object->get_owner() != this)
		{
			add(object);
			return;
		}

		if(add_to_child(object))
		{
			remove_object(object);
		}
	}
	else 
	{
		if (m_parent)
		{
			m_parent->update(object);
		}
		else
		{
			m_tree->on_object_out(object);
		}
		remove_object(object);		
	}
}

void OctreeNode::find(const Sphere& s, std::vector<OctreeObject*>& objects)
{
	if (hit_test(s))
	{
		for (OctreeObject* obj : m_objects)
		{
			if (obj->hit_test(s))
			{
				objects.push_back(obj);
			}
		}

		for (size_t i = 0; i < 8; ++i)
		{
			if (m_children[i] && m_children[i]->hit_test(s))
			{
				m_children[i]->find(s, objects);
			}
		}
	}
}

void OctreeNode::find(const Sphere& s, const ViewFrustum& frustum, std::vector<OctreeObject*>& objects)
{
	if (hit_test(s))
	{
		for (OctreeObject* obj : m_objects)
		{
			if (obj->hit_test(frustum))
			{
				objects.push_back(obj);
			}
		}

		for (size_t i = 0; i < 8; ++i)
		{
			if (m_children[i] && m_children[i]->hit_test(s))
			{
				m_children[i]->find(s, frustum, objects);
			}
		}
	}
}

bool OctreeNode::hit_test(const Sphere& s) const
{
	return ::hit_test(s, m_region);
}

void OctreeNode::fill_octant(std::array<Box, 8>& octant) const
{
	const glm::vec3 dimensions = m_region.size();

	const glm::vec3 half = dimensions / 2.0f;
	const glm::vec3 center = m_region.min + half;

	octant[0].min = m_region.min;
	octant[0].max = center;

	octant[1].min = {center.x, m_region.min.y, m_region.min.z};
	octant[1].max = {m_region.max.x, center.y, center.z};

	octant[2].min = {center.x, m_region.min.y, center.z};
	octant[2].max = {m_region.max.x, center.y, m_region.max.z};

	octant[3].min = {m_region.min.x, m_region.min.y, center.z};
	octant[3].max = {center.x, center.y, m_region.max.z};

	octant[4].min = {m_region.min.x, center.y, m_region.min.z};
	octant[4].max = {center.x, m_region.max.y, center.z};

	octant[5].min = {center.x, center.y, m_region.min.z};
	octant[5].max = {m_region.max.x, m_region.max.y, center.z};

	octant[6].min = center;
	octant[6].max = m_region.max;

	octant[7].min = {m_region.min.x, center.y, center.z};
	octant[7].max = {center.x, m_region.max.y, m_region.max.z};
}

void OctreeNode::remove_object(OctreeObject* obj)
{
	if (m_objects.empty())
	{
		return;
	}
	auto it = std::find(m_objects.begin(), m_objects.end(), obj);
	
	if (it != m_objects.end())
	{
		m_objects.erase(it);

		if(obj->get_owner() == this)
			obj->set_owner(nullptr);

		if(m_objects.empty() && m_child_count == 0 && m_parent != nullptr)
		{
			m_parent->remove_child(this, m_index);

			m_tree->free_node(this);
		}
	}
}

void OctreeNode::remove_child(OctreeNode* node, uint16_t index)
{
	assert(m_child_count > 0);
	if (m_children[index] == this)
	{
		m_children[index] = nullptr;
		--m_child_count;
	}
}

Octree::Octree(EntityComponentManager* manager, const Box& b)
	: m_manager(manager)
	, m_root(m_node_pool.alloc(this, b, 0, nullptr))
{
}

OctreeNode* Octree::create_node(const Box& enclosing_box, OctreeNode* parent, int layer)
{
	return  m_node_pool.alloc(this, enclosing_box, layer, parent);
}

void Octree::free_node(OctreeNode* node)
{
	m_node_pool.free(node);
}

bool Octree::add_object(OctreeObject* object)
{
	return m_root->add(object);
}

void Octree::free_object(OctreeObject* object)
{
	m_object_pool.free(object);
}

void Octree::find_objects(const Sphere& s, std::vector<OctreeObject*>& objects) const
{
	m_root->find(s, objects);
}

void Octree::find_objects(const Sphere& s, const ViewFrustum& frustum, std::vector<OctreeObject*>& objects) const
{
	m_root->find(s, frustum, objects);
}

uint32_t Octree::bytes_total_size() const
{
	return m_node_pool.get_memory_size() +
		m_object_pool.get_memory_size();
}

void Octree::on_object_out(OctreeObject* object)
{
	auto it = std::find(m_out_objects.begin(), m_out_objects.end(), object);
	if(it == m_out_objects.end())
	{
		m_out_objects.push_back(object);
	}
}
