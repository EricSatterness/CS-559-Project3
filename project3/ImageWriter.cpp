#include "ImageWriter.h"
#include <fstream>


bool RecordingImage::GetScreen(int width, int height)
{
	
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

	//each line is an additional 0 to 4 bytes longer than necessairy because the pixels are word alligned by row
	int rasterWidth = width;// + ((width % 4 == 0) ? 0 : (4 - (width % 4)));
	unsigned long lImageSize = rasterWidth * 3 * height;

	GLbyte	*pBits = NULL; 
	pBits = (GLbyte *)malloc(lImageSize);
    if(pBits == NULL){
        return false;
	}

	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBits);

	//fill in the recording image's pixel data. The recording image
	//has it's own width which is different from the window's width
	//if glui is being used 
	unsigned char *cbits = (unsigned char *)bits;
	for(int i=0; i<this->height; i++)
	{
		for(int j=0; j<this->width; j++)
		{
			int i1, i2;
			i1 = 3*(i*rasterWidth + j);
			i2 = 3*(i*this->width + j);
			for(int k=0; k<3; k++)
			{
				cbits[i2+k] = pBits[i1+k];
			}
		}
	}
	free(pBits);
	return true;
}
RecordingImage::RecordingImage(int width, int height)
{
	//each line is an additional 0 to 4 bytes longer than necessairy because the 
	//pixels in bitmaps are word alligned by row
	width = width + ((width % 4 == 0) ? 0 : (4 - (width % 4))); //raster width
	this->width = width;
	this->height = height;
	hdcscreen=GetDC(0);
	hdc=CreateCompatibleDC(hdcscreen); 
	ReleaseDC(0,hdcscreen); 

	//clear bitmap info
	ZeroMemory(&bi,sizeof(bi));

	//fill in header info
	bih = bi.bmiHeader;
	bih.biSize=sizeof(bih);
	bih.biWidth=width;
	bih.biHeight=height;
	bih.biBitCount=24;
	bih.biCompression=BI_RGB;
	bih.biSizeImage = width*height*3;

	//not really sure what the following paramters do
	bih.biPlanes=1;
	bih.biXPelsPerMeter=10000;
	bih.biYPelsPerMeter=10000;
	bih.biClrUsed=0;
	bih.biClrImportant=0;

	//initialize/allocate
	hbm=CreateDIBSection(hdc,(BITMAPINFO*)&bih,DIB_RGB_COLORS,&bits,NULL,NULL);
	holdb=SelectObject(hdc,hbm);
}
RecordingImage::~RecordingImage()
{
	SelectObject(hdc, holdb); 
	DeleteObject(hbm);
	DeleteDC(hdc); 
}

void CreateDir(string path)
{
	ifstream f(path.c_str());
	if(!f.is_open())
	{
		CreateDirectoryA(path.c_str(), NULL);
	}
	else
		f.close();
}
bool FileExists(string path)
{
	ifstream f(path.c_str());
	if(f.is_open())
	{
		f.close();
		return true;
	}
	else
		return false;
}
ImageWriter::ImageWriter(int width, int height)
{
	this->width = width;
	this->height = height;
	recording = false;
	recordImage = NULL;
}
//define header, this is probably defined somewhere in some window's header
//but I wasn't sure where
#pragma pack(1)
typedef struct
{
	char b, m;
	unsigned long fileSize;
	unsigned short reserved[2];
	unsigned long headerSize, bmpSize, bmpWidth, bmpHeight;
	unsigned short lanes, count;
	unsigned long compression, size, xpels, ypels,clrused, clrimportant;
} BMPHEADER;
#pragma pack(8)
bool _savePixels(string path, GLbyte *pBits, int width, int height) //assumes bits are word alligned
{
	FILE *pFile;                // File pointer
    BMPHEADER bmpHeader;		
    unsigned long lImageSize;   // Size in bytes of image
    
	int rasterWidth = width + ((width % 4 == 0) ? 0 : (4 - (width % 4)));//pad;
	if(rasterWidth != width)
	{
		printf("Error in _savePixels, bits must be word alligned\n");
		//bmp file pixels are word alligned and this method does not
		//word allign the bits for you. If the bits are word alligned,
		//but the width just doesn't account for that yet, the method
		//could be continued
		return 0;
	}
	
    // How big is the image going to be 
	lImageSize = rasterWidth * 3 * height;

	//fill in header
	bmpHeader.b = 'B';
	bmpHeader.m = 'M';
	bmpHeader.reserved[0] = 0;
	bmpHeader.reserved[1] = 0;
	bmpHeader.headerSize = sizeof(BMPHEADER);
	bmpHeader.compression = 0;
	bmpHeader.size = 0;
	bmpHeader.xpels = 0;
	bmpHeader.ypels = 0;
	bmpHeader.clrimportant = 0;
	bmpHeader.clrused = 0;
	bmpHeader.lanes = 1;
	bmpHeader.count = 24;
	bmpHeader.bmpSize = 40; //size of filesize, reserved, headersize
	bmpHeader.fileSize = bmpHeader.bmpSize+bmpHeader.headerSize;
	bmpHeader.bmpHeight = height;
	bmpHeader.bmpWidth = rasterWidth;

	printf("%i, %i, %i, %i\n", sizeof(bmpHeader.fileSize), sizeof(bmpHeader.reserved[0]), bmpHeader.headerSize, sizeof(bmpHeader.headerSize));

	
    // Allocate block. If this doesn't work, go home
    if(pBits == NULL)
        return 0;
	                
    // Attempt to open the file
	#pragma warning(disable: 4996)
	pFile = fopen(path.c_str(), "wb");
	#pragma warning(default: 4996)
    if(pFile == NULL)
        return 0;
	
    fwrite(&bmpHeader, sizeof(BMPHEADER), 1, pFile);
    fwrite(pBits, lImageSize, 1, pFile);
    fclose(pFile);
    
    // Success!
    return 1;
}
bool ImageWriter::_SaveImage(string path)
{
	int rasterWidth = width + ((width % 4 == 0) ? 0 : (4 - (width % 4)));
	unsigned long lImageSize = rasterWidth * 3 * height;

	GLbyte	*pBits = NULL; 
	pBits = (GLbyte *)malloc(lImageSize);
    if(pBits == NULL)
        return 0;

	//read the pixels. do not specify front buffer or glui stuff will get in the way
	glReadPixels(0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBits);

	//chop out the blank region normally covered by glui
	int newWidth = width;
	int height = height; 

	int newRasterWidth = newWidth + ((newWidth % 4 == 0) ? 0 : (4 - (newWidth % 4)));

	GLbyte	*pixels = NULL; 
	pixels = (GLbyte *)malloc(height*newRasterWidth*3);
    if(pBits == NULL)
	{
		free(pBits);
        return 0;
	}

	//copy the pixels into a new word alligned array for matching the viewport's width
	for(int i=0; i<height; i++)
	{
		for(int j=0; j<newRasterWidth; j++)
		{
			int i1, i2;
			i1 = 3*(i*rasterWidth + j);
			i2 = 3*(i*newRasterWidth + j);
			for(int k=0; k<3; k++)
			{
				pixels[i2+k] = pBits[i1+k];
			}
		}
	}
	free(pBits);
	bool ret = _savePixels(path, pixels, newRasterWidth, height);
	free(pixels);

	return ret;
}
bool ImageWriter::SaveImage(string fileName)
{
	return ImageWriter::_SaveImage(fileName);	
}
bool ImageWriter::_init(string videoPath, int framesPerSecond)
{
	avi = CreateAvi(videoPath.c_str(),framesPerSecond,NULL);
	if(recordImage != NULL)
		 delete recordImage;
	recordImage = new RecordingImage(width, height);
	firstRecordedImage = true;
	fps = framesPerSecond;
	return true;
}
bool ImageWriter::InitRecording(string videoPath, int framesPerSecond)
{
	recording = _init(videoPath, framesPerSecond);
	return recording;
}

void CompressVideo(HAVI avi, HBITMAP hbm, bool dialog)
{
	AVICOMPRESSOPTIONS opts; ZeroMemory(&opts,sizeof(opts));
	if(dialog)
	{
		HWND hwnd = GetForegroundWindow();
		//note: not all of the options in the dialog
		//will actually produce a video, some of the
		//compression options apear to not be supported
		SetAviVideoCompression(avi,hbm,&opts,true,hwnd);
	}
	else
	{
		//some compression parameters I recorded that worked
		//fairly well from calling this method with dialog
		//set to true.
		opts.fccHandler = 1668707181;
		opts.dwQuality = 7500; //I think quality can range from 0 to 10000
		opts.dwFlags = 8;
		opts.cbParms = 4;
		SetAviVideoCompression(avi,hbm,&opts,false,NULL);
	}
}
bool ImageWriter::RecordImage()
{
	bool success = recordImage->GetScreen(width, height);
	if(firstRecordedImage)
		CompressVideo(avi, recordImage->hbm, false);
	AddAviFrame(avi,recordImage->hbm);
	firstRecordedImage = false;
	return success;
}
bool ImageWriter::EndRecording()
{	
	if(recording)
	{
		recording = false;
		CloseAvi(avi);
		delete recordImage;
		return true;
	}
	else
		return false;
}