#pragma once
#include <iostream>
#include <memory>

namespace ImageResizer
{
	// the number of channels in the images, in the case of RGBA it is set to 4
	const unsigned NUMBER_OF_CHANNELS = 4;
	// min and max pixel sizes that the resizer will resize to
	const unsigned MIN_VALID_WIDTH = 2;
	const unsigned MIN_VALID_HEIGHT = 2;
	const unsigned MAX_VALID_WIDTH = 8192;
	const unsigned MAX_VALID_HEIGHT = 8192;

	struct Image
	{
		Image() : width(0), height(0), data(nullptr){}
		Image(int inWidth, int inHeight) : width(inWidth), height(inHeight){ data = new unsigned char[width * height * NUMBER_OF_CHANNELS]; }
		~Image(){ delete[] data; }
		
		// image data stored per pixel in the order rgba
		unsigned char *data;

		// image size in number of pixels
		unsigned width, height;
	};

	Image *readImageFromFile(const char *filename);
	void saveImageToFile(const char *filename, const Image *image);
	bool isValidSize(const int width, const int height);
	Image *bicubicInterpolation(const Image *image, const float widthScale, const float heightScale);
	Image *bicubicInterpolation(const Image *image, const int width, const int height);
	Image *bilinearInterpolation(const Image *image, const float width, const float height);
	Image *bilinearInterpolation(const Image *image, const int width, const int height);
	Image *nearestNeighbourInterpolation(const Image *image, const float width, const float height);
	Image *nearestNeighbourInterpolation(const Image *image, const int width, const int height);
};