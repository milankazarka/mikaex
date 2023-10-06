/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

/** rootSectors handles areas of the rootController - this is mainly 
 *  used when you like to poll if an object is in a speciffic area
 *
 *  todo - possibility to register areas dynamically
 */

#include "rootSectors.h"
#include "config.h"

#define _USE_COMBINED_NO
#define _USE_TABLECONFIG
int rootSectorByPosition( int x, int y, int w, int h, int rootw, int rooth )
{
  #ifdef _DEBUG
    printf("rootSectorByPosition\n");
  #endif
  
  int sector = _ROOT_SECTOR_NONE;
  
  double lowborder = 0.4;
  double highborder = 0.6;
  
  int centerx = x+(w/2);
  int centery = y+(h/2);
  
  if ( 	
    centerx<rootw*lowborder 
    #ifndef _USE_TABLECONFIG
      &&
      centery>rooth*lowborder && centery<rooth*highborder
    #endif
    )
    sector = _ROOT_SECTOR_LEFT;
  #ifdef _USE_COMBINED
    else if (	
      centerx<rootw*lowborder &&
      centery<rooth*lowborder
      )
      sector = _ROOT_SECTOR_LEFTUP;
    else if (
      centerx<rootw*lowborder &&
      centery>rooth*highborder
      )
      sector = _ROOT_SECTOR_LEFTDOWN;
  #endif
  else if (
    centerx>rootw*lowborder && centerx<rootw*highborder &&
    #ifdef _USE_TABLECONFIG
      centery<rooth*0.5
    #else
      centery<rooth*lowborder
    #endif
    )
    sector = _ROOT_SECTOR_UP;
  else if (
    centerx>rootw*lowborder && centerx<rootw*highborder &&
    #ifdef _USE_TABLECONFIG
      centery>rooth*0.5
    #else
      centery>rooth*highborder
    #endif
    )
    sector = _ROOT_SECTOR_DOWN;
  else if (
    centerx>rootw*highborder 
    #ifndef _USE_TABLECONFIG
      &&
      centery>rooth*lowborder && centery<rooth*highborder
    #endif
    )
    sector = _ROOT_SECTOR_RIGHT;
  #ifdef _USE_COMBINED
    else if (
      centerx>rootw*highborder &&
      centery<rooth*lowborder
      )
      sector = _ROOT_SECTOR_RIGHTUP;
    else if (
      centerx>rootw*highborder &&
      centery>rooth*highborder
      )
      sector = _ROOT_SECTOR_RIGHTDOWN;
  #endif
  else
    sector = _ROOT_SECTOR_CENTER;
  
  return sector;
}
