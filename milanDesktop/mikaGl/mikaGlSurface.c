#include "mikaGlSurface.h"
#include "agg.h"

mikaGlSurface::mikaGlSurface( int w, int h )
{
  mw = w;
  mh = h;
  pixels = (unsigned char*)malloc(mw*4*mh);
  if (!pixels)
  {
    #ifdef _GLDEBUG
      printf("mikaGlSurface::mikaGlSurface error, couldn't create surface\n");
    #endif
  }
  
  int nx, ny;
  for(ny = 0; ny < h; ny++)
  {
    for(nx = 0; nx < w; nx++)
    {
      memset(pixels+(ny*(w*4))+(nx*4),0xff,3);
      memset(pixels+(ny*(w*4))+(nx*4)+3,0xff,1);
    }
  }
}

mikaGlSurface::~mikaGlSurface( )
{
}

