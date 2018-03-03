#pragma once
#include <string>

//struct Pixel { unsigned char r, g, b; };

class Pixel {
private:
	unsigned char r, g, b;

public:
	Pixel() : Pixel(0, 0, 0) {}
	Pixel(unsigned int r, unsigned int g, unsigned int b) : r(r), g(g), b(b) {}

	//Set and get
	void set_r(unsigned char r) { this->r = r; }
	void set_g(unsigned char g) { this->g = g; }
	void set_b(unsigned char b) { this->b = b; }

	unsigned char get_r() { return r; }
	unsigned char get_g() { return g; }
	unsigned char get_b() { return b; }
	
	unsigned int color_distance(Pixel p);

	//Operators
	Pixel operator-(Pixel p);
};

//Pass på å trekke fra minste verdi
unsigned int Pixel::color_distance(Pixel p) {
	Pixel p_diff = *this - p;
	return sqrt(pow(p_diff.get_r(), 2) + pow(p_diff.get_g(), 2) + pow(p_diff.get_b(), 2));
}

Pixel Pixel::operator-(Pixel p) {
	Pixel rhs;

	this->r > p.get_r() ? rhs.set_r(this->r - p.get_r()) : rhs.set_r(p.get_r() - this->r);
	this->g > p.get_g() ? rhs.set_g(this->g - p.get_g()) : rhs.set_g(p.get_g() - this->g);
	this->b > p.get_b() ? rhs.set_b(this->b - p.get_b()) : rhs.set_b(p.get_b() - this->b);

	return rhs;

}


class Image {
private:
	Pixel* picture;
public:
	Image();
	Image(char* image_dir);

	~Image() { picture = nullptr; }

	Pixel calculate_centroid(Pixel* segment, unsigned int segment_length);
};