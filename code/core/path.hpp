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
		Path(const Path&) = default;
		Path(Path&& path);

		Path& operator = (const Path& other) {
			m_path = other.m_path;
			m_file_name = other.m_file_name;
			return *this;
		}

		bool is_valid() const;

		Path subpath(size_t pos, size_t count = std::string::npos) const;

		std::string get_raw_path(char separator = 0) const;

		std::string get_raw_file_path(const char separator = 0) const;

		std::wstring get_raw_file_path_wide() const;

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

		std::string get_name() const;

		std::string_view get_root_name() const;

		void remove_last();

		static Path add(Path base, const std::string_view dir);

		static Path parse(const std::string& raw_path, std::string_view in_separator = "");

		static std::string get_patform_separator();

		const FileName& get_filename() const;

	private:
		static std::string m_separator;
		FileName m_file_name;
		std::vector<std::string> m_path;
	};
}

