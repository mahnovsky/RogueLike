#include "file_system.hpp"

namespace core
{
	std::string Path::m_separator = "/";

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
	void FileSystem::mount(const Path& file)
	{
	}
}