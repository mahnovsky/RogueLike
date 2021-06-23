#include "file_name.hpp"

namespace core
{
	FileName::FileName(const std::string_view name, const std::string_view ext)
		:m_name(name)
		, m_ext(ext)
	{}

	bool FileName::is_valid() const
	{
		return !m_name.empty();
	}

	std::string FileName::get_name_with_ext() const
	{
		std::string file_name{ m_name };
		file_name.append(".");
		file_name.append(m_ext);
		return std::move(file_name);
	}

	std::string_view FileName::get_name() const
	{
		return m_name;
	}

	std::string_view FileName::get_extension() const
	{
		return m_ext;
	}

	FileName FileName::parse(const std::string& file_name)
	{
		size_t pos = file_name.find('.');
		std::string name;
		std::string ext;
		if (pos != std::string::npos)
		{
			name = file_name.substr(0, pos);
			ext = file_name.substr(pos + 1);
		}

		return {name, ext};
	}
}
