// bpmReadWrite.cpp
//   This code will read and write a Windows bit-mapped file (BMP)

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

#include "bmpReadWrite.h"
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

#define DEBUG true

struct RGBQuad			// This struct is needed when biBitCount = 32
{
	ubyte rgbBlue;		// Blue value
	ubyte rgbGreen;		// Green value
	ubyte rgbRed;		// Red value
	ubyte rgbReserved;	// unused
};

void readHeaders(ifstream		&fin,
	BitMapFileHeader	&FileHeader,
	BitMapInfoHeader	&InfoHeader)
{
	fin.read((char*)&FileHeader.bfType, 2);
	fin.read((char*)&FileHeader.bfsize, 4);
	fin.read((char*)&FileHeader.unused, 4);
	fin.read((char*)&FileHeader.bfOffBits, 4);
	fin.read((char*)&InfoHeader.biSize, 4);
	fin.read((char*)&InfoHeader.biWidth, 4);
	fin.read((char*)&InfoHeader.biHeight, 4);
	fin.read((char*)&InfoHeader.biPlanes, 2);
	fin.read((char*)&InfoHeader.biBitCount, 2);
	fin.read((char*)&InfoHeader.biCompression, 4);
	fin.read((char*)&InfoHeader.biSizeImage, 4);
	fin.read((char*)&InfoHeader.bixPelsPerMeter, 4);
	fin.read((char*)&InfoHeader.biyPelsPerMeter, 4);
	fin.read((char*)&InfoHeader.biClrUsed, 4);
	fin.read((char*)&InfoHeader.biClrImportant, 4);
}

void printHeaders(BitMapFileHeader FileHeader,
	BitMapInfoHeader InfoHeader)
{
	cout << "bfType:          " << FileHeader.bfType[0] << FileHeader.bfType[1] << endl;
	cout << "bfSize:          " << FileHeader.bfsize << endl;
	cout << "unused:          " << FileHeader.unused << endl;
	cout << "bfOffBits:       " << FileHeader.bfOffBits << endl;
	cout << "biSize:          " << InfoHeader.biSize << endl;
	cout << "biWidth:         " << InfoHeader.biWidth << endl;
	cout << "biHeight:        " << InfoHeader.biHeight << endl;
	cout << "biPlanes:        " << InfoHeader.biPlanes << endl;
	cout << "biBitCount:      " << InfoHeader.biBitCount << endl;
	cout << "biCompression:   " << InfoHeader.biCompression;
	switch (InfoHeader.biCompression) {
	case 0:		cout << " (BI_RGB)" << endl; break;
	case 1:		cout << " (BI_RLE8)" << endl; break;
	case 2:		cout << " (BI_RLE4)" << endl; break;
	case 3:		cout << " (BI_BITFIELDS)" << endl; break;
	default:	cout << endl;
	}
	cout << "biSizeImage:     " << InfoHeader.biSizeImage << endl;
	cout << "bixPelsPerMeter: " << InfoHeader.bixPelsPerMeter << endl;
	cout << "biyPelsPerMeter: " << InfoHeader.biyPelsPerMeter << endl;
	cout << "biClrUsed:       " << InfoHeader.biClrUsed << endl;
	cout << "biClrImportant:  " << InfoHeader.biClrImportant << endl;
}

void Read24BitPixelData(ifstream		&fin,
	RGBTriple		pixels[],
	BitMapFileHeader	FileHeader,
	BitMapInfoHeader	InfoHeader)
{
	char  garbage[3];

	myulong numRows = InfoHeader.biHeight;	// number of pixels per column
	myulong numCols = InfoHeader.biWidth;		// number of pixels per row
	myulong numBytesPerRow = 3 * numCols;		// number of bytes per row with 3 bytes per pixel;
	myulong numGarbageBytes = numCols % 4;
	myulong numBytesRead = 0;

	fin.seekg(FileHeader.bfOffBits);		// Seek to start of pixel data

	for (unsigned int row = 0; row<numRows; row++) {	// Read one row at a time
		fin.read((char*)&pixels[row*numCols], numBytesPerRow);
		if (numGarbageBytes > 0)
			fin.read((char*)&garbage, numGarbageBytes);
		numBytesRead += (numBytesPerRow + numGarbageBytes);
	}

#ifdef DEBUG 
	cout << "Number of pixel bytes read = " << numBytesRead << " with " << numRows << " pixels per column and "
		<< numCols << " pixels per row and with " << numGarbageBytes << " garbage bytes per row" << endl;
#endif
}

void Read32BitPixelData(ifstream		&fin,
	RGBTriple		pixels[],
	BitMapFileHeader	FileHeader,
	BitMapInfoHeader	InfoHeader)
{
	myulong rows, cols, size;
	myulong index;

	rows = InfoHeader.biHeight;		// number of pixels per column
	cols = InfoHeader.biWidth;		// number of pixels per row
	size = 4 * rows*cols;				// 4 bytes per pixel;
	RGBQuad *data = new RGBQuad[InfoHeader.biHeight*InfoHeader.biWidth];

	fin.seekg(FileHeader.bfOffBits);
	fin.read((char*)&data[0], size);
#ifdef DEBUG
	cout << "Number of pixel bytes: " << fin.gcount() << " with " << rows << " pixels per column and "
		<< cols << " pixels per row" << endl;
#endif

	for (myulong i = 0; i<rows; i++) {
		for (myulong j = 0; j<cols; j++) {
			index = i*cols + j;
			pixels[index].rgbBlue = data[index].rgbBlue;
			pixels[index].rgbGreen = data[index].rgbGreen;
			pixels[index].rgbRed = data[index].rgbRed;
		}
	}
	delete[]data;
}

void readPixelData(ifstream		&fin,
	RGBTriple		pixels[],
	BitMapFileHeader 	FileHeader,
	BitMapInfoHeader 	InfoHeader)
{
	bool OK = true;
	if ((InfoHeader.biSize < 40)) {
		cout << "This program will not work with BMP files with headers size < 40." << endl;
		OK = false;
	}
	if (InfoHeader.biCompression != 0) {
		cout << "This program will not work with BMP files that use compression." << endl;
		OK = false;
	}

	if (OK) {
		if (InfoHeader.biBitCount == 24) {
			Read24BitPixelData(fin, pixels, FileHeader, InfoHeader);
		}
		else if (InfoHeader.biBitCount == 32) {
			Read32BitPixelData(fin, pixels, FileHeader, InfoHeader);
		}
		else {
			cout << "This program will not work with BMP files that do not use 24-bit or 32-bit pixels." << endl;
		}
	}
}

void writeHeaders(ofstream		&fout,
	BitMapFileHeader	FileHeader,
	BitMapInfoHeader	InfoHeader)
{
	fout.seekp(0);
	fout.write((char*)&FileHeader.bfType, 2);
	fout.write((char*)&FileHeader.bfsize, 4);
	fout.write((char*)&FileHeader.unused, 4);
	fout.write((char*)&FileHeader.bfOffBits, 4);
	fout.write((char*)&InfoHeader.biSize, 4);
	fout.write((char*)&InfoHeader.biWidth, 4);
	fout.write((char*)&InfoHeader.biHeight, 4);
	fout.write((char*)&InfoHeader.biPlanes, 2);
	fout.write((char*)&InfoHeader.biBitCount, 2);
	fout.write((char*)&InfoHeader.biCompression, 4);
	fout.write((char*)&InfoHeader.biSizeImage, 4);
	fout.write((char*)&InfoHeader.bixPelsPerMeter, 4);
	fout.write((char*)&InfoHeader.biyPelsPerMeter, 4);
	fout.write((char*)&InfoHeader.biClrUsed, 4);
	fout.write((char*)&InfoHeader.biClrImportant, 4);
}

void Write24BitPixelData(ofstream		&fout,
	RGBTriple		pixels[],
	BitMapFileHeader	FileHeader,
	BitMapInfoHeader	InfoHeader)
{
	char garbage[3] = "  ";

	myulong numRows = InfoHeader.biHeight;
	myulong numCols = InfoHeader.biWidth;
	myulong numBytesPerRow = 3 * numCols;
	myulong numGarbageBytes = numCols % 4;
	myulong numBytesWritten = 0;

	fout.seekp(FileHeader.bfOffBits);	// Skip over headers

	for (unsigned int row = 0; row<numRows; row++) {	// Write one row at a time
		fout.write((char*)&pixels[row*numCols], numBytesPerRow);
		if (numGarbageBytes > 0) {
			fout.write((char*)&garbage, numGarbageBytes);
		}
		numBytesWritten += (numBytesPerRow + numGarbageBytes);
	}
#ifdef DEBUG
	cout << "Wrote " << numBytesWritten << " bytes of pixel data with " << numCols << " pixels "
		<< "per row and " << numRows << " rows" << endl;
#endif
}

void Write32BitPixelData(ofstream		&fout,
	RGBTriple		pixels[],
	BitMapFileHeader	FileHeader,
	BitMapInfoHeader	InfoHeader)
{
	myulong NumBytes;
	myulong rows, cols;
	myulong index;

	rows = InfoHeader.biHeight;		// number of pixels per column
	cols = InfoHeader.biWidth;		// number of pixels per row
	NumBytes = 4 * rows*cols;			// 4 bytes per pixel;
	RGBQuad *data = new RGBQuad[InfoHeader.biHeight*InfoHeader.biWidth];

	fout.seekp(FileHeader.bfOffBits);
	for (myulong i = 0; i<rows; i++) {
		for (myulong j = 0; j<cols; j++) {
			index = i*cols + j;
			data[index].rgbBlue = pixels[index].rgbBlue;
			data[index].rgbGreen = pixels[index].rgbGreen;
			data[index].rgbRed = pixels[index].rgbRed;
			data[index].rgbReserved = 0;
		}
	}
	fout.write((char*)&data[0], NumBytes);

	delete[]data;
}

void writePixelData(ofstream		&fout,
	RGBTriple		pixels[],
	BitMapFileHeader	FileHeader,
	BitMapInfoHeader	InfoHeader)
{
	if (InfoHeader.biBitCount == 24) {
		Write24BitPixelData(fout, pixels, FileHeader, InfoHeader);
	}
	else if (InfoHeader.biBitCount == 32) {
		Write32BitPixelData(fout, pixels, FileHeader, InfoHeader);
	}
}

bool readBMPFile(BitMapFileHeader &FileHeader,
	BitMapInfoHeader &InfoHeader,
	RGBTriple* 		&pixels)
{
	bool success = true;
	ifstream	fin;
	string		filename;

	cout << "Bit mapped filename [.bmp]? ";
	getline(cin, filename);
	if (filename.find(".") == string::npos) filename += ".bmp";

	fin.open(filename.c_str(), ios::binary);
	while (fin.fail() && success) {
		fin.clear();
		if (filename == "")
			success = false;
		else {
			cout << "Unable to open file: " << filename << endl;
			cout << "Enter a new filename: ";
			getline(cin, filename);
			if (filename == "") {
				success = false;
			}
			else {
				if (filename.find(".") == string::npos) filename += ".bmp";
				fin.open(filename.c_str(), ios::binary);
			}
		}
	}

	if (success) {
		readHeaders(fin, FileHeader, InfoHeader);
#ifdef DEBUG
		cout << endl << "Header information from file: " << filename << endl;
		printHeaders(FileHeader, InfoHeader);
#endif

		if ((FileHeader.bfType[0] != 'B' || FileHeader.bfType[1] != 'M') || (FileHeader.unused != 0)) {
			cout << "File is not a Windows BitMap File (BMP)." << endl;
			fin.close();
			success = false;
		}
	}

	if (success) {
		myulong numRows = InfoHeader.biHeight;
		myulong numCols = InfoHeader.biWidth;
		myulong numPixels = numRows * numCols;
		pixels = new RGBTriple[numPixels];

#ifdef DEBUG
		cout << endl << "Reading " << numPixels << " pixels from file: " << filename << endl;
#endif
		readPixelData(fin, pixels, FileHeader, InfoHeader);

#ifdef DEBUG
		cout << "Created array: pixels[" << numPixels << "]" << " using "
			<< sizeof(RGBTriple)* numPixels << " bytes" << endl;
#endif

		fin.close();		// Finished with the input file
	}
	return success;
}

bool writeBMPFile(BitMapFileHeader	FileHeader,
	BitMapInfoHeader	InfoHeader,
	RGBTriple		pixels[])
{
	bool WriteNewFile = true;
	ifstream fin;
	ofstream fout;
	string filename;

	cout << endl << endl << "Output Filename [.bmp]? ";
	getline(cin, filename);
	if (filename == "") {
		WriteNewFile = false;
	}
	else {
		if (filename.find(".") == string::npos) filename += ".bmp";
		fin.open(filename.c_str());			// Open for input to see if it already exists.
		if (!fin.fail() && WriteNewFile)
		{
			fin.close();
			cout << "This file already exists.  Do you want to overwrite it? ";
			string answer;
			getline(cin, answer);
			if (!(answer[0] == 'Y' || answer[0] == 'y')) {
				WriteNewFile = false;
			}
		}
	}

	if (WriteNewFile)
	{
		fout.open(filename.c_str(), ios::binary);		// Now, open for output

#ifdef DEBUG
		cout << "Writing headers to file: " << filename << endl;
#endif
		writeHeaders(fout, FileHeader, InfoHeader);

#ifdef DEBUG
		cout << "Writing " << (InfoHeader.biWidth * InfoHeader.biHeight)
			<< " pixels" << endl;
#endif
		writePixelData(fout, pixels, FileHeader, InfoHeader);
		fout.close();
	}

	return WriteNewFile;
}