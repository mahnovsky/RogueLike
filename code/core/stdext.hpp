#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

using std::int8_t;
using std::int16_t;
using std::int32_t;

namespace stdext
{
	template <class Item, class Alloc>
	bool is_contains(std::vector<Item, Alloc>& cont, const Item& item)
	{
		return std::find(cont.begin(), cont.end(), item) != cont.end();
	}

	/* Push new item in vector if item dont contains in vector. 
		Return true if item actually pushed. */
	template <class Item, class Alloc>
	bool push_unique(std::vector<Item, Alloc>& cont, Item& item)
	{
		const bool contains = is_contains(cont, item);
		if (!is_contains(cont, item))
		{
			cont.push_back(item);

			return true;
		}

		return false;
	}

	template <class Item, class Alloc>
	bool remove(std::vector<Item, Alloc>& cont, const Item& item)
	{
		auto it = std::remove(cont.begin(), cont.end(), item);
		const bool removed = it != cont.end();
		if (removed)
		{
			cont.erase(it, cont.end());
		}
		return removed;
	}
}