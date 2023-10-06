/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef __AGG_H__
#define __AGG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _POLY_EDGES	256
#define _POLY_DIR       "poly"

enum {
  GL_ROT_NONE,
  GL_ROT_LEFT,
  GL_ROT_RIGHT,
  GL_ROT_DOWN
};

typedef struct gui_GlColor
{
  unsigned char		rgba[4];
} gui_GlColor;

typedef struct gui_GlPoint
{
  unsigned char		used; // 0x00 0x01
  unsigned short int	x;    // after transformation
  unsigned short int	y;
  unsigned char         cx;   // point position w/o transformation
  unsigned char         cy;
} gui_GlPoint;

typedef struct gui_GlPolyTrans
{
  unsigned short        height;
  unsigned short        width;
  unsigned short        x;
  unsigned short        y;
  unsigned short        rotation; // GL_ROT_NONE, GL_ROT_LEFT, GL_ROT_RIGHT, GL_ROT_DOWN
} gui_GlPolyTrans;

typedef struct gui_GlPoly
{
  int			id;
  int			movex;
  int			movey;
  int			nedges;
  gui_GlPoint		edges[_POLY_EDGES];
  int			iedges[_POLY_EDGES*2];
  struct gui_GlColor	color;
  struct gui_GlPolyTrans
                        trans;
} gui_GlPoly;

typedef struct gui_GlPolyBuffer
{
  
} gui_GlPolyBuffer;

typedef struct gui_GlFont
{
  int			active; // 0,1
  int			width;
  int			height;
  int			origWidth;
  int			origHeight;
  struct gui_GlColor	color;
  double		angle;
} gui_GlFont;

typedef struct gui_Gl
{
  int			ctype;
  unsigned char		r_pos;
  unsigned char		g_pos;
  unsigned char		b_pos;
  unsigned char		a_pos;
  struct gui_GlColor	color;
} gui_Gl;

#ifdef __cplusplus
  extern "C" {
#endif

struct gui_Gl *gui_GlInit( struct gui_Gl *gl, void *surface, int w, int h );
int gui_GlSetColor( struct gui_Gl *gl, struct gui_GlColor *gc );
int gui_GlDrawRoundRect( struct gui_Gl *gl, int x1, int y1, int x2, int y2, int round );

struct gui_GlPoly *gui_GlPolyInit( struct gui_Gl *gl, struct gui_GlPoly *glpoly );
struct gui_GlPoly *gui_GlPolyInitFromFile( struct gui_Gl *gl, struct gui_GlPoly *glpoly, char *filename );
int gui_GlPolyAddEdge( struct gui_GlPoly *glpoly, int x, int y );
int gui_GlPolyAddProtoEdge( struct gui_GlPoly *glpoly, unsigned char cx, unsigned char cy );
int gui_GlPolyTransformProtoEdges( struct gui_GlPoly *glpoly );
int gui_GlPolyDraw( struct gui_Gl *gl, struct gui_GlPoly *glPoly );
struct gui_GlPolyBuffer *gui_GlPolyBufferInit( struct gui_GlPolyBuffer *pb );

int gui_GlColorSet( struct gui_Gl *gl, struct gui_GlColor *color, unsigned char r, unsigned char g, unsigned char b, unsigned char a );

struct gui_GlFont *gui_GlFontInit( struct gui_Gl *gl, struct gui_GlFont *gf, char *filename );
int gui_GlFontDraw( struct gui_Gl *gl, struct gui_GlFont *gf, char *text, int x1, int y1, int x2, int y2 );
int gui_GlFontGetSize( struct gui_Gl *gl, struct gui_GlFont *gf, char *text, int *w, int *h );

#ifdef __cplusplus
  }
#endif

#endif
