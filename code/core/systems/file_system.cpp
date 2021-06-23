#include "file_system.hpp"
#include "file.hpp"
#include "file_node.hpp"

namespace core
{
	FileSystem::FileSystem(SystemManager& manager)
		:TSystem<FileSystem, SystemUID::SUID_FileSystem>(manager)
	{
	}

	void FileSystem::initialize()
	{
		Path base_path = get_binary_path();
		m_content_directory = Path({}, std::vector<std::string>(base_path.begin(), base_path.end()));

		std::string raw_path;
		std::string_view content_dir = "data";

		do
		{
			Path content_path = Path::add(base_path, content_dir);

			raw_path = content_path.get_raw_path();

			if (is_dir_exist(raw_path.c_str()))
			{
				m_content_directory = content_path;
				break;
			}

			base_path.remove_last();

			if (!base_path.is_valid())
			{
				break;
			}
		}
		while (true);
	}

	void FileSystem::shutdown()
	{
	}

	void FileSystem::destroy()
	{
	}

	Path FileSystem::get_binary_directory() const
	{
		return m_binary_directory;
	}

	Path FileSystem::get_content_directory() const
	{
		return m_content_directory;
	}

	std::unique_ptr<IFileNode> FileSystem::open_file_node(const Path& path)
	{
		if (path.is_valid())
		{
			std::string root = *path.begin();

			auto it = m_mounted.find(root);
			if (it != m_mounted.end())
			{
				return it->second->get_child_node(path);
			}
			else
			{
				return open_file(path);
			}
		}
		return nullptr;
	}

	void FileSystem::mount(const Path& file_path)
	{
	}
}