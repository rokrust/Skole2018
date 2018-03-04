#pragma once
#include <vector>
#include <iterator>

#define IMAGE_HEIGHT 481
#define IMAGE_WIDTH 321
#define N_PIXELS IMAGE_HEIGHT*IMAGE_WIDTH

struct Pixel {
	unsigned char r, g, b;
	unsigned int color_distance(Pixel p);

	//Operators
	Pixel operator-(Pixel p);
};

struct Index { int x, y; };

class Segment {
private:
	std::vector<Pixel> pixel;

public:
	Segment();
	~Segment() {}


	const unsigned int get_length() const { return pixel.size(); }
	std::vector<Pixel> get_pixels() const { return pixel; }
	
	//Iterators
	std::vector<Pixel>::iterator begin() { return pixel.begin(); }
	std::vector<Pixel>::iterator end() { return pixel.end(); }

	//Math
	Pixel calculate_centroid();

	// Operators
	Pixel& operator[](const unsigned int & index) { return pixel[index]; }
	const Pixel& operator[](const unsigned int & index) const { return pixel[index]; }
};

class Image {
private:
	Pixel* image;
	unsigned int n_pixels;
	unsigned int height;
	unsigned int width;

public:
	Image(char* image_dir);
	~Image() { delete image; }

	Pixel& operator[](const unsigned int & index) { return image[index]; }
	const Pixel& operator[](const unsigned int & index) const { return image[index]; }

};