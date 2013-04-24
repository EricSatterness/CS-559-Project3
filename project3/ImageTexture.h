#pragma once
#include <FreeImage.h>
#include <gl/glew.h>
#include <gl/freeglut.h>

class ImageTexture
{
private:
	GLuint handle;
public:
	ImageTexture();
	bool Initialize(const char* fileName);
	void Use();
};