/**
 * @file image.h
 * 
 * Structure d'une image et methodes pour sauver et charger des images. 
 * Utilise par le traitement d'image de la webcam et pour les textures du
 * viewer3D
 * 
 * @Author Laurent Saint-Marcel
 * @Author Benjamin Piette
 * @Date 07/11/02
 *
 */

#ifndef __VIDEO_IMAGE_H__
#define __VIDEO_IMAGE_H__

#include <stdio.h>

static const unsigned int IMAGE_MAX_WIDTH  = 640; 
static const unsigned int IMAGE_MAX_HEIGHT = 480;

/* Structure of an RGB pixel */

typedef struct Pixel{
    unsigned char r,g,b;
    Pixel(): r(0), g(0), b(0) {}
} Pixel;


/** 
 * @class Image
 * Image structure, used for communication 
 * between capture and treatment modules. 
 */
class Image {
 public:
    unsigned int width;
    unsigned int height;
    unsigned int bpp; // bits per pixel: 24 only 
    Pixel *      data;
    bool         needFreeMem;

 public:
    Image();
    Image(Image const& img);
    ~Image();
    void copy(Image const& imgSrc);
    void alloc(unsigned int width, 
               unsigned int heigh);
    void copyAndPow2(Image const& imgSrc);
    Image operator=(Image const& imgSrc);
};

// ------------------------------------------------------------------------
// Image::Image
// ------------------------------------------------------------------------
inline Image::Image():
  width(0), height(0), bpp(24), data(NULL), needFreeMem(false)
{
}

// ------------------------------------------------------------------------
// Image::Image
// ------------------------------------------------------------------------
inline Image::Image(Image const& img): 
  width(img.width), height(img.height), 
     bpp(img.bpp), data(img.data), needFreeMem(false) 
{
}

// ------------------------------------------------------------------------
// Image::~Image
// ------------------------------------------------------------------------
inline Image::~Image()
{
    if (needFreeMem && data) {
        delete[] data;
    }
    data = NULL;
}

// ------------------------------------------------------------------------
// Image::operator=
// ------------------------------------------------------------------------
inline Image Image::operator=(Image const& imgSrc)
{
    if (needFreeMem && data) {
        delete[] data;
        data = NULL;
    }
    width  = imgSrc.width;
    height = imgSrc.height;
    bpp    = imgSrc.bpp;
    data   = imgSrc.data;
    needFreeMem = false;
    return *this;
}

// ------------------------------------------------------------------------
// Image::alloc
// ------------------------------------------------------------------------
inline void Image::alloc(unsigned int Width, 
                         unsigned int Height)
{
    if (!needFreeMem) {
        data=NULL;
    }
    if (needFreeMem && data) {
        if (Width*Height>width*height) {
            delete[] data;
            data = NULL;
        }
    }
    if (!data) {
        data = new Pixel[Width*Height];
        needFreeMem = true;
    }
    width = Width;
    height = Height;
}


#include "robotFile.h"
namespace BmpBitmap {

    bool load(Image &img,
	      const char * filename, 
	      bool compress=false);
    bool save(Image &img, 
	      const char * filename, 
	      bool   compressed=false,
	      bool   rotate=false,
	      double scale=1.0);

};

namespace TgaBitmap {

    bool load(Image &img, const char * filename, bool compress=false);
    bool save(Image &img, 
	      const char * filename,
	      bool compressed=false,
	      bool   rotate=false,
	      double scale=1.0);

    // Header found in the TGA 1.0 spec
    typedef struct{
        unsigned char  idLength;	// length of ID string 
        unsigned char  mapType;	        // color map type, if 1 then there is a
                                        // palette. if 0, then there is no 
                                        // palette.
        unsigned char  imageType;	// image type code 
        unsigned short mapStart;	// starting index of map 
        unsigned short mapLength;	// size of map in elements (next field
                                        // is the element size.
        //unsigned char	mapDepth;	// width of map in bits 8 15,16,24,32
        unsigned short xOrigin;		// x-origin of image 
        unsigned short yOrigin;		// y-origin of image 
        unsigned short imageWidth;	// width of image 
        unsigned short imageHeight;	// height of image 
        unsigned char  pixelDepth;	// bits per pixel 
        unsigned char  imageDesc;	// image descriptor 
        //1.) bits 0-3 contain the number of attribute bits per pixel. (16, 
        //      and 32bit only) alpha channel, overlay or interrupt bits...
        //2.) bits 4,5 image origin location (0,0) of image. 0,0 is origin at 
        //      lower corner...
        //3.) bits 6,7 unused set to 0
    } TGAHeader;
    
    // Load an Uncompressed file
    bool loadUncompressed(Image *img, TGAHeader& tgaheader , File *fptr);
    // Load a Compressed file  
    bool loadCompressed(Image *img, TGAHeader& tgaheader, File *fptr);   
};


#endif // __VIDEO_IMAGE_H__
