#pragma once
#include <omp.h>
#include <cmath>
#include <cstdint>
#include <vector>
#include <string>

#include "gif.h"

#include "Body.h"
#include "Constants.h"

class NbodyDrawer
{
	uint8_t image[width * height * 4];
	GifWriter writer = {};
	std::vector<Body>& bodies;
	std::string filename;
public:

	NbodyDrawer(std::vector<Body>& _bodies, std::string _filename) : bodies(_bodies), filename(_filename) {
		GifBegin(&writer, filename.c_str(), width, height, 2, 8, true);
		update();

	}

	void cls() {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				SetPixel(i, j, 255, 255, 255);
			}
		}
	}

	void update() {
		cls();
		for (auto& body : bodies) {
			std::pair<int, int> xy = positionToPicture(body.position);
			xy.first += width / 2;
			xy.second += height / 2;
			if (xy.first > 255 || xy.first < 0 || xy.second>255 || xy.second < 0) continue;
			SetPixel(xy.first, xy.second, 0, 0, 255);
		}
		GifWriteFrame(&writer, image, width, height, 2, 8, true);
	}

	void write() {
		GifEnd(&writer);
	}

	std::pair<int, int> positionToPicture(Vector2 position) {
		return { std::round(position.x * coeff_w),std::round(position.y * coeff_h) };
	}

	void SetPixel(int xx, int yy, uint8_t red, uint8_t grn, uint8_t blu)
	{
		uint8_t* pixel = &image[(yy * width + xx) * 4];
		pixel[0] = red;
		pixel[1] = grn;
		pixel[2] = blu;
		pixel[3] = 255;  // no alpha
	}

};

