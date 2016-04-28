#include "resizer.h"
#include "lodepng.h"

// Load .png image from file.
// Takes path to file including filename as argument.
// A pointer to the loaded image is then returned, or a nullptr if the image could not be loaded.
Resizer::Image *Resizer::readImageFromFile(const char *filename)
{
    Resizer::Image *image = new Resizer::Image();
    unsigned error = lodepng_decode32_file(&image->data, &image->width, &image->height, filename);
    if (error)
    {
        std::cout << "Error " << error << ": " << lodepng_error_text(error) << std::endl;
        return nullptr;
    }
    std::cout << "Image loaded: " << filename << std::endl;
    return image;
}

// Save .png image to file.
// Takes path to file including filename and a pointer to a image as arguments.
void Resizer::saveImageToFile(const char *filename, const Resizer::Image *image)
{
    unsigned error = lodepng_encode32_file(filename, image->data, image->width, image->height);
    if (error)
    {
        std::cout << "Error " << error << ": " << lodepng_error_text(error) << std::endl;
        return;
    }
    std::cout << "Image saved: " << filename << std::endl;
}

// Checks to see if a choosen image size is valid to resize to.
// Takes image width and height in pixels as arguments.
// If the image size is valid it returns true.
bool Resizer::isValidSize(const int width, const int height)
{
    return (width >= Resizer::MIN_VALID_WIDTH && height >= Resizer::MIN_VALID_HEIGHT && width <= Resizer::MAX_VALID_WIDTH && height <= Resizer::MAX_VALID_HEIGHT) ? true : false;
}

// Not implemented yet.
Resizer::Image *Resizer::bicubicInterpolation(const Resizer::Image *image, const float widthScale, const float heightScale)
{
    return Resizer::bicubicInterpolation(image, (int)(image->width * widthScale), (int)(image->height * heightScale));
}

// Not implemented yet.
Resizer::Image *Resizer::bicubicInterpolation(const Resizer::Image *image, const int width, const int height)
{
    if (!Resizer::isValidSize(width, height)) return nullptr;
    return nullptr;
}

// Creates a resized copy of a image using bilinear interpolation.
// Takes a original image and how much to scale the width and height in percentage.
// It then returns a pointer to the resized image or nullptr if something went wrong.
Resizer::Image *Resizer::bilinearInterpolation(const Resizer::Image *image, const float widthScale, const float heightScale)
{
    return Resizer::bilinearInterpolation(image, (int)(image->width * widthScale), (int)(image->height * heightScale));
}

// Creates a resized copy of a image using bilinear interpolation.
// Takes a original image and the wanted pixel size of the resized image.
// It then returns a pointer to the resized image or nullptr if something went wrong.
Resizer::Image *Resizer::bilinearInterpolation(const Resizer::Image *image, const int width, const int height)
{
    if (!Resizer::isValidSize(width, height)) return nullptr;

    Resizer::Image *scaledImage = new Resizer::Image(width, height);
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            // scaled image pixel id
            int id1 = (i * width + j) * Resizer::NUMBER_OF_CHANNELS;

            // sample position in original image
            float y = (i / (float)height) * image->height;
            float x = (j / (float)width) * image->width;

            // y sample pixels in the original image
            int pixel1 = ((int)(y + 0) * image->width + (int)x + 0) * Resizer::NUMBER_OF_CHANNELS;
            int pixel2 = ((int)(y + 0) * image->width + (int)x + 1) * Resizer::NUMBER_OF_CHANNELS;
            int pixel3 = ((int)(y + 1) * image->width + (int)x + 0) * Resizer::NUMBER_OF_CHANNELS;
            int pixel4 = ((int)(y + 1) * image->width + (int)x + 1) * Resizer::NUMBER_OF_CHANNELS;

            // make sure no pixel id is bigger than number of pixels in image
            int max = image->width * image->height * Resizer::NUMBER_OF_CHANNELS;
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

// Creates a resized copy of a image using nearest neighbour interpolation.
// Takes a original image and how much to scale the width and height in percentage.
// It then returns a pointer to the resized image or nullptr if something went wrong.
Resizer::Image *Resizer::nearestNeighbourInterpolation(const Resizer::Image *image, const float widthScale, const float heightScale)
{
    return Resizer::nearestNeighbourInterpolation(image, (int)(image->width * widthScale), (int)(image->height * heightScale));
}

// Creates a resized copy of a image using nearest neighbour interpolation.
// Takes a original image and the wanted pixel size of the resized image.
// It then returns a pointer to the resized image or nullptr if something went wrong.
Resizer::Image *Resizer::nearestNeighbourInterpolation(const Resizer::Image *image, const int width, const int height)
{
    if (!Resizer::isValidSize(width, height)) return nullptr;
    int id1, id2, x, y;
    Resizer::Image *scaledImage = new Resizer::Image(width, height);
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            y = (i / (float)height) * image->height;
            x = (j / (float)width) * image->width;
            id1 = (i * width + j) * Resizer::NUMBER_OF_CHANNELS;
            id2 = (y * image->width + x) * Resizer::NUMBER_OF_CHANNELS;
            scaledImage->data[id1 + 0] = image->data[id2 + 0];
            scaledImage->data[id1 + 1] = image->data[id2 + 1];
            scaledImage->data[id1 + 2] = image->data[id2 + 2];
            scaledImage->data[id1 + 3] = image->data[id2 + 3];
        }
    }
    return scaledImage;
}
