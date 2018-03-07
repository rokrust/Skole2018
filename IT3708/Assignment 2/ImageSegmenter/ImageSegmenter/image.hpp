#pragma once
#include <vector>
#include <iterator>

#define IMAGE_HEIGHT 321
#define IMAGE_WIDTH 481
#define N_PIXELS IMAGE_HEIGHT*IMAGE_WIDTH
#define IMAGE_DIR "Test image.jpg"

struct Pixel; struct Index; class Image; class Segment;

enum GRAPH_EDGE_DIR { LEFT = 0, RIGHT, UP, DOWN, NONE };

//okay
struct Pixel {
	unsigned char r, g, b;
	double color_distance(Pixel p); //okay

	//Operators
	Pixel operator-(Pixel p); //okay
	//Pixel operator=(Pixel p) { this->r = p.r; this->g = p.g; this->b = p.b; return *this; }
};

//okay
struct Index { 
	int row, col; 

	bool is_none() { return row == -1; }
};

class Image {
private:
	Pixel pixels[IMAGE_HEIGHT][IMAGE_WIDTH];

public:

	Image();
	Image(char* image_dir);

	void read(char* image_dir);

	Index next_index(int row, int col, GRAPH_EDGE_DIR dir) const;
	void get_neighbors(int row, int col, Index* neighbor) const;
	void get_neighbors(int row, int col, std::array<Index, 4> neighbor) const;

	Pixel* operator[](const unsigned int & index) { return pixels[index]; }
	const Pixel* operator[](const unsigned int & index) const { return pixels[index]; }

};
static const Image image(IMAGE_DIR);

//okay
class Segment {
private:
	std::vector<Index> pixel;

public:
	Segment() {}

	size_t get_length() const { return pixel.size(); }
	
	//Vector functions
	std::vector<Index>::iterator begin() { return pixel.begin(); }	//okay
	std::vector<Index>::iterator end() { return pixel.end(); }	//okay
	void add_pixel(int row, int col) { this->pixel.push_back({ row, col }); }

	//Math
	Pixel calculate_centroid(); //okay

	// Operators
	Index& operator[](unsigned int & index) { return pixel[index]; }	//okay
	const Index& operator[](const unsigned int & index) const { return pixel[index]; }	//okay

};