#pragma once
#include <vector>

//struct Pixel { unsigned char r, g, b; };

struct Pixel {
	unsigned char r, g, b;
	unsigned int color_distance(Pixel p);

	//Operators
	Pixel operator-(Pixel p);
};

class Segment {
private:
	std::vector<Pixel> pixel;
	unsigned int segment_length;

public:
	Segment();
	//Segment(char* image_dir);

	~Segment() {}

	Pixel calculate_centroid();
};