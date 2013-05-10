#include "ImageTexture.h"
#include <iostream>

using namespace std;
void ImageTexture::TakeDown()
{
	glDeleteTextures(1, &handle);
	delete textura;
}
ImageTexture::ImageTexture()
{
	handle = -1;
	textura = NULL;
	w = 0;
	h = 0;
}
void ImageTexture::UpdatePixels()
{
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, w, h, 0, GL_RGBA,GL_UNSIGNED_BYTE,(GLvoid*)textura );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
bool ImageTexture::Initialize(int w, int h, glm::ivec3 initColor)
{
	this->w = w;
	this->h = h;

	delete textura;
	textura = new GLubyte[4*w*h];
	for(int j= 0; j<w*h; j++){
		textura[j*4+0]= initColor.x;
		textura[j*4+1]= initColor.y;
		textura[j*4+2]= initColor.z;
		textura[j*4+3]= (GLubyte)1.0f;
	}
 
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, w, h, 0, GL_RGBA,GL_UNSIGNED_BYTE,(GLvoid*)textura );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	 
	GLenum huboError = glGetError();
	if(huboError){ 
		return false;
	}
	return true;
}
bool ImageTexture::Initialize(const char *fileName)
{
	FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(fileName,0);//Automatocally detects the format(from over 20 formats!)
	FIBITMAP* imagen = FreeImage_Load(formato, fileName);
 
	FIBITMAP* temp = imagen;
	imagen = FreeImage_ConvertTo32Bits(imagen);
	FreeImage_Unload(temp);
 
	w = FreeImage_GetWidth(imagen);
	h = FreeImage_GetHeight(imagen);
 
	textura = new GLubyte[4*w*h];
	char* pixeles = (char*)FreeImage_GetBits(imagen);
	//FreeImage loads in BGR format, so you need to swap some bytes(Or use GL_BGR).
 
	for(int j= 0; j<w*h; j++){
		textura[j*4+0]= pixeles[j*4+2];
		textura[j*4+1]= pixeles[j*4+1];
		textura[j*4+2]= pixeles[j*4+0];
		textura[j*4+3]= pixeles[j*4+3];
		//cout<<j<<": "<<textura[j*4+0]<<"**"<<textura[j*4+1]<<"**"<<textura[j*4+2]<<"**"<<textura[j*4+3]<<endl;
	}
 
	//Now generate the OpenGL texture object 
 
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, w, h, 0, GL_RGBA,GL_UNSIGNED_BYTE,(GLvoid*)textura );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	 
	GLenum huboError = glGetError();
	if(huboError){ 
		return false;
	}
	return true;
}
void ImageTexture::Use()
{
	if(handle == -1)
	{
		printf("bad texture handle\n");
		return;
	}
	glBindTexture(GL_TEXTURE_2D, handle);
}