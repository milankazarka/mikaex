#ifndef _ROOT_DRAW_H_
#define _ROOT_DRAW_H_

#include <stdio.h>
#include <cairo/cairo.h>
#include "rootFrame.h"
#include "rootGc.h"

#ifdef __cplusplus
  extern "C"
#endif
int rootDrawDrawControlCircle( cairo_t *cairo, struct rootFrame *frame );
#ifdef __cplusplus
  extern "C"
#endif
int rootDrawDrawWindowBorder( cairo_t *cairo, struct rootFrame *frame );
#ifdef __cplusplus
  extern "C"
#endif
int rootDrawDrawRoundRect( cairo_t *cairo, struct rootFrame *frame, int round );
#ifdef __cplusplus
  extern "C"
#endif
int rootDrawDrawRoundRectGlow( cairo_t *cairo, struct rootFrame *frame, int round );
#ifdef __cplusplus
  extern "C"
#endif
int rootDrawDrawControlCircleGC( cairo_t *cairo, struct rootGc *gc, struct rootFrame *frame );

#endif
