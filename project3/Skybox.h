#pragma once
#include "ImageTexture.h"
#include "Cube.h"

#define NUM_SKYBOX_TEXTURES 5
extern const char* skybox_files[NUM_SKYBOX_TEXTURES];
class Skybox
{
private:
	Cube *box;
	ImageTexture* skybox_textures[NUM_SKYBOX_TEXTURES];
	int skyboxIndex;
	ImageTexture *tex;
	Shader shader;
	float size;
public:
	Skybox();
	bool Initialize(float size);
	void NextTexture();
	void ChangeTexture(int index);
	void Draw(glm::mat4 projection_matrix, glm::mat4 worldModelView, glm::ivec2 size);
};