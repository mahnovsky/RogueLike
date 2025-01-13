#pragma once

#include <optional>
#include <unordered_map>


class HashToIndex
{
public:

	std::optional<uint32_t> get_index(uint32_t hash) const;
	uint32_t get_or_create_index(uint32_t hash);

private:
	std::unordered_map<uint32_t, uint32_t> m_hash_to_index;
	uint32_t m_index_counter = 0;
};
