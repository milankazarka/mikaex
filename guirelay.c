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
#include "global.h"
  
  int _gui_relay_free( struct trop_List *current )
  {
    static struct trop_Relay *trel = NULL;
    static struct gui_Relay *rel = NULL;
    
    if (current==NULL) return -1;
    
    trel = current->ptr;
    rel = trel->data;
    
    free(rel);
    trel->data = NULL;
    free(trel);
    current->ptr = NULL;
    
    return 0;
  }
  
  int _gui_widget_do_relay( struct gui_Relay *rel )
  {
    if (!rel) return -1;
    return 0;
  }
  
  struct gui_Relay *gui_WidgetNextRelay( struct gui_Widget *wi )
  {
    static struct gui_Relay *rel = NULL;
    static struct trop_Relay *trel = NULL;
    static struct trop_List *current = NULL;
    static struct trop_List *last = NULL;
    
    if (!wi) return NULL;
    
    rel 	= NULL;
    trel 	= NULL;
    last 	= NULL;
    
    current = wi->relay;
    while(current!=NULL)
    {
      trel = current->ptr;
      rel = trel->data;
      if (current->next==NULL)
      {
        if (last!=NULL)
        {
          _gui_widget_do_relay(rel);
          _gui_relay_free(current);
          free(current);
          last->next = NULL;
        }
        else
        {
          _gui_widget_do_relay(rel);
          _gui_relay_free(current);
          free(current);
          wi->relay = NULL;
        }
      }
      last = current;
      current = current->next;
    }
    
    return rel;
  }

  int gui_WidgetDoRelays( struct gui_Widget *wi, struct gui_Event *ev )
  {
    struct gui_Relay *rel = NULL;
    
    while( (rel = gui_WidgetNextRelay(wi)) != NULL )
    {
      
    }
    return 0;
  }
  
  int _gui_widget_relay_event_expose( 
    struct gui_Event *ev 
  )
  {
    if (!ev) return -1;
    //gui_EventExpose(ev);
    return 0;
  }
  
  int _gui_widget_relay_event_bpress( 
    struct gui_Event *ev 
  )
  {
    if (!ev) return -1;
    return 0;
  }
  
  int _gui_widget_relay_event_brelease( 
    struct gui_Event *ev 
  )
  {
    if (!ev) return -1;
    return 0;
  }
  
  int _gui_widget_relay_event_bmotion( 
    struct gui_Event *ev 
  )
  {
    if (!ev) return -1;
    return 0;
  }
  
  int _form_canvas_event_key(
    struct gui_Event *ev
  )
  {
    if (!ev) return -1;
    return 0;
  }
  
  int _form_editable_event_key(
    struct gui_Event *ev
  )
  {
    if (!ev) return -1;
    
    if (ev->key == -1)
    {
      form_EditableRemoveChar(ev->widget);
    }
    else if (ev->key != 0x00)
    {
      form_EditableAppendChar(ev->widget,ev->key);
    }
    
    return 0;
  }
  
  int _gui_widget_relay_event_key( 
    struct gui_Event *ev 
  )
  {
    if (!ev) return -1;
    
    switch(ev->widget->attr.form)
    {
      case FORM_CANVAS:
        _form_canvas_event_key(ev);
      break;
      case FORM_EDITABLE:
        _form_editable_event_key(ev);
      break;
      default:
      break;
    }
    
    return 0;
  }

  struct trop_List *_gui_widget_add_relay_obj(
    struct gui_Widget *wi
  )
  { 
    struct trop_List *current = NULL;
    
    if (!wi) return NULL;
  
    current = wi->relay;
    while(current!=NULL)	// todo - add to base layer
    {
      if (current->next==NULL)
      {
        break;
      }
      current = current->next;
    }
    if (current==NULL)
    {
      wi->relay = (struct trop_List*)malloc(sizeof(struct trop_List));
      current = wi->relay;
    }
    else
    {
      current->next = (struct trop_List*)malloc(sizeof(struct trop_List));
      current = current->next;
    }
  
    return current;
  }

  int gui_WidgetAddRelayMsg(
    struct gui_Widget *wi,
    struct gui_Msg *msg // preinitialized msg
  )
  {
    static struct gui_Relay *rel = NULL;
    static struct trop_Relay *trel = NULL;
    static struct trop_List *current = NULL;
  
    if (!wi||!msg) return -1;
    
    current = _gui_widget_add_relay_obj(wi);
    if (current==NULL) return -1;
    
    current->next = NULL;
    current->ptr = (struct trop_Relay*)malloc(sizeof(struct trop_Relay));
    trel = current->ptr;
    trel->data = (struct gui_Relay*)malloc(sizeof(struct gui_Relay));
    rel = trel->data;
    rel->wi = NULL;
    
    rel->msg = msg;
    
    return 0;
  }
  
  /*
    add a relay to a widget - the widget in the relay will recieve events
    which were intended for the widget holding the relay 
  */
  int gui_WidgetAddRelayEx( struct gui_Widget *wi, struct gui_Widget *rwi )
  {
    static struct gui_Relay *rel = NULL;
    static struct trop_Relay *trel = NULL;
    static struct trop_List *current = NULL;
    
    if ( !wi || !rwi ) return -1;
    if ( wi == rwi )
    {
      printf("Relay holder same as relay, relay invalid\n"); 
      return -1;
    }
    
    current = _gui_widget_add_relay_obj(wi);
    if (current==NULL) return -1;
    
    current->next = NULL;
    current->ptr = (struct trop_Relay*)malloc(sizeof(struct trop_Relay));
    trel = current->ptr;
    trel->data = (struct gui_Relay*)malloc(sizeof(struct gui_Relay));
    rel = trel->data;
    rel->wi = rwi;
    rel->msg = NULL;
    
    return 0;
  }
  
  /*
    in Static, the relays don't get freed 
  */
  int gui_WidgetDoRelaysStatic( struct gui_Widget *wi, struct gui_Event *ev )
  {
    static struct gui_Relay *rel = NULL;
    static struct trop_Relay *trel = NULL;
    static struct trop_List *current = NULL;
    static struct trop_List *last = NULL;
    static struct gui_Widget *rwi = NULL;
    static struct gui_Widget *ewi = NULL;
    static struct gui_Msg *msg = NULL;
    
    if (!wi) return -1;
    
    if (DEBUG==TRUE) printf("gui_WidgetDoRelaysStatic\n");
    
    rel 	= NULL;
    trel 	= NULL;
    last 	= NULL;
    
    ewi = ev->widget;
    
    current = wi->relay;
    while(current!=NULL)
    {
      trel = current->ptr;
      if (trel==NULL) { current = current->next; continue; } 
      rel = trel->data;
      if (rel==NULL) { current = current->next; continue; }
      
      msg = rel->msg; // first we try msg relaying
      if (msg!=NULL)
      {
        gui_MsgDoMsg(msg);
        current = current->next;
        continue;
      }
      
      rwi = rel->wi;
      if (rwi==NULL) { current = current->next; continue; }
      
      ev->widget = rwi;
      switch(ev->type)
      {
        case GEVENT_NONE:
        break;
        case GEVENT_EXPOSE:
        //  _gui_widget_relay_event_expose(ev);
        break;
        case GEVENT_BPRESS:
          _gui_widget_relay_event_bpress(ev);
        break;
        case GEVENT_BRELEASE:
          _gui_widget_relay_event_brelease(ev);
        break;
        case GEVENT_MOTION:
          _gui_widget_relay_event_bmotion(ev);
        break;
        case GEVENT_KEY:
          _gui_widget_relay_event_key(ev);
        break;
        default:
        break;
      }
      
      current = current->next;
    }
    
    ev->widget = ewi;
    
    return 0;
  }
  
  int gui_WidgetDoEvent( struct gui_Widget *wi, struct gui_Event *ev )
  {
    if ( !wi || !ev ) return -1;
    if (DEBUG==TRUE) printf("gui_WidgetDoEvent\n");
    
    gui_WidgetDoRelaysStatic(wi,ev);
    
    ev->widget = wi;
    switch(ev->type)
    {
      case GEVENT_NONE:
      break;
      case GEVENT_EXPOSE:
        _gui_widget_relay_event_expose(ev);
      break;
      case GEVENT_BPRESS:
        _gui_widget_relay_event_bpress(ev);
      break;
      case GEVENT_BRELEASE:
        _gui_widget_relay_event_brelease(ev);
      break;
      case GEVENT_MOTION:
        _gui_widget_relay_event_bmotion(ev);
      break;
      case GEVENT_KEY:
        _gui_widget_relay_event_key(ev);
      break;
      default:
      break;
    }
    
    return 0;
  }
  
  inline void gui_EventInit( struct gui_Event *ev )
  {
    ev->type 	= GEVENT_NONE;
    ev->widget 	= NULL;
    ev->sub 	= NULL;
    ev->x 	= 0;
    ev->y 	= 0;
    ev->w 	= 0;
    ev->h 	= 0;
    ev->key 	= 0x00;
  }
