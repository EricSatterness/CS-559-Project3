#pragma once
#include <FreeImage.h>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm\glm.hpp>

class ImageTexture
{
private:
	GLuint handle;
public:
	ImageTexture();
	void TakeDown();
	void UpdatePixels();
	bool Initialize(const char* fileName);
	bool Initialize(int w, int h, glm::ivec3 initColor);
	void Use();
	GLubyte* textura;
	int w, h;
};