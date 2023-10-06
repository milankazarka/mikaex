#ifndef _ROOT_POINTER_H_
#define _ROOT_POINTER_H_

#include "rootController.h"

int rootPointerDrawESYNC( struct rootDesktop *rd, cairo_t *cairo, int x, int y );
int rootPointerSetActive( struct rootDesktop *rd );
int rootPointerSetInactive( struct rootDesktop *rd );

#endif
