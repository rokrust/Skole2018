#pragma once
#include <vector>

static int MAX_SEGMENTS = 300;
#define IMAGE_HEIGHT 1450//321
#define IMAGE_WIDTH 269//481
#define N_PIXELS IMAGE_HEIGHT*IMAGE_WIDTH
#define IMAGE_DIR "hvetemel.jpg"
//#define IMAGE_DIR "../../Test Images/160068/Test image.jpg"

struct Pixel; struct Index; class Image; class Segment;

enum GRAPH_EDGE_DIR { LEFT = 0, RIGHT, UP, DOWN, NONE };

//okay
struct Pixel {
	unsigned char r, g, b;
	double color_distance(Pixel p); //okay
	double color_distance(Pixel p) const;

	//Operators
	Pixel operator-(Pixel p); //okay
	Pixel Pixel::operator-(Pixel p) const;
};

//okay
struct Index { 
	int row, col; 

	bool is_none() { return row == -1; }
	GRAPH_EDGE_DIR get_direction_to(Index p1);
	bool operator==(Index rhs) { return this->row == rhs.row && this->col == rhs.col; }
};

class Image {
private:
	Pixel** pixels;
	double* vertical_color_distance;
	double* horizontal_color_distance;

public:

	Image();
	Image(char* image_dir);
	Image(const Image& image_in);

	~Image();
	void read(char* image_dir);
	void write(char* image_dir);

	void calculate_color_distances() const;
	double distance_between(const Index& p1, const Index& p2) const;

	Index next_index(int row, int col, GRAPH_EDGE_DIR dir) const;
	void get_neighbors(int row, int col, std::array<Index, 4>& neighbor) const;

	Pixel* operator[](const unsigned int & index) { return pixels[index]; }
	const Pixel* operator[](const unsigned int & index) const { return pixels[index]; }

};
static const Image image(IMAGE_DIR);

//okay
class Segment {
private:
	std::vector<Index> pixel;
	bool infinite_length;

public:
	Segment() { infinite_length = false; }

	size_t get_length() const { 
		if (infinite_length) {
			return std::numeric_limits<int>::infinity();
		}
		return pixel.size(); 
	}
	
	//Vector functions
	std::vector<Index>::iterator begin() { return pixel.begin(); }	//okay
	std::vector<Index>::iterator end() { return pixel.end(); }	//okay
	void add_pixel(int row, int col) { this->pixel.push_back({ row, col }); }
	void set_infinite_length() { infinite_length = true; }

	//Math
	Pixel calculate_centroid(); //okay

	// Operators
	Index& operator[](unsigned int & index) { return pixel[index]; }	//okay
	const Index& operator[](const unsigned int & index) const { return pixel[index]; }	//okay

};