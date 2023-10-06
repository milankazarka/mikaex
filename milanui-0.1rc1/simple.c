/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "system.h"
#include "gui.h"
#include "xml.h"

  int form_SimpleInit( struct gui_Widget *w )
  {
    struct form_Simple *fsimple = NULL;
    
    if (!w) return -1;
    
    w->attr.form = FORM_SIMPLE;
    
    fsimple = (struct form_Simple*)malloc(sizeof(struct form_Simple));
    fsimple->pimage = NULL;
    fsimple->name[0] = 0x00;
    fsimple->long_desc[0] = 0x00;
    fsimple->short_desc[0] = 0x00;
    
    return 0;
  }
  
  int form_SimpleRelease( struct gui_Widget *w )
  {
    return 0;
  }
  
  int form_SimpleConfigure( 
    struct gui_Widget *w,
    char *image_filename,
    char *name,
    char *long_desc,
    char *short_desc 
  )
  {
    static struct form_Simple *fsimple = NULL;
    
    if (!w) return -1;
    
    fsimple = w->form;
    
    if (name) 		strcpy((char*)fsimple->name,name);
    if (long_desc) 	strcpy((char*)fsimple->long_desc,long_desc);
    if (short_desc)	strcpy((char*)fsimple->short_desc,short_desc);
    
    if (image_filename)
    {
      fsimple->pimage = &fsimple->image;
      strcpy((char*)fsimple->pimage->filename,image_filename);
      if ( gui_CreateImage(gui_GetMain(),fsimple->pimage) != 0 )
      {
        printf("Couldn't register image (%s)\n",image_filename);
        fsimple->pimage = NULL;
      }
    }
      
    return 0;
  }

