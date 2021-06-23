#include "file.hpp"
#include "path.hpp"
#include "file_node.hpp"

#include <windows.h>
#include <fileapi.h>

namespace basic
{
	bool is_dir_exist(const char* path)
	{
		DWORD dwAttrib = GetFileAttributes(path);

		return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
			(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
	}
}

bool is_path_valid(const char* raw_path)
{
	DWORD dwAttrib = GetFileAttributes(raw_path);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES);
}

class WinFileNode : public core::IFileNode
{
public:
	WinFileNode(HANDLE handle, const core::Path& path)
		:m_handle(handle)
		,m_path(path)
	{}

	~WinFileNode()
	{
		if (m_handle)
		{
			CloseHandle(m_handle);
		}
	}

	NodeType get_type() const override
	{
		BY_HANDLE_FILE_INFORMATION info;
		GetFileInformationByHandle(m_handle, &info);

		if (info.dwFileAttributes != INVALID_FILE_ATTRIBUTES &&
			(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			return NodeType::Directory;
		}
		return NodeType::File;
	}

	const IFileNode* get_parent() const
	{
		core::Path parent = m_path.get_parent();
		if (parent.is_valid() && is_path_valid(parent.get_raw_path().c_str()))
		{
			return open_file(parent);
		}
		return nullptr;
	}

	std::vector<IFileNode*> get_children() const override
	{
		std::vector<IFileNode*> res;

		return std::move(res);
	}

	std::string_view get_name() const override
	{
		return m_path.get_name();
	}

	void get_content(std::vector<uint8_t>& cont) const
	{
		if (get_type() == NodeType::File)
		{
			DWORD file_size = 0;
			DWORD ret = GetFileSize(m_handle, &file_size);
			if (ret != INVALID_FILE_SIZE)
			{
				cont.resize(file_size);
				DWORD actualy_readed = 0;
				if (FAILED(ReadFile(m_handle, cont.data(), file_size, &actualy_readed, nullptr)))
				{
					cont.resize(0);
				}
			}
		}
	}
private:
	HANDLE m_handle;
	core::Path m_path;
};

core::IFileNode* open_file(const core::Path& path)
{
	const std::string raw_path = path.get_raw_file_path();
	if (is_path_valid(raw_path.c_str()))
	{
		HANDLE hFile = CreateFile(raw_path.c_str(),
			// 0, //GENERIC_READ,
			GENERIC_READ | GENERIC_WRITE,
			// 0, //FILE_SHARE_READ, //FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			NULL);

		return new WinFileNode(hFile, path);
	}
	return nullptr;
}