#pragma once

#include "file_name.hpp"

#include <vector>

namespace core
{
	class Path
	{
	public:
		Path() = default;
		Path(const FileName& file_name);
		Path(const FileName& file_name, const std::vector<std::string>& path);
		Path(const std::initializer_list<std::string>& path);

		bool is_valid() const;

		std::string get_raw_path() const;

		std::string get_raw_file_path() const;

		void set_file_name(const FileName file_name);

		void append(const std::string_view directory);

		std::vector<std::string>::const_iterator begin() const
		{
			return m_path.begin();
		}

		std::vector<std::string>::const_iterator end() const
		{
			return m_path.end();
		}

		Path get_parent() const;

		std::string_view get_name() const;

		std::string_view get_root_name() const;

		void remove_last();

		static Path add(Path base, const std::string_view dir);

		static Path parse(const std::string& raw_path, std::string_view in_separator = "");

	private:
		static std::string m_separator;
		FileName m_file_name;
		std::vector<std::string> m_path;
	};
}

