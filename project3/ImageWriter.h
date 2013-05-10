#pragma once
#include <Windows.h>
#include <Vfw.h>
#include <string>
#include <GL\freeglut.h>
#include "avi_utils.h"

using namespace std;

//class for creating HBITMAPs to pass to addFrame
class RecordingImage
{
private:
	int width, height;
public:
	HDC hdcscreen;
	HDC hdc;
	BITMAPINFO bi;
	BITMAPINFOHEADER bih;
	HBITMAP hbm;
	HGDIOBJ holdb;
	void *bits; 
	RecordingImage(int width, int height);
	~RecordingImage();
	bool GetScreen(int width, int height);
};

class ImageWriter
{
private:
	bool firstRecordedImage;
	int width, height;
	int fps;
	HAVI avi;
	RecordingImage *recordImage;
	bool recording;

	bool _init(string videoPath, int framesPerSecond);
	bool _SaveImage(string path);
public:
	ImageWriter(int width, int height);
	
	//saves screenshot
	bool SaveImage(string fileName);//if asked to save image without params, savePath is assumed 
										//to be the desired path

	//returns true if avi could be succesfully created and recording has begun. 
	//if requireDeleteExistingVideo is true, the method will return false if creating
	//the avi would overwrite an existing file. If it is false, it will overwrite
	//any existing file
	bool InitRecording(string videoPath, int framesPerSecond);
	//stops the recording, returns true unless it wasn't actually recording	
	bool EndRecording();

	bool RecordImage();
};