#pragma once

//#include "defines.hpp"
#include "system.hpp"
#include "file_name.hpp"
#include "path.hpp"
#include "file_node.hpp"

#include <memory>

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

		IFileNode* get_node(const Path& path);

		void mount(const Path& file_path);

	private:
		std::unique_ptr<IFileNode> m_root;
	};
}

