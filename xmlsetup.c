/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "system.h"
#include "gui.h"
#include "global.h"
#include "xml.h"

  int __read_file_content( 
    FILE 	*fd, 
    char 	*line, 
    char 	*content 
    )
  {
    int linelen = 0;
    int pos = 0;
    while(1)
    {
      if ( fgets((char*)line,255,fd) != NULL )
      {
        linelen = strlen((char*)line);
        line[linelen] = 0x00;
        if (content[0]==0x00)
          strcpy((char*)content,(char*)line);
        else
          strcat((char*)content,(char*)line);
        pos+=linelen;
      }
      else
        break;
    }
    return 0;
  }
  
  /*
    canvas setup through xml 
  */
  int _gui_setup_canvas_init( struct gui_Widget *can, char *filename )
  {
    FILE	*fdsetup		= NULL;
    static char content[65536];
    static char line[256];
    int		linelen			= 0;
    static int	x, y, w, h, round 	= 0;
    static int	stat 			= 0;	// in canvas we can have static objects
    
    struct xml_Inst	xml;
    struct xml_Object	*o 	= NULL;
    struct xml_Object	*oo	= NULL;
    struct xml_Bit	*b 	= NULL;
    
    struct gui_Widget	*wi	= NULL;
    
    if ((fdsetup=fopen(filename,"r"))==NULL)
    {
      printf("Could not open canvas setup (%s)\n",(char*)filename);
      return -1;
    }

    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("_gui_canvas_setup_init\n");
    #endif
    
    xml.data 	= NULL;
    content[0] 	= 0x00;
    
    __read_file_content(fdsetup,(char*)content,(char*)line);
    
    fclose(fdsetup);
    
    xml.filename[0] = 0x00;
    xml.data = (void*)content;
    xml_Init(&xml);
    
    o = xml.obj;
    while(o!=NULL)
    {
      oo = xml_GetObject(&xml,(char*)o->name);
      if (strlen(oo->name)>=strlen("widget"))
      {
        if (strncmp(oo->name,"widget",6)==0)
        {
          wi = gui_NewWidget();
          printf("xmlsetup create Widget\n");
          
          x = y = w = h = round = 0;
          stat = 0;
            
          b = xml_GetObjectBit(oo,"x");	if (b!=NULL) x = atoi((char*)b->value);
          b = xml_GetObjectBit(oo,"y");	if (b!=NULL) y = atoi((char*)b->value);
          b = xml_GetObjectBit(oo,"w");	if (b!=NULL) w = atoi((char*)b->value);
          b = xml_GetObjectBit(oo,"h");	if (b!=NULL) h = atoi((char*)b->value);
          b = xml_GetObjectBit(oo,"round");	
            if (b!=NULL) round = atoi((char*)b->value);
          
          wi->attr.x = x;
          wi->attr.y = y;
          wi->attr.w = w;
          wi->attr.h = h;
          
          if (round>0)
            gui_WidgetSetRound(wi,round);
          
          gui_WidgetInit(wi,gui_GetMain());
          gui_WidgetMap(wi);
          
          b = xml_GetObjectBit(oo,"color");
          if (b!=NULL) 
          {
            gui_WidgetSetColor(wi,(char*)b->value);
          }
          
          b = xml_GetObjectBit(oo,"static");
          if (b!=NULL)
          {
            stat = atoi((char*)b->value);
          }
          
          b = xml_GetObjectBit(oo,"form");
          if (strcmp((char*)b->value,"button")==0)
          {
            wi->attr.form = FORM_BUTTON;
            #ifdef _DEBUG
              if (DEBUG==TRUE) printf("\txml button setup (%d %d %d %d)\n",w,h,x,y);
            #endif
            if (stat==0) 	form_CanvasAddObject(can,wi);
            else if (stat==1) 	form_CanvasAddObjectStatic(can,wi);
          }
          else
          {
            // todo - cleanup
          }
        }
      }
      o = o->next;
    }
    
    return 0;
  }
  
  int _gui_setup_object_get_position(
    struct xml_Object *oo,
    int *x, int *y,
    int *w, int *h
  )
  {
    struct xml_Bit *b = NULL;
  
    if (!oo||!x||!y||!w||!h) return -1;
    
    *x = *y = *w = *h = -1;
            
    b = xml_GetObjectBit(oo,"x");	if (b!=NULL) *x = atoi((char*)b->value);
    b = xml_GetObjectBit(oo,"y");	if (b!=NULL) *y = atoi((char*)b->value);
    b = xml_GetObjectBit(oo,"w");	if (b!=NULL) *w = atoi((char*)b->value);
    b = xml_GetObjectBit(oo,"h");	if (b!=NULL) *h = atoi((char*)b->value);
    
    if (*x==-1||*y==-1||*w==-1||*h==-1) return -1;
    
    return 0;
  }
  
  int _gui_setup_toplevel_init(
    int x, int y,
    int w, int h 
  )
  {
    struct gui_Widget *main = NULL;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("_gui_setup_toplevel_init\n");
    #endif
    
    main = gui_NewWidget();
    main->attr.x = x;
    main->attr.y = y;
    main->attr.w = w;
    main->attr.h = h;
    main->attr.form = -1;
    if ( gui_WidgetInit(main,NULL) != 0 )
    {
      printf("err: could not initialize toplevel window\n");
      return -1;
    }
    return 0;
  }
  
  struct gui_Layout *gui_LayoutInitEx(
    struct gui_Layout *_layout,
    char *name
  )
  {
    FILE	*fdlayout = NULL;
    char	filename[128];
    char	content[65536];
    char	line[256];
    char	value[128];
    int		linelen = 0;
    int		x, y, w, h;
    return _layout;
  }
  
  struct gui_Layout *gui_LayoutInit( struct gui_Layout *_layout, char *name )
  {
    FILE 	*fdlayout = NULL;
    char 	filename[128];
    char 	content[65536];
    char 	line[256];
    char	value[128];
    int		linelen = 0;
    int		x, y, w, h;
    
    struct xml_Inst	xml;
    struct xml_Object	*o 	= NULL;
    struct xml_Object	*oo	= NULL;
    struct xml_Bit	*b 	= NULL;
    
    struct gui_Widget	*wi	= NULL;
    struct gui_Image	*im	= NULL;
    
    if (_layout==NULL)
    {
      _layout = (struct gui_Layout*)malloc(sizeof(struct gui_Layout));
    }
    _layout->parent = NULL;
    _layout->links = NULL;
    _layout->widgets = NULL;
    
    filename[0] = 0x00;
    if ( layout==NULL || strncmp(name,"main",4)==0 )
    {
      memcpy((void*)filename,"scene/main.xml",14);
      filename[14] = 0x00;
    }
    else
    {
      sprintf((char*)filename,"scene/%s.xml",name);
    }
    printf("Opening (%s)\n",(char*)filename);
    if ((fdlayout = fopen((char*)filename,"r"))==NULL)
    {
      printf("Could not open scene layout (%s)\n",(char*)filename);
      return NULL;
    }
    xml.data = NULL;
    content[0] = 0x00;
    
    __read_file_content(fdlayout,(char*)content,(char*)line);
    
    fclose(fdlayout);
    
    xml.filename[0] = 0x00;
    xml.data = (void*)content;
    xml_Init(&xml);
    
    wi = gui_GetMain();
    
    o = xml.obj;
    while(o)
    {
      oo = xml_GetObject(&xml,o->name);
      
      if (strlen(o->name)>=strlen("main"))
      {
        if (strcmp(o->name,"main")==0)
        {
          if (DEBUG==TRUE) printf("gui_LayoutInit found main\n");
          if (oo!=NULL)
          {
            x = y = w = h = 0;
            
            if ( _gui_setup_object_get_position(oo,&x,&y,&w,&h) != 0 )
            {
              printf("err: could not determine toplevel window geometry\n");
            }
          
            if ( _gui_setup_toplevel_init(x,y,w,h) != 0 )
            {
            }
          }
        }
      }
      if (strlen(o->name)>=strlen("bg"))
      {
        if (oo!=NULL)
        {
          b = xml_GetObjectBit(oo,"value");
          if (b!=NULL)
          {
            gui_WidgetSetColor(wi,(char*)b->value);
          }
        }
      }
      if (strlen(o->name)>=strlen("image"))
      {
        b = xml_GetObjectBit(oo,"value");
        if (b!=NULL)
        {
          im = (struct gui_Image*)malloc(sizeof(struct gui_Image));
          strcpy((char*)im->filename,(char*)b->value);
          strcpy((char*)im->name,(char*)b->value);
          if (gui_CreateImage(wi,im) != 0)
          {
            printf("Could not set image(%s) to main window\n",im->name);
            oo = oo->next;
            continue;
          }
          gui_WidgetAddImage(wi,im);
        }
      }
      if (strlen(o->name)>=strlen("widget"))
      {
        if (strncmp(o->name,"widget",6)==0)
        { 
          if (oo!=NULL)
          {
            wi = gui_NewWidget();
            
            x = y = w = h = 0;
            
            b = xml_GetObjectBit(oo,"x");	if (b!=NULL) x = atoi((char*)b->value);
            b = xml_GetObjectBit(oo,"y");	if (b!=NULL) y = atoi((char*)b->value);
            b = xml_GetObjectBit(oo,"w");	if (b!=NULL) w = atoi((char*)b->value);
            b = xml_GetObjectBit(oo,"h");	if (b!=NULL) h = atoi((char*)b->value);
          
            wi->attr.x = x;
            wi->attr.y = y;
            wi->attr.w = w;
            wi->attr.h = h;
          
            b = xml_GetObjectBit(oo,"form");
            if (b!=NULL)
            {
              if (strcmp((char*)b->value,"canvas")==0)
              {
                wi->attr.form = FORM_CANVAS;
                gui_WidgetInit(wi,gui_GetMain());
                gui_WidgetMap(wi);
                form_CanvasInit(wi);
              
                b = xml_GetObjectBit(oo,"setup");
                if (b!=NULL)
                {
                  sprintf((char*)filename,"scene/%s.xml",(char*)b->value);
                  _gui_setup_canvas_init(wi,(char*)filename);
                }
              }
            }
          }
        }
      }
      o = o->next;
    }
    
    layout = trop_ListAdd(layout,_layout);
    return _layout;
  }

