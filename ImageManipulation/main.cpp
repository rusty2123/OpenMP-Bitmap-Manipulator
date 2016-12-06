#include <iostream>
#include <string>
#include <chrono>
#include <omp.h>
#include "bmpReadWrite.h"

using namespace std;
using namespace std::chrono;

void greyScale(BitMapFileHeader& fileHeader, BitMapInfoHeader& infoHeader, RGBTriple*& pixels) {

	readBMPFile(fileHeader, infoHeader, pixels);

	int rowCount = infoHeader.biHeight;		// number of pixels per column
	int colCount = infoHeader.biWidth;		// number of pixels per row

	RGBTriple newPixel;
	RGBTriple currentPixel;

	#pragma omp parallel for private(newPixel, currentPixel)
	for (int c = 0; c < colCount; ++c) {

		for (int r = 0; r < rowCount; ++r) {

			currentPixel = pixels[r*colCount + c];

			newPixel.rgbBlue = (currentPixel.rgbBlue + currentPixel.rgbGreen + currentPixel.rgbRed) / 3;
			newPixel.rgbGreen = newPixel.rgbBlue;
			newPixel.rgbRed = newPixel.rgbBlue;

			pixels[r*colCount + c] = newPixel;
		}
	}

	writeBMPFile(fileHeader, infoHeader, pixels);
}

void changeBrightness(BitMapFileHeader& fileHeader, BitMapInfoHeader& infoHeader, RGBTriple*& pixels) {

	float brightness;
	cout << "Change in brightness? ";
	cin >> brightness;
	cout << endl;

	readBMPFile(fileHeader, infoHeader, pixels);

	int rowCount = infoHeader.biHeight;		// number of pixels per column
	int colCount = infoHeader.biWidth;		// number of pixels per row

	RGBTriple newPixel;
	RGBTriple currentPixel;

	#pragma omp parallel for private(newPixel, currentPixel)
	for (int c = 0; c < colCount; ++c) {

		for (int r = 0; r < rowCount; ++r) {

			currentPixel = pixels[r*colCount + c];

			if (currentPixel.rgbBlue * brightness <= 255) {
				newPixel.rgbBlue = currentPixel.rgbBlue * brightness;
			}
			else {
				newPixel.rgbBlue = 255;
			}

			if (currentPixel.rgbGreen * brightness <= 255) {
				newPixel.rgbGreen = currentPixel.rgbGreen * brightness;
			}
			else {
				newPixel.rgbGreen = 255;
			}

			if (currentPixel.rgbRed * brightness <= 255) {
				newPixel.rgbRed = currentPixel.rgbRed * brightness;
			}
			else {
				newPixel.rgbRed = 255;
			}
			pixels[r*colCount + c] = newPixel;
		}
	}

	writeBMPFile(fileHeader, infoHeader, pixels);
}

void blur(BitMapFileHeader& fileHeader, BitMapInfoHeader& infoHeader, RGBTriple*& pixels) {

	readBMPFile(fileHeader, infoHeader, pixels);

	int rowCount = infoHeader.biHeight;		// number of pixels per column
	int colCount = infoHeader.biWidth;		// number of pixels per row

	RGBTriple newPixel;

	#pragma omp parallel for private(newPixel)
	for (int c = 1; c < colCount - 1; ++c) {

		for (int r = 1; r < rowCount - 1; ++r) {

			newPixel.rgbBlue = (pixels[(r - 1)*colCount + (c - 1)].rgbBlue +
								pixels[(r - 1)*colCount + c].rgbBlue +
								pixels[(r - 1)*colCount + (c + 1)].rgbBlue +
								pixels[r*colCount + (c - 1)].rgbBlue +
								pixels[r*colCount + c].rgbBlue +
								pixels[r*colCount + (c + 1)].rgbBlue +
								pixels[(r + 1)*colCount + (c - 1)].rgbBlue +
								pixels[(r + 1)*colCount + c].rgbBlue +
								pixels[(r + 1)*colCount + (c + 1)].rgbBlue) /
								9;
			newPixel.rgbGreen = (pixels[(r - 1)*colCount + (c - 1)].rgbGreen +
								pixels[(r - 1)*colCount + c].rgbGreen +
								pixels[(r - 1)*colCount + (c + 1)].rgbGreen +
								pixels[r*colCount + (c - 1)].rgbGreen +
								pixels[r*colCount + c].rgbGreen +
								pixels[r*colCount + (c + 1)].rgbGreen +
								pixels[(r + 1)*colCount + (c - 1)].rgbGreen +
								pixels[(r + 1)*colCount + c].rgbGreen +
								pixels[(r + 1)*colCount + (c + 1)].rgbGreen) /
								9;
			newPixel.rgbRed = (pixels[(r - 1)*colCount + (c - 1)].rgbRed +
								pixels[(r - 1)*colCount + c].rgbRed +
								pixels[(r - 1)*colCount + (c + 1)].rgbRed +
								pixels[r*colCount + (c - 1)].rgbRed +
								pixels[r*colCount + c].rgbRed +
								pixels[r*colCount + (c + 1)].rgbRed +
								pixels[(r + 1)*colCount + (c - 1)].rgbRed +
								pixels[(r + 1)*colCount + c].rgbRed +
								pixels[(r + 1)*colCount + (c + 1)].rgbRed) /
								9;

			pixels[r*colCount + c] = newPixel;
		}
	}

	writeBMPFile(fileHeader, infoHeader, pixels);
}

int main() {

	BitMapFileHeader	fileHeader;
	BitMapInfoHeader	infoHeader;
	RGBTriple			*pixels;

	//greyScale(fileHeader, infoHeader, pixels);
	//changeBrightness(fileHeader, infoHeader, pixels);
	//blur(fileHeader, infoHeader, pixels);

}
