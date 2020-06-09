#include "BitmapReader.h"
#include <windows.h>
#include <cstdio>
#include <cmath>

BitmapReader::BitmapReader(const char *filename)
{
	img = 0;

	tagBITMAPFILEHEADER fh;
	tagBITMAPINFOHEADER infoh;
	FILE *fp = fopen(filename, "rb");//open as binary (Windows...)
	if (!fp) 
		return;
	fread(&fh, sizeof(fh), 1, fp);
	fread(&infoh, sizeof(infoh), 1, fp);

	if (infoh.biBitCount != 24) // incorrect format, it must be a 24bit BMP file
	{
		fclose(fp);
		return;
	}

	width = infoh.biWidth;
	height = abs(infoh.biHeight);
	int w3 = 3*width;
	int size = w3*height;
	img = new unsigned char[size];

	int x, t = (-w3)&0x03;
	for (int j = height - 1; j >= 0 ; --j)//the line positions are inverted.
	{
		fread(img + j*w3, w3, 1, fp);
		if (t) fread(&x, t, 1, fp); // Discard the extra bytes at the end of each line.
//		fseek(fp,t,SEEK_CUR);
	}
	fclose(fp);

	unsigned char temp;
	for (int i = 0; i < size; i+=3)
	{
		temp = img[i]; // BMP is bgr instead rgb
		img[i] = img[i + 2];
		img[i + 2] = temp;
	}
}

BitmapReader::~BitmapReader() {
	if (img) delete[]img;
}

