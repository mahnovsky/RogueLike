#include "path.hpp"
#include "file.hpp"

namespace core
{
#if defined(_WIN32)
	std::string Path::m_separator = "\\";
#else
	std::string Path::m_separator = "/";
#endif
	Path::Path(const FileName& file_name)
		:m_file_name(file_name)
	{
	}

	Path::Path(const FileName& file_name, const std::vector<std::string>& path)
		:m_file_name(file_name)
		,m_path(path)
	{
	}

	Path::Path(const std::initializer_list<std::string>& path)
		: m_path(path.begin(), path.end())
	{}

	bool Path::is_valid() const
	{
		return !m_path.empty();
	}

	std::string Path::get_raw_path() const
	{
		std::string path;
		for (auto& name : m_path)
		{
			path.append(name);
			path.append(m_separator);
		}
		return std::move(path);
	}

	std::string Path::get_raw_file_path() const
	{
		std::string file_path = get_raw_path();

		std::string file_name = m_file_name.get_name_with_ext();
		if (!file_name.empty())
		{
			file_path.append(file_name);
		}

		return std::move(file_path);
	}

	void Path::set_file_name(const FileName file_name)
	{
		m_file_name = file_name;
	}

	void Path::append(const std::string_view directory)
	{
		m_path.emplace_back(directory);
	}

	Path Path::get_parent() const
	{
		if (!m_path.empty())
		{
			std::vector<std::string> parent_path{ m_path.begin(), std::prev(m_path.end()) };
			return Path{ m_file_name, parent_path };
		}
		return Path{};
	}

	std::string_view Path::get_name() const
	{
		if (m_file_name.is_valid())
		{
			return m_file_name.get_name();
		}
		if (!m_path.empty())
		{
			return m_path.back();
		}
		return "";
	}

	std::string_view Path::get_root_name() const
	{
		if (!m_path.empty())
		{
			return m_path.front();
		}

		return "";
	}

	void Path::remove_last()
	{
		if (!m_path.empty())
		{
			m_path.pop_back();
		}
	}

	Path Path::add(Path base, const std::string_view dir)
	{
		Path new_path = base;
		new_path.append(dir);
		return std::move(new_path);
	}

	Path Path::parse(const std::string& raw_path, std::string_view in_separator)
	{
		Path result;
		size_t pos = 0;
		std::string_view separator = in_separator.empty() ? m_separator : in_separator;
		while (pos != std::string::npos)
		{
			size_t prev_pos = pos;
			pos = raw_path.find(separator, prev_pos);
			if(pos == std::string::npos)
			{
				std::string name = raw_path.substr(prev_pos);
				if (is_dir_exist(raw_path.c_str()))
				{
					result.append(name);
				}
				else
				{
					result.set_file_name(FileName::parse(name));
				}
				break;
			}

			result.append(raw_path.substr(prev_pos, pos - prev_pos));

			pos += separator.size();
		}

		return std::move(result);
	}
}