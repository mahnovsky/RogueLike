#pragma once

#include "generic/generic_object.hpp"

template <class T>
class Buffer
{
public:
	Buffer(uint32_t in_count)
		: m_max_count(in_count)
		, m_free_elements(in_count, true)
		, m_elements(basic::mem_alloc(sizeof(T)* in_count))
	{}

	~Buffer()
	{
		basic::mem_free(m_elements);
	}

	bool is_contain(const T* element) const
	{
		uint32_t offset = element - m_elements;
		return (offset >= 0 && offset < (sizeof(T) * m_max_count));
	}

	T* create()
	{
		uint32_t index = 0;
		for (uint32_t index = 0; index < m_max_count; ++index)
		{
			T* elem = create(index);
			if (elem)
			{
				return elem;
			}
		}

		return nullptr;
	}

	T* create(uint32_t index)
	{
		if (index < m_max_count, m_free_elements[index])
		{
			m_free_elements[index] = false;
			return new (m_elements + index) T();
		}

		return nullptr;
	}

	void destroy(uint32_t index)
	{
		if (index < m_max_count && !m_free_elements[index])
		{
			T* element = m_elements + index;
			element->~T();
			m_free_elements[index] = true;
		}
	}

	void destroy(T* element)
	{
		destroy(element - m_elements);
	}

	T* get(uint32_t index)
	{
		if (index < m_max_count && !m_free_elements[index])
		{
			return m_elements + index;
		}
		return nullptr;
	}

private:
	const uint32_t m_max_count;
	std::vector<bool> m_free_elements;
	T* m_elements;
};

using EntityId = uint32_t;

template <class T>
class ComponentConteiner
{
public:
	ComponentConteiner(uint32_t max_buffer_elements)
		:m_max_buffer_elements(max_buffer_elements)
	{}

	T* create(EntityId entity_id)
	{
		uint32_t buffer_index = std::floor(static_cast<float>(entity_id) / m_max_buffer_elements);

		if (m_buffers.size() <= buffer_index)
		{
			m_buffers.resize(buffer_index + 1);
		}

		Buffer<T>& buff = m_buffers[buffer_index];

		return buff.create(entity_id % m_max_buffer_elements);
	}

	void destroy(EntityId entity_id)
	{
		uint32_t buffer_index = std::floor(static_cast<float>(entity_id) / m_max_buffer_elements);

		if (m_buffers.size() < buffer_index)
		{
			auto& buff = m_buffers[buffer_index];

			buff.destroy(entity_id % m_max_buffer_elements);
		}
	}

private:
	const uint32_t m_max_buffer_elements;
	std::vector<Buffer<T>> m_buffers;
};

class Component : public IGenericObject
{
public:
	Component(Entity* ent):m_entity(ent){}
	~Component() override = default;

	Entity* get_entity() const
	{
		return m_entity;
	}

protected:
	Entity* m_entity;
};
