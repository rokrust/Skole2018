#pragma once
#include <vector>
#include <iterator>

#define IMAGE_HEIGHT 5//481
#define IMAGE_WIDTH 5//321
#define N_PIXELS IMAGE_HEIGHT*IMAGE_WIDTH

//okay
struct Pixel {
	unsigned char r, g, b;
	double color_distance(Pixel p); //okay

	//Operators
	Pixel operator-(Pixel p); //okay
};

//okay
struct Index { 
	int row, col; 

	bool is_none() { return row == -1; }
};

//okay
class Segment {
private:
	std::vector<Pixel> pixel;

public:
	Segment();
	~Segment() {}


	size_t get_length() const { return pixel.size(); }
	std::vector<Pixel> get_pixels() const { return pixel; }
	
	//Iterators
	std::vector<Pixel>::iterator begin() { return pixel.begin(); }	//okay
	std::vector<Pixel>::iterator end() { return pixel.end(); }	//okay

	//Math
	Pixel calculate_centroid(); //okay

	void test() {
		pixel.push_back({ 10, 10, 10 });
		pixel.push_back({ 40, 40, 40 });
		pixel.push_back({ 5, 5, 5 });
		pixel.push_back({ 45, 45, 45 });
	}

	// Operators
	Pixel& operator[](const unsigned int & index) { return pixel[index]; }	//okay
	const Pixel& operator[](const unsigned int & index) const { return pixel[index]; }	//okay
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