#pragma once
#include <iostream>
#include <memory>

namespace ImageResizer
{
	const unsigned NUMBER_OF_CHANNELS = 4;
	struct Image
	{
		Image() : width(0), height(0), data(nullptr){}
		Image(int inWidth, int inHeight) : width(inWidth), height(inHeight){ data = new unsigned char[width * height * NUMBER_OF_CHANNELS]; }
		~Image(){ delete[] data; }
		unsigned char *data;
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