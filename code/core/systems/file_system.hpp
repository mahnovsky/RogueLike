#pragma once

//#include "defines.hpp"
#include "system.hpp"
#include "file_name.hpp"
#include "path.hpp"
#include "file_node.hpp"

#include <memory>
#include <map>

namespace core
{
	class FileSystem
		: public TSystem< FileSystem, SystemUID::SUID_FileSystem>
	{
	public:
		FileSystem(SystemManager& manager);

		void initialize() override;

		void shutdown() override;

		void destroy() override;

		Path get_binary_directory() const;

		Path get_content_directory() const;

		std::unique_ptr<IFileNode> open_file_node(const Path& path);

		void mount(const Path& file_path);

	private:
		Path m_binary_directory;
		Path m_content_directory;
		std::map<std::string, std::unique_ptr<IFileNode>> m_mounted;
	};
}

