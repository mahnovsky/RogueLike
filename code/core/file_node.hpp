#pragma once

#include <vector>
#include <string>
#include <string_view>

namespace core
{
	class IFileNode
	{
	public:
		virtual ~IFileNode() = default;

		enum NodeType
		{
			Directory,
			File
		};

		virtual NodeType get_type() const = 0;

		virtual const IFileNode* get_parent() const = 0;

		virtual std::vector<IFileNode*> get_children() const = 0;

		virtual std::string_view get_name() const = 0;

		virtual void get_content(std::vector<uint8_t>& cont) const = 0;
	};
}