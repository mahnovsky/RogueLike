#pragma once

#include "defines.hpp"
#include "system.hpp"

#include <memory>

namespace core
{
	class FileName
	{
	public:
		FileName() = default;
		FileName(const std::string_view name, const std::string_view ext = "")
			:m_name(name)
			,m_ext(ext)
		{}

		bool is_valid() const
		{
			return !m_name.empty();
		}

		std::string get_name_with_ext() const
		{
			std::string file_name{ m_name };
			file_name.append(".");
			file_name.append(m_ext);
			return std::move(file_name);
		}

		std::string_view get_name() const
		{
			return m_name;
		}

		std::string_view get_extension() const
		{
			return m_ext;
		}

	private:
		std::string m_name;
		std::string m_ext;
	};

	class Path
	{
	public:
		Path() = default;
		Path(const FileName& file_name)
			:m_file_name(file_name)
		{
		}

		std::string to_string() const 
		{
			std::string path;
			for (auto& name : m_path)
			{
				path.append(name);
				path.append(m_separator);
			}
			std::string file_name = m_file_name.get_name_with_ext();
			if (!file_name.empty())
			{
				path.append(file_name);
			}
			return std::move(path);
		}

		void set_file_name(const FileName file_name)
		{
			m_file_name = file_name;
		}

		void append(const std::string_view directory)
		{
			m_path.emplace_back(directory);
		}

	private:
		static std::string m_separator;
		FileName m_file_name;
		std::vector<std::string> m_path;
	};

	class FileSystem
		: public TSystem< FileSystem, SystemUID::SUID_FileSystem>
	{
	public:
		FileSystem(SystemManager& manager);

		void initialize() override;

		void shutdown() override;

		void destroy() override;

		void mount(const Path& file);

	private:
		Path m_root;
	};
}

