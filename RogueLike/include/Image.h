#pragma once

#include "common.h"

class Image
{
public:
	Image();
	~Image();

	bool load(const char* fileName);

	int getWidth() const;

	int getHeight() const;

	const char* getData() const;

private:
	std::vector<char> _buffer;

	int _width;
	int _height;
};