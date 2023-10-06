/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "low.h"
#include "system.h" // todo, remove

#ifdef USEFT
  #include "freetype/fttrop.h"
  
  FTDemo_Display 	*ftdisplay  = NULL;
  FTDemo_Handle		*fthandle   = NULL;
  BOOL			ftb         = FALSE;
#endif

#define FLOOR( x )  (   (x)        & -64 )
#define CEIL( x )   ( ( (x) + 63 ) & -64 )
#define ROUND( x )  ( ( (x) + 32 ) & -64 )
#define TRUNC( x )  (   (x) >> 6 )

#define _MAX_GLYPHS 128

typedef union _low_font_color_
{
    long		value;
    unsigned char	chroma[4];
} _low_font_color;

typedef struct _low_font
{
    #ifdef USEFT
        FT_Error		error;
        FT_Encoding	        encoding;
        FT_Bitmap		bitmap;
        FT_Byte		        gamma_ramp[256];
        FT_Matrix		trans_matrix;
        FT_Library		library;
        FTC_Manager		cache_manager;
        FTC_ImageCache	        image_cache;
        FTC_SBitCache	        sbits_cache;
        FTC_CMapCache	        cmap_cache;
        PFont		        *fonts;
        PFont		        current_font;
        FTC_ScalerRec	        scaler;
        FT_ULong		load_flags;
        FT_Fixed		center;
        FT_Matrix		*matrix;
        TGlyph		        string[_MAX_GLYPHS];
        _low_font_color 	fore_color;
        _low_font_color 	back_color;
    #endif
  
    int			size;
    char		*name;
    
    int			render_mode;
    int			res;
    int			ptsize;
    double		gamma;
    int			angle;
    int			font_index;
    
    int			num_fonts;
    int			max_fonts;
    
    int			use_sbits_cache;
    
    int			hinted; 	// is glyph hinting active?
    int			antialias; 	// is antialiasing active?
    int			use_sbits;	// do we use embedded bitmaps?
    int			low_prec;	// force low precission
    int			autohint;	// force auto-hinting
    int			lcd_mode;
    int			preload;	// force font file preloading

    // string context
    int			kerning_mode;
    int			kerning_degree;
    int			vertical;
    
    int			string_length;
    
    int			w;
    int			h;
} _low_font;

static const char *file_suffixes[] = { ".ttf", ".ttc", ".otf", ".pfa", ".pfb", 0 };

FT_Error _get_size( struct _low_font *fontclx, FT_Size* asize );

/**
 *  helper function from the ft2_app sources
 */
static FT_Error _face_requester( FTC_FaceID  face_id,
                     FT_Library  lib,
                     FT_Pointer  request_data,
                     FT_Face*    aface )
{
    PFont  font = (PFont)face_id;

    FT_UNUSED( request_data );

    if ( font->file_address != NULL )
      error = FT_New_Memory_Face( lib,
                                  (const FT_Byte*)font->file_address,
                                  font->file_size,
                                  font->face_index,
                                  aface );
    else
      error = FT_New_Face( lib,
                           font->filepathname,
                           font->face_index,
                           aface );
    if ( !error )
    {
      char*  suffix;
      char   orig[4];


      suffix = strrchr( font->filepathname, '.' );
      if ( suffix && ( strcasecmp( suffix, ".pfa" ) == 0 ||
                       strcasecmp( suffix, ".pfb" ) == 0 ) )
      {
        suffix++;

        memcpy( orig, suffix, 4 );
        memcpy( suffix, "afm", 4 );
        FT_Attach_File( *aface, font->filepathname );

        memcpy( suffix, "pfm", 4 );
        FT_Attach_File( *aface, font->filepathname );
        memcpy( suffix, orig, 4 );
      }

      if ( (*aface)->charmaps )
        (*aface)->charmap = (*aface)->charmaps[font->cmap_index];
    }

    return error;
}

int _update_current_flags( struct _low_font *fontclx )
{
    FT_UInt32   flags, target;
      
    if (!fontclx) return -1;

    flags = FT_LOAD_DEFAULT;  /* really 0 */

    flags |= FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH;

    if ( fontclx->autohint )
      flags |= FT_LOAD_FORCE_AUTOHINT;

    if ( !fontclx->use_sbits )
      flags |= FT_LOAD_NO_BITMAP;

    if ( fontclx->hinted )
    {
      target = 0;

      if ( fontclx->antialias )
      {
        switch ( fontclx->lcd_mode )
        {
          case LCD_MODE_LIGHT:
            target = FT_LOAD_TARGET_LIGHT;
            break;

          case LCD_MODE_RGB:
          case LCD_MODE_BGR:
            target = FT_LOAD_TARGET_LCD;
            break;

          case LCD_MODE_VRGB:
          case LCD_MODE_VBGR:
            target = FT_LOAD_TARGET_LCD_V;
            break;

          default:
            target = FT_LOAD_TARGET_NORMAL;
        }
      }
      else
        target = FT_LOAD_TARGET_MONO;

      flags |= target;
    }
    else
      flags |= FT_LOAD_NO_HINTING;

    fontclx->load_flags    = flags;
  
    return 0;
}

FT_Error _install_font( struct _low_font *fontclx, const char *filepath )
{
    static char  filename[1024 + 5];
    int          i, len, num_faces;
    FT_Face      face;

    fontclx->num_fonts = 0;
    fontclx->max_fonts = 0;

    len = strlen( filepath );
    if ( len > 1024 )
      len = 1024;

    strncpy( filename, filepath, len );
    filename[len] = 0;

    error = FT_New_Face( fontclx->library, filename, 0, &face );

#ifndef macintosh
    
    if ( error )
    {
      const char**  suffix;
      char*         p;
      int           found = 0;

      suffix = file_suffixes;
      p      = filename + len - 1;

      while ( p >= filename && *p != '\\' && *p != '/' )
      {
        if ( *p == '.' )
          break;

        p--;
      }

      if ( p < filename || *p != '.' )
        p = filename + len;

      for ( suffix = file_suffixes; suffix[0]; suffix++ )
      {
        strcpy( p, suffix[0] );

        error = FT_New_Face( fontclx->library, filename, 0, &face );
        if ( !error )
        {
          found = 1;

          break;
        }
      }

      if ( !found )
        return error;
    }
#endif

    // allocate new font object
    num_faces = face->num_faces;
    for ( i = 0; i < num_faces; i++ )
    {
      PFont  font;

      if ( i > 0 )
      {
        error = FT_New_Face( fontclx->library, filename, i, &face );
        if ( error )
          continue;
      }

      if ( fontclx->encoding != FT_ENCODING_NONE )
      {
        error = FT_Select_Charmap( face, fontclx->encoding );
        if ( error )
        {
          FT_Done_Face( face );
          return error;
        }
      }

      font = (PFont)malloc( sizeof ( *font ) );

      font->filepathname = (char*)malloc( strlen( filename ) + 1 );
      strcpy( (char*)font->filepathname, filename );

      font->face_index = i;
      font->cmap_index = face->charmap ? FT_Get_Charmap_Index( face->charmap ) : 0;

      if ( fontclx->preload )
      {
        FILE*   file = fopen( filename, "rb" );
        size_t  file_size;

        if ( file == NULL )  // shouldn't happen
        {
          free( font );
          return FT_Err_Invalid_Argument;
        }

        fseek( file, 0, SEEK_END );
        file_size = ftell( file );
        fseek( file, 0, SEEK_SET );

        font->file_address = malloc( file_size );
        fread( font->file_address, 1, file_size, file );

        font->file_size    = file_size;

        fclose( file );
      }
      else
      {
        font->file_address = NULL;
        font->file_size    = 0;
      }

      switch ( fontclx->encoding )
      {
      case FT_ENCODING_NONE:
        font->num_indices = face->num_glyphs;
        break;

      case FT_ENCODING_UNICODE:
        font->num_indices = 0x110000L;
        break;

      case FT_ENCODING_ADOBE_LATIN_1:
      case FT_ENCODING_ADOBE_STANDARD:
      case FT_ENCODING_ADOBE_EXPERT:
      case FT_ENCODING_ADOBE_CUSTOM:
      case FT_ENCODING_APPLE_ROMAN:
        font->num_indices = 0x100L;
        break;

      // some fonts use range 0x00-0x100, others have 0xF000-0xF0FF
      case FT_ENCODING_MS_SYMBOL:
        font->num_indices = 0x10000L;

      default:
        font->num_indices = 0x10000L;
      }

      FT_Done_Face( face );
      face = NULL;

      if ( fontclx->max_fonts == 0 )
      {
        fontclx->max_fonts = 16;
        fontclx->fonts     = (PFont*)calloc( fontclx->max_fonts,
                                            sizeof ( PFont ) );
      }
      else if ( fontclx->num_fonts >= fontclx->max_fonts )
      {
        fontclx->max_fonts *= 2;
        fontclx->fonts      = (PFont*)realloc( fontclx->fonts,
                                              fontclx->max_fonts *
                                                sizeof ( PFont ) );

        memset( &fontclx->fonts[fontclx->num_fonts], 0,
                ( fontclx->max_fonts - fontclx->num_fonts ) *
                  sizeof ( PFont ) );
      }

      fontclx->fonts[fontclx->num_fonts++] = font;
      
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("set_font\n"); // todo, apply sanity
      #endif
      fontclx->current_font = font;
      fontclx->scaler.face_id = (FTC_FaceID)font;
    }
    return FT_Err_Ok;
}

int low_FontDelete( struct low_Font *_lowf )
{
  struct _low_font *low;
  
  if (!_lowf) return -1;
  low = _lowf->data;
  if (low->name)
  {
    free(low->name);
    low->name = NULL;
  }
  free(low);
  free(_lowf);
  return 0;
}
/*
 FTDemo_Display*
  FTDemo_Display_New( grPixelMode  mode )
  {
    FTDemo_Display*  display;
    grSurface*       surface;
    grBitmap         bit;


    display = (FTDemo_Display *)malloc( sizeof( FTDemo_Display ) );
    if ( !display )
      return NULL;

    if ( mode != gr_pixel_mode_gray  &&
         mode != gr_pixel_mode_rgb24 )
      return NULL;

    grInitDevices();

    bit.mode  = mode;
    bit.width = DIM_X;
    bit.rows  = DIM_Y;
    bit.grays = 256;

    surface = grNewSurface( 0, &bit );

    if ( !surface )
    {
      free( display );
      return NULL;
    }

    display->surface = surface;
    display->bitmap  = &surface->bitmap;

    grSetGlyphGamma( 1.0 );

    memset( &display->fore_color, 0, sizeof( grColor ) );
    memset( &display->back_color, 0xff, sizeof( grColor ) );

    return display;
  }
*/
struct low_Font *low_FontNew( char *filename, int size )
{
  struct low_Font   *lowf = (struct low_Font*)malloc(sizeof(struct low_Font));
  struct _low_font  *font;
  grBitmap          bit; 
  
  lowf->data = (struct _low_font*)malloc(sizeof(struct _low_font));
  font = lowf->data;
  
  // string context
  font->encoding	= FT_ENCODING_UNICODE;
  font->res		= 72;
  font->ptsize		= 48;
  font->gamma		= 2.0;
  font->angle		= 0;
  font->ptsize		= size*64;
  font->size		= size;
  
  font->error = FT_Init_FreeType(&font->library);
  if (font->error) printf("err: could not initialize FreeType\n");
  
  font->error = FTC_Manager_New(font->library,0,0,0,_face_requester,0,&font->cache_manager);
  if (font->error) printf("err: could not initialize cache manager\n");
  
  font->error = FTC_SBitCache_New(font->cache_manager,&font->sbits_cache);
  if (font->error) printf("err: could not initialize small bitmaps cache\n");
  
  font->error = FTC_ImageCache_New(font->cache_manager,&font->image_cache);
  if (font->error) printf("err: could not initialize glyph image cache\n");
  
  font->error = FTC_CMapCache_New(font->cache_manager,&font->cmap_cache);
  if (font->error) printf("err: could not initialize charmap cache");
  
  FT_Bitmap_New(&font->bitmap);
  
  font->hinted		= 1;
  font->antialias	= 1;
  font->use_sbits	= 0;
  font->use_sbits	= 0;
  font->autohint	= 0;
  font->lcd_mode	= 0;
  font->use_sbits_cache	= 1;
  
  memset(font->string, 0, sizeof(TGlyph)*_MAX_GLYPHS);
  _update_current_flags(font);
  font->error = _install_font(font,filename);
  
  if (font->error) printf("err: failed to install font\n");
  if (font->num_fonts == 0) printf("err: failed to open any font file\n");
  
  font->scaler.width 	= size*64;
  font->scaler.height 	= size*64;
  font->scaler.pixel 	= 0;
  font->scaler.x_res 	= 64;
  font->scaler.y_res 	= 64;
  
  font->fore_color.value = 200+(200*0xff)+(200*0xff*0xff);
  font->back_color.value = 200+(200*0xff)+(200*0xff*0xff);
  
  bit.mode  = gr_pixel_mode_rgb24;
  bit.width = 480;
  bit.rows  = 640;
  bit.grays = 256;

  //font->surface = grNewSurface( NULL, 0, &bit );
  
  return lowf;
}

static FT_Error _string_load( struct _low_font *fontclx )
{
    int        n;
    FT_Size    size;
    FT_Face    face;
    FT_Pos     prev_rsb_delta = 0;

    error = _get_size( fontclx, &size );
    if ( error )
      return error;

    face = size->face;

    for ( n = 0; n < fontclx->string_length; n++ )
    {
      PGlyph  glyph = fontclx->string + n;

      /* clear existing image if there is one */
      if ( glyph->image )
      {
        FT_Done_Glyph( glyph->image );
        glyph->image = NULL;
      }

      /* load the glyph and get the image */
      if ( !FT_Load_Glyph( face, glyph->glyph_index,
                           fontclx->load_flags )  &&
           !FT_Get_Glyph( face->glyph, &glyph->image ) )
      {
        FT_Glyph_Metrics*  metrics = &face->glyph->metrics;

        /* note that in vertical layout, y-positive goes downwards */

        glyph->vvector.x  = metrics->vertBearingX - metrics->horiBearingX;
        glyph->vvector.y  = -metrics->vertBearingY - metrics->horiBearingY;

        glyph->vadvance.x = 0;
        glyph->vadvance.y = -metrics->vertAdvance;

        if ( prev_rsb_delta - face->glyph->lsb_delta >= 32 )
          glyph->delta = -1 << 6;
        else if ( prev_rsb_delta - face->glyph->lsb_delta < -32 )
          glyph->delta = 1 << 6;
        else
          glyph->delta = 0;
      }
    }

    return FT_Err_Ok;
}

static void _gamma_ramp_apply( FT_Byte gamma_ramp[256], grBitmap *bitmap )
{
    int       i, j;
    FT_Byte*  p = (FT_Byte*)bitmap->buffer;

    if ( bitmap->pitch < 0 )
      p += -bitmap->pitch * ( bitmap->rows - 1 );

    for ( i = 0; i < bitmap->rows; i++ )
    {
      for ( j = 0; j < bitmap->width; j++ )
        p[j] = gamma_ramp[p[j]];

      p += bitmap->pitch;
    }
}

FT_Error _get_size( struct _low_font *fontclx, FT_Size* asize )
{
    FT_Size        size;

    error = FTC_Manager_LookupSize( fontclx->cache_manager, &fontclx->scaler, &size );

    if ( !error )
      *asize = size;

    return error;
}

FT_UInt _get_index( struct _low_font *fontclx, FT_UInt32 charcode )
{
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("face_id = %d\n",fontclx->scaler.face_id);
    #endif
    
    FTC_FaceID  face_id = fontclx->scaler.face_id;
    PFont       font    = fontclx->current_font;

    if (!font)
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("err:\n");
      #endif
    }

    return FTC_CMapCache_Lookup( fontclx->cmap_cache, face_id,
                                 font->cmap_index, charcode );
}

int _string_set( struct _low_font *fontclx, const char *string )
{
    const char*     p = string;
    const char*     end = p + strlen( string );
    unsigned long   codepoint;
    int             ch;
    int             expect;
    PGlyph          glyph = fontclx->string;

    fontclx->string_length = 0;
    codepoint = expect = 0;

    for (;;)
    {
      ch = utf8_next( &p, end );
      if ( ch < 0 )
        break;

      codepoint = ch;

      if ( fontclx->encoding != FT_ENCODING_NONE )
        glyph->glyph_index = _get_index( fontclx, codepoint );
      else
        glyph->glyph_index = codepoint;

      glyph++;
      fontclx->string_length++;

      if ( fontclx->string_length >= _MAX_GLYPHS )
        break;
    }
}

FT_Error _string_render_prepare( struct _low_font *fontclx, FT_Vector* advances )
{
    FT_Face     face;
    FT_Size     size;
    PGlyph      glyph;
    FT_Pos      track_kern   = 0;
    FT_UInt     prev_index   = 0;
    FT_Vector*  prev_advance = NULL;
    FT_Vector   extent       = {0, 0};
    FT_Int      i;


    error = _get_size( fontclx, &size );
    if ( error )
      return error;

    face = size->face;
    
    if ( !fontclx->vertical && fontclx->kerning_degree )
    {
      FT_Fixed  ptsize;

      ptsize = FT_MulFix( face->units_per_EM, face->size->metrics.x_scale );

      if ( FT_Get_Track_Kerning( face, ptsize << 10,
                                 -fontclx->kerning_degree,
                                 &track_kern ) )
        track_kern = 0;
      else
        track_kern >>= 10;
    }

    for ( i = 0; i < fontclx->string_length; i++ )
    {
      glyph = fontclx->string + i;

      if ( !glyph->image )
        continue;

      if ( fontclx->vertical )
        advances[i] = glyph->vadvance;
      else
      {
        advances[i] = glyph->image->advance;
        advances[i].x >>= 10;
        advances[i].y >>= 10;

        if ( prev_advance )
        {
          prev_advance->x += track_kern;

          if ( fontclx->kerning_mode )
          {
            FT_Vector  kern;


            FT_Get_Kerning( face, prev_index, glyph->glyph_index,
                FT_KERNING_UNFITTED, &kern );

            prev_advance->x += kern.x;
            prev_advance->y += kern.y;

            if ( fontclx->kerning_mode > KERNING_MODE_NORMAL )
              prev_advance->x += glyph->delta;
          }
        }
      }

      if ( prev_advance )
      {
        if ( fontclx->hinted )
        {
          prev_advance->x = ROUND( prev_advance->x );
          prev_advance->y = ROUND( prev_advance->y );
        }

        extent.x += prev_advance->x;
        extent.y += prev_advance->y;
      }

      prev_index   = glyph->glyph_index;
      prev_advance = advances + i;
    }

    if ( prev_advance )
    {
      if ( fontclx->hinted )
      {
        prev_advance->x = ROUND( prev_advance->x );
        prev_advance->y = ROUND( prev_advance->y );
      }

      extent.x += prev_advance->x;
      extent.y += prev_advance->y;
    }

    /* store the extent in the last slot */
    i = fontclx->string_length - 1;
    advances[i] = extent;

    return FT_Err_Ok;
}

FT_Error _glyph_to_bitmap( 
  struct _low_font      *fontclx,
  FT_Glyph 		glyf,
  grBitmap* 	        target,
  int* 			left,
  int* 			top,
  int*			x_advance,
  int*			y_advance,
  FT_Glyph*		aglyf 
)
{
    FT_BitmapGlyph  bitmap;
    FT_Bitmap*      source;

    #ifdef _DEBUG
      printf("_glyph_to_bitmap\n");
    #endif
    
    *aglyf = NULL;

    error = FT_Err_Ok;

    if ( glyf->format == FT_GLYPH_FORMAT_OUTLINE )
    {
      FT_Render_Mode  render_mode = FT_RENDER_MODE_MONO;

      if ( fontclx->antialias )
      {
        if ( fontclx->lcd_mode == 0 )		render_mode = FT_RENDER_MODE_NORMAL;
        else if ( fontclx->lcd_mode == 1 )	render_mode = FT_RENDER_MODE_LIGHT;
        else if ( fontclx->lcd_mode <= 3 )	render_mode = FT_RENDER_MODE_LCD;
        else					render_mode = FT_RENDER_MODE_LCD_V;
      }

      // render the glyph to a bitmap, don't destroy original
      error = FT_Glyph_To_Bitmap( &glyf, render_mode, NULL, 0 );
      if ( error )
      {
        printf("err: FT_Glypt_To_Bitmap\n");
        return error;
      }

      *aglyf = glyf;
    }

    if ( glyf->format != FT_GLYPH_FORMAT_BITMAP )
      printf( "err: invalid glyph format returned!" );

    bitmap = (FT_BitmapGlyph)glyf;
    source = &bitmap->bitmap;

    target->rows   = source->rows;
    target->width  = source->width;
    target->pitch  = source->pitch;
    target->buffer = source->buffer;
    target->grays  = source->num_grays;

    switch ( source->pixel_mode )
    {
    case FT_PIXEL_MODE_MONO:
      target->mode = gr_pixel_mode_mono;
      break;

    case FT_PIXEL_MODE_GRAY:
      target->mode  = gr_pixel_mode_gray;
      target->grays = source->num_grays;
      break;

    case FT_PIXEL_MODE_GRAY2:
    case FT_PIXEL_MODE_GRAY4:
      (void)FT_Bitmap_Convert( fontclx->library, source, &fontclx->bitmap, 1 );
      target->pitch  = fontclx->bitmap.pitch;
      target->buffer = fontclx->bitmap.buffer;
      target->mode   = gr_pixel_mode_gray;
      target->grays  = fontclx->bitmap.num_grays;
      break;

    case FT_PIXEL_MODE_LCD:
      target->mode  = fontclx->lcd_mode == 2 ? gr_pixel_mode_lcd
                                            : gr_pixel_mode_lcd2;
      target->grays = source->num_grays;
      break;

    case FT_PIXEL_MODE_LCD_V:
      target->mode  = fontclx->lcd_mode == 4 ? gr_pixel_mode_lcdv
                                            : gr_pixel_mode_lcdv2;
      target->grays = source->num_grays;
      break;

    default:
      return FT_Err_Invalid_Glyph_Format;
    }

    *left = bitmap->left;
    *top  = bitmap->top;

    *x_advance = ( glyf->advance.x + 0x8000 ) >> 16;
    *y_advance = ( glyf->advance.y + 0x8000 ) >> 16;

    return error;
}

int _list_bitmap( grBitmap *bm )
{
 int x;
 int y;
 
 #ifdef _DEBUG
   if (DEBUG==TRUE)
   {
     if (!bm) return -1;
 
     printf("_list_bitmap (rows(%d) width(%d))\n",bm->rows,bm->width);
 
     for(y = 0; y < bm->rows; y++)
     {
       printf("_list_bitmap bitmap_row ");
       for(x = 0; x < bm->width; x++)
       {
         printf("[%02x]",bm->buffer[(y*bm->width)+x]);
       }
       printf("\n");
     }
     return 0;
   }
 #endif 
}

/**
 *  draw a string to the main surface
 */
int low_FontDrawText( 
                struct low_Font *_lowf, 
                void *mysurface, 
                char *str, 
                int strlen, 
                int x, int y, int w, int h,
                int surfacew, int surfaceh )
{
    int                     n;
    FT_Vector               pen, advances[MAX_GLYPHS];
    FT_Size                 size;
    FT_Face                 face;
    struct _low_font        *fontclx;
    static grBitmap	    bmp;
    grColor                 grc;

    if (!_lowf||!str||strlen<=0) return -1;

    fontclx = (struct _low_font*)_lowf->data;
    
    _string_set(fontclx,str);
    
    error = _get_size( fontclx, &size );
    if ( error )
    {
      printf("err: get string size\n");
      return -1;
    }

    face = size->face;

    error = _string_load( fontclx );
    if ( error )
    {
      printf("err: string load\n");
      return error;
    }
    
    error = _string_render_prepare( fontclx, advances );
    if ( error )
    {
      printf("err: string render prepare\n");
      return error;
    }

    y = surfaceh - y;

    pen = advances[fontclx->string_length - 1];

    pen.x = FT_MulFix( pen.x, fontclx->center );
    pen.y = FT_MulFix( pen.y, fontclx->center );

    if ( fontclx->matrix && FT_IS_SCALABLE( face ) )
    {
      FT_Vector_Transform( &pen, fontclx->matrix );
      pen.x = ( x << 6 ) - pen.x;
      pen.y = ( y << 6 ) - pen.y;
    }
    else
    {
      pen.x = ROUND( ( x << 6 ) - pen.x );
      pen.y = ROUND( ( y << 6 ) - pen.y );
    }

    for ( n = 0; n < fontclx->string_length; n++ )
    {
      PGlyph    glyph = fontclx->string + n;
      FT_Glyph  image;
      FT_BBox   bbox;

      if ( !glyph->image )
        continue;

      error = FT_Glyph_Copy( glyph->image, &image );
      if ( error )
      {
        printf("err: FT_Glyph_Copy\n");
        continue;
      }

      if ( image->format != FT_GLYPH_FORMAT_BITMAP )
      {
        if ( fontclx->vertical )
          error = FT_Glyph_Transform( image, NULL, &glyph->vvector );

        if ( !error )
          error = FT_Glyph_Transform( image, fontclx->matrix, &pen );

        if ( error )
        {
          FT_Done_Glyph( image );
          continue;
        }
      }
      else
      {
        FT_BitmapGlyph  bitmap = (FT_BitmapGlyph)image;

        if ( fontclx->vertical )
        {
          bitmap->left += ( glyph->vvector.x + pen.x ) >> 6;
          bitmap->top  += ( glyph->vvector.x + pen.y ) >> 6;
        }
        else
        {
          bitmap->left += pen.x >> 6;
          bitmap->top  += pen.y >> 6;
        }
      }

      if ( fontclx->matrix )
        FT_Vector_Transform( advances + n, fontclx->matrix );

      pen.x += advances[n].x;
      pen.y += advances[n].y;

      FT_Glyph_Get_CBox( image, FT_GLYPH_BBOX_PIXELS, &bbox );

#if 0
      if ( n == 0 )
      {
        fprintf( stderr, "bbox = [%ld %ld %ld %ld]\n",
            bbox.xMin, bbox.yMin, bbox.xMax, bbox.yMax );
      }
#endif
        
      if ( bbox.xMax > 0 &&
           bbox.yMax > 0 &&
           bbox.xMin < surfacew &&
           bbox.yMin < surfaceh  )
      {
        int       	left, top, dummy1, dummy2;
        grBitmap  	bit3;
        FT_Glyph  	glyf;

        error = _glyph_to_bitmap( fontclx, image, &bit3, &left, &top,
                                  &dummy1, &dummy2, &glyf );
        _list_bitmap(&bit3);
        
        if ( !error )
        {
          if ( fontclx->gamma_ramp )
            _gamma_ramp_apply( fontclx->gamma_ramp, &bit3 );

          top = surfaceh - top;

          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("_ft draw_to_surface bit(%d)\n",bit3.mode);
          #endif
          /*if ( grBlitGlyphToBitmap( &fontclx->surface->bitmap, &bit3, left, top,
                                grc ) != 0 )
          {
            #ifdef _DEBUG
                if (DEBUG==TRUE) printf("_ft draw_to_surface failed\n");
            #endif
          }*/

          if ( glyf )
            FT_Done_Glyph( glyf );
        }
      }

      FT_Done_Glyph( image );
    }
    
    return 0;
}
