/*
 * Robotique E=M6 Eurobot, 2003
 * image.cpp
 * TGA functions for loading & saving images from/to TGA to/from an
 * Image buffer
 * 
 * @Author Benjamin Piette
 * @Date 15/11/02
 *
 */

#include <stdio.h>
#include "robotImage.h"
#include <cstdlib>
#include <memory.h>
#include "robotFile.h"

// ----------------------------------------------------------------------------
// Image::copyAndPow2
// ----------------------------------------------------------------------------
void Image::copyAndPow2(Image const& image)
{
  double xPow2 =0, yPow2 =0;
  int    ixPow2=0, iyPow2=0;
  int    xSize2=0, ySize2=0;
  
  if (image.width == 0 || image.height == 0) return;
  xPow2 = log((double)image.width) / log(2.0);
  yPow2 = log((double)image.height) / log(2.0);
  ixPow2 = (int)xPow2;
  iyPow2 = (int)yPow2;
  if (xPow2 != (double)ixPow2) ixPow2++;
  if (yPow2 != (double)iyPow2) iyPow2++;
  xSize2 = 1 << ixPow2;
  ySize2 = 1 << iyPow2;
  alloc(xSize2, ySize2);
  if ((width != image.width || height != image.height)
      && width!=0 && height!=0) {
    double scalex=(image.width/(double)width);
    double scaley=(image.height/(double)height);
    int wh=image.height*image.width;
    for(int j=0; j<(int)height; j++) {
      int u=(int)(j*scaley)*image.width;
      int v=j*width;
      for(int i=0; i<(int)width; i++) {
	data[v+i] = image.data[min(u+(int)(i*scalex), wh)];
      }
    }
  } else {
    memcpy(data, image.data, width*height*sizeof(Pixel));
  }
}

// ----------------------------------------------------------------------------
// TgaBitmap::save
// ----------------------------------------------------------------------------
// Save the parameter picture to a file , TGA format 
// ----------------------------------------------------------------------------
bool TgaBitmap::save(Image &img, 
		     const char *filename,
		     bool compress,
		     bool rotate,
		     double scale){
  File* fTGA=NULL;
  TGAHeader tgaheader;
  Pixel *temp_buffer=NULL;
  unsigned char tmp_color=0;
  int i=0, j=0,k=0;

  int newW=(int)(scale*img.width);
  int newH=(int)(scale*img.height);

  printf("Saving buffer to file %s%s%s\n", 
         KB_INFO, filename, KB_RESTORE);
  if (compress) fTGA = new ZFile();
  else fTGA = new File();
  if (!fTGA->open(filename, FILE_MODE_WRITE)) {
    printf("%sCannot save buffer to file%s\n", 
           KB_ERROR, KB_RESTORE);
    return false;
  }
  
  tgaheader.idLength    = 0;
  tgaheader.mapType     = 0;
  tgaheader.imageType   = 2;
  tgaheader.mapStart    = 0;
  tgaheader.mapLength   = 0;
  tgaheader.xOrigin     = 0;
  tgaheader.yOrigin     = 0;
  tgaheader.imageWidth  = newW;
  tgaheader.imageHeight = newH;
  tgaheader.pixelDepth  = 24;
  tgaheader.imageDesc   = 0x0001 << 5;  // Data is recorded from top to bottom 
                                      // AND no horizontal flipping
  
  fTGA->write((Byte*)(&tgaheader), sizeof(TGAHeader));



  temp_buffer = new Pixel[newW*newH];
  int maxSize=img.width*img.height;
  if (!rotate && scale==1.) {
    memcpy(temp_buffer, img.data, maxSize*3);
    for(i=0; i<maxSize; i++){
      tmp_color = temp_buffer[i].r;
      temp_buffer[i].r = temp_buffer[i].b;
      temp_buffer[i].b = tmp_color;
    }
  } else {
    k=0;
    int maxSize=img.width*img.height;
    if (rotate) {
      for(i=0; i<newH; i++){
	for(j=0; j<newW; j++){
	  k=(int)(i/scale)*img.width+(int)(j/scale);
	  if (k < 0) k=0;
	  else if (k >= maxSize) k = maxSize-1;
	  temp_buffer[i*img.width+j].r = img.data[k].b;
	  temp_buffer[i*img.width+j].g = img.data[k].g;
	  temp_buffer[i*img.width+j].b = img.data[k].r;
	}
      }
    } else {
      for(i=newH-1; i>=0; i--){
	for(j=newW-1; j>=0; j--){
	  k=(int)(i/scale)*img.width+(int)(j/scale);
	  if (k < 0) k=0;
	  else if (k >= maxSize) k = maxSize-1;
	  temp_buffer[i*img.width+j].r = img.data[k].b;
	  temp_buffer[i*img.width+j].g = img.data[k].g;
	  temp_buffer[i*img.width+j].b = img.data[k].r;
	}
      }
    }
  }

  fTGA->write((Byte*)temp_buffer, sizeof(Pixel)*newW*newH);

  delete[] temp_buffer;

  delete fTGA;
  return true;
}


// ----------------------------------------------------------------------------
// TgaBitmap::load
// ----------------------------------------------------------------------------
// load a TGA picture
// ----------------------------------------------------------------------------
bool TgaBitmap::load(Image &img, const char * filename, bool compressed)
{
  File * fTGA = NULL;
  if (!compressed)
    fTGA = new File();
  else
    fTGA = new ZFile();
  
  if(!fTGA->open(filename, FILE_MODE_READ)) {
      printf("%sCannot open image file %s %s\n", 
             KB_ERROR, filename, KB_RESTORE);
      delete fTGA;
      return false;
  }

  TGAHeader tgaheader;
  if(fTGA->read((Byte*)(&tgaheader), sizeof(TGAHeader)) == 0) {
      printf("%sUnable to read TGA header%s\n",
             KB_ERROR, KB_RESTORE);
      if(fTGA != NULL){
          delete(fTGA);
      }
      return false;
  }

  if (tgaheader.imageType == 2) {
      if (!loadUncompressed(&img, tgaheader, fTGA)) {
          delete(fTGA);
          return false;
      }
  } else if (tgaheader.imageType == 10) {
      if (!loadCompressed(&img, tgaheader, fTGA)) {
          delete(fTGA);
          return false;
      }
  } else {
      printf("%sIncorrect TGA file format%s\n",
             KB_ERROR, KB_RESTORE);
      delete(fTGA);
      return false;
  }
  printf("%sSuccessfuly read Tga file: %s %s\n", 
         KB_OK, filename, KB_RESTORE);  
  delete(fTGA);
  return true;
}

// ----------------------------------------------------------------------------
// TgaBitmap::loadUncompressed
// ----------------------------------------------------------------------------
// load a TGA picture
// ----------------------------------------------------------------------------
bool TgaBitmap::loadUncompressed(Image *    texture, 
                                 TGAHeader& tgaheader, 
                                 File *     fTGA)
{
  unsigned int cswap=0;
  unsigned int direction=0;
  Pixel *p=NULL;
  bool upsideDown=0, horizontalFlip=0;

  // Is image data written from top to bottom or from bottom to top?
  upsideDown = (((tgaheader.imageDesc & (0x0001<<5)) == (0x0001<<5))?true:false);
  
  // Are the rows flipped?
  horizontalFlip = (((tgaheader.imageDesc & (0x0001<<4)) == (0x0001<<4))?true:false);
//  printf("horiz flip: %s\n",(horizontalFlip?"true":"false"));
  
  if((tgaheader.imageWidth <= 0) 
     || (tgaheader.imageHeight <= 0) 
     || ((tgaheader.pixelDepth != 24))) {
      printf("%sInvalid texture information or image not in 24-bits format%s\n",
             KB_ERROR, KB_RESTORE);
      return false;
  }
  
  texture->alloc(tgaheader.imageWidth,
                 tgaheader.imageHeight);
  
  if(texture->data == NULL) {
      printf("%sMemory allocation for Image failed%s\n",
             KB_ERROR, KB_RESTORE);
      return false;
  }


  if (upsideDown) {
      p = texture->data;
      direction = tgaheader.imageWidth;
  } else {
      p = texture->data + tgaheader.imageWidth*(tgaheader.imageHeight-1);
      direction = -tgaheader.imageWidth;
  }

  while(fTGA->read((Byte*)p, tgaheader.imageWidth*sizeof(Pixel))>0) {
      p+=direction;
  }

  for(cswap = 0; 
      cswap < (unsigned int)tgaheader.imageWidth*tgaheader.imageHeight ; 
      cswap++) {
      texture->data[cswap].r ^= texture->data[cswap].b ^=
          texture->data[cswap].r ^= texture->data[cswap].b;
  }

  return true;
}


// ----------------------------------------------------------------------------
// TgaBitmap::loadCompressedTGA
// ----------------------------------------------------------------------------
// load a TGA picture
// ----------------------------------------------------------------------------
bool TgaBitmap::loadCompressed(Image *    texture, 
                               TGAHeader& tgaheader, 
                               File *     fTGA)
{ 
  unsigned int pixelcount=0;
  unsigned int currentpixel=0;
  unsigned int currentbyte=0;
  unsigned char colorbuffer[3]={0,0,0};
  short counter=0;
    
  if((tgaheader.imageWidth <= 0) 
     || (tgaheader.imageHeight <= 0) 
     || ((tgaheader.pixelDepth != 24))) {
      printf("%sInvalid texture information or image not in 24-bits format%s\n",
             KB_ERROR, KB_RESTORE);
      return false;
  }

  texture->alloc(tgaheader.imageWidth,
                 tgaheader.imageHeight);
  
  pixelcount	= tgaheader.imageHeight * tgaheader.imageWidth;
  currentpixel	= 0;
  currentbyte	= 0;

  do {
      unsigned char chunkheader = 0;
      if (fTGA->read(&chunkheader, sizeof(unsigned char)) <= 0) {
	  printf("%sCannot reader RLE Header%s\n",
                 KB_ERROR, KB_RESTORE);
	  return false;
      }
      
      if(chunkheader < 128) {
	  chunkheader++;
	  for(counter = 0; counter < chunkheader; counter++) {
	      if(fTGA->read(colorbuffer, 3) != 3) {
		  printf("Cannot read image data\n");
		  return false;	
		}

	      // write to memory
	      texture->data[currentpixel].r = colorbuffer[2];
	      texture->data[currentpixel].g = colorbuffer[1];
	      texture->data[currentpixel].b = colorbuffer[0];

	      currentbyte += tgaheader.pixelDepth/8;
	      currentpixel++;
          }
      } else {
	  chunkheader -= 127;
	  if (fTGA->read(colorbuffer, 3) != 3) {	
	      printf("Cannot read from file\n");
	      return false;
          }
	  
	  for(counter = 0; counter < chunkheader; counter++) {			
	      texture->data[currentpixel].r = colorbuffer[2];
	      texture->data[currentpixel].g = colorbuffer[1];
	      texture->data[currentpixel].b = colorbuffer[0];
	      currentbyte += tgaheader.pixelDepth/8;
	      currentpixel++;		
          }
      }
  } while(currentpixel < pixelcount);
  return true;	
}

// ----------------------------------------------------------------------------
// BmpBitmap::load
// ----------------------------------------------------------------------------
// load a BMP picture
// ----------------------------------------------------------------------------
bool BmpBitmap::load(Image &img, const char * filename, bool compress)
{
  int i=0,j=0,c=0;
  File * fBMP = NULL;
  if (compress) fBMP = new ZFile();
  else fBMP = new File();
  if (!fBMP->open(filename, FILE_MODE_READ)) {
    printf("%sCannot load file %s %s\n", 
           KB_ERROR, filename, KB_RESTORE);
    return false;
  }
 
  for(i=0;i<54;i++){
    Byte  data[3];
    fBMP->read(data, 1);
    c=data[0];
    //largeur
    if(i==18) img.width=c;
    if(i==19) img.width+=c*256;
    //hauteur
    if(i==22) img.height=c;
    if(i==23) img.height+=c*256;
  }
  img.alloc(img.width, img.height);
  for(i=img.height-1; i>=0; i--){
    Byte  data[3];
    for(j=img.width-1; j>=0; j--){
      fBMP->read(data, 3);
      img.data[img.width*i+j].b = data[0];
      img.data[img.width*i+j].g = data[1];
      img.data[img.width*i+j].r = data[2];
    }/*GLubyte*/ 
    for(j=0; j<(int)((4-(img.width*3)%4)%4); j++){
      fBMP->read(data, 1);
    }
  }
  fBMP->close();
  return true;
}

static const Byte BMP_HEADER[54]={
 66, 77, 54, 96, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 40, 0, 0, 0, 128, 0, 0, 0, 64, 0, 0, 0, 1, 0, 24, 0, 0, 0, 0, 0, 0, 96, 0, 0, 19, 11, 0, 0, 19, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

// ----------------------------------------------------------------------------
// BmpBitmap::save
// ----------------------------------------------------------------------------
// save a BMP picture
// ----------------------------------------------------------------------------
bool BmpBitmap::save(Image &img, 
		     const char * filename, 
		     bool   compressed,
		     bool   rotate,
		     double scale)
{
  int i=0,j=0;
  if(scale<=0) scale=1;
  int newW=(int)(img.width*scale);
  int newH=(int)(img.height*scale);
  File * fBMP = NULL;
  if (compressed) fBMP = new ZFile();
  else fBMP = new File();
  if (!fBMP->open(filename, FILE_MODE_WRITE)) {
    printf("%sCannot save file %s %s\n", 
           KB_ERROR, filename, KB_RESTORE);
    return false;
  }
  Byte data[54];
  memcpy(data, BMP_HEADER, 54);
  data[18]=newW%256;
  data[19]=newW/256;
  data[22]=newH%256;
  data[23]=newH/256;
  fBMP->write(data, 54);
  int k=0;
  int maxSize=img.width*img.height;
  if (rotate) {
    for(i=0; i<newH; i++){
      for(j=0; j<newW; j++){
	k=(int)(i/scale)*img.width+(int)(j/scale);
	if (k < 0) k=0;
	else if (k >= maxSize) k = maxSize-1;
	data[0] = img.data[k].b;
	data[1] = img.data[k].g;
	data[2] = img.data[k].r;
	fBMP->write(data, 3);
      }
      for(j=0; j<(newW*3)%4; j++){
	fBMP->write(data, 1);
      }
    }
  } else {
    for(i=newH-1; i>=0; i--){
      for(j=0; j<newW; j++){
	k=(int)(i/scale)*img.width+(int)(j/scale);
	if (k < 0) k=0;
	else if (k >= maxSize) k = maxSize-1;
	data[0] = img.data[k].b;
	data[1] = img.data[k].g;
	data[2] = img.data[k].r;
	fBMP->write(data, 3);
      }
      for(j=0; j<(int)((4-(newW*3)%4)%4); j++){
	fBMP->write(data, 1);
      }
    }
  }
  fBMP->close();
  return true;
}
