#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <memory>

namespace core
{
	class Path;

	enum class FileNodeType
	{
		Directory,
		File
	};

	class IFileNode
	{
	public:
		virtual ~IFileNode() = default;

		virtual FileNodeType get_type() const = 0;

		virtual const Path get_path() const = 0;

		virtual std::vector<IFileNode*> get_children() const = 0;

		virtual std::string_view get_name() const = 0;

		virtual void get_content(std::vector<uint8_t>& cont) const = 0;

		virtual std::unique_ptr<IFileNode> get_child_node(Path path) const = 0;
	};
}