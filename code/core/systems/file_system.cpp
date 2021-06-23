#include "file_system.hpp"

namespace core
{
	FileSystem::FileSystem(SystemManager& manager)
		:TSystem<FileSystem, SystemUID::SUID_FileSystem>(manager)
	{
	}

	void FileSystem::initialize()
	{
		
	}

	void FileSystem::shutdown()
	{
	}

	void FileSystem::destroy()
	{
	}

	IFileNode* FileSystem::get_node(const Path& path)
	{

		return nullptr;
	}

	void FileSystem::mount(const Path& file_path)
	{
	}
}