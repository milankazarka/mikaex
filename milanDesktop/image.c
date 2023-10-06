#include "image.h"
#include "config.h"

milanImage::milanImage( unsigned char *filename )
{
  #ifdef _DEBUG
    printf("milanImage::milanImage\n");
  #endif
  mx = 0;
  my = 0;
  glImage = new mikaGlImage(filename);
  if (!glImage)
  {
      #ifdef _DEBUG
          printf("milanImage::milanImage error, couldn't create glImage\n");
      #endif
  }
}

milanImage::~milanImage( )
{
    delete glImage;
}
