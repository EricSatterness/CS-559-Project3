//#pragma once
//#include <Windows.h>
//#include <Vfw.h>
//#include <string>
//#include "myStructs.h"
//#include "WindowData.h"
//#include "avi_utils.h"
//
//using namespace std;
//
////class for creating HBITMAPs to pass to addFrame
//class RecordingImage
//{
//private:
//	int width, height;
//public:
//	HDC hdcscreen;
//	HDC hdc;
//	BITMAPINFO bi;
//	BITMAPINFOHEADER bih;
//	HBITMAP hbm;
//	HGDIOBJ holdb;
//	void *bits; 
//	RecordingImage(int width, int height);
//	~RecordingImage();
//	bool GetScreen(WindowData *wndData);
//};
//
//class ImageWriter
//{
//private:
//	bool firstRecordedImage;
//	WindowData *wndData;
//	int fps;
//	HAVI avi;
//	RecordingImage *recordImage;	
//	bool recordRequested;
//	bool recording;
//
//	bool _init(string videoPath, int framesPerSecond, bool requireDeleteExistingVideo);
//	bool _SaveImage(string path);
//public:
//	ImageWriter(WindowData *wndData);
//	
//	string savePath;
//	//saves screenshop
//	bool SaveImage(string fileName = "");//if asked to save image without params, savePath is assumed 
//										//to be the desired path
//
//	//returns true if avi could be succesfully created and recording has begun. 
//	//if requireDeleteExistingVideo is true, the method will return false if creating
//	//the avi would overwrite an existing file. If it is false, it will overwrite
//	//any existing file
//	bool InitRecording(string videoPath, int framesPerSecond, bool requireDeleteExistingVideo = false);
//	//stops the recording, returns true unless it wasn't actually recording	
//	bool EndRecording();
//
//
//	//these shouldn't need to be called by anything other than NanoCyteDisplay.cpp
//	bool RecordImage();
//	void RequestRecord();
//	bool RecordIfRequested();
//};