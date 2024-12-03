#pragma once

#include <string>
#include <string_view>

namespace core
{
	class FileName
	{
	public:
		FileName() = default;
		FileName(const std::string_view name, const std::string_view ext = "");
		FileName(FileName&& other) noexcept;
		FileName(const FileName& other) = default;

		FileName& operator=(const FileName& other) = default;

		bool is_valid() const;

		std::string get_name_with_ext() const;

		std::string_view get_name() const;

		std::string_view get_extension() const;

		static FileName parse(const std::string& file_name);

	private:
		std::string m_name;
		std::string m_ext;
	};
}