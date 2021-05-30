#pragma once

#include "types.hpp"

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

		int32 to_int(const char* str, uint32 len, ConvStatus& status);

		int32 to_int(const String& str, ConvStatus& status);

		float to_float(const char* str, uint32 len, ConvStatus& status);

		float to_float(const String& str, ConvStatus& status);
	}

	template<class Type>
	struct Conv
	{
		static Type As(const String& , conv::ConvStatus& ) { return Type{}; }
	};

	template<>
	struct Conv<int32>
	{
		static int32 As(const String& s, conv::ConvStatus& status) { return conv::to_int(s, status); }
	};

	template<>
	struct Conv<uint8>
	{
		static uint8 As(const String& s, conv::ConvStatus& status) { return conv::to_int(s, status); }
	};

	template<>
	struct Conv<uint16>
	{
		static uint16 As(const String& s, conv::ConvStatus& status) { return conv::to_int(s, status); }
	};

	template<>
	struct Conv<uint32>
	{
		static uint32 As(const String& s, conv::ConvStatus& status) { return conv::to_int(s, status); }
	};

	template<>
	struct Conv<float>
	{
		static float As(const String& s, conv::ConvStatus& status) { return conv::to_float(s, status); }
	};

	template <class Type>
	Type string_to(const String& s, conv::ConvStatus& status)
	{
		return Conv<Type>::As(s, status);
	}
}
