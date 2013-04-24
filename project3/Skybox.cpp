#include "Skybox.h"

//skybox images from http://www.quake3world.com/forum/viewtopic.php?t=9242
const char* skybox_files[5] = {"miramar_large.jpg", "grimmnight_large.jpg", "interstellar_large.jpg", "stormydays_large.jpg", "violentdays_large.jpg"};

Skybox::Skybox()
{
	skyboxIndex = 0;
}

bool Skybox::Initialize(float size)
{
	this->size = size;
	box = new Cube();
	box->Initialize(1.0f);
	if(!shader.Initialize("SkyBoxShader.vert", "SkyBoxShader.frag"))
		return false;
	for(int i=0; i<NUM_SKYBOX_TEXTURES; i++)
	{
		skybox_textures[i] = new ImageTexture();
		if(!skybox_textures[i]->Initialize(skybox_files[i]))
			return false;
	}
	tex = skybox_textures[skyboxIndex];
	return true;
}
void Skybox::NextTexture()
{
	skyboxIndex++;
	if(skyboxIndex>= NUM_SKYBOX_TEXTURES)
		skyboxIndex = 0;
	tex = skybox_textures[skyboxIndex];
}
void Skybox::ChangeTexture(int index)
{
	if(index > NUM_SKYBOX_TEXTURES || index < 0)
	{
		printf("Invalid Skybox Texture Index\n");
		return;
	}
	skyboxIndex = index;
	tex = skybox_textures[skyboxIndex];
}
void Skybox::Draw(glm::mat4 projection_matrix, glm::mat4 worldModelView, glm::ivec2 size)
{
	tex->Use();
	currShader = &shader;
	box->Draw(projection_matrix, glm::scale(worldModelView, glm::vec3(this->size)), size);
}