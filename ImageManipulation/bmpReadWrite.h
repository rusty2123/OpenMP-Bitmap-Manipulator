#pragma once
// bmpReadWrite.h
//   Define types, structs and functions needed to read and write Windows BitMap files.

// Windows BMP files are stored using the following format:
//	BitMapFileHeader (14 bytes)
//	BitMapInfoHeader (>=40 bytes)
//	Color Palette    (Used when bits per pixel < 24.  For these cases the Color
//						Palette will occupy 4*NumColor bytes where NumColor=2^biBitCount.
//						For example, if biBitCount=8, then NumColor=256 and the
//						color palette will occupy 1024 bytes.
//
//						HOWEVER, this program doesn't support files that use a Palette.
//						Only BMP files with 24 or 32 bits per pixel are supported.)
//
//	Pixel Data		 (Starts at offset: bfOffBits and 
//						will contain biWidth*biHeight pixels.)

#include <fstream>

typedef unsigned int   myulong;
typedef unsigned short ushort;
typedef unsigned char  ubyte;
typedef char byte;

struct BitMapFileHeader {
	char	bfType[2];	// Should contain the characters: BM
	myulong	bfsize;		// File size in bytes  (should match actual disk file size)
	myulong	unused;		// Must be 0.
	myulong	bfOffBits;	// Offset to start of pixel data.
};

struct BitMapInfoHeader {
	myulong	biSize;				// Header size (should be >= 40)
	myulong	biWidth;			// Image Width
	myulong	biHeight;			// Image Height
	ushort	biPlanes;			// Should be 1
	ushort	biBitCount;			// Bits per pixel (1, 4, 8, 16, 24, or 32)
	myulong	biCompression;		// Compression type (0=BI_RGB,  1=BI_RLE8
								//                   2=BI_RLE4, 3=BI_BITFIELDS)
	myulong	biSizeImage;		// Image Size (May be zero if not compressed)
	myulong	bixPelsPerMeter;	// Preferred resolution in pixels per meter (horizontal)
	myulong	biyPelsPerMeter;	// Preferred resolution in pixels per meter (vertical)
	myulong	biClrUsed;			// Number of entries in the color map that are actually used
	myulong	biClrImportant;		// Number of significant colors
};

struct RGBTriple {		// This struct is used to hold the pixel data
	ubyte rgbBlue;		// Blue value
	ubyte rgbGreen;		// Green value
	ubyte rgbRed;		// Red value
};

void readHeaders(std::ifstream		&fin,			// IN  - input filehandle
	BitMapFileHeader	&FileHeader,	// OUT
	BitMapInfoHeader	&InfoHeader);	// OUT

void printHeaders(BitMapFileHeader	FileHeader,		// IN
	BitMapInfoHeader	InfoHeader);	// IN

void writeHeaders(std::ofstream		&fout,			// IN  - output filehandle
	BitMapFileHeader	FileHeader,		// IN
	BitMapInfoHeader	InfoHeader);	// IN

void readPixelData(std::ifstream		&fin, 			// IN  - input filehandle
	RGBTriple			pixels[], 		// OUT - array of RGB pixels
	BitMapFileHeader	FileHeader,		// IN
	BitMapInfoHeader	InfoHeader);	// IN

void writePixelData(std::ofstream		&fout,			// IN  - output filehandle
	RGBTriple			pixels[],		// IN  - array of RGB pixels
	BitMapFileHeader	FileHeader,		// IN
	BitMapInfoHeader	InfoHeader);	// IN

bool readBMPFile(BitMapFileHeader &FileHeader,
	BitMapInfoHeader &InfoHeader,
	RGBTriple* 		&pixels);

bool writeBMPFile(BitMapFileHeader	FileHeader,
	BitMapInfoHeader	InfoHeader,
	RGBTriple		pixels[]);