#include "imageResizer.h"
#include "lodepng.h"

//
ImageResizer::Image *ImageResizer::readImageFromFile(const char *filename)
{
	ImageResizer::Image *image = new ImageResizer::Image();
	unsigned error = lodepng_decode32_file(&image->data, &image->width, &image->height, filename);
	if (error)
	{
		std::cout << "Error " << error << ": " << lodepng_error_text(error) << std::endl;
		return nullptr;
	}
	std::cout << "Image loaded: " << filename << std::endl;
	return image;
}

//
void ImageResizer::saveImageToFile(const char *filename, const ImageResizer::Image *image)
{
	unsigned error = lodepng_encode32_file(filename, image->data, image->width, image->height);
	if (error)
	{
		std::cout << "Error " << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}
	std::cout << "Image saved: " << filename << std::endl;
}

//
bool ImageResizer::isValidSize(const int width, const int height)
{
	return (width < 2 || height < 2) ? false : true;
}

//
ImageResizer::Image *ImageResizer::bicubicInterpolation(const ImageResizer::Image *image, const float widthScale, const float heightScale)
{
	return ImageResizer::bicubicInterpolation(image, (int)(image->width * widthScale), (int)(image->height * heightScale));
}

// Not implemented yet.
ImageResizer::Image *ImageResizer::bicubicInterpolation(const ImageResizer::Image *image, const int width, const int height)
{
	if (!ImageResizer::isValidSize(width, height)) return nullptr;
	return nullptr;
}

//
ImageResizer::Image *ImageResizer::bilinearInterpolation(const ImageResizer::Image *image, const float widthScale, const float heightScale)
{
	return ImageResizer::bilinearInterpolation(image, (int)(image->width * widthScale), (int)(image->height * heightScale));
}

//
ImageResizer::Image *ImageResizer::bilinearInterpolation(const ImageResizer::Image *image, const int width, const int height)
{
	if (!ImageResizer::isValidSize(width, height)) return nullptr;

	ImageResizer::Image *scaledImage = new ImageResizer::Image(width, height);
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			// scaled image pixel id
			int id1 = (i * width + j) * NUMBER_OF_CHANNELS;

			// sample position in original image
			float y = (i / (float)height) * image->height;
			float x = (j / (float)width) * image->width;

			// y sample pixels in the original image
			int pixel1 = ((int)(y + 0) * image->width + (int)x + 0) * NUMBER_OF_CHANNELS;
			int pixel2 = ((int)(y + 0) * image->width + (int)x + 1) * NUMBER_OF_CHANNELS;
			int pixel3 = ((int)(y + 1) * image->width + (int)x + 0) * NUMBER_OF_CHANNELS;
			int pixel4 = ((int)(y + 1) * image->width + (int)x + 1) * NUMBER_OF_CHANNELS;

			// make sure no pixel id is bigger than number of pixels in image
			int max = image->width * image->height * NUMBER_OF_CHANNELS;
			pixel1 = (pixel1 < max) ? pixel1 : max;
			pixel2 = (pixel2 < max) ? pixel2 : max;
			pixel3 = (pixel3 < max) ? pixel3 : max;
			pixel4 = (pixel4 < max) ? pixel4 : max;

			// interpolate step 1
			float s1, s2, x1, x2, y1, y2;
			x1 = (int)x;
			x2 = (int)(x + 1);
			s1 = ((x2 - x) / (x2 - x1));
			s2 = ((x - x1) / (x2 - x1));
			unsigned char r1_r = s1 * image->data[pixel1 + 0] + s2 * image->data[pixel2 + 0];
			unsigned char r1_g = s1 * image->data[pixel1 + 1] + s2 * image->data[pixel2 + 1];
			unsigned char r1_b = s1 * image->data[pixel1 + 2] + s2 * image->data[pixel2 + 2];
			unsigned char r1_a = s1 * image->data[pixel1 + 3] + s2 * image->data[pixel2 + 3];
			unsigned char r2_r = s1 * image->data[pixel3 + 0] + s2 * image->data[pixel4 + 0];
			unsigned char r2_g = s1 * image->data[pixel3 + 1] + s2 * image->data[pixel4 + 1];
			unsigned char r2_b = s1 * image->data[pixel3 + 2] + s2 * image->data[pixel4 + 2];
			unsigned char r2_a = s1 * image->data[pixel3 + 3] + s2 * image->data[pixel4 + 3];
			
			// interpolate step 2
			y1 = (int)y;
			y2 = (int)(y + 1);
			s1 = ((y2 - y) / (y2 - y1));
			s2 = ((y - y1) / (y2 - y1));
			unsigned char p_r = s1 * r1_r + s2 * r2_r;
			unsigned char p_g = s1 * r1_g + s2 * r2_g;
			unsigned char p_b = s1 * r1_b + s2 * r2_b;
			unsigned char p_a = s1 * r1_a + s2 * r2_a;

			// store result
			scaledImage->data[id1 + 0] = p_r;
			scaledImage->data[id1 + 1] = p_g;
			scaledImage->data[id1 + 2] = p_b;
			scaledImage->data[id1 + 3] = p_a;
		}
	}
	return scaledImage;
}

//
ImageResizer::Image *ImageResizer::nearestNeighbourInterpolation(const ImageResizer::Image *image, const float widthScale, const float heightScale)
{
	return ImageResizer::nearestNeighbourInterpolation(image, (int)(image->width * widthScale), (int)(image->height * heightScale));
}

//
ImageResizer::Image *ImageResizer::nearestNeighbourInterpolation(const ImageResizer::Image *image, const int width, const int height)
{
	if (!ImageResizer::isValidSize(width, height)) return nullptr;
	int id1, id2, x, y;
	ImageResizer::Image *scaledImage = new ImageResizer::Image(width, height);
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			y = (i / (float)height) * image->height;
			x = (j / (float)width) * image->width;
			id1 = (i * width + j) * NUMBER_OF_CHANNELS;
			id2 = (y * image->width + x) * NUMBER_OF_CHANNELS;
			scaledImage->data[id1 + 0] = image->data[id2 + 0];
			scaledImage->data[id1 + 1] = image->data[id2 + 1];
			scaledImage->data[id1 + 2] = image->data[id2 + 2];
			scaledImage->data[id1 + 3] = image->data[id2 + 3];
		}
	}
	return scaledImage;
}
