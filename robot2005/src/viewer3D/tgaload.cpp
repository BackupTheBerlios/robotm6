
/*
   AUTHER You Yuanxia (Julia) from P.R.China

   RELEASE DATE 05/07/2001

   EMAIL yxyou@yahoo.com.cn, yxyou@hotmail.com, yxyou@263.net

   HOMEPAGE http://yxyou.home.sohu.com

   Aug 19th:
   Added support for runlength encoding - changed some stuff around to make this
   possible.  Works well :)

   Sept 7th:
   Improved error trapping and recovery.

   Oct 22nd:
   Major source clearout & Can compress the image using S3_TC algorithm if the 
   driver supports it.
  
   Nov 10th:
   'Settled' version of the code - traps for nearly all errors - added a 
   LoadAndBind function for even lazier people :)
   TGA_NO_PASS was added in case you need to load an image and pass it yourself.
   Finally exorcised all the paletted texture code...

   Jan 1rst:
   L. St-Marcel adds BMP load/bind
*/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "tgaload.h"

/* Extension Management */
#ifdef WIN32
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC  glCompressedTexImage2DARB  = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEARBPROC glGetCompressedTexImageARB = NULL;
#endif

/* Default support - lets be optimistic! */
bool tgaCompressedTexSupport = true;


void tgaGetExtensions ( void )
{  
#ifdef WIN32
   glCompressedTexImage2DARB  = ( PFNGLCOMPRESSEDTEXIMAGE2DARBPROC  ) 
                   wglGetProcAddress ( "glCompressedTexImage2DARB"  );
   glGetCompressedTexImageARB = ( PFNGLGETCOMPRESSEDTEXIMAGEARBPROC ) 
                   wglGetProcAddress ( "glGetCompressedTexImageARB" );
#endif
   /*   
      if ( glCompressedTexImage2DARB == NULL 
	  || glGetCompressedTexImageARB == NULL )
      tgaCompressedTexSupport = false;
   */
}


void tgaSetTexParams  ( unsigned int min_filter, unsigned int mag_filter, unsigned int application )
{
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter );

   glTexEnvf ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, (float)application );
}


unsigned char *tgaAllocMem ( tgaHeader_t info )
{
   unsigned char  *block;

   block = (unsigned char*) malloc ( info.bytes );

   if ( block == NULL )
      return 0;

   memset ( block, 0x00, info.bytes );

   return block;
}

void tgaPutPacketTuples ( image_t *p, unsigned char *temp_colour, int &current_byte )
{
   if ( p->info.components == 3 )
   {
      p->data[current_byte]   = temp_colour[2];
      p->data[current_byte+1] = temp_colour[1];
      p->data[current_byte+2] = temp_colour[0];
      current_byte += 3;
   }

   if ( p->info.components == 4 )    // Because its BGR(A) not (A)BGR :(
   {
      p->data[current_byte]   = temp_colour[2];
      p->data[current_byte+1] = temp_colour[1];
      p->data[current_byte+2] = temp_colour[0];
      p->data[current_byte+3] = temp_colour[3];
      current_byte += 4;
   }
}


void tgaGetAPacket ( int &current_byte, image_t *p, FILE *file )
{
   unsigned char  packet_header;
   int            run_length;
   unsigned char  temp_colour[4] = { 0x00, 0x00, 0x00, 0x00 };

   fread        ( &packet_header, ( sizeof ( unsigned char )), 1, file );
   run_length = ( packet_header&0x7F ) + 1;

   if ( packet_header&0x80 )  // RLE packet
   {
      fread ( temp_colour, ( sizeof ( unsigned char )* p->info.components ), 1, file );

      if ( p->info.components == 1 )  // Special optimised case :)
      {
         memset ( p->data + current_byte, temp_colour[0], run_length );
         current_byte += run_length;
      } else
      for ( int i = 0; i < run_length; i++ )
         tgaPutPacketTuples ( p, temp_colour, current_byte );
   }

   if ( !( packet_header&0x80 ))  // RAW packet
   {
      for ( int i = 0; i < run_length; i++ )
      {
         fread ( temp_colour, ( sizeof ( unsigned char )* p->info.components ), 1, file );

         if ( p->info.components == 1 )
         {
            memset ( p->data + current_byte, temp_colour[0], run_length );
            current_byte += run_length;
         } else
            tgaPutPacketTuples ( p, temp_colour, current_byte );
      }
   }
} 


void tgaGetPackets ( image_t *p, FILE *file )
{
  int current_byte = 0;

  while ( current_byte < p->info.bytes )
    tgaGetAPacket ( current_byte, p, file );
}


void tgaGetImageData ( image_t *p, FILE *file, unsigned int computeTransparency)
{
   unsigned char  temp;
   int  fileSize=p->info.bytes;
   unsigned char* data;
   data = new unsigned char[p->info.bytes];
   if(computeTransparency && p->info.tgaColourType == GL_RGB) {
		p->info.tgaColourType=GL_RGBA;
		p->info.bytes+=p->info.bytes/3+1;
		p->info.components=4;	
   }
   p->data = tgaAllocMem ( p->info );

   /* Easy unRLE image */
   if ( p->info.image_type == 1 || p->info.image_type == 2 || p->info.image_type == 3 )
   {   
      fread (data, sizeof (unsigned char), fileSize, file );

      /* Image is stored as BGR(A), make it RGB(A)     */
      if(computeTransparency) {
			int k=0;
			int i=0;
			for (i = 0; i < fileSize; i += p->info.components-1 )
			{
				p->data[k] = data[i + 2];
				p->data[k+1] = data[i + 1];
				p->data[k+2] = data[i];
				if(p->data[k] ==0 && p->data[k+1] ==0 && p->data[k+2] ==0)
					p->data[k+3]=0;
				else
					p->data[k+3]=255;
				k+=4;
			}
			k=4;
			// super effet flou sur les bords
			for (i = 3+3*p->info.width; i < fileSize-3-3*p->info.width; i += 3 )
			{
				int k2=k+p->info.width*4;
				int k3=k-p->info.width*4;
				if(p->data[k+3] ==255 ){
				if((p->data[k+4] ==0 && p->data[k+5] ==0 && p->data[k+6] ==0)
					||(p->data[k-4] ==0 && p->data[k-3] ==0 && p->data[k-2] ==0)
					||(p->data[k2] ==0 && p->data[k2+1] ==0 && p->data[k2+2] ==0)
					||(p->data[k3] ==0 && p->data[k3+1] ==0 && p->data[k3+2] ==0))
					p->data[k+3]=125;
				}
				k+=4;
			}
			delete[] data;
	 } else {
			memcpy(p->data,data,fileSize);
			delete[] data;
			for (int i = 0; i < fileSize; i += p->info.components )
			{
				temp = p->data[i];
				p->data[i] = p->data[i + 2];
				p->data[i + 2] = temp;
			}
		}
   }
   
   /* RLE compressed image */
   if ( p->info.image_type == 9 || p->info.image_type == 10 )
      tgaGetPackets ( p, file );
}


void tgaUploadImage ( image_t *p, tgaFLAG mode )
{
   /*  Determine TGA_LOWQUALITY  internal format
       This directs OpenGL to upload the textures at half the bit
       precision - saving memory
    */
   GLenum internal_format = p->info.tgaColourType;

   if ( mode&TGA_LOW_QUALITY )
   {
      switch ( p->info.tgaColourType )
      {
         case GL_RGB       : internal_format = GL_RGB4; break;
         case GL_RGBA      : internal_format = GL_RGBA4; break; 
         case GL_LUMINANCE : internal_format = GL_LUMINANCE4; break;
         case GL_ALPHA     : internal_format = GL_ALPHA4; break;          
      }
   }

   /*  Let OpenGL decide what the best compressed format is each case. */   
   if ( mode&TGA_COMPRESS && tgaCompressedTexSupport )
   {            
      switch ( p->info.tgaColourType )
      {
         case GL_RGB       : internal_format = GL_COMPRESSED_RGB_ARB; break;
         case GL_RGBA      : internal_format = GL_COMPRESSED_RGBA_ARB; break; 
         case GL_LUMINANCE : internal_format = GL_COMPRESSED_LUMINANCE_ARB; break; 
         case GL_ALPHA     : internal_format = GL_COMPRESSED_ALPHA_ARB; break;          
      }
   }                      
                        
   /*  Pass OpenGL Texture Image */
   if ( !( mode&TGA_NO_MIPMAPS ))
      gluBuild2DMipmaps ( GL_TEXTURE_2D, internal_format, p->info.width,
                          p->info.height, p->info.tgaColourType, GL_UNSIGNED_BYTE, p->data );
   else
      glTexImage2D ( GL_TEXTURE_2D, 0, internal_format, p->info.width,
                     p->info.height, 0, p->info.tgaColourType, GL_UNSIGNED_BYTE, p->data );  
}


void tgaFree ( image_t *p )
{
   if ( p->data != NULL )
      free ( p->data );
}


void tgaChecker ( image_t *p )
{
  unsigned char TGA_CHECKER[16384];
  unsigned char *pointer;

  // 8bit image
  p->info.image_type = 3;

  p->info.width  = 128;
  p->info.height = 128;

  p->info.pixel_depth = 8;

  // Set some stats
  p->info.components = 1;
  p->info.bytes      = p->info.width * p->info.height * p->info.components;

  pointer = TGA_CHECKER;

  for ( int j = 0; j < 128; j++ )
  {
    for ( int i = 0; i < 128; i++ )
    {
      if ((i ^ j) & 0x10 )
        pointer[0] = 0x00;
      else
        pointer[0] = 0xff;
      pointer ++;
    }
  }

  p->data = TGA_CHECKER;

  glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE4, p->info.width,
                 p->info.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, p->data );

/*  Should we free?  I dunno.  The scope of TGA_CHECKER _should_ be local, so it
    probably gets destroyed automatically when the function completes... */
//  tgaFree ( p );
}


void tgaError ( char *error_string, char *file_name, FILE *file, image_t *p )
{
   printf  ( "%s - %s\n", error_string, file_name );
   tgaFree ( p );

   fclose ( file );

   tgaChecker ( p );
}


void tgaGetImageHeader ( FILE *file, tgaHeader_t *info )
{
   /*   Stupid byte alignment means that we have to fread each field
        individually.  I tried splitting tgaHeader into 3 structures, no matter
        how you arrange them, colour_map_entry_size comes out as 2 bytes instead
        1 as it should be.  Grrr.  Gotta love optimising compilers - theres a pragma
        for Borland, but I dunno the number for MSVC or GCC :(
    */
   fread ( &info->id_length,       ( sizeof (unsigned char )), 1, file );
   fread ( &info->colour_map_type, ( sizeof (unsigned char )), 1, file );
   fread ( &info->image_type,      ( sizeof (unsigned char )), 1, file );

   fread ( &info->colour_map_first_entry, ( sizeof (short int )), 1, file );
   fread ( &info->colour_map_length     , ( sizeof (short int )), 1, file );
   fread ( &info->colour_map_entry_size , ( sizeof (unsigned char )), 1, file );

   fread ( &info->x_origin , ( sizeof (short int )), 1, file );
   fread ( &info->y_origin , ( sizeof (short int )), 1, file );
   fread ( &info->width,     ( sizeof (short int )), 1, file );
   fread ( &info->height,    ( sizeof (short int )), 1, file );

   fread ( &info->pixel_depth,     ( sizeof (unsigned char )), 1, file );
   fread ( &info->image_descriptor,( sizeof (unsigned char )), 1, file );

   // Set some stats
   info->components = info->pixel_depth / 8;
   info->bytes      = info->width * info->height * info->components;
}

int tgaLoadTheImage ( char *file_name, image_t *p, tgaFLAG mode, unsigned int computeTransparency)
{
   FILE   *file;

   tgaGetExtensions ( );

   if (( file = fopen ( file_name, "rb" )) == NULL )
   {
      tgaError ( "File not found", file_name, file, p );
      return 0;
   }

   tgaGetImageHeader ( file, &p->info );

   switch ( p->info.image_type )
   {
      case 1 :
         tgaError ( "8-bit colour no longer supported", file_name, file, p );
         break;

      case 2 :
         if ( p->info.pixel_depth == 24 )
            p->info.tgaColourType = GL_RGB;
         else if ( p->info.pixel_depth == 32 )
            p->info.tgaColourType = GL_RGBA;
                        else
                 tgaError ( "Unsupported RGB format", file_name, file, p );
         break;

      case 3 :
         if ( mode&TGA_LUMINANCE )
            p->info.tgaColourType = GL_LUMINANCE;
         else if ( mode&TGA_ALPHA )
            p->info.tgaColourType = GL_ALPHA;
        else
                 tgaError ( "Must be LUMINANCE or ALPHA greyscale", file_name, file, p );
         break;

      case 9 :
         tgaError ( "8-bit colour no longer supported", file_name, file, p );
         break;

      case 10 :
         if ( p->info.pixel_depth == 24 )
            p->info.tgaColourType = GL_RGB;
         else if ( p->info.pixel_depth == 32 )
            p->info.tgaColourType = GL_RGBA;
         else
                 tgaError ( "Unsupported compressed RGB format", file_name, file, p );
         break;
   }

   tgaGetImageData ( p, file, computeTransparency);

   fclose  ( file );
   return 1;
}

void tgaLoad ( char *file_name, image_t *p, tgaFLAG mode, unsigned int computeTransparency )
{
   tgaLoadTheImage ( file_name, p, mode,computeTransparency );

   if  ( !( mode&TGA_NO_PASS ))
      tgaUploadImage  ( p, mode );

   if ( mode&TGA_FREE )
     tgaFree ( p );

}

GLuint tgaLoadAndBind ( char *file_name, unsigned int mode, unsigned int computeTransparency)
{
   GLuint   texture_id;
   image_t  p;

   tgaLoadTheImage ( file_name, &p, mode, computeTransparency);

   glGenTextures ( 1, &texture_id );
   glBindTexture ( GL_TEXTURE_2D, texture_id );

   tgaUploadImage  ( &p, mode );
   tgaFree       ( &p );

   return texture_id;
}

// -----------------------------------------------------------------------------------------
// ReadBMPImage
// -----------------------------------------------------------------------------------------

unsigned char * ReadBMPImage(char *name, int *w, int *h)
{
  /*	BITMAPFILEHEADER fileheader;
	BITMAPINFOHEADER infoheader;
    unsigned char *  image;
    FILE            *image_in;
    int             components;

    if ( (image_in = fopen(name, "rb")) == NULL) { 
        return 0;
    }

    components = 1;

    if (components != 1)
        return 0;
	fseek(image_in, 0, SEEK_SET);
	fread(&fileheader,sizeof (BITMAPFILEHEADER), 1, image_in);
	fseek(image_in, sizeof(BITMAPFILEHEADER), SEEK_SET);
	fread(&infoheader, sizeof(BITMAPINFOHEADER), 1, image_in);

	*h = infoheader.biHeight;
	if (( infoheader.biWidth * infoheader.biBitCount ) % 32 ==0)
		*w = (infoheader.biWidth* infoheader.biBitCount)/24;
	else
		*w = (infoheader.biWidth* infoheader.biBitCount)/24  + 4;

	image = (GLubyte *)malloc(sizeof(unsigned char) * *w * *h * 3);
	if (infoheader.biBitCount == 24){
		fseek(image_in, fileheader.bfOffBits, SEEK_SET);
	   fread(image, BYTEOFCOLOR, *w * *h, image_in);
	}

	unsigned char tempRGB;
	int i;
	for(i=0;i<*w * *h -1; i++)
		{tempRGB=image[i*3], image[i*3]=image[i*3+2], image[i*3+2]=tempRGB;}

    fclose(image_in);  
    return image;
  */
  return NULL;
} // ReadBMPImage

// -----------------------------------------------------------------------------------------
// bmpLoadAndBind
// -----------------------------------------------------------------------------------------
GLuint bmpLoadAndBind(char *filename)
{
	GLubyte* tempimg;
	GLint w, h;
	GLuint texname;
	tempimg = ReadBMPImage(filename, &w, &h);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texname);
	glBindTexture(GL_TEXTURE_2D, texname);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, tempimg);

	GLenum error = glGetError ();

	if (error == GL_NO_ERROR){
		w = 0;
	}
	else{
		w = 0;
	}
	free (tempimg);
	return texname;
} // bmpLoadAndBind
