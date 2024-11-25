#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <memory>

namespace core
{
	class Path;
	class IFileNode;

	using FileNodePtr = std::shared_ptr<IFileNode>;

	enum class FileNodeType
	{
		Directory,
		File
	};

	class IFileIterator
	{
	public:
		virtual ~IFileIterator() = default;

		virtual FileNodePtr get() = 0;

		virtual void next() = 0;

		virtual bool is_valid() const = 0;
	};

	class FileIterator
	{
	public:
		FileIterator(IFileIterator* impl)
			:m_impl(impl)
		{
		}

		FileNodePtr operator* ()
		{
			return m_impl->get();
		}

		void operator ++ ()
		{
			m_impl->next();
		}

		bool is_valid() const 
		{
			return m_impl->is_valid();
		}

	private:
		std::unique_ptr<IFileIterator> m_impl;
	};

	class IFileNode
	{
	public:
		virtual ~IFileNode() = default;

		virtual FileNodeType get_type() const = 0;

		virtual Path get_path() const = 0;

		virtual std::string get_name() const = 0;

		virtual void get_content(std::vector<uint8_t>& cont) const = 0;

		virtual FileNodePtr get_child_node(Path path) const = 0;

		virtual FileIterator get_iterator(const std::string& mask) const = 0;

		virtual bool is_valid() const { return false; }
	};

	class IFileProvider
	{
	public:
		virtual FileIterator open(Path path, bool catche);
	};

}