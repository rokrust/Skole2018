#pragma once
#include <vector>
#include <iterator>

#define IMAGE_HEIGHT 5//481
#define IMAGE_WIDTH 5//321
#define N_PIXELS IMAGE_HEIGHT*IMAGE_WIDTH

struct Pixel; struct Index; class Image; class Segment;

enum GRAPH_EDGE_DIR { LEFT = 0, RIGHT, UP, DOWN, NONE };

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

class Image {
private:
	Pixel image[IMAGE_HEIGHT][IMAGE_WIDTH];
	unsigned int n_pixels;
	unsigned int height;
	unsigned int width;

public:

	Image() {}
	Image(char* image_dir);
	Image(const unsigned int height, const unsigned int width) : height(height), width(width), n_pixels(height*width) {}

	void read(char* image_dir);

	Index next_index(int row, int col, GRAPH_EDGE_DIR dir) const;
	void get_neighbors(int row, int col, Index* neighbor) const;

	Pixel* operator[](const unsigned int & index) { return image[index]; }
	const Pixel* operator[](const unsigned int & index) const { return image[index]; }

};
static const Image image(IMAGE_HEIGHT, IMAGE_WIDTH);

//okay
class Segment {
private:
	std::vector<Index> pixel;
	std::vector<Index> outline;

public:
	Segment();

	size_t get_length() const { return pixel.size(); }
	//std::vector<Pixel> get_pixels() const { return pixel; }
	
	//Vector functions
	std::vector<Index>::iterator begin() { return pixel.begin(); }	//okay
	std::vector<Index>::iterator end() { return pixel.end(); }	//okay
	//void push_back(Pixel p) { this->pixel.push_back(p); }
	void add_pixel(int row, int col) { this->pixel.push_back({ row, col }); }
	void add_pixel_to_outline(int row, int col) { this->outline.push_back({ row, col }); }

	//Math
	Pixel calculate_centroid(); //okay

	// Operators
	Index& operator[](unsigned int & index) { return pixel[index]; }	//okay
	const Index& operator[](const unsigned int & index) const { return pixel[index]; }	//okay

};