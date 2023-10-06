/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "gui.h"
#include "global.h"

  /**
   *  register a new goid with a (custom id) of myid
   *  - returns the id of the new goid
   */
  int _goid_id_descr_add_id(
    int myid,
    char *descr
  )
  {
    int				i;
    int 			goid 	= -1;
    struct _goid_id_descr	*gid 	= NULL;
    struct trop_List		*cur	= NULL;
    
    for (i = 0; i < sizeof(_goid_id_descr_pool)/sizeof(struct _goid_id_descr); i++)
    {
      if (_goid_id_descr_pool[i].id > goid)
      {
        goid = _goid_id_descr_pool[i].id;
      }
    }

    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("_goid_id_descr_add_id.01\n");
    #endif
    
    cur = _lst_goid_custom;
    while(cur)
    {
      if (cur->ptr)
      {
        gid = (struct _goid_id_descr*)cur->ptr;
        if (gid->id > goid)
        {
          goid = gid->id;
        }
      }
      cur = cur->next;
    }
    
    goid++;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("_goid_id_descr_add_id.02\n");
    #endif
    
    gid = (struct _goid_id_descr*)malloc(sizeof(struct _goid_id_descr));
    gid->id = goid;
    gid->myid = myid;
    gid->descr = (char*)malloc(strlen(descr)+1);
    strcpy((char*)gid->descr,descr);
    
    _lst_goid_custom = trop_ListAdd(_lst_goid_custom,gid);
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("_goid_id_descr_add_id :register_goid (%d)\n",goid);
    #endif
    
    return goid;
  }
  
  /** get the goid id, that we registered with myid - only searches in custom goid's
   */
  int _goid_id_descr_by_myid(
    int myid
  )
  {
    int				goid	= -1;
    struct _goid_id_descr	*gid	= NULL;
    struct trop_List		*cur	= NULL;
    
    cur = _lst_goid_custom;
    while(cur)
    {
      gid = (struct _goid_id_descr*)cur->ptr;
      if (gid->myid==myid)
      {
        return gid->id;
      }
      cur = cur->next;
    }
    
    return goid;
  }

  /** Add custom gui objects - either set obj or value 
   */
  int gui_WidgetAddCustomGobj(
    struct gui_Widget 	*w,
    int 		goid,
    void 		*obj,
    int			value
  )
  {
    struct gui_object *current = w->obj;
    
    if (obj) value = -1;
    
    if (current == NULL)
    {
      w->obj = (struct gui_object *)malloc(sizeof(struct gui_object));
      w->obj->id = goid;
      w->obj->value = value;
      w->obj->data = obj;
      w->obj->next = NULL;
    }
    else
    {
      current = (struct gui_object *)malloc(sizeof(struct gui_object));
      current->next = w->obj;
      current->id = goid;
      current->value = value;
      current->data = obj;
      w->obj = current;
    }
  
    return 0;
  }
  
  /** get gui object at goid, only returns 1# occurance 
   */
  void *gui_WidgetGetObjAtGoid( struct gui_Widget *w, int goid )
  {
    struct gui_object	*gobj = NULL;
    
    gobj = w->obj;
    while(gobj)
    {
      if (gobj->id==goid)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("gui_WidgetGetObjAtGoid found(%d)\n",goid);
        #endif
        return gobj->data;
      }
      gobj = gobj->next;
    }
    
    return NULL;
  }

int _gui_widget_release_goids( struct gui_Widget *w )
{
  struct gui_object	*gobj	= NULL;
  struct gui_object	*nexto	= NULL;
  
  struct gui_GC		*gc	= NULL;
  struct gui_Text	*_ts	= NULL;
  struct gui_TextSim	*ts	= NULL;
  struct gui_Widget	*_w	= NULL;
  
  gobj = w->obj;
  while(gobj)
  {
    nexto = gobj->next;
    switch(gobj->id)
    {
      case GOID_WIDGET:
        _w = (struct gui_Widget*)gobj->data;
        if (_w)
        {
          gui_WidgetReleaseEx(_w);
          gobj->data = NULL;
        }
        break;
      case GOID_COLOR:
        gc = (struct gui_GC*)gobj->data;
        if (gc)
        {
          if (gc->colorl)
          {
            free(gc->colorl);
            gc->colorl = NULL;
          }
          free(gc);
          gobj->data = NULL;
        }
        break;
      case GOID_TEXT:
        _ts = (struct gui_Text*)gobj->data;
        if (_ts)
        {
          if (_ts->str)
          {
            free(_ts->str);
            _ts->str = NULL;
          }
          _ts->font = NULL;
          free(_ts);
          gobj->data = NULL;
        }
        break;
      case GOID_TEXTSIM:
      case GOID_TEXTSIMB:
      case GOID_TEXTSIMC:
      case GOID_TEXTSIMD:
        ts = (struct gui_TextSim*)gobj->data;
        if (ts)
        {
          if (ts->str)
          {
            free(ts->str);
            ts->str = NULL;
          }
          free(ts);
          gobj->data = NULL;
        }
        break;
      default:
        if(gobj->data)
        {
          free(gobj->data);
          gobj->data = NULL;
        }
        break;
    }
    free(gobj);
    gobj = nexto;
  }
  
  return 0;
}
  
int gui_WidgetReleaseEx( struct gui_Widget *w )
{
  if (!w) return -1;
  
  switch(w->attr.form)
  {
    case FORM_LBENTRY:
      form_LBEntryRelease(w);
      break;
    case FORM_TEXT:
      break;
    default:
      break;
  }
  
  _gui_widget_release_goids(w);
  free(w);
  
  return 0;
}
