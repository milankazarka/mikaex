/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

/** this is an ugly implementation of an event loop - needs to get
 *  nicer
 */

#include "system.h"
#include "gui.h"
#include "xml.h"
#include "interface.h"
#include "global.h"
#include "config.h"

  BOOL bprocessEvents = FALSE;
  
  /**
   *  Event Loop initialization
   *  an event loop started best as a thread
   *
   *  if the event starts as a thread or in the application thread - this
   *  is defined in guiMain using _WQ_TYPE
   *
   */
  #ifdef USETCL
    void *_gui_WidgetQueue( void *data ) 
    {
      bprocessEvents = TRUE; 
    }
    
    void _tkEventProc( ClientData clientData, XEvent *eventPtr )
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("  :system TkEventProc\n");
      #endif
      struct gui_Widget *main 	= gui_GetMain();
  #else
    void *_gui_WidgetQueue( void *data )
    {
    
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":_gui_idgetQueue\n");
      #endif
    
      bprocessEvents = TRUE;
      struct gui_Widget *main 	= (struct gui_Widget*)data;
  #endif
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":_gui_WidgetQueue ptr(%p)\n",data);
    #endif
    
    static int run = 0;
    
    if (bprocessEvents==FALSE)
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":_gui_WidgetQueue exiting\n");
      #endif
      return;
    }
    
      struct interface_event_t ievent;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":_gui_WidgetQueue before loop\n");
    #endif
    
    #ifndef USETCL
     for(;;)
     {
        bigInterface->nextGuiEvent(bigInterface,&ievent);
        
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf(":_gui_WidgetQueue processed event\n");
        #endif
        
        run++;
    #else
       event = *eventPtr;
       if (event.type == ConfigureNotify) event.type = Expose;
    #endif
    
    /**
     *
     *   PUSH the os event or the app generated event
     *
     */
    gui_WidgetPushEvent(&ievent);
    
    
    #ifndef USETCL
     }
    #endif
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":_gui_WidgetQueue after loop\n");
    #endif
  }

  //struct low_Font *fontclx = NULL;
  struct gui_GlFont *glfont = NULL;

  #define _EVENT_EXPOSE_SYNC_NO
  
  /**
   *  push an event to be processed right away
   */
  int gui_WidgetPushEvent( void *event )
  {
    struct interface_event_t    *ieventptr = (struct interface_event_t*)event;
    struct interface_event_t    ievent;
    
    struct gui_Widget *main 	= gui_GetMain();
    
    struct gui_Widget *current 	= NULL;
    struct gui_Widget *_current = NULL;
    struct gui_object *gobj 	= NULL;
    struct gui_Image *image	= NULL;
    struct gui_GC *gc		= NULL;
    struct gui_Font *font	= NULL;
    struct gui_Text *text	= NULL;
    struct trop_List *list	= NULL;
    struct gui_Event e;
    int x, y, w, h;
    int	vw;
    int	vh;
    
    //#ifdef _DEBUG
      if (DEBUG==TRUE) printf(":gui_WidgetPushEvent\n");
    //#endif
    
    ievent.type = ieventptr->type;
    //#ifdef _DEBUG
      if (DEBUG==TRUE) printf("_transform (%d/%d)\n",ieventptr->x,ieventptr->y);
    //#endif
    ievent.x = ieventptr->x;
    ievent.y = ieventptr->y;
    ievent.width = ieventptr->width;
    ievent.height = ieventptr->height;
    
        switch(ievent.type)
        {
          case INTERFACE_EVENT_BDOWN:
            _transform_coord(&ievent.x,&ievent.y);
            current = gui_WidgetAt( ievent.x, ievent.y );
            if (!current)
              break;
            
            //#ifdef _DEBUG 
              printf("gui_WidgetEventPush INTERFACE_EVENT_BDOWN UEVENT EVENTFLOW\n");
            //#endif 
              
            e.widget = current;
            e.x = ievent.x - current->attr.x;
            e.y = ievent.y - current->attr.y;
            e.type = ievent.type;
            
            if (current->attr.form == FORM_CANVAS)
            {
              /** handle onbuttondown on canvas entry by canvas
               */
              form_CanvasButtonPress(&e);
            }
            else
            {
              if (current->attr.onbuttondown(&e))
                current->attr.onbuttondown(&e);
            }
            
            break;
            
          case INTERFACE_EVENT_BUP:
            _transform_coord(&ievent.x,&ievent.y);
            current = gui_WidgetAt( ievent.x, ievent.y );
            if (!current)
            {
              //#ifdef _DEBUG
                if (DEBUG==TRUE) printf("gui_WidgetPushEvent fallback 01\n");
              //#endif
              break;
            }
            //#ifdef _DEBUG
              printf("gui_WidgetPushEvent INTERFACE_EVENT_BUP UEVENT EVENTFLOW\n");
            //#endif
            e.widget = current;
            e.x = ievent.x - current->attr.x;
            e.y = ievent.y - current->attr.y;
            e.type = ievent.type;
            
            //#ifdef _DEBUG
              if (DEBUG==TRUE) printf(":ButtonPress %d %d\n", e.x,e.y);
            //#endif
          
            if (current->attr.form == FORM_SLIDEBAR)
              form_SlidebarButtonPress(&e);
            else if (current->attr.form == FORM_MENU)
              form_MenuButtonPress(&e);
            else if (current->attr.form == FORM_CANVAS)
              form_CanvasButtonPress(&e);
            else if (current->attr.form == FORM_OVERLAY)
              form_OverlayButtonPress(&e);
            else if (current->attr.form == FORM_TITLEBAR)
              form_TitlebarButtonPress(&e);
            else if (current->attr.form == FORM_KEYBOARD)
              form_KeyboardButtonPress(&e);
            else
            {
              /** by principle all widgets should be placed into a canvas
               *  so reaching this point means avoiding the old design principles
               */
              #ifdef _DEBUG
                printf("gui_WidgetPushEvent fallback 02\n");
              #endif
            
              if (current->attr.onbuttonup != NULL)
              {
                current->attr.onbuttonup(&e);
              }
              else if (current->attr.onclick != NULL)
              {
                #ifdef _DEBUG
                  if (DEBUG==TRUE) 
                  {
                    printf(":onclick\n");
                    printf("\tgui_Event %d %d\n",
                      e.x, e.y);
                  }
                #endif
                current->attr.onclick(&e);
              } 
            }
             
            break;
        
          case INTERFACE_EVENT_LEAVE:
            _transform_coord(&ievent.x,&ievent.y);
            current = gui_WidgetAt( ievent.x, ievent.y );
            if (!current) break;
            e.widget = current;
            e.x = ievent.x - current->attr.x;
            e.y = ievent.y - current->attr.y;
        
            #ifdef _DEBUG
              if (DEBUG==TRUE) printf(":leave\n");
            #endif
          
            if (current != gui_GetMain())
            {
              if (current->attr.form == FORM_CANVAS)
              {
                form_CanvasRelease(&e);
              }
            }
            _gui_WidgetResetMotion(main);
            break;
          case INTERFACE_EVENT_MOTION:
            _transform_coord(&ievent.x,&ievent.y);
            current = gui_WidgetAt( ievent.x, ievent.y );
            if (!current) break;
            e.widget = current;
            e.x = ievent.x - current->attr.x;
            e.y = ievent.y - current->attr.y;
        
            if (expose!=NULL) // if pending expose, ignore motion
              break;
            #ifdef _DEBUG
              if (DEBUG==TRUE) printf(":motion\n");
            #endif
          
            _transform_coord(&ievent.x,&ievent.y);
          
            current = gui_WidgetAt( ievent.x, ievent.y );
            if (!current)
            {
              break;
            }
          
            if (current->attr.form == FORM_CANVAS)
            {
              trop_MemLock();
              form_CanvasMotion(&e);
              trop_MemUnlock();
            }
            break;
          case INTERFACE_EVENT_EXPOSE:
            #ifdef _DEBUG
              printf("gui_WidgetPushEvent INTERFACE_EVENT_EXPOSE start UEVENT EVENTFLOW\n");
            #endif
          
            #ifdef _GUI_CLEAR_ON_EXPOSE
              gui_Clear();
            #endif
          
            current = main;
            while(current != NULL)
            {
              if (expose!=NULL && expose!=current)
              {
                current = current->next;
                continue;
              }
              
              if (gui_SceneIsWidget(scene,current) == FALSE &&
                current != gui_GetMain()) // main is member of all scenes
              {
                current = current->next;
                continue;
              }
              if (current->attr.bvisible == FALSE)
              {
                current = current->next;
                continue;
              }
            
              if (current->parent != main && current->parent != NULL) // we only handle toplevel here
              {
                current = current->next;
                continue;
              }
            
              if (gui_WidgetIsViewable(current) == FALSE)
              {
                current = current->next;
                continue;
              }
            
              #ifdef _DEBUG
                if (DEBUG==TRUE) printf(":expose on window\n");
              #endif
               
            if (current->attr.onexposeEx)
            {
              if (current->attr.form==FORM_CANVAS)
                current->attr.onexposeEx(
                                       current,
                                       current->attr.x,current->attr.y,
                                       current->attr.w,current->attr.h
                                       );
              if (current->attr.form == FORM_CANVAS)
              {
                if (expose!=NULL) 
                {
                  #ifdef _DEBUG
                    printf("gui_WidgetPushEvent INTERFACE_EVENT_EXPOSE finish\n");
                  #endif
                }
              }
            }

            current = current->next;
          }
          
          if (!glfont) // testing purposes
          {
            glfont = gui_GlFontInit( gl, NULL, DEFA_FT );
            glfont->color.rgba[0] = 220;
            glfont->color.rgba[1] = 220;
            glfont->color.rgba[2] = 220;
            glfont->color.rgba[3] = 220;
          }
          
          #ifdef _DEBUG
            printf("gui_WidgetPushEvent repaint surface UEVENT\n");
          #endif
          _gui_surface_to_window( );
          gui_Flush();
           
          #ifdef _DEBUG
            printf("gui_WidgetPushEvent INTERFACE_EVENT_EXPOSE finish\n");
          #endif
          
          break;
      }
    
    return 0;
  }
