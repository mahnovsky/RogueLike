#include "convert.hpp"
#include "debug.hpp"
#include "string.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/integer.hpp>
#include <algorithm>

#define USE_STD_CHAR_CONV 

#if defined(USE_STD_CHAR_CONV)
#include <type_traits>
#include <cstdint>
#include <charconv>
#endif

namespace basic
{
	namespace  conv
	{
#if defined(USE_STD_CHAR_CONV)

		int32 to_int(const char* str, size_t len, ConvStatus& status)
		{
			int32 result = 0;
			auto conv_result = std::from_chars(str, str + len, result);
			if (conv_result.ec == std::errc())
			{
				status = ConvStatus::Failed;
				return 0;
			}
			return result;
		}

		float to_float(const char* str, size_t len, ConvStatus& status)
		{
			float result = 0.f;
			auto conv_result = std::from_chars(str, str + len, result);
			if (conv_result.ec == std::errc())
			{
				status = ConvStatus::Failed;
				return 0.f;
			}
			return result;
		}
#else
		int32 to_int(const char* str, uint32 len, ConvStatus& status)
		{
			ASSERT(len > 0);

			int32 result = 0;
			const int32 nlen = static_cast<int32>(len) - 1;

			for (int32 i = nlen; i >= 0; --i)
			{
				ASSERT(str[i] != '\0');
				if (str[i] == '-')
				{
					return -result;
				}

				if (str[i] < '0' || str[i] > '9')
				{
					status = ConvStatus::Failed;
					break;
				}

				const int32 num = (str[i] - '0');
				result += (num * glm::pow(10, nlen - i));
			}

			status = ConvStatus::Ok;

			return result;
		}

		int32 to_int(const String& str, ConvStatus& status)
		{
			return to_int(str.get_cstr(), str.get_size() - 1, status);
		}

		float to_float(const char* str, uint32 len, ConvStatus& status)
		{
			bool sign = str[0] == '-';

			for (uint32 i = 0; i < len; ++i)
			{

				if (str[i] == '.' && i > 0)
				{
					ConvStatus int_status = ConvStatus::Ok;
					const char* pos = str + (sign ? 1 : 0);

					float result = to_int(pos, (str + i) - pos, int_status);
					if (int_status != ConvStatus::Ok)
					{
						status = int_status;
						return result;
					}

					const int32 count = len - i - 1;
					const int32 m = to_int(str + i + 1, count, int_status);

					if (int_status != ConvStatus::Ok)
					{
						status = int_status;
						return result;
					}

					result += (m * (1.f / static_cast<float>(glm::pow(10, count))));

					status = ConvStatus::Ok;
					return (sign ? -result : result);
				}
			}

			status = ConvStatus::Ok;
			return static_cast<float>(to_int(str, len, status));
		}

		float to_float(const String& str, ConvStatus& status)
		{
			return to_float(str.get_cstr(), str.get_size() - 1, status);
		}
#endif
	}
}
