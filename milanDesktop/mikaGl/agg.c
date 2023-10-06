/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <iterator>
#include <list>
#include <vector>

#include "libAgg/include/agg_basics.h"
#include "libAgg/include/agg_rendering_buffer.h"
#include "libAgg/include/agg_image_accessors.h"
#include "libAgg/include/agg_span_image_filter_rgba.h"
#include "libAgg/include/agg_span_image_filter_rgb.h"
#include "libAgg/include/agg_rasterizer_scanline_aa.h"
#include "libAgg/include/agg_scanline_p.h"
#include "libAgg/include/agg_scanline_u.h"
#include "libAgg/include/agg_scanline_bin.h"
#include "libAgg/include/agg_renderer_scanline.h"
#include "libAgg/include/agg_renderer_primitives.h"
#include "libAgg/include/agg_renderer_outline_aa.h"
#include "libAgg/include/agg_rasterizer_scanline_aa.h"
#include "libAgg/include/agg_rasterizer_outline_aa.h"
#include "libAgg/include/agg_rendering_buffer.h"
#include "libAgg/include/agg_pixfmt_rgb.h"
#include "libAgg/include/agg_pixfmt_rgba.h"
#include "libAgg/include/agg_pixfmt_rgb_packed.h"
#include "libAgg/include/agg_ellipse.h"
#include "libAgg/include/agg_rounded_rect.h"
#include "libAgg/include/agg_conv_stroke.h"
#include "libAgg/include/agg_color_rgba.h"
#include "libAgg/include/agg_gamma_lut.h"
#include "libAgg/include/agg_path_storage.h"
#include "libAgg/include/agg_conv_clip_polyline.h"
#include "libAgg/include/agg_conv_curve.h"
#include "libAgg/include/agg_conv_contour.h"
#include "libAgg/include/agg_font_freetype.h"
#include "libAgg/include/agg_font_cache_manager.h"
#include "libAgg/include/agg_conv_transform.h"
#include "libAgg/include/agg_glyph_raster_bin.h"
#include "libAgg/include/agg_span_interpolator_linear.h"

#include "agg.h"
//#include "config.h"

#define _STATIC_NO


using namespace std;

template <class T>
struct GlBase
{
  typedef agg::renderer_base<T> _render_base;
  
  typedef agg::renderer_scanline_aa_solid<_render_base>		_ren_scanaa;
  typedef agg::renderer_scanline_bin_solid<_render_base>	_ren_scanbin;
  typedef agg::rasterizer_scanline_aa<agg::rasterizer_sl_clip_int> _ras_scanline;
  typedef agg::renderer_outline_aa<_render_base> 		_ren_linetype;
  typedef agg::rasterizer_outline_aa<_ren_linetype> 		_ras_linetype;
  typedef agg::renderer_primitives<_render_base> 		_render_prime;
  // freetype rendering through agg:
  typedef agg::font_engine_freetype_int32                       font_engine_type;
  typedef agg::font_cache_manager<font_engine_type>             font_manager_type;
  typedef agg::conv_curve<font_manager_type::path_adaptor_type> font_curves;
  typedef agg::conv_contour<agg::conv_curve<font_manager_type::path_adaptor_type> > font_contour;
  
  T *__pix;
  
  agg::rendering_buffer	*__buf;
  agg::int8u		*__surface;
  
  _render_base		*__base;
  _ren_scanaa		*__ren_scanaa;
  _ren_scanbin		*__ren_scanbin;
  _ras_scanline		__ras;
  agg::line_profile_aa	*__line_profile;
  _ren_linetype		*__ren_linetype;
  _ras_linetype		*__ras_linetype;
  
  _render_prime		*__prime;
  
  agg::scanline_u8	antialias;
  agg::scanline_bin	alias;
  
  font_engine_type             *m_feng;
  font_manager_type            *m_fman;
  
  font_curves                  *m_curves;
  font_contour                 *m_contour;
  
  void Init( agg::int8u *surface, int w, int h ) 
  {
    __surface = surface;
    __buf = new agg::rendering_buffer(surface,w,h,w*4);	if (!__buf) printf("agg:: buffer error\n");
    __pix = new T(*__buf);				if (!__pix) printf("agg:: pixfmt error\n");
    __base = new _render_base(*__pix);			if (!__base) printf("agg:: base error\n");
    __ren_scanaa = new _ren_scanaa(*__base);		if (!__ren_scanaa) printf("agg:: ren_scanaa error\n");
    __ren_scanbin = new _ren_scanbin(*__base);		if (!__ren_scanbin) printf("agg:: ren_scanbin error\n");
    __line_profile = new agg::line_profile_aa();	if (!__line_profile) printf("agg:: line_profile error\n");
    
    __line_profile->smoother_width(0);
    __line_profile->min_width(0);
    
    __ren_linetype = new _ren_linetype(*__base,*__line_profile);	if (!__ren_linetype) printf("agg:: ren_linetype error\n");
    __ras_linetype = new _ras_linetype(*__ren_linetype);		if (!__ras_linetype) printf("agg:: ras_linetype error\n");
    
    m_feng = new font_engine_type();
    m_fman = new font_manager_type(*m_feng);
    
    m_curves  = new font_curves(m_fman->path_adaptor());
    m_contour = new font_contour(*m_curves);
    
    m_curves->approximation_scale(10.0);
    m_contour->auto_detect_orientation(false);
    m_contour->width(3);
    
    __ras.clip_box(0,0,w,h);
  }
};

GlBase<agg::pixfmt_rgba32> glbase32;
GlBase<agg::pixfmt_rgb565> glbase565;

int gui_GlColorSet(
  struct gui_Gl *gl, 
  struct gui_GlColor *glc, 
  unsigned char r, 
  unsigned char g, 
  unsigned char b, 
  unsigned char a 
)
{
  if (!glc||r<0||g<0||b<0||a<0) return -1;
  glc->rgba[gl->r_pos] = r;
  glc->rgba[gl->g_pos] = g;
  glc->rgba[gl->b_pos] = b;
  glc->rgba[gl->a_pos] = a;
  return 0;
}

struct gui_Gl *gui_GlInit( struct gui_Gl *gl, void *surface, int w, int h )
{
  if (!gl)
    gl = (struct gui_Gl*)malloc(sizeof(struct gui_Gl));
  gl->ctype = -1;
  
  gl->r_pos = 2;
  gl->g_pos = 1;
  gl->b_pos = 0;
  gl->a_pos = 3;
  
  glbase32.Init((agg::int8u*)surface,w,h);
  
  // test - move to external
  //gui_GlFont *glf = gui_GlFontInit(gl,NULL,(char*)"fonts/xsuni.ttf");
  //gui_GlFont *glf = gui_GlFontInit(gl,NULL,(char*)"fonts/arenaBold.ttf");
  return gl;
}

int gui_GlSetColor( struct gui_Gl *gl, struct gui_GlColor *gc )
{
  if (!gl||!gc) return -1;
  
  gl->color.rgba[gl->r_pos] = gc->rgba[gl->r_pos];
  gl->color.rgba[gl->g_pos] = gc->rgba[gl->g_pos];
  gl->color.rgba[gl->b_pos] = gc->rgba[gl->b_pos];
  gl->color.rgba[gl->a_pos] = gc->rgba[gl->a_pos];
  
  return 0;
}

#define _TEST_ROTATION_NO
int gui_GlDrawRoundRect(
  struct gui_Gl *gl,
  int x1, int y1,
  int x2, int y2,
  int round
)
{
  if (!gl) return -1;
  
  agg::trans_affine mtx;
  agg::trans_affine mtx2;
  
  #ifdef _TEST_ROTATION
    mtx *= agg::trans_affine_rotation(agg::deg2rad(7.0));
  #endif
  
  agg::rounded_rect rrect( x1, y1, x2, y2, round<<1 );
  agg::conv_transform<agg::rounded_rect> trans(rrect,mtx);
  agg::conv_curve<agg::rounded_rect> vs(rrect);

  // mika - I do not know how to do it differently
  //
  agg::path_storage path;
  path.concat_path(vs);
  path.transform(mtx);

  glbase32.__ras.add_path(path);
  
  
  // mika - test source transformation
  //
  //
  mtx2 *= agg::trans_affine_rotation(agg::deg2rad(4.0));
  //glbase32.__base->clear(agg::rgba8(0,0,0));
  typedef agg::span_interpolator_linear<> interpolator_type;
  interpolator_type interpolator(mtx2);
  
  agg::row_ptr_cache<agg::int8u> buf(glbase32.__surface,640,480,640*4);
  //agg::pixfmt_rgba32 img_pixf(glbase32.__surface);
  agg::pixfmt_rgba32 pixf(*glbase32.__buf);
  
  typedef agg::span_image_filter_rgb_bilinear_clip<agg::pixfmt_rgba32, 
    interpolator_type> span_gen_type;
  span_gen_type sg(pixf,agg::rgba(0,0.4,0,0.5),interpolator);  
  
  glbase32.__ren_scanaa->color(
    agg::rgba8(
      gl->color.rgba[0],
      gl->color.rgba[1],
      gl->color.rgba[2],
      gl->color.rgba[3]
    ));
  
  agg::render_scanlines(glbase32.__ras,glbase32.antialias,*glbase32.__ren_scanaa);
  
  return 0;
}

/**
 *  draw text using agg and freetype
 */
int gui_GlFontDraw( 
  struct gui_Gl *gl, 
  struct gui_GlFont *gf, 
  char *text,
  int x1, int y1,
  int x2, int y2 
  )
{
  unsigned 	num_glyphs;
  char 		*p;
  
  double 	x = x1;
  double 	y = y2;
  
  if (!gl||!gf) return -1;
  if (gf->active==0) return -1;

  num_glyphs = 0;
  p = text;
  
  agg::glyph_rendering gren = agg::glyph_ren_agg_gray8;
  agg::trans_affine mtx;
  mtx *= agg::trans_affine_rotation(agg::deg2rad(gf->angle));
  glbase32.m_feng->transform(mtx);
  
  glbase32.m_feng->hinting(true);
  glbase32.m_feng->height(gf->width);
  glbase32.m_feng->width(gf->height);
  glbase32.m_feng->flip_y(true);
  
  glbase32.__ren_scanaa->color(
    agg::rgba8(
        gf->color.rgba[0],
        gf->color.rgba[1],
        gf->color.rgba[2],
        gf->color.rgba[3]
      )
    );
  
  while(*p)
  {
    const agg::glyph_cache* glyph = glbase32.m_fman->glyph(*p);
    if(glyph)
    {
        //glbase32.m_fman->add_kerning(&x, &y);
        
        glbase32.m_fman->init_embedded_adaptors(glyph, x, y);
        
        switch(glyph->data_type)
        {
          case agg::glyph_data_mono:
            #ifdef _DEBUG
              printf("font glyph_data_mono\n");
            #endif
            printf("glyph_unsupported\n");
            break;  
          case agg::glyph_data_gray8:
            #ifdef _DEBUG
              printf("font glyph_data_gray8\n");
            #endif
            agg::render_scanlines(
                                  glbase32.m_fman->gray8_adaptor(), 
                                  glbase32.m_fman->gray8_scanline(), 
                                  *glbase32.__ren_scanaa
                                  );
            break;
          case agg::glyph_data_outline:
            #ifdef _DEBUG
              printf("font glyph_data_outline\n");
            #endif
            printf("glyph_unsupported\n");
            break;
          default:
            #ifdef _DEBUG
              printf("font default\n");
            #endif
            printf("glyph_unsupported\n");
            break;
        }
        x += glyph->advance_x;
        y += glyph->advance_y;
    }
    #ifdef _DEBUG
      printf("glyph_advance(%d,%d)\n",glyph->advance_x,glyph->advance_y);
    #endif
    ++num_glyphs;
    ++p;
  }
  
  return 0;
}

/**
 *  get the size of a text string
 */
int gui_GlFontGetSize( struct gui_Gl *gl, struct gui_GlFont *gf, char *text, int *w, int *h )
{
  unsigned 	num_glyphs;
  char 		*p;
  
  if (!gl||!gf) return -1;
  if (gf->active==0) return -1;

  num_glyphs = 0;
  p = text;
  
  agg::glyph_rendering gren = agg::glyph_ren_agg_gray8;
  agg::trans_affine mtx;
  mtx *= agg::trans_affine_rotation(agg::deg2rad(gf->angle));
  glbase32.m_feng->transform(mtx);
  
  glbase32.m_feng->hinting(true);
  glbase32.m_feng->height(gf->width);
  glbase32.m_feng->width(gf->height);
  glbase32.m_feng->flip_y(true);
  
  *w = 0;
  *h = 0;
  
  while(*p)
  {
    const agg::glyph_cache* glyph = glbase32.m_fman->glyph(*p);
    if(glyph)
    {
      *w += glyph->advance_x;
      *h += glyph->advance_y;
    }
    ++num_glyphs;
    ++p;
  }
  
  return 0;
}

struct gui_GlFont *gui_GlFontInit( struct gui_Gl *gl, struct gui_GlFont *gf, char *filename )
{
  if (!gl||!filename) return NULL;
  if (!gf) gf = (struct gui_GlFont*)malloc(sizeof(struct gui_GlFont));
  
  gf->active = 0;
  gf->angle = 0;
  gf->color.rgba[0] = 255;
  gf->color.rgba[1] = 255;
  gf->color.rgba[2] = 255;
  gf->color.rgba[3] = 255;
  gf->height = 20;
  gf->width = 20;
  
  agg::glyph_rendering gren = agg::glyph_ren_agg_gray8;
  
  if(glbase32.m_feng->load_font(filename, 0, gren))
  {
    gf->active = 1;
  
    glbase32.m_feng->hinting(true);
    glbase32.m_feng->height(gf->height);
    glbase32.m_feng->width(gf->width);
    glbase32.m_feng->flip_y(false);
  }
  else
  {
    printf("could not load font '%s'\n",filename);
  }
  
  return gf;
}

struct gui_GlPoly *_gui_glpoly_init_super(
                                    struct gui_Gl *gl,
                                    struct gui_GlPoly *glpoly,
                                    char *filename
                                    )
{
  int           n;
  FILE          *fd;
  char          *complete_filename;
  static char   polyline[256];
  int           edges;
  char          edge[4];
  unsigned char cedge;
  unsigned char cedgex;
  unsigned char cedgey;
  unsigned short int
                mode;
  
  if (!gl) return NULL;
  if (!glpoly)
    glpoly = (struct gui_GlPoly*)malloc(sizeof(struct gui_GlPoly));
  for(n = 0; n < _POLY_EDGES; n++)
  {
    glpoly->edges[n].used = 0x00;
  }
  glpoly->movex         = 0;
  glpoly->movey         = 0;
  glpoly->nedges        = 0;
   
  glpoly->trans.width   = 0;
  glpoly->trans.height  = 0;
  glpoly->trans.x       = 0;
  glpoly->trans.y       = 0;
  glpoly->trans.rotation = GL_ROT_NONE;
    
  if (filename)
  {
    complete_filename = (char*)malloc(strlen(_POLY_DIR)+strlen(filename)+2);
    sprintf(complete_filename,"%s/%s",_POLY_DIR,filename);
    if ((fd = fopen((char*)complete_filename,"r"))!=NULL)
    {
      while(1)
      {
        fgets((char*)polyline,255,fd);
        if (polyline[0]!=0x00)
        {
          if (polyline[strlen((char*)polyline)-1]=='\n')
            polyline[strlen((char*)polyline)-1] = 0x00;
          edges = strlen((char*)polyline)/5;
          
          mode = 0;
          for(n = 0; n < edges; n++)
          {
            memcpy((void*)edge,(char*)polyline+(n*5)+1,3);
            edge[3] = 0x00;
            cedge = atoi((char*)edge);  
            switch(mode)
            {
              case 0:
                cedgex = cedge;
                mode++;
              break;
              case 1:
                cedgey = cedge;
                mode--;
                gui_GlPolyAddProtoEdge(glpoly,cedgex,cedgey);
              break;
            }
          }
          
        }
        if (feof(fd)) break;
      }
      fclose(fd);
    }
    free(complete_filename);
  }
    
  return glpoly;
}


struct gui_GlPoly *gui_GlPolyInit( struct gui_Gl *gl, struct gui_GlPoly *glpoly )
{
  return _gui_glpoly_init_super(gl,glpoly,NULL);
}

/**
 *  ./poly/file
 *  [000][000][255][000][255][255][000][000]
 *  defines a square with edges in the corners in the polygon matrix
*/
struct gui_GlPoly *gui_GlPolyInitFromFile( struct gui_Gl *gl, struct gui_GlPoly *glpoly, char *filename )
{
  return _gui_glpoly_init_super(gl,glpoly,filename);
}

int gui_GlPolyTransformProtoEdges( struct gui_GlPoly *glpoly )
{
  int n;
  
  if (!glpoly) return -1;
    
  for(n = 0; n < glpoly->nedges; n++)
  {
    glpoly->iedges[n*2] =
      (int)((float)((float)glpoly->trans.width/255.0)*glpoly->edges[n].cx)+
      glpoly->trans.x;
    glpoly->iedges[(n*2)+1] =
      (int)((float)((float)glpoly->trans.height/255.0)*glpoly->edges[n].cy)+
      glpoly->trans.y;
    #ifdef _DEBUG
      printf(":proto_edge (%d)(%d)\n",glpoly->iedges[n*2],glpoly->iedges[(n*2)+1]);
    #endif
  }
    
  return 0;
}

int gui_GlPolyAddProtoEdge(
                           struct gui_GlPoly *glpoly,
                           unsigned char cx,
                           unsigned char cy
                           )
{
  if (!glpoly) return -1;
  if (glpoly->nedges==_POLY_EDGES) return -1;
  if (glpoly->edges[glpoly->nedges].used!=0x00) return -1;
      
  glpoly->edges[glpoly->nedges].x = 0;
  glpoly->edges[glpoly->nedges].y = 0;
  glpoly->iedges[glpoly->nedges*2] = 0;
  glpoly->iedges[(glpoly->nedges*2)+1] = 0;
  glpoly->edges[glpoly->nedges].used = 0x01;
  
  glpoly->edges[glpoly->nedges].cx = cx;
  glpoly->edges[glpoly->nedges].cy = cy;

      
  glpoly->nedges++;
  
  return 0;
}

int gui_GlPolyAddEdge( struct gui_GlPoly *glpoly, int x, int y )
{
  if (!glpoly) return -1;
  if (glpoly->nedges==_POLY_EDGES) return -1;
  if (glpoly->edges[glpoly->nedges].used!=0x00) return -1;
      
  glpoly->edges[glpoly->nedges].x = x;
  glpoly->edges[glpoly->nedges].y = y;
  glpoly->iedges[glpoly->nedges*2] = x;
  glpoly->iedges[(glpoly->nedges*2)+1] = y;
  glpoly->edges[glpoly->nedges].used = 0x01;
      
  glpoly->nedges++;
  
  return 0;
}

int gui_GlPolyDraw( struct gui_Gl *gl, struct gui_GlPoly *glpoly )
{ 
  if (!gl||!glpoly) return -1;
      
  glbase32.__ren_scanaa->color(
    agg::rgba8(
      glpoly->color.rgba[0],
      glpoly->color.rgba[1],
      glpoly->color.rgba[2],
      glpoly->color.rgba[3]
    ));
  
  agg::poly_plain_adaptor<int> vs((int*)glpoly->iedges,glpoly->nedges,false);
  glbase32.__ras.add_path(vs);
  agg::render_scanlines(glbase32.__ras,glbase32.antialias,*glbase32.__ren_scanaa);
  
  return 0;
}

struct gui_GlPolyBuffer *gui_GlPolyBufferInit( struct gui_GlPolyBuffer *pb )
{
  return pb;
}

#ifdef _STATIC
  int main( )
  {
    return 0;
  }
#endif
