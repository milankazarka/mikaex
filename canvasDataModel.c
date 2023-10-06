/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

/** non-finished implementation of a way to automatically load data
 *  objects into the canvas.
 *
 *  we should think of a better way
 */

#include "system.h"
#include "gui.h"
#include "xml.h"
#include "interface.h"
#include "global.h"

#define _CANVAS_DM_DATALEN 4096

#define _CANVAS_DM_NTYPES 1
enum {
  _CANVAS_DM_LBITEM
};

typedef struct _canvas_dm
{
  int			type;
  unsigned char		*name;
} _canvas_dm_lbitem;

struct _canvas_dm _canvas_dm_lst[1] = 
  {
    { _CANVAS_DM_LBITEM, (unsigned char*)"CANVAS_DM_LBITEM" }
  };

/**
 *  we interpret the data model into the canvas graphics
 *
 *  list member:
 *       0
 *      TYPE
 *       +--> type speciffic:
 *         _CANVAS_DM_LBITEM:
 *            1        2           3
 *          image, short_desc, long_desc
 */
int form_CanvasDataModelInitFromCSV(
  struct gui_Widget *w,
  unsigned char *filename
)
{
  static struct form_Canvas *canvas;
  static int lenList;
  static int lenListMem;
  static int n, nn, type;
  
  static unsigned char strType[_CANVAS_DM_DATALEN];
  static int strTypeLen;
  
  if (!w||!filename) return -1;

  canvas = w->form;

  w->dataModel = trop_DataModelGetFromCsv(w->dataModel,filename);
  
  if (!w->dataModel)
    return -1;

  lenList = trop_DataModelGetListSize(w->dataModel);

  #ifdef _DEBUG
    if (DEBUG==TRUE)
      printf(":form_CanvasDataModelInitFromCSV list size(%d)\n",lenList);
  #endif

  for(n = 0; n < lenList; n++)
  {
    lenListMem = trop_DataModelGetListMemberSize(w->dataModel,n);
    #ifdef _DEBUG
      if (DEBUG==TRUE)
        printf(":form_CanvasDataModelInitFromCSV list(%d) mems(%d)\n",n,lenListMem);
    #endif
    
    strTypeLen = 0;
    strType[0] = 0x00;
    
    if ( trop_DataModelGetDataAtPosition(
                                  w->dataModel,
                                  n, 0,
                                  _CANVAS_DM_DATALEN, &strType, &strTypeLen
                                  ) != 0 )
    {
      continue;
    }
    
    strType[strTypeLen] = 0x00;
    
    if (strTypeLen<1)
      continue;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":form_CanvasDataModelInitFromCSV DM TYPE (%s)\n",strType);
    #endif
    
    for (nn = 0; nn < _CANVAS_DM_NTYPES; nn++)
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) 
          printf(":form_CanvasDataModelInitFromCSV check types (%s) (%s)\n",
                                                      strType,
                                                      (char*)_canvas_dm_lst[nn].name);
      #endif
    
      if (
        strlen((char*)_canvas_dm_lst[nn].name) == strTypeLen
      )
      {
        if (
          strncmp((char*)_canvas_dm_lst[nn].name,(char*)strType,strTypeLen) == 0
        )
        {
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf(":form_CanvasDataModelInitFromCSV found type\n");
          #endif
          
        }
      }
    }
    
    for(nn = 0; nn < lenListMem; nn++)
    {
      
    }
  }

  return 0;
}
