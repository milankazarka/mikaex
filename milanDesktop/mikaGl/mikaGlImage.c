#include "mikaGlImage.h"
#include "mikaGlConfig.h"

void __jpeg_cb_error_exit(j_common_ptr cinfo)
{
  printf("jpeg_error\n");
}

/** generate a mikaGlImage from a jpeg file
 */
mikaGlImage::mikaGlImage( unsigned char *filename )
{
    #ifdef _GLDEBUG
      printf("mikaGlImage::mikaGlImage\n");
    #endif
  
    struct jpeg_decompress_struct cinfo;
    struct jpeg_decompress_struct *ciptr;
    struct jpeg_error_mgr *emgr = NULL;
    FILE *fd = NULL;
    JSAMPLE *lb;
    ciptr = &cinfo;
      
    if ((fd=fopen((char*)filename,"rb"))==NULL)
    {
      printf("failed to open jpeg (%s)\n",(char*)filename);
    }
    else
    {
      #ifdef _GLDEBUG
        printf("opened '%s'\n",(char*)filename);
      #endif
    }
      
    emgr = (struct jpeg_error_mgr*)malloc(sizeof(struct jpeg_error_mgr));
    ciptr->err = jpeg_std_error(emgr);
    emgr->error_exit = __jpeg_cb_error_exit;
      
    jpeg_create_decompress(ciptr);
    jpeg_stdio_src(ciptr,fd);
    jpeg_read_header(ciptr,1);
    ciptr->out_color_space = JCS_RGB;
    jpeg_start_decompress(ciptr);

    static int h, w, cc, xs, yn, xn;
    static unsigned char pixel[4];
    pixel[0] = 0x10;
    pixel[1] = 0x10;
    pixel[2] = 0x10;
    pixel[3] = 0xff;
      
    w = ciptr->output_width;
    h = ciptr->output_height;
    cc = ciptr->output_components;
    mw = w;
    mh = h;

    pixels = (unsigned char*)malloc(mw*4*mh);
    for(yn = 0; yn < mh; yn++)
    {
      for(xn = 0; xn < mw; xn++)
      {
        memcpy(
          pixels+(yn*(mw*4))+(xn*4),
          (void*)&pixel,4
          );
      }
    }
      
    #ifdef _GLDEBUG
      printf("jpeg_image_attr h(%d) w(%d)\n",w,h);
    #endif
      
    lb = (JSAMPLE *)(*ciptr->mem->alloc_small)(
      (j_common_ptr)ciptr,
      JPOOL_PERMANENT,
      cc*w);
            
    yn = 0;
    while(ciptr->output_scanline < ciptr->output_height)
    {
      jpeg_read_scanlines(ciptr,&lb,1);
      for (xs = 0; xs < w; xs++)
      {
        pixel[2] = *(lb+3*xs);
        pixel[1] = *(lb+3*xs+1);
        pixel[0] = *(lb+3*xs+2);
            
        memcpy(
          pixels+(yn*(mw*4))+(xs*4),
          (void*)&pixel,4
          );
      }
      yn++;
    }
      
    jpeg_finish_decompress(ciptr);
    jpeg_destroy_decompress(ciptr);
      
    fclose(fd);
}

mikaGlImage::~mikaGlImage( )
{
  free(pixels);
}
