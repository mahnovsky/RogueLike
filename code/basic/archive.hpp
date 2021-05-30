#pragma once

#include <vector>
#include <string_view>

class IArchiveNode
{
public:
	enum NodeType
	{
		Directory,
		File
	};

	virtual NodeType get_type() const = 0;
	virtual const IArchiveNode* get_parent() const = 0;
	virtual std::vector<IArchiveNode*> get_children() const = 0;
	virtual std::string_view get_name() const = 0;
	virtual void get_content(std::vector<uint8_t>& cont) const = 0;
};

class IArchive
{
public:
	virtual void open(const char* file_name) = 0;
	virtual void close() = 0;
	virtual class IArchiveNode* get_root() = 0;
};