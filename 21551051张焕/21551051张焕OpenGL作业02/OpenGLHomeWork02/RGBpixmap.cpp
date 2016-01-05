// RGBpixmap.cpp - routines to read a BMP file
#include "RGBpixmap.h"

typedef unsigned short ushort;
typedef unsigned long ulong;
fstream inf; // global in this file for convenience


ushort getShort()
{
		char ic;
		ushort ip;
		inf.get(ic); ip = ic;
		inf.get(ic);  ip |= ((ushort)ic << 8);
		return ip;
}

ulong getLong()
{

		ulong ip = 0;
		char ic = 0;
		unsigned char uc = ic;
		inf.get(ic); uc = ic; ip = uc;
		inf.get(ic); uc = ic; ip |=((ulong)uc << 8);
		inf.get(ic); uc = ic; ip |=((ulong)uc << 16);
		inf.get(ic); uc = ic; ip |=((ulong)uc << 24);
		return ip;
	}

int RGBpixmap:: readBMPFile(string fname, bool hasAlpha) //读取BMP文件
{
	inf.open(fname.c_str(), ios::in|ios::binary);
	if(!inf) {
        cout << " can't open file: " << fname << endl;
        return 0;
    }
	int k, row, col, numPadBytes, nBytesInRow;


	char ch1, ch2;
	inf.get(ch1);
	inf.get(ch2);
	ulong fileSize      = getLong();
	ushort reserved1    = getShort();	// always 0
	ushort reserved2    = getShort();	// always 0 
	ulong offBits       = getLong();
	ulong headerSize    = getLong();	// always 40
	ulong numCols       = getLong();
	ulong numRows       = getLong();
	ushort planes       = getShort();	// always 1 
	ushort bitsPerPixel = getShort();
	ulong compression   = getLong();
	ulong imageSize     = getLong();
	ulong xPels         = getLong();	// always 0 
	ulong yPels         = getLong();	// always 0 
	ulong numLUTentries = getLong();
	ulong impColors     = getLong();	// always 0 

	if(bitsPerPixel != 24) 
	{
		// error - must be a 24 bit uncompressed image
		cout << "not a 24 bit/pixelimage, or is compressed!\n";
		inf.close();
		return 0;
	} 

	// Add bytes at end of each row so 
	// total # is a multiple of 4;
	// round up 3*numCols to next mult. of 4
	nBytesInRow = ((3 * numCols + 3)/4) * 4;
	numPadBytes = nBytesInRow - 3 * numCols; // need this many
	nRows = numRows; // set class's data members
	nCols = numCols;
	pixel = new mRGB[nRows * nCols]; //make space for array
	if (!pixel)
		return 0; // out of memory!

	long count = 0;
	char dum;
	for(row = 0; row < nRows; row++) // read pixel values
	{
		for(col = 0; col < nCols; col++)
		{
			char r,g,b;
			inf.get(b);
			inf.get(g);
			inf.get(r); //read bytes
			pixel[count].r = r;
			pixel[count].g = g;
			pixel[count].b = b;
			if ( (hasAlpha) && (r==-1) && (g==-1) && (b==-1) )
				pixel[count++].a = 0;
			else
				pixel[count++].a = 255;
		}
   		for(k = 0; k < numPadBytes ; k++)
			inf >> dum;
	}
	inf.close();
	return 1;
}

void RGBpixmap :: setTexture(GLuint textureName)
{
	glBindTexture(GL_TEXTURE_2D,textureName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nCols, nRows,0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
}

