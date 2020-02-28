#pragma once
#include <iostream>
#include <string>
#include <stb_image.h>

class Texture
{
public:
	Texture(const char* path1, const char* path2);
	~Texture();

	void bind();
	unsigned int getTexture();
private:
	unsigned int texture1;
	unsigned int texture2;
};

