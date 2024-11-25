#include "file.hpp"
#include "path.hpp"
#include "file_node.hpp"

#include <windows.h>
#include <fileapi.h>

namespace core
{
	bool is_dir_exist(const char* raw_path)
	{
		DWORD dwAttrib = GetFileAttributes(raw_path);

		return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
			(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
	}

	bool is_path_valid(const char* raw_path)
	{
		DWORD dwAttrib = GetFileAttributes(raw_path);

		return (dwAttrib != INVALID_FILE_ATTRIBUTES);
	}

	class WinFileNode;

	class WinFileIterator : public IFileIterator
	{
	public:
		WinFileIterator(Path path)
			:m_file(INVALID_HANDLE_VALUE)
			,m_path(path)
		{
			std::string raw_path = m_path.get_raw_path() + "*";
			
			m_file = FindFirstFileA(raw_path.c_str(), &m_current_find_data);
		}

		~WinFileIterator()
		{
			FindClose(m_file);
			m_file = INVALID_HANDLE_VALUE;
		}

		core::FileNodePtr get();

		virtual void next() override
		{
			if (m_file && !FindNextFileA(m_file, &m_current_find_data))
			{
				m_file = INVALID_HANDLE_VALUE;
			}
		}

		bool is_valid() const override
		{
			return m_file != INVALID_HANDLE_VALUE;
		}

	private:
		HANDLE m_file;
		Path m_path;
		WIN32_FILE_ATTRIBUTE_DATA m_attribs;
		WIN32_FIND_DATA m_current_find_data;
	};

	class WinFileNode : public core::IFileNode
	{
	public:
		WinFileNode(HANDLE handle, const core::Path& path)
			:m_handle(handle)
			, m_path(path)
		{}

		~WinFileNode()
		{
			if (m_handle)
			{
				CloseHandle(m_handle);
				m_handle = nullptr;
			}
		}

		FileNodeType get_type() const override
		{
			BY_HANDLE_FILE_INFORMATION info;
			GetFileInformationByHandle(m_handle, &info);

			if (info.dwFileAttributes != INVALID_FILE_ATTRIBUTES &&
				(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				return FileNodeType::Directory;
			}
			return FileNodeType::File;
		}

		Path get_path() const override
		{
			return m_path;
		}

		std::string get_name() const override
		{
			return m_path.get_name();
		}

		void get_content(std::vector<uint8_t>& cont) const
		{
			if (get_type() == FileNodeType::File)
			{
				DWORD result = 0;
				DWORD file_size = GetFileSize(m_handle, &result);
				if (result != INVALID_FILE_SIZE)
				{
					cont.resize(file_size);
					DWORD actualy_readed = 0;
					if (!ReadFile(m_handle, cont.data(), file_size, &actualy_readed, nullptr))
					{
						cont.resize(0);
					}
				}
			}
		}

		FileNodePtr get_child_node(Path path) const override
		{
			std::string full_path = path.get_raw_file_path();
			if (is_dir_exist(full_path.c_str()))
			{
				return open_file(path);
			}
			return nullptr;
		}

		FileIterator get_iterator(const std::string& mask) const override
		{
			return FileIterator(new WinFileIterator(m_path));
		}

	private:
		HANDLE m_handle;
		core::Path m_path;
	};

	std::unique_ptr<IFileNode> open_file(const core::Path& path)
	{
		const std::string raw_path = path.get_raw_path();
		if (is_path_valid(raw_path.c_str()))
		{
			HANDLE hFile = CreateFile(path.get_raw_file_path().c_str(),
				// 0, //GENERIC_READ,
				GENERIC_READ | GENERIC_WRITE,
				// 0, //FILE_SHARE_READ, //FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_BACKUP_SEMANTICS,
				NULL);

			return std::make_unique<WinFileNode>(hFile, path);
		}
		return nullptr;
	}

	Path get_binary_path()
	{
		char exeDirectory[1024];
		DWORD ret = GetModuleFileName(NULL, exeDirectory, 1024);
		
		if (ret)
		{
			return core::Path::parse(std::string(exeDirectory, exeDirectory + ret));
		}

		return {};
	}

	FileNodePtr WinFileIterator::get()
	{
		Path file_path = m_path;
		file_path.remove_last();
		file_path.set_file_name({m_current_find_data.cFileName});
		return open_file(file_path);
	}

}

