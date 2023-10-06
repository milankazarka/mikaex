#include "system.h"

#define FLOOR( x )  (   (x)        & -64 )
#define CEIL( x )   ( ( (x) + 63 ) & -64 )
#define ROUND( x )  ( ( (x) + 32 ) & -64 )
#define TRUNC( x )  (   (x) >> 6 )

static const char *file_suffixes[] = { ".ttf", ".ttc", ".otf", ".pfa", ".pfb", 0 };

typedef struct _font_bitmap
{
 int		rows;
 int		width;
 int		pitch;
 int		grays;
 int		mode;
 unsigned char	*buffer;
} _font_bitmap;

FT_Error _get_size( struct gui_FontCLX *fontclx, FT_Size* asize );

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

int _update_current_flags( struct gui_FontCLX *fontclx )
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

FT_Error _install_font( struct gui_FontCLX *fontclx, const char *filepath )
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

int gui_FontCLXDelete( struct gui_FontCLX *font )
{
  if (!font) return -1;
  if (font->name)
  {
    free(font->name);
    font->name = NULL;
  }
  free(font);
  return 0;
}

struct gui_FontCLX *gui_FontCLXNew( char *filename, int size )
{
  struct gui_FontCLX *font = (struct gui_FontCLX*)malloc(sizeof(struct gui_FontCLX));
  // string context
  //font->render_mode 	= RENDER_MODE_STRING;
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
  
  return font;
}

static FT_Error _string_load( struct gui_FontCLX *fontclx )
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

static void _gamma_ramp_apply( FT_Byte gamma_ramp[256], _font_bitmap *bitmap )
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

FT_Error _get_size( struct gui_FontCLX *fontclx, FT_Size* asize )
{
    FT_Size        size;

    error = FTC_Manager_LookupSize( fontclx->cache_manager, &fontclx->scaler, &size );

    if ( !error )
      *asize = size;

    return error;
}

FT_UInt _get_index( struct gui_FontCLX *fontclx, FT_UInt32 charcode )
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

int _string_set( struct gui_FontCLX *fontclx, const char *string )
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

FT_Error _string_render_prepare( struct gui_FontCLX *fontclx, FT_Vector* advances )
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
  struct gui_FontCLX* 	fontclx,
  FT_Glyph 		glyf,
  _font_bitmap* 	target,
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

    //if ( glyf->format == FT_GLYPH_FORMAT_OUTLINE )
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

    target->mode   = source->pixel_mode;

    *left = bitmap->left;
    *top  = bitmap->top;

    *x_advance = ( glyf->advance.x + 0x8000 ) >> 16;
    *y_advance = ( glyf->advance.y + 0x8000 ) >> 16;

    return error;
}

int _list_bitmap( struct _font_bitmap *bm )
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
 *  blit a glyph to the selected surface
 */
int gui_FontCLXDrawToPixbuf( struct _font_bitmap *glyph, grPos x, grPos y )
{
  int nx;
  int ny;
  long pixel;
  char component;
  
  if (!glyph) return -1;
  
  for(ny = 0; ny < glyph->rows; ny++)
  {
    for(nx = 0; nx < glyph->width; nx++)
    {
      component = glyph->buffer[(ny*glyph->width)+nx];
      if (component==0x00) continue;
      //component = 0xff-component;
      pixel = component;
      pixel += component*0xff;
      pixel += component*0xff*0xff;
      
      pixel = 0xac+(0xac*256)+(0xac+256*256);
      
      XPutPixel(gui_GetMain()->surface,nx,ny,pixel);
      int c;
      for(c==0;c<4;c++)
      {
     //   gui_GetMain()->_surface->data[(ny*gui_GetMain()->attr.w*4)+(nx*4)+c] = component;
      }
    }
  }
  
  return 0;
}

/**
 *  draw a string to the main surface
 */
int gui_FontCLXDrawText( struct gui_FontCLX *fontclx, void *surface, char *str, int strlen, int x, int y, int w, int h,
			 int surfacew, int surfaceh )
{
    int        n;
    FT_Vector  pen, advances[MAX_GLYPHS];
    FT_Size    size;
    FT_Face    face;

    if (!fontclx||!str||strlen<=0) return -1;
    
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

    // change to Cartesian coordinates 
    y = surfaceh - y;

    // get the extent, which we store in the last slot
    pen = advances[fontclx->string_length - 1];

    pen.x = FT_MulFix( pen.x, fontclx->center );
    pen.y = FT_MulFix( pen.y, fontclx->center );

    // get pen position
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

      // copy image
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

#if 1
      if ( n == 0 )
      {
        fprintf( stderr, "bbox = [%ld %ld %ld %ld]\n",
            bbox.xMin, bbox.yMin, bbox.xMax, bbox.yMax );
      }
#endif

      // check bounding box; if it is completely outside the 
      // display surface, we don't need to render it         
      if ( bbox.xMax > 0 &&
           bbox.yMax > 0 &&
           bbox.xMin < surfacew &&
           bbox.yMin < surfaceh  )
      {
        int       	left, top, dummy1, dummy2;
        _font_bitmap  	bit3;
        FT_Glyph  	glyf;


        error = _glyph_to_bitmap( fontclx, image, &bit3, &left, &top,
                                  &dummy1, &dummy2, &glyf );
        _list_bitmap(&bit3);
        
        if ( !error )
        {
          //if ( fontclx->gamma_ramp )
          //  _gamma_ramp_apply( fontclx->gamma_ramp, &bit3 );

          // change back to the usual coordinates
          top = surfaceh - top;

          gui_FontCLXDrawToPixbuf( &bit3, left, top );

          return 0;

          // now render the bitmap into the display surface
          //grBlitGlyphToBitmap( display->bitmap, &bit3, left, top,
          //                     display->fore_color );

          if ( glyf )
            FT_Done_Glyph( glyf );
        }
      }

      FT_Done_Glyph( image );
    }
    
    return 0;
}
