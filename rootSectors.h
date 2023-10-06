/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */
#ifndef _ROOT_SECTORS_H_
#define _ROOT_SECTORS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

enum  _ROOT_SECTORS {
  _ROOT_SECTOR_NONE,
  _ROOT_SECTOR_CENTER,
  _ROOT_SECTOR_LEFT,
  _ROOT_SECTOR_RIGHT,
  _ROOT_SECTOR_UP,
  _ROOT_SECTOR_DOWN,
  _ROOT_SECTOR_LEFTUP,
  _ROOT_SECTOR_LEFTDOWN,
  _ROOT_SECTOR_RIGHTUP,
  _ROOT_SECTOR_RIGHTDOWN
};

int rootSectorByPosition( int x, int y, int w, int h, int rootw, int rooth );

#endif
