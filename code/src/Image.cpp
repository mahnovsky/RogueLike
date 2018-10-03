#include "Image.h"

Image::Image()
	:_buffer()
	,_width(0)
	,_height(0)
{
}

Image::~Image()
{
}

bool Image::load(const char * fileName)
{
	int chanels = 0;
	stbi_uc* data = stbi_load(fileName, &_width, &_height, &chanels, 4);

	return false;
}

int Image::getWidth() const
{
	return 0;
}

int Image::getHeight() const
{
	return 0;
}

const char * Image::getData() const
{
	return nullptr;
}
