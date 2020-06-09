#ifndef BITMAPREADER_H
#define BITMAPREADER_H

// In OpenGL, use glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
class BitmapReader
{
public:
	int height, width;
	unsigned char *img;

	BitmapReader(const char *filename);
	unsigned char *ImageMatrix() {
		return img;
	}
	~BitmapReader();
};

#endif

