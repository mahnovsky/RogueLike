#include "hash_to_index.hpp"

std::optional<uint32_t> HashToIndex::get_index(uint32_t hash) const
{
	if (const auto it = m_hash_to_index.find(hash); it != m_hash_to_index.end()) {
		return it->second;
	}

	return {};
}

uint32_t HashToIndex::get_or_create_index(uint32_t hash)
{
	if (const auto index_opt = get_index(hash)) {
		return *index_opt;
	}

	const uint32_t index = m_index_counter;
	m_hash_to_index.insert(std::pair { hash, index });
	++m_index_counter;

	return index;
}
