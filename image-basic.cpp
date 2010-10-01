/******************************************
image-basic.cpp

Contains the basic constructors and readers for the Image class.
Jaime Silvela
January 2000
******************************************/


#include <tiffio.h>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include "image.hpp"

const int grey_depth = 256;
const grey white = 255;
const grey black= 0;

const queue<point> the_empty_queue;

Image::Image()
{
    height=0;
    width=0;
    size=0;
    matrix=0;
    start=0;
    end=0;

    boundary_queue = the_empty_queue;

    offset_4neighbor[0] = 1;
    offset_4neighbor[1] = -width;
    offset_4neighbor[2] = -1;
    offset_4neighbor[3] = width;

    offset_8neighbor[0] = 1;
    offset_8neighbor[1] = -width +1;
    offset_8neighbor[2] = -width;
    offset_8neighbor[3] = -width -1;
    offset_8neighbor[4] = -1;
    offset_8neighbor[5] = width -1;
    offset_8neighbor[6] = width;
    offset_8neighbor[7] = width +1;
}

Image::Image(int h, int w)
{
    height = h;
    width = w;
    size = h * w;
    matrix = new grey* [height];
    start = new grey[size];
    for (int i=0; i<height; ++i)
        matrix[i]=start+i*width;
    std::memset (start, black, size*sizeof(grey));
    end = start + size -1;

    boundary_queue = the_empty_queue;

    offset_4neighbor[0] = 1;
    offset_4neighbor[1] = -width;
    offset_4neighbor[2] = -1;
    offset_4neighbor[3] = width;

    offset_8neighbor[0] = 1;
    offset_8neighbor[1] = -width +1;
    offset_8neighbor[2] = -width;
    offset_8neighbor[3] = -width -1;
    offset_8neighbor[4] = -1;
    offset_8neighbor[5] = width -1;
    offset_8neighbor[6] = width;
    offset_8neighbor[7] = width +1;
}

Image::Image(const Image& im)
{
    height = im.height;
    width = im.width;
    size = im.size;
    matrix = new grey* [height];
    start = new grey[size];
    for (int i=0; i<height; ++i)
        matrix[i]=start+i*width;
    std::memcpy (start, im.start, size*sizeof(grey));
    end = start + size -1;

    boundary_queue = im.boundary_queue;

    offset_4neighbor[0] = 1;
    offset_4neighbor[1] = -width;
    offset_4neighbor[2] = -1;
    offset_4neighbor[3] = width;

    offset_8neighbor[0] = 1;
    offset_8neighbor[1] = -width +1;
    offset_8neighbor[2] = -width;
    offset_8neighbor[3] = -width -1;
    offset_8neighbor[4] = -1;
    offset_8neighbor[5] = width -1;
    offset_8neighbor[6] = width;
    offset_8neighbor[7] = width +1;
}

Image& Image::operator=(const Image& im)
{
    if (this != &im) {
        delete [] start;
        delete [] matrix;
        height = im.height;
        width = im.width;
        size = im.size;
        matrix = new grey* [height];
        start = new grey[size];
        for (int i=0; i<height; ++i)
            matrix[i]=start+i*width;
        std::memcpy (start, im.start, size*sizeof(grey));
        end = start + size -1;

        boundary_queue = im.boundary_queue;

        offset_4neighbor[0] = 1;
        offset_4neighbor[1] = -width;
        offset_4neighbor[2] = -1;
        offset_4neighbor[3] = width;

        offset_8neighbor[0] = 1;
        offset_8neighbor[1] = -width +1;
        offset_8neighbor[2] = -width;
        offset_8neighbor[3] = -width -1;
        offset_8neighbor[4] = -1;
        offset_8neighbor[5] = width -1;
        offset_8neighbor[6] = width;
        offset_8neighbor[7] = width +1;
    }
    return *this;
}

Image::~Image ()
{
    delete [] start;
    delete [] matrix;
    boundary_queue = the_empty_queue;
}

int Image::get_height() {return height;}
int Image::get_width() {return width;}
int Image::get_size() {return size;}
grey** Image::get_matrix() {return matrix;}
point Image::get_start() {return start;}
point Image::get_end() {return end;}


// read_TIFF_bis: more general but less efficient than read_TIFF below
// Use read_TIFF
void Image::read_TIFF_bis(char *filename)
{
    TIFF* tif = TIFFOpen(filename, "r");
    if (!tif) {
        std::cerr << "Cannot read file: " << filename << endl;
        return;
    }
    _filename = filename;
    feature_vector[letternum] = filename[0] - 'a';
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    size = height * width;
    delete []start;
    delete []matrix;
    matrix = new grey* [height];
    start = new grey[size];
    for (int i=0; i<height; ++i)
        matrix[i]=start+i*width;
    end = start + size -1;

    boundary_queue = the_empty_queue;

    offset_4neighbor[0] = 1;
    offset_4neighbor[1] = -width;
    offset_4neighbor[2] = -1;
    offset_4neighbor[3] = width;

    offset_8neighbor[0] = 1;
    offset_8neighbor[1] = -width +1;
    offset_8neighbor[2] = -width;
    offset_8neighbor[3] = -width -1;
    offset_8neighbor[4] = -1;
    offset_8neighbor[5] = width -1;
    offset_8neighbor[6] = width;
    offset_8neighbor[7] = width +1;

    uint32 *aux = new uint32[size];
    if (TIFFReadRGBAImage(tif, width, height, aux, 0))
        for (int i=0; i < size; ++i)
            start[i]=TIFFGetR(aux[i]);
    TIFFClose(tif);
    delete []aux;
}

void Image::write_TIFF(char *filename)
{
    TIFF* tif = TIFFOpen(filename, "w");
    if (!tif) {
        std::cerr << "Cannot write file: " << filename << endl;
        return;
    }
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_MINSAMPLEVALUE, 0);
    TIFFSetField(tif, TIFFTAG_MAXSAMPLEVALUE, 255);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_BOTLEFT);
    for (int r = 0; r < height; r++)
        TIFFWriteScanline(tif, matrix[r], r);
    TIFFClose(tif);
}

void Image::read_TIFF(char *filename)
{
    TIFF* tif = TIFFOpen(filename, "r");
    if (!tif) {
        std::cerr << "Cannot read file: " << filename << endl;
        return;
    }
    _filename = filename;
    feature_vector[letternum] = filename[0] - 'a';
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    size = height * width;

    boundary_queue = the_empty_queue;

    offset_4neighbor[0] = 1;
    offset_4neighbor[1] = -width;
    offset_4neighbor[2] = -1;
    offset_4neighbor[3] = width;

    offset_8neighbor[0] = 1;
    offset_8neighbor[1] = -width +1;
    offset_8neighbor[2] = -width;
    offset_8neighbor[3] = -width -1;
    offset_8neighbor[4] = -1;
    offset_8neighbor[5] = width -1;
    offset_8neighbor[6] = width;
    offset_8neighbor[7] = width +1;

    if (VERBOSE) std::cout << "TIFF Image\nwidth: " << width
                           << "\nheight: " << height << "\n" << endl;
    delete []start;
    delete []matrix;
    matrix = new grey* [height];
    start = new grey[size];
    for (int r=0; r<height; ++r)
        matrix[r]=start+r*width;
    end = start + size -1;
    for (int r = 0; r < height; ++r)
        TIFFReadScanline(tif, matrix[r], r);
    TIFFClose(tif);
}

//Construct a white rectangle of given length starting at (10, 10)
// Use for testing and debugging
void Image::rectangle(int h, int w)
{
    if (h>390 || w>390) {
        std::cerr << "rectangle: too big!" << endl;
        return;
    }
    delete [] start;
    delete [] matrix;
    height = 400;
    width = 400;
    size = height*width;
    matrix = new grey* [height];
    start = new grey[size];
    end = start + size -1;
    boundary_queue = the_empty_queue;
    std::memset (start, black, size*sizeof(grey));
    for (int i=0; i<height; ++i)
        matrix[i]=start+i*width;
    for (int r=10; r<10+h; ++r)
        std::memset(matrix[r]+10, white, w);

    boundary_queue = the_empty_queue;

    offset_4neighbor[0] = 1;
    offset_4neighbor[1] = -width;
    offset_4neighbor[2] = -1;
    offset_4neighbor[3] = width;

    offset_8neighbor[0] = 1;
    offset_8neighbor[1] = -width +1;
    offset_8neighbor[2] = -width;
    offset_8neighbor[3] = -width -1;
    offset_8neighbor[4] = -1;
    offset_8neighbor[5] = width -1;
    offset_8neighbor[6] = width;
    offset_8neighbor[7] = width +1;

    std::memset (start, black, size*sizeof(grey));
    for (int r=10; r<10+h; ++r)
        std::memset(matrix[r]+10, white, w);
}
