#pragma once

#include "types.hpp"
#include <optional>
#include <string_view>

namespace basic
{
	class String;

	namespace conv
	{
		enum class ConvStatus
		{
			Ok,
			Failed
		};

		int32 to_int(const char* str, size_t len, ConvStatus& status);

		float to_float(const char* str, size_t len, ConvStatus& status);
	}

	template<class Type>
	struct Conv
	{
		static Type As(const std::string_view& , conv::ConvStatus& ) { return Type{}; }
	};

	template<>
	struct Conv<int32>
	{
		static int32 As(const std::string_view& s, conv::ConvStatus& status) { return conv::to_int(s.data(), s.size(), status); }
	};

	template<>
	struct Conv<uint8>
	{
		static uint8 As(const std::string_view& s, conv::ConvStatus& status) { return conv::to_int(s.data(), s.size(), status); }
	};

	template<>
	struct Conv<uint16>
	{
		static uint16 As(const std::string_view& s, conv::ConvStatus& status) { return conv::to_int(s.data(), s.size(), status); }
	};

	template<>
	struct Conv<uint32>
	{
		static uint32 As(const std::string_view& s, conv::ConvStatus& status) { return conv::to_int(s.data(), s.size(), status); }
	};

	template<>
	struct Conv<float>
	{
		static float As(const std::string_view& s, conv::ConvStatus& status) { return conv::to_float(s.data(), s.size(), status); }
	};

	template <class Type>
	Type string_to(const std::string_view& s, conv::ConvStatus& status)
	{
		return Conv<Type>::As(s, status);
	}
}
