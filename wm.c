/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "wm.h"

struct trop_List *lstWindows;

int tr_WmInit( )
{
  int i;
  
  lstWindows = NULL;
  
  for (i = 0; i < sizeof(_tr_goid_action_list)/sizeof(struct _tr_goid_action); i++)
  {
    _goid_id_descr_add_id(_tr_goid_action_list[i].id,_tr_goid_action_list[i].descr);
  }
  
  if ( mikaSkinGlobalInit() != 0 )
  {
    #ifdef _DEBUG
      printf("tr_WmInit skin couldn't be initialized\n");
    #endif
  }
  
  return 0;
}

int tr_WmListWindows( )
{
  if (!lstWindows) return -1;
  int i = 0;
  struct trop_List *current = lstWindows;
  struct tr_Window *win = NULL;
  while(current)
  {
    if (current->ptr)
    {
      win = (struct tr_Window*)current->ptr;
      if (win)
      {
        if (win->attr)
        {
          #ifdef _DEBUG
            if (DEBUG==TRUE)
            {
              printf("window %d\n",i);
              printf("   x: %d\n",win->attr->x);
              printf("   y: %d\n",win->attr->y);
              printf("   w: %d\n",win->attr->w);
              printf("   h: %d\n",win->attr->h);
            }
          #endif
        }
      }
      i++;
    }
    current = current->next;
  }
  return 0;
}

/**
 * get the toplevel window 
 */
struct tr_Window *tr_WindowMain( )
{
  struct trop_List *current = lstWindows;
  while(current != NULL)
  {
    if (((struct tr_Window*)current->ptr)->parent==NULL)
      return (struct tr_Window*)current->ptr;
    current = current->next;
  }
  return NULL;
}

/**
 * initializes a window, if the parent window is NULL, than this is a 
 * toplevel window
 */
struct tr_Window *tr_WindowInit( struct tr_Window *w, struct tr_Window *parent, 
  struct tr_Attr *attr )
{
  struct gui_Widget *subparent = NULL;
  if (!parent)
  {
    if (lstWindows!=NULL)
    {
      printf("err: adding a toplevel window to a session which already has one\n");
      return NULL;
    }
  }
  else
  {
    if (parent->sub!=gui_GetMain())
    {
      printf("err: adding a window to a non top level window\n");
      return NULL;
    }
    subparent = parent->sub;
  }
  if (w==NULL)
    w = (struct tr_Window*)malloc(sizeof(struct tr_Window));
  w->scene = gui_NewScene();
  w->attr = attr;
  w->attr->bvisible = FALSE;
  w->parent = parent;
  w->widgets = NULL;
  lstWindows = trop_ListAdd(lstWindows,w);
  if (parent==NULL)
  {
    gui_SceneSet(w->scene);
    w->sub = gui_NewWidget();
    w->sub->attr.x = attr->x;
    w->sub->attr.y = attr->y;
    w->sub->attr.w = attr->w;
    w->sub->attr.h = attr->h;
    w->sub->attr.form = -1;
    if ( gui_WidgetInit(w->sub,subparent) != 0 )
    {
      printf("err: initialising lower window object\n");
      // todo: free window structure
      return NULL;
    }
    gui_WidgetSetColor(w->sub,(char*)TR_DEFA_BG);
    gui_WidgetMap(w->sub);
  }
  return w;
}

struct tr_WindowEx *tr_WindowExInit( 
  struct tr_WindowEx *w, 
  struct tr_WindowEx *parent,
  struct tr_Attr *attr
)
{
  if (!attr) return NULL;
  
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf("tr_WindowExInit\n");
  #endif
  
  if (w==NULL)
  {
    w = (struct tr_WindowEx*)malloc(sizeof(struct tr_WindowEx));
    w->window = NULL;
    if (parent)
      w->window = tr_WindowInit( w->window, parent->window, attr );
    else
      w->window = tr_WindowInit( w->window, NULL, attr );
    tr_WindowToTop(w->window);
  }
  w->canvas = tr_CanvasInit(NULL,attr,w->window);
  #ifdef _DEBUG
    if (!w->canvas)
    {
      if (DEBUG==TRUE) printf("tr_WindowExInit w->canvas = NULL\n");
    }
  #endif
  w->cfgfile[0] = 0x00;
  return w;
}

/**
 * general action handling
 * - in layout we pack our own action data inside a gobj
 */
void *tr_doAction( void *data ) // struct gui_Event[widget]
{
  int goid;
  
  struct gui_Event *gevent			= (struct gui_Event*)data;
  struct tr_Action *traction 			= NULL;
  struct tr_ActionSceneSwitch *tractionsw	= NULL;
  struct tr_WindowEx *trwindowex		= NULL;
  
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf(":tr_doAction\n");
  #endif

  goid = _goid_id_descr_by_myid(TR_GOID_ACTION);  
  traction = (struct tr_Action*)gui_WidgetGetObjAtGoid(gevent->widget,goid);
  
  if (!traction) 
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":tr_doAction !traction\n");
    #endif
    return NULL;
  }
  switch(traction->type)
  {
    case TR_ACTION_TYPE_SCENE_SWITCH:
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":tr_doAction TR_ACTION_TYPE_SCENE_SWITCH\n");
      #endif
      tractionsw = (struct tr_ActionSceneSwitch*)traction->sub;
      trwindowex = (struct tr_WindowEx*)tractionsw->pscene;
      tr_WindowToTop(trwindowex->window);
      tr_WindowExpose(trwindowex->window);
    break;
    default:
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":tr_doAction TR_ACTION_(unknown)\n");
      #endif
    break;
  }
}

/**
 * setup window from configuration file 
 */
int tr_WindowExSetup(
  struct tr_WindowEx *w,
  char *filename
)
{
  if (!w||!filename) return -1;
  return 0;
}

static char *__dummy = (char*)"@";

typedef struct __layout_o_as_general_value
{
  int			value;
} __layout_o_as_general_value;

typedef struct _layout_o_as_position
{
  int			x;
  int			y;
  int			w;
  int			h;
} _layout_o_as_position;

typedef struct _layout_o_as_window
{
  char			parent[32];
  int			x;
  int			y;
  int			w;
  int			h;
} _layout_o_as_window;

/**
 * the form_Simple should be dynamically managed by a module rather
 * than by the layout manager
 */  
typedef struct _layout_o_as_simple
{
  int			w;
  int			h;
} _layout_o_as_simple;

typedef struct _layout_o_as_start
{
  char			id[32];
} _layout_o_as_start;

typedef struct _layout_o_as_bg
{
  char			color[32];
} _layout_o_as_bg;

/** 
 * - defines a text field in a widget
 * <.main.b00.t00 type=text str=something x=0 y=0 w=200 h=100> 
 */
typedef struct _layout_o_as_text
{
  char			str[256]; 	// todo, define a general value
  int			x;
  int			y;
  int			w;
  int			h;
} _layout_o_as_text;

typedef struct _layout_o_as_text _layout_o_as_longdesc;
typedef struct _layout_o_as_text _layout_o_as_shortdesc;

/**
 * - defines a text field widget 
 */
typedef struct _layout_o_as_textfield
{
  char			str[256]; 	// todo, define a general value
  int			x;
  int			y;
  int			w;
  int			h;
} _layout_o_as_textfield;

typedef struct _layout_o_as_image
{
  char			filename[256];
  int			pos;
  int			x;
  int			y;
  int			w;
  int			h;
} _layout_o_as_image;

typedef struct _layout_o_as_button
{
  BOOL			bstatic;
  char			parent[32];
  struct _layout_o_as_position	pos;
} _layout_o_as_button;

typedef struct _layout_o_as_lbentry
{
  char			parent[32];
  struct _layout_o_as_position	pos;
} _layout_o_as_lbentry;

enum {
  _LAYOUT_O_AS_ACTION_MISC
};

typedef struct _layout_o_as_action
{
  int			type;
  int			atype;		// TR_ACTION_TYPE_SCENE_NONE
  char			relay[32];
} _layout_o_as_action;

typedef struct _layout_o_as_move
{
  int			type;
} _layout_o_as_move;

/* layout object association */
typedef struct _layout_o_as
{
  char			name[32];
  int			type;
  char			holder[32];
  void			*form;
  void			*gobj; 		// graphical object (Widget,Image,etc.), NULL uninitialized 
} _layout_o_as;

enum {
  _LAYOUT_O_AS_NONE,
  _LAYOUT_O_AS_WINDOW,
  _LAYOUT_O_AS_START,			// which window is the startup window
  _LAYOUT_O_AS_SIMPLE,			// simple general embedded widget 
  _LAYOUT_O_AS_BUTTON,
  _LAYOUT_O_AS_LBENTRY,			// listbox entry
  _LAYOUT_O_AS_LONGDESC,		// general long description
  _LAYOUT_O_AS_SHORTDESC,		// general short descrition
  _LAYOUT_O_AS_BG,
  _LAYOUT_O_AS_ROUND,			// roundness
  _LAYOUT_O_AS_TEXT,			// text field in widget
  _LAYOUT_O_AS_TEXTFIELD,		// text field widget
  _LAYOUT_O_AS_MOVE,			// movement type (canvas)
  _LAYOUT_O_AS_IMAGE,
  _LAYOUT_O_AS_ACTION
};

int _layout_o_as_do_obj( struct tr_Layout *l, struct trop_List *losalst, struct _layout_o_as *losa );

/**
 * add a layout object association 
 */
struct trop_List *_layout_o_as_add( struct trop_List *losalst, xml_Object *o )
{
  struct _layout_o_as 		*losa 	 = NULL;
  struct xml_Bit		*b 	 = NULL;
  struct _layout_o_as_window	*window	 = NULL;
  struct _layout_o_as_start	*start	 = NULL;
  struct _layout_o_as_bg 	*bg 	 = NULL;
  struct _layout_o_as_text 	*text 	 = NULL;
  struct _layout_o_as_textfield	*textf	 = NULL;
  struct _layout_o_as_image 	*image 	 = NULL;
  struct _layout_o_as_button	*button	 = NULL;
  struct _layout_o_as_lbentry	*lbentry = NULL;
  struct _layout_o_as_action	*action  = NULL;
  struct _layout_o_as_move	*move	 = NULL;
  struct __layout_o_as_general_value
                                *value   = NULL;
  int				x, y, w, h;
  
  if (!o) return NULL;

  #ifdef _DEBUG
    if (DEBUG==TRUE) printf("_layout_o_as_add\n");
  #endif
  
  losa = (struct _layout_o_as *)malloc(sizeof(struct _layout_o_as));
  
  losa->form 		= NULL;
  losa->holder[0]	= 0x00;
  losa->type		= _LAYOUT_O_AS_NONE;
  losa->gobj		= NULL;
  
  strcpy((char*)losa->name,o->name);
  
  b = xml_GetObjectBit(o,(char*)"type");
  if (b)
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("xml.o type=%s\n",b->value);
    #endif
    if (strcmp(b->value,"window")==0)		losa->type = _LAYOUT_O_AS_WINDOW;
    else if (strcmp(b->value,"start")==0)	losa->type = _LAYOUT_O_AS_START;
    else if (strcmp(b->value,"button")==0)	losa->type = _LAYOUT_O_AS_BUTTON;
    else if (strcmp(b->value,"lbentry")==0)	losa->type = _LAYOUT_O_AS_LBENTRY;
    else if (strcmp(b->value,"bg")==0)		losa->type = _LAYOUT_O_AS_BG;
    else if (strcmp(b->value,"text")==0)	losa->type = _LAYOUT_O_AS_TEXT;
    else if (strcmp(b->value,"long_desc")==0)	losa->type = _LAYOUT_O_AS_LONGDESC;
    else if (strcmp(b->value,"short_desc")==0)	losa->type = _LAYOUT_O_AS_SHORTDESC;
    else if (strcmp(b->value,"textfield")==0)	losa->type = _LAYOUT_O_AS_TEXTFIELD;
    else if (strcmp(b->value,"image")==0)	losa->type = _LAYOUT_O_AS_IMAGE;
    else if (strcmp(b->value,"action")==0)	losa->type = _LAYOUT_O_AS_ACTION;
    else if (strcmp(b->value,"move")==0)	losa->type = _LAYOUT_O_AS_MOVE;
    else if (strcmp(b->value,"round")==0)	losa->type = _LAYOUT_O_AS_ROUND;
  }
     
  switch(losa->type)
  {
    case _LAYOUT_O_AS_WINDOW:
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("xml.o _LAYOUT_O_AS_WINDOW\n");
      #endif
      window = (struct _layout_o_as_window*)malloc(sizeof(struct _layout_o_as_window));
      window->x = 0;
      window->y = 0;
      window->w = 0;
      window->h = 0;
      _gui_setup_object_get_position(o,&x,&y,&w,&h);
      window->x = x;
      window->y = y;
      window->w = w;
      window->h = h;
      b = xml_GetObjectBit(o,(char*)"parent");
      if (b)
      {
        strcpy((char*)window->parent,b->value);
      }
      losa->form = window;
    break;
    case _LAYOUT_O_AS_START:
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("xml.o _LAYOUT_O_AS_START\n");
      #endif
      start = (struct _layout_o_as_start*)malloc(sizeof(struct _layout_o_as_start));
      start->id[0] = 0x00;
      b = xml_GetObjectBit(o,"start");
      if (b)
      {
        strcpy((char*)start->id,b->value);
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("xml.o _LAYOUT_O_AS_START start(%s)\n",(char*)start->id);
        #endif
      }
      losa->form = start;
    break;
    case _LAYOUT_O_AS_BUTTON:
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("xml.o _LAYOUT_O_AS_BUTTON\n");
      #endif
      button = (struct _layout_o_as_button*)malloc(sizeof(struct _layout_o_as_button));
      button->pos.x = 0;
      button->pos.y = 0;
      button->pos.w = 0;
      button->pos.h = 0;
      button->bstatic = FALSE;
      _gui_setup_object_get_position(o,&x,&y,&w,&h);
      button->pos.x = x;
      button->pos.y = y;
      button->pos.w = w;
      button->pos.h = h;
      button->parent[0] = 0x00;
      b = xml_GetObjectBit(o,"parent");
      if (b)
      {
        strcpy((char*)button->parent,b->value);
      }
      b = xml_GetObjectBit(o,"static");
      if (b)
      {
        if (atoi(b->value)==1)		button->bstatic = TRUE;
      }
      losa->form = button;
    break;
    case _LAYOUT_O_AS_LBENTRY:
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("xml.o _LAYOUT_O_AS_LBENTRY\n");
      #endif
      lbentry = (struct _layout_o_as_lbentry*)malloc(sizeof(struct _layout_o_as_lbentry));
      lbentry->pos.x = 0;
      lbentry->pos.y = 0;
      lbentry->pos.w = 0;
      lbentry->pos.h = 0;
      _gui_setup_object_get_position(o,&x,&y,&w,&h);
      lbentry->pos.x = x;
      lbentry->pos.y = y;
      lbentry->pos.w = w;
      lbentry->pos.h = h;
      lbentry->parent[0] = 0x00;
      b = xml_GetObjectBit(o,"parent");
      if (b)
      {
        strcpy((char*)lbentry->parent,b->value);
      }
      losa->form = lbentry;
    break;
    case _LAYOUT_O_AS_BG:
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("xml.o _LAYOUT_O_AS_BG\n");
      #endif
      bg = (struct _layout_o_as_bg*)malloc(sizeof(struct _layout_o_as_bg));
      bg->color[0] = 0x00;
      b = xml_GetObjectBit(o,"color");
      if (b)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("xml.o _LAYOUT_O_AS_BG setting color\n");
        #endif
        strcpy((char*)bg->color,b->value);
      }
      else
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("xml.o _LAYOUT_O_AS_BG not color bit found\n");
        #endif
      }
      losa->form = bg;
    break;
    case _LAYOUT_O_AS_TEXT:
    case _LAYOUT_O_AS_SHORTDESC:
    case _LAYOUT_O_AS_LONGDESC:
      text = (struct _layout_o_as_text*)malloc(sizeof(struct _layout_o_as_text));
      text->x = -1;
      text->y = -1;
      text->w = -1;
      text->h = -1;
      text->str[0] = 0x00;
      _gui_setup_object_get_position(o,&x,&y,&w,&h);
      text->x = x;
      text->y = y;
      text->w = w;
      text->h = h;
      b = xml_GetObjectBit(o,"text");
      if (b)
      {
        strcpy((char*)text->str,b->value);
      }
      losa->form = text;
    break;
    case _LAYOUT_O_AS_TEXTFIELD:
      textf = (struct _layout_o_as_textfield*)malloc(sizeof(struct _layout_o_as_textfield));
      textf->x = -1;
      textf->y = -1;
      textf->w = -1;
      textf->h = -1;
      textf->str[0] = 0x00;
      _gui_setup_object_get_position(o,&x,&y,&w,&h);
      textf->x = x;
      textf->y = y;
      textf->w = w;
      textf->h = h;
      b = xml_GetObjectBit(o,"text");
      if (b)
      {
        strcpy((char*)textf->str,(char*)b->value);
      }
      losa->form = textf;
    break;
    case _LAYOUT_O_AS_IMAGE:
      image = (struct _layout_o_as_image*)malloc(sizeof(struct _layout_o_as_image));
      image->pos = PLACE_NONE;
      image->x = 0;
      image->y = 0;
      image->w = 0;
      image->h = 0;
      image->filename[0] = 0x00;
      _gui_setup_object_get_position(o,&x,&y,&w,&h);
      image->x = x;
      image->y = y;
      image->w = w;
      image->h = h;
      b = xml_GetObjectBit(o,"filename");
      if (b)
      {
        strcpy((char*)image->filename,b->value);
      }
      b = xml_GetObjectBit(o,"pos");
      if (b)
      {
        if (strcmp((char*)b->value,"center")==0)
        {
          image->pos = PLACE_CENTER;
        }
      }
      losa->form = image;
    break;
    case _LAYOUT_O_AS_ACTION:
      action = (struct _layout_o_as_action*)malloc(sizeof(struct _layout_o_as_action));
      action->type = _LAYOUT_O_AS_ACTION_MISC;
      b = xml_GetObjectBit(o,"relay");
      if (b)
      {
        strcpy((char*)action->relay,b->value);
      }
      b = xml_GetObjectBit(o,"atype");
      if (b)
      {
        if (strcmp((char*)b->value,"scene_switch")==0)
        {
          action->atype = TR_ACTION_TYPE_SCENE_SWITCH;
        }
        else if (strcmp((char*)b->value,"system")==0)
        {
          action->atype = TR_ACTION_TYPE_SYSTEM;
        }
      }
      losa->form = action;
    break;
    case _LAYOUT_O_AS_MOVE:
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("xml.o register _LAYOUT_O_AS_MOVE\n");
      #endif
      move = (struct _layout_o_as_move*)malloc(sizeof(struct _layout_o_as_move));
      b = xml_GetObjectBit(o,"value");
      if (b)
      {
        if (strcmp(b->value,"scroll")==0)	move->type = CAN_MOV_SLIDE;
        else if (strcmp(b->value,"slide")==0)	move->type = CAN_MOV_SCROLL;
        else if (strcmp(b->value,"none")==0)	move->type = CAN_MOV_NONE;
      }
      losa->form = move;
    break;
    case _LAYOUT_O_AS_ROUND:
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("xml.o register _LAYOUT_O_AS_ROUND\n");
      #endif
      value = (struct __layout_o_as_general_value*)malloc(sizeof(struct __layout_o_as_general_value));
      b = xml_GetObjectBit(o,"value");
      if (b)
      {
        value->value = atoi(b->value);
      }
      losa->form = value;
    break;
  }   
     
  b = xml_GetObjectBit(o,"holder");
  if (b)	strcpy((char*)losa->holder,b->value);
  
  losalst = trop_ListAdd(losalst,losa);
  
  return losalst;
}

struct _layout_o_as *_layout_o_as_getbyname( 
  struct trop_List *losalst, 
  char *name 
)
{
  struct trop_List *current = NULL;
  struct _layout_o_as *losa = NULL;
  
  if (!losalst||!name) return NULL;
  
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf("_layout_o_as_getbyname (%s)\n",(char*)name);
  #endif
  
  current = losalst;
  while(current)
  {
    if (current->ptr) 
    {
      losa = (struct _layout_o_as*)current->ptr;
      if (losa->name[0]==0x00)
      {
        current = current->next;
        continue;
      }
      if (strcmp((char*)losa->name,name)==0)
        return losa;
    }
    current = current->next;
  }
  
  return losa;
}

/**
 * get direct parent _layout_o_as object 
 */
struct _layout_o_as *_layout_o_as_get_parent(
  struct trop_List *losalst,
  char *objname
)
{
  int 			n, nn;
  int			len;
  char			name[32];
  struct _layout_o_as	*parent 	= NULL;

  name[0] = 0x00;
  if (objname[0]!=0x00)
  {
    len = strlen((char*)objname);
    len--;
    for(n = len; n > -1; n--)
    {
      if (objname[n]=='.')
      {
        for(nn = 0; nn < n; nn++)
        {
          name[nn] = objname[nn];
        }
        name[n] = 0x00;
        break;
      }
    }
  }
 
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf("_layout_o_as_do_obj on parent (%s)\n",(char*)name);
  #endif
 
  parent = _layout_o_as_getbyname(losalst,(char*)name); 

  return parent;
}

/**
 * get the _layout_o_as of parent window ID 
 */
struct _layout_o_as *_layout_o_as_get_parentw(
  struct trop_List *losalst,
  char *objname
)
{
  int 			n;
  int			len;
  char			name[32];
  struct _layout_o_as	*parentw 	= NULL;
  
  name[0] = 0x00;
  if (objname[0]!=0x00)
  {
    len = strlen((char*)objname);
    for(n = 0; n < len; n++)
    {
      name[n] = objname[n];
      if (objname[n]=='.')
      {
        break;
      }
      if (objname[n+1]==0x00)
      {
        n++;
        break;
      }
    }
  }
  name[n] = 0x00;
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf("_layout_o_as_do_obj on window (%s)\n",(char*)name);
  #endif
  
  parentw = _layout_o_as_getbyname(losalst,(char*)name);
  
  return parentw;
}

/**
 * create the root window 
 */
int _layout_o_as_do_root_window(
  struct tr_Layout *l,
  struct trop_List *losalst
)
{
  struct _layout_o_as_window	*window		= NULL;

  struct _layout_o_as		*losa		= NULL;
  struct trop_List 		*currenta	= NULL;
  
  struct tr_Attr		*attr		= NULL;
  
  if (!losalst||!l) return -1;
  
  currenta = losalst;
  while(currenta)
  {
    losa = (struct _layout_o_as*)currenta->ptr;
    if (!losa) { currenta = currenta->next; continue; }
    
    if (losa->type==_LAYOUT_O_AS_WINDOW)
    {
      window = (struct _layout_o_as_window*)losa->form;
      if (!losa) { currenta = currenta->next; continue; }
      if (
        !losa->gobj &&
        strcmp("none",(char*)window->parent) == 0
      )
      {
        attr = (struct tr_Attr*)malloc(sizeof(struct tr_Attr)); // todo, reimplement attribute parsin
        attr->x = window->x;
        attr->y = window->y;
        attr->w = window->w;
        attr->h = window->h;
        
        losa->gobj = tr_WindowExInit(NULL,NULL,attr);
        l->root = (struct tr_WindowEx*)losa->gobj;
      }
    }
    
    currenta = currenta->next;
  }
  
  if (l->root==NULL) return -1;
  
  return 0;
}

/**
 * create all needed windows 
 */
int _layout_o_as_do_windows(
  struct tr_Layout *l,
  struct trop_List *losalst
)
{
  struct _layout_o_as_window	*window		= NULL;

  struct _layout_o_as		*losa		= NULL;
  struct trop_List 		*currenta	= NULL;

  struct tr_Attr		*attr		= NULL;

  if (!losalst||!l) return -1;
  
  if ( _layout_o_as_do_root_window(l,losalst) != 0 ) return -1;
  
  currenta = losalst;
  while(currenta)
  {
    losa = (struct _layout_o_as*)currenta->ptr;
    if (!losa) { currenta = currenta->next; continue; }
    
    if (losa->type==_LAYOUT_O_AS_WINDOW)
    {
      window = (struct _layout_o_as_window*)losa->form;
      if (!losa) { currenta = currenta->next; continue; }
      if (!losa->gobj)
      {
        attr = (struct tr_Attr*)malloc(sizeof(struct tr_Attr));
        
        attr->x = window->x;
        attr->y = window->y;
        attr->w = window->w;
        attr->h = window->h;
        
        losa->gobj = tr_WindowExInit(NULL,l->root,attr); // todo, make root dynamic
      }
    }
    
    currenta = currenta->next;
  }
  
  tr_WindowMap(l->root->window);
  tr_WindowToTop(l->root->window);
  
  return 0;
}

/**
 * parse the text to be used in GUI objects 
 */
int __parse_gui_text( char *str )
{
  int n;
  
  if (!str) return -1;
  
  for(n = 0; n < strlen(str); n++)
  {
    if (str[n]=='_') str[n] = ' ';
  }
  
  return 0;
}

/**
 * create and process window objects 
 */
int _layout_o_as_do_obj( 
  struct tr_Layout *l,
  struct trop_List *losalst, 
  struct _layout_o_as *losa 
)
{
  struct _layout_o_as		*parent		= NULL;
  struct _layout_o_as		*relay		= NULL; // relay/action
  struct _layout_o_as		*mainw		= NULL; // parent window assoc
  struct _layout_o_as		*root		= NULL; // root window
  
  struct _layout_o_as_window	*window		= NULL;
  struct _layout_o_as_start	*start		= NULL;
  struct _layout_o_as_button	*button		= NULL;
  struct _layout_o_as_lbentry	*lbentry	= NULL;
  struct _layout_o_as_bg	*bg		= NULL;
  struct _layout_o_as_image	*image		= NULL;
  struct _layout_o_as_text	*text		= NULL;
  struct _layout_o_as_textfield	*textf		= NULL;
  struct _layout_o_as_move	*move		= NULL;
  struct _layout_o_as_action	*action		= NULL;
  struct __layout_o_as_general_value
                                *value		= NULL;
  
  struct gui_Widget		*gw		= NULL;
  
  struct tr_WindowEx		*windowex	= NULL;
  struct tr_LBEntry		*trlbentry	= NULL;
  struct tr_Attr		attr;
  struct tr_Attr		*pattr		= NULL;
  struct tr_Action		*traction	= NULL;
  struct tr_ActionSceneSwitch	*tractionsw	= NULL;
  struct tr_Object		*tobj		= NULL;

  char 				name[32];
  int				n, nn;
  int				len;
  int				goid;
  
  if ( !losa || !l || !losalst ) 
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("_layout_o_as_do_obj argument error\n");
    #endif
    return -1;
  }
  if (losa->gobj) return -1; // already initialized
  if (!losa->form)
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("_layout_o_as_do_obj !losa->form\n");
    #endif
    return -1;
  }
  
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf("_layout_o_as_do_obj (%s)\n",(char*)losa->name);
  #endif
 
  parent = _layout_o_as_get_parent(losalst,(char*)losa->name); // direct parent
  mainw = _layout_o_as_get_parentw(losalst,(char*)losa->name); // parent window
  
  if (mainw->gobj)
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("_layout_o_as_do_obj tr_WindowToTop(%s)\n",
        mainw->name);
    #endif
    tr_WindowToTop(((struct tr_WindowEx*)(mainw->gobj))->window);  
  }
    
  switch(losa->type)
  {
    case _LAYOUT_O_AS_START:
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("_layout_o_as_do_obj set start.pre\n");
      #endif
      if (!losa->form) break;
      if (!parent)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("_layout_o_as_do_obj set start. !parent error\n");
        #endif 
        break;
      }
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("_layout_o_as_do_obj set start\n");
      #endif
      start = (struct _layout_o_as_start*)losa->form;
      if (parent->gobj==NULL)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("  _layout_o_as_do_obj (parent->gobj==NULL)\n");
        #endif
        _layout_o_as_do_obj(l,losalst,parent);
      }
      relay = _layout_o_as_getbyname(losalst,(char*)start->id);
      if (!relay)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("     _layout_o_as_do_obj start !relay\n");
        #endif
      }
      else
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("     _layout_o_as_do_obj start relay(%s)\n",relay->name);
        #endif
      }
      if (relay->gobj==NULL)
      {
        _layout_o_as_do_obj(l,losalst,relay);
      }
      windowex = ((struct tr_WindowEx*)(relay->gobj));
      l->start = windowex;
      losa->gobj = windowex;
    break;
    case _LAYOUT_O_AS_BUTTON:
      if (!losa->form) break;
      if (!parent) break;
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("_layout_o_as_do_obj create button\n");
      #endif
      button = (struct _layout_o_as_button*)losa->form;
      if (parent->gobj==NULL)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("  _layout_o_as_do_obj (parent->gobj==NULL)\n");
        #endif
        _layout_o_as_do_obj(l,losalst,parent);
      }
      if (parent->gobj)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("    _layout_o_as_do_obj (parent->gobj) generating button\n");
        #endif
        pattr = (struct tr_Attr*)malloc(sizeof(struct tr_Attr));
        pattr->x = button->pos.x;
        pattr->y = button->pos.y;
        pattr->w = button->pos.w;
        pattr->h = button->pos.h;
        windowex = (struct tr_WindowEx*)(parent->gobj);
        losa->gobj = tr_ButtonInitEx2(NULL,pattr,windowex->canvas);
        if (!losa->gobj)
        {
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("      _layout_o_as_do_obj Could not create Button\n");
          #endif
        }
        tr_SetRound((struct tr_Object*)losa->gobj,2);
        switch(parent->type)
        {
          case _LAYOUT_O_AS_WINDOW:
            #ifdef _DEBUG
              if (DEBUG==TRUE) printf("        _layout_o_as_do_obj Add button to Canvas\n");
            #endif
            if (button->bstatic==FALSE)
            {
              tr_CanvasAddObj(((struct tr_WindowEx*)(parent->gobj))->canvas,(struct tr_Object*)losa->gobj);
              
              struct trop_Rss rss;
              trop_RssInit(&rss);
              trop_RssParseFile(&rss,"topstories.htm");
              form_CanvasAddObjByRss(((struct tr_Object*)(((struct tr_WindowEx*)(parent->gobj))->canvas))->widget.sub,&rss);
            }
            else
              tr_CanvasAddObjStatic(((struct tr_WindowEx*)(parent->gobj))->canvas,losa->gobj);
          break;
        }
      }
    break;
    case _LAYOUT_O_AS_LBENTRY:
      if (!losa->form) break;
      if (!parent) break;
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("_layout_o_as_do_obj create lbentry\n");
      #endif
      lbentry = losa->form;
      if (parent->gobj==NULL)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("  _layout_o_as_do_obj (parent->gobj==NULL)\n");
        #endif
        _layout_o_as_do_obj(l,losalst,parent);
      }
      if (parent->gobj)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("    _layout_o_as_do_obj (parent->gobj) generating lbentry\n");
        #endif
        pattr = (struct tr_Attr*)malloc(sizeof(struct tr_Attr));
        
        pattr->x = lbentry->pos.x;
        pattr->y = lbentry->pos.y;
        pattr->w = lbentry->pos.w;
        pattr->h = lbentry->pos.h;
        
        losa->gobj = tr_LBEntryInit(NULL,pattr,parent->gobj);
        
        switch(parent->type)
        {
          case _LAYOUT_O_AS_WINDOW:
            windowex = (struct tr_WindowEx*)(parent->gobj);
          break;
        }
        
        if (!losa->gobj)
        {
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("      _layout_o_as_do_obj Could not create Button\n");
          #endif
        }
        tr_SetRound(losa->gobj,2);
        switch(parent->type)
        {
          case _LAYOUT_O_AS_WINDOW:
            #ifdef _DEBUG
              if (DEBUG==TRUE) printf("        _layout_o_as_do_obj Add button to Canvas\n");
            #endif
            tr_CanvasAddObj(((struct tr_WindowEx*)(parent->gobj))->canvas,losa->gobj);
          break;
        }
      }
    break;
    case _LAYOUT_O_AS_BG:
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("_layout_o_as_do_obj process color association form(%p) parent(%p)\n",
          losa->form,parent);
      #endif
      if (!losa->form) break;
      if (!parent) break;
      bg = losa->form;
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("_layout_o_as_do_obj process color association (%s) (%s)\n",
          (char*)losa->name,(char*)bg->color);
      #endif
      if (parent->gobj==NULL) // initialize parent object
        _layout_o_as_do_obj(l,losalst,parent);
      if (parent->gobj)
      {
        switch(parent->type)
        {
          case _LAYOUT_O_AS_WINDOW:
            #ifdef _DEBUG
              if (DEBUG==TRUE) printf("_layout_o_as_do_obj tr_SetColor Canvas(%s)\n",(char*)bg->color);
            #endif
            windowex = parent->gobj;
            tr_SetColor(windowex->canvas,(char*)bg->color);
          break;
          default:
            #ifdef _DEBUG
              if (DEBUG==TRUE) printf("_layout_o_as_do_obj tr_SetColor obj(%s) (%s)\n",
                parent->name,(char*)bg->color);
            #endif
            tr_SetColor(parent->gobj,(char*)bg->color);
          break;
        }
      }
      losa->gobj = __dummy;
    break;
    case _LAYOUT_O_AS_IMAGE:
      if (!losa->form) break;
      if (!parent) break;
      image = losa->form;
      pattr = (struct tr_Attr*)malloc(sizeof(struct tr_Attr));
      pattr->x = image->x;
      pattr->y = image->y;
      pattr->w = image->w;
      pattr->h = image->h;
      if (parent->gobj==NULL) // initialize parent object
        _layout_o_as_do_obj(l,losalst,parent);
      if (parent->gobj)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("_layout_o_as_do_obj tr_AddImage (%s)\n",(char*)image->filename);
        #endif
        losa->gobj = tr_ImageInitEx(NULL,pattr,(char*)image->filename);
        switch(parent->type)
        {
          case _LAYOUT_O_AS_WINDOW:
            windowex = parent->gobj;
            tr_AddImage(windowex->canvas,losa->gobj);
          break;
          case _LAYOUT_O_AS_LBENTRY:
            trlbentry = parent->gobj;
            tr_LBEntrySetImage(trlbentry,losa->gobj);
          break;
          default:
            tr_AddImage((struct tr_Object*)(parent->gobj),losa->gobj);
            tr_ImagePosCenter(losa->gobj,(struct tr_Object*)(parent->gobj));
          break;
        }
      }
    break;
    case _LAYOUT_O_AS_ROUND:
      if (!losa->form) break;
      if (!parent) break;
      value = losa->form;
      if (parent->gobj==NULL) // initialize parent object
        _layout_o_as_do_obj(l,losalst,parent);
      if (!parent->gobj) break;
      switch(parent->type)
      {
        default:
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("  _layout_o_as_do_obj tr_SetRound(%d)\n",
              value->value);
          #endif
          tr_SetRound((struct tr_Object*)(parent->gobj),value->value);
        break;
      }
    break;
    case _LAYOUT_O_AS_TEXT:
      if (!losa->form) break;
      if (!parent) break;
      text = losa->form;
      if (parent->gobj==NULL) // initialize parent object
        _layout_o_as_do_obj(l,losalst,parent);
      if (!parent->gobj) break;
      switch(parent->type)
      {
        default:
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("  _layout_o_as_do_obj tr_SetTextEx(%s)\n",
              (char*)text->str);
          #endif
          __parse_gui_text((char*)text->str);
          tr_SetTextEx((struct tr_Object*)(parent->gobj),(char*)text->str,PLACE_CENTER);
        break;
      }
    break;
    case _LAYOUT_O_AS_LONGDESC:
      if (!losa->form) break;
      if (!parent) break;
      text = losa->form;
      if (parent->gobj==NULL) // initialize parent object
        _layout_o_as_do_obj(l,losalst,parent);
      if (!parent->gobj) break;
      switch(parent->type)
      {
        default:
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("  _layout_o_as_do_obj tr_LBEntryConfigure long_desc(%s)\n",
              (char*)text->str);
          #endif
          __parse_gui_text((char*)text->str);
          tr_LBEntryConfigure(parent->gobj,(char*)text->str,NULL);
        break;
      }
    break;
    case _LAYOUT_O_AS_SHORTDESC:
      if (!losa->form) break;
      if (!parent) break;
      text = losa->form;
      if (parent->gobj==NULL) // initialize parent object
        _layout_o_as_do_obj(l,losalst,parent);
      if (!parent->gobj) break;
      switch(parent->type)
      {
        default:
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("  _layout_o_as_do_obj tr_LBEntryConfigure short_desc(%s)\n",
              (char*)text->str);
          #endif
          __parse_gui_text((char*)text->str);
          tr_LBEntryConfigure(parent->gobj,NULL,(char*)text->str);
        break;
      }
    break;
    case _LAYOUT_O_AS_TEXTFIELD:
      if (!losa->form) break;
      if (!parent) break;
      textf = losa->form;
      if (parent->gobj==NULL) // initialize parent object
        _layout_o_as_do_obj(l,losalst,parent);
      if (!parent->gobj) break;
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("  _layout_o_as_do_obj _LAYOUT_O_AS_TEXTFIELD attr(%dx%d+%d+%d)\n",
          textf->w,textf->h,textf->x,textf->y);
      #endif
      
      pattr = (struct tr_Attr*)malloc(sizeof(struct tr_Attr));
      
      pattr->x = textf->x;
      pattr->y = textf->y;
      pattr->w = textf->w;
      pattr->h = textf->h;
      
      switch(parent->type)
      {
        case _LAYOUT_O_AS_WINDOW:
          windowex = parent->gobj;
          losa->gobj = tr_TextInit(NULL,pattr,windowex->canvas);
          if (!losa->gobj)
          {
            #ifdef _DEBUG
              if (DEBUG==TRUE) printf("   _layout_o_as_do_obj _LAYOUT_O_AS_TEXTFIELD (!losa->gobj)\n");
            #endif
          }
          else
          {
            #ifdef _DEBUG
              if (DEBUG==TRUE) printf("   _layout_o_as_do_obj _LAYOUT_O_AS_TEXTFIELD (losa->gobj)\n");
            #endif
          }
          tr_SetColor((struct tr_Object*)(losa->gobj),"white");
          tr_SetRound((struct tr_Object*)(losa->gobj),2);
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("     _layout_o_as_do_obj _LAYOUT_O_AS_TEXTFIELD\n");
          #endif
          if (textf->str[0]!=0x00)
          {
            __parse_gui_text((char*)textf->str);
            form_TextAddText(((struct tr_Object*)(losa->gobj))->widget.sub,(char*)textf->str);
          }
          tr_CanvasAddObj(windowex->canvas,losa->gobj);
        break;
      }
    break;
    case _LAYOUT_O_AS_MOVE:
      if (!losa->form) break;
      if (!parent) break;
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("_layout_o_as_do_obj form_CanvasSetMov\n");
      #endif
      move = losa->form;
      if (parent->gobj==NULL) // initialize parent object
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf(" _layout_o_as_do_obj _LAYOUT_O_AS_MOVE initialize parent (%s)\n",
            (char*)parent->name);
        #endif
        _layout_o_as_do_obj(l,losalst,parent);
      }
      if (!parent->gobj) break;
      switch(parent->type)
      {
        case _LAYOUT_O_AS_WINDOW:
          form_CanvasSetMov(((struct tr_WindowEx*)(parent->gobj))->canvas->widget.sub,move->type);
        break;
      }
    break;
    case _LAYOUT_O_AS_ACTION:
      if (!losa->form) break;
      if (!parent) 
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("_layout_o_as_do_obj _LAYOUT_O_AS_ACTION !parent\n");
        #endif
        break;
      }
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("_layout_o_as_do_obj form_CanvasSetMov\n");
      #endif
      action = losa->form;
      if (parent->gobj==NULL) // initialize parent object
        _layout_o_as_do_obj(l,losalst,parent);
      if (!parent->gobj) break;
      switch(action->atype) // action type
      {
        case TR_ACTION_TYPE_SCENE_SWITCH:
          relay = _layout_o_as_getbyname(losalst,(char*)action->relay);
          if (relay)
          {
            if (relay->gobj==NULL)
              _layout_o_as_do_obj(l,losalst,relay);
            if (relay->gobj)
            {
              if (relay->type==_LAYOUT_O_AS_WINDOW)
              {
                tobj = parent->gobj;
                gw = (struct gui_Widget*)tobj->widget.sub;
                #ifdef _DEBUG
                  if (!gw)
                  {
                    if (DEBUG==TRUE) printf("_layout_o_as_do_obj _LAYOUT_O_AS_ACTION !gw\n");
                  }
                #endif
                
                /*
                  we register our own scene switch objects into the gui_Widget->obj gui Objects 
                */
                
                gw->attr.onclick = tr_doAction;
                
                tractionsw = (struct tr_ActionSceneSwitch*)malloc(sizeof(struct tr_ActionSceneSwitch));
                tractionsw->pscene = relay->gobj;
                tractionsw->scene = NULL; // todo name
                
                traction = (struct tr_Action*)malloc(sizeof(struct tr_Action));
                traction->type = TR_ACTION_TYPE_SCENE_SWITCH;
                traction->sub = tractionsw;
                
                goid = _goid_id_descr_by_myid(TR_GOID_ACTION);
                gui_WidgetAddCustomGobj(
                  gw,
                  goid, 
                  traction, 
                  -1
                );
              }
            }
          }
        break;
      }
    break;
  }
  
  return 0;
}

/**
 * process the list of associated objects 
 */
int _layout_o_as_do( struct tr_Layout *l, struct trop_List *losalst )
{
  struct _layout_o_as		*losa		= NULL;
  struct trop_List 		*currenta	= NULL;

  if (!losalst) return -1;
  
  if ( _layout_o_as_do_windows(l,losalst) != 0 ) return -1;
  
  currenta = losalst;
  while(currenta)
  {
    losa = currenta->ptr;
    if (!losa) { currenta = currenta->next; continue; }
    
    _layout_o_as_do_obj(l,losalst,losa);
    
    currenta = currenta->next;
  }

  if (l->start)
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":layout->start set\n");
    #endif
    tr_WindowToTop(l->start->window);
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":layout->start set.end\n");
    #endif
  }
  
  return 0;
}

/**
 * list the layout objects
 */
int tr_LayoutList(
  struct tr_Layout *l
)
{
  struct trop_List 		*current 	= NULL;
  struct _layout_o_as		*losa		= NULL;
  struct _layout_o_as_window	*window		= NULL;
  struct _layout_o_as_bg	*bg		= NULL;
  struct _layout_o_as_image	*image		= NULL;
  
  if (!l) return -1;
  
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf("tr_LayoutList\n");
  #endif
  
  current = l->losalst;
  while(current)
  {
    losa = current->ptr;
    if (!losa) { current = current->next; continue; }
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("\t_layout_o_as object type(%d)\n",losa->type);
    #endif
    switch(losa->type)
    {
      case _LAYOUT_O_AS_WINDOW:
        window = losa->form;
        if (!window)
        {
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("\t_layout_o_as no form\n");
          #endif
          break;
        }
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("\t_layout_o_as\twindow\n");
        #endif
      break;
      case _LAYOUT_O_AS_BG:
        bg = losa->form;
        if (!bg)
        {
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("\t_layout_o_as no form\n");
          #endif
          break;
        }
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("\t_layout_o_as\tBG\tcolor(%s)\n",
            (char*)bg->color);
        #endif
      break;
      case _LAYOUT_O_AS_IMAGE:
        image = losa->form;
        if (!image)
        {
          break;
        }
      break;
    }
    current = current->next;
  }
  
  return 0;
}

/**
 * initialize the GUI from layout setup 
 */
struct tr_Layout *tr_LayoutInit(
  struct tr_Layout *l,
  char *name
)
{
  FILE			*fd 	= NULL;
  char			filename[256];
  char			content[65536];
  char			line[256];
  
  struct xml_Inst	xml;
  struct xml_Object	*o 		= NULL;
  struct xml_Object	*oo 		= NULL;
  struct xml_Bit	*b 		= NULL;
  
  struct trop_List	*losalst	= NULL;
  struct _layout_o_as	*losa		= NULL;
  
  if (name==NULL) name = "main";
  if (l==NULL)
  {
    l = (struct tr_Layout*)malloc(sizeof(struct tr_Layout));
  }
  
  sprintf((char*)filename,"layout/%s.xml",name);
  
  if ((fd = fopen((char*)filename,"r")) == NULL)
  {
    printf("could not open layout file (layout/%s.xml)\n");
    return NULL;
  }
  
  xml.data = NULL;
  content[0] = 0x00;
  
  __read_file_content(fd,(char*)line,(char*)content);
  
  #ifdef _DEBUG
    if (DEBUG==TRUE)
    {
      printf("tr_LayoutInit content(%s)\n",(char*)content);
    }
  #endif
  
  fclose(fd);
  
  xml.filename[0] = 0x00;
  xml.data = (void*)content;
  xml_Init(&xml);
  
  o = xml.obj;
  while(o)
  {  
    losalst = _layout_o_as_add(losalst,o);    
    o = o->next;
  }
 
  l->root = NULL;
  l->start = NULL;
  l->losalst = losalst;
  tr_LayoutList(l);
  _layout_o_as_do(l,losalst);
  
  #ifdef _DEBUG
    if (DEBUG==TRUE)
    {
      printf("tr_LayoutInit end\n");
    }
  #endif
  
  return l;
}

int tr_WindowQueue( struct tr_Window *w )
{
  if (!w)
  {
    printf("err: window queue on non-existing object\n");
    return 0;
  }
  
  if (!w->sub)
  {
    printf("err: window queue on object with no parent\n");
  }
  
  if (w->sub->parent != NULL)
  {
    printf("err: starting event loop on non top level window\n");
    return -1;
  }
  gui_WidgetQueue(w->sub);
  while(1) { sleep(1); }
  return 0;
}

int tr_WindowAddWidget( struct tr_Window *win, struct tr_Widget *wid )
{
  if (!win || !wid) return -1;
  trop_ListAdd(win->widgets,wid);
  return 0;
}

int tr_WindowExAddObj(
  struct tr_WindowEx *w,
  struct tr_Object *o
)
{
  if (!w||!o) return -1;
  tr_CanvasAddObj(w->canvas,o);
  return 0;
}

int tr_WindowExAddObjStatic(
  struct tr_WindowEx *w,
  struct tr_Object *o
)
{
  if (!w||!o) return -1;
  tr_CanvasAddObjStatic(w->canvas,o);
  return 0;
}

/**
 * free data associated with a window
 */
int tr_WindowFree( struct tr_Window *w )
{
  if (!w) return -1;
  if (w->sub) free(w->sub);
  free(w);
  return 0;
}

int tr_WindowDestroy( struct tr_Window *w )
{
  if (!w) return -1;
  lstWindows = trop_ListDeleteMember(lstWindows,w);
  tr_WindowFree(w);
  return 0;
}

int tr_WindowMap( struct tr_Window *w )
{
  if (!w) return -1;
  if (!w->attr) return -1;
  w->attr->bvisible = TRUE;
  return 0;
}

int tr_WindowUnmap( struct tr_Window *w )
{
  if (!w) return -1;
  if (!w->attr) return -1;
  w->attr->bvisible = FALSE;
  return 0;
}

int tr_WindowMove( struct tr_Window *w, int x, int y )
{
  if (!w) return -1;
  if (!w->attr) return -1;
  w->attr->x = x;
  w->attr->y = y;
  return 0;
}

int tr_WindowMoveBy( struct tr_Window *w, int x, int y )
{
  if (!w) return -1;
  if (!w->attr) return -1;
  w->attr->x += x;
  w->attr->y += y;
  return 0;
}

int tr_WindowResize( struct tr_Window *win, int w, int h )
{
  if (!win) return -1;
  if (!win->attr) return -1;
  win->attr->w = w;
  win->attr->h = h;
  return 0;
}

int tr_WindowToTop( struct tr_Window *w )
{
  struct trop_List *current 	= NULL;
  struct tr_Window *wcurrent 	= NULL;
  
  if (!w) 
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("tr_WindowToTop !w error\n");
    #endif
    return -1; 
  }
  if (!w->attr) 
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("tr_WindowToTop !w->attr error\n");
    #endif
    return -1; 
  }
  
  current 			= lstWindows;
  wcurrent 			= NULL;
  
  while(current)
  {
    wcurrent = current->ptr;
    wcurrent->attr->btop = FALSE;
    current = current->next;
  }
  w->attr->btop = TRUE;
  gui_SceneSet(w->scene);
  return 0;
}

int tr_WindowExToTop( struct tr_WindowEx *w )
{
  if (!w) return -1;
  return tr_WindowToTop(w->window);
}

/** 
 * get Window matching name stored in it's Widget body 
 */
struct tr_Window *tr_WindowNamedGet( char *name )
{
  struct trop_List *current = NULL;
  struct tr_Window *window = NULL;
  char *_name = NULL;
  
  if (!name) return NULL;
  
  current = lstWindows;
  while(current)
  {
    window = (struct tr_Window*)current->ptr;
    if (window!=NULL)
    {
      _name = gui_WidgetGetName(window->sub);
      if (_name!=NULL)
      {
        if (strlen(_name)==strlen(name))
        {
          if (strcmp(_name,name)==0)
          {
            return window;
          }
        }
      }
    }
    current = current->next;
  }
  return NULL;
}

int tr_WindowNamedToTop( char *name )
{
  return tr_WindowToTop(tr_WindowNamedGet(name));
}

int tr_WindowExpose( struct tr_Window *w )
{
  if (w==NULL) return -1;
  gui_TriggerExpose(gui_GetMain());
  return 0;
}

int _set_gui_object_attr( struct gui_Widget *w, struct tr_Attr *attr )
{
  w->attr.x = attr->x;
  w->attr.y = attr->y;
  w->attr.w = attr->w;
  w->attr.h = attr->h;
  return 0;
}

/**
 * get a window to expose, currently we only expose the
 * top window
 */
struct tr_Window *tr_WindowExposable( )
{
  struct trop_List *current = lstWindows;
  struct tr_Window *wcurrent = NULL;
  while(current)
  {
    wcurrent = (struct tr_Window*)current->ptr;
    if (wcurrent->attr->btop == TRUE)
      return wcurrent;
    current = current->next;
  }
  return NULL;
}

int tr_SetName(
  struct tr_Object *o,
  char *name
)
{
  int ret = 0;
  if (!o||!name) return -1;
  ret = gui_WidgetSetName(o->widget.sub,name);
  return ret;
}

char *tr_GetName(
  struct tr_Object *o
)
{
  char *name = NULL;
  if (!o) return NULL;
  name = gui_WidgetGetName(o->widget.sub);
  return name;
}

/**
 * set parent of widget object
 */
int tr_SetParent( struct tr_Object *o, struct tr_Object *parent )
{
  if (!o||!parent) return -1;
  
  o->widget.sub = (struct gui_Widget*)parent->widget.sub;
  
  return 0;
}

int tr_ObjectRelease( struct tr_Object *o )
{
  if (!o) return -1;
  
  switch(o->widget.type)
  {
    // the objects, which have wdata associated to them:
    case TWT_IMAGE:
      tr_ImageRelease(o);
      break;
    case TWT_BUTTON:
      tr_ButtonRelease(o);
      break;
    default:
      tr_ObjectReleaseSub(o);
      break;
  }
  
  return 0;
}

/**
 *  release the gui_Widget associated to tr_Object
 */
int tr_ObjectReleaseSub( struct tr_Object *o )
{
  if (!o) return -1;
  gui_WidgetReleaseEx((struct gui_Widget*)o->widget.sub);
  return 0;
}

struct tr_Object *tr_ListboxInit( struct tr_Object *lb, struct tr_Attr *attr, struct tr_Window *parent )
{
  if (!parent||!attr) return NULL;
  #ifdef _DEBUG
    printf("tr_ListboxInit\n");
  #endif
  if (!lb)
  {
    lb = (struct tr_Object*)malloc(sizeof(struct tr_Object));
    lb->widget.wdata = (struct tr_Listbox*)malloc(sizeof(struct tr_Listbox));
    lb->widget.attr = attr;
    lb->widget.type = TWT_LISTBOX;
  }
  lb->widget.sub = gui_NewWidget();
  _set_gui_object_attr((struct gui_Widget*)lb->widget.sub,attr);
  lb->widget.sub->attr.form = FORM_LISTBOX;
  
  gui_WidgetInit(lb->widget.sub,gui_GetMain());
  gui_WidgetMap(lb->widget.sub);
  form_ListboxInit(lb->widget.sub);
  
  return lb;
}

struct tr_Object *tr_ImageInit( struct tr_Object *image, struct tr_Attr *attr, char *filename )
{
  struct gui_Image *gi = NULL;
  if (image == NULL)
  {
    image = (struct tr_Object*)malloc(sizeof(struct tr_Object));
    image->widget.wdata = (struct tr_WidgetImage*)malloc(sizeof(struct tr_WidgetImage));
    image->widget.attr = attr;
    image->widget.type = TWT_IMAGE;
  }
  gi = &((struct tr_WidgetImage*)image->widget.wdata)->sub;
  strcpy((char*)gi->filename,filename);
  return image;
}

struct tr_Object *tr_ImageInitEx( struct tr_Object *image, struct tr_Attr *attr, char *filename )
{
  struct gui_Image *gi = NULL;
  if (image == NULL)
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":tr_ImageInitEx alloc tr_Object for image\n");
    #endif
    image = (struct tr_Object*)malloc(sizeof(struct tr_Object));
    image->widget.wdata = (struct tr_WidgetImage*)malloc(sizeof(struct tr_WidgetImage));
    image->widget.attr = attr;
    image->widget.type = TWT_IMAGE;
  }
  gi = &((struct tr_WidgetImage*)image->widget.wdata)->sub;
  
  strcpy((char*)gi->filename,filename);
  if ( gui_CreateImage(gui_GetMain(),
    &((struct tr_WidgetImage*)image->widget.wdata)->sub) )
  {
    printf("Couldn't register image (%s)\n",filename);
    return NULL;
  }
  gi->x = image->widget.attr->x;
  gi->y = image->widget.attr->y;
  return image;
}

int tr_ImageRelease( struct tr_Object *image )
{
  struct gui_Image *gi = NULL;
  
  if (!image) return -1;
  gi = &((struct tr_WidgetImage*)image->widget.wdata)->sub;
  gui_ImageFreeContent(gi);
  free(image->widget.wdata);
  image->widget.wdata = NULL;
  free(image);
  return 0;
}

/**
 * add image to object 
 */
int tr_AddImage( struct tr_Object *parent, struct tr_Object *image )
{
  gui_WidgetAddImage(parent->widget.sub,&((struct tr_WidgetImage*)image->widget.wdata)->sub);
  return 0;
}

/**
 * position an image to an object (relative in an object) 
 */
int tr_ImagePosCenter( struct tr_Object *image, struct tr_Object *parent )
{
  gui_ImagePosCenter(&((struct tr_WidgetImage*)image->widget.wdata)->sub,parent->widget.sub);
  return 0;
}

int tr_ImagePosLeft( struct tr_Object *image, struct tr_Object *parent )
{
  gui_ImagePosLeft(&((struct tr_WidgetImage*)image->widget.wdata)->sub,parent->widget.sub);
  return 0;
}

int tr_WindowAddImage( struct tr_Window *w, struct tr_Object *image )
{
  struct gui_Image *gi = NULL;
  gi = &((struct tr_WidgetImage*)image->widget.wdata)->sub;
  gui_CreateImage(gui_GetMain(),&((struct tr_WidgetImage*)image->widget.wdata)->sub);
  gi->x = image->widget.attr->x;
  gi->y = image->widget.attr->y;
  gui_WidgetAddImage(w->sub,&((struct tr_WidgetImage*)image->widget.wdata)->sub);
  return 0;
}

struct tr_Object *tr_ButtonInit( struct tr_Object *button, struct tr_Attr *attr )
{
  if (button == NULL)
  {
    button = (struct tr_Object*)malloc(sizeof(struct tr_Object));
    button->widget.wdata = (struct tr_WidgetButton*)malloc(sizeof(struct tr_WidgetButton));
    button->widget.attr = (struct tr_Attr*)malloc(sizeof(struct tr_Attr));
    memcpy(button->widget.attr,attr,sizeof(struct tr_Attr));
    button->widget.type = TWT_BUTTON;
  }
  button->widget.sub = gui_NewWidget( );
  _set_gui_object_attr((struct gui_Widget*)button->widget.sub,attr);
  button->widget.sub->attr.form = FORM_BUTTON;
  return button;
}

struct tr_Object *tr_ButtonInitEx( struct tr_Object *button, struct tr_Attr *attr, 
  struct tr_Window *parent )
{
  button = tr_ButtonInit(button,attr);
  if (tr_WindowAddButton(parent,button)!=0) return NULL;
  return button;
}

int tr_ButtonRelease( struct tr_Object *button )
{
  if (!button) return -1;
  
  free(button->widget.attr);
  button->widget.attr = NULL;
  
  free(button->widget.wdata);
  button->widget.wdata = NULL;
  
  tr_ObjectReleaseSub( button );
  
  return 0;
}

/**
 * initialize button to parent object, not in window
 */
struct tr_Object *tr_ButtonInitEx2( struct tr_Object *button, struct tr_Attr *attr, 
  struct tr_Object *parent )
{
  button = tr_ButtonInit(button,attr);
  gui_WidgetInit(button->widget.sub,parent->widget.sub);
  gui_WidgetMap(button->widget.sub);
  #ifdef _USE_SKIN
    struct mikaSkinNode *node = mikaSkinGlobalGetNode((unsigned char*)"mikaButton");
    tr_SetColor(button,(char*)node->color.hex);
  #endif
  return button;
}

/**
 * multiline text 
 */
struct tr_Object *tr_TextInit( struct tr_Object *text, struct tr_Attr *attr,
  struct tr_Object *parent )
{
  if (text == NULL)
  {
    text = (struct tr_Object*)malloc(sizeof(struct tr_Object));
    text->widget.wdata = (struct tr_WidgetText*)malloc(sizeof(struct tr_WidgetText));
    text->widget.attr = attr;
    text->widget.type = TWT_TEXT;
  }
  text->widget.sub = gui_NewWidget( );
  _set_gui_object_attr((struct gui_Widget*)text->widget.sub,attr);
  text->widget.sub->attr.form = FORM_TEXT;
  
  gui_WidgetInit(text->widget.sub,parent->widget.sub);
  gui_WidgetMap(text->widget.sub);
  form_TextInit(text->widget.sub);
  
  return text;
}

/**
 * keyboard widget 
 */
struct tr_Object *tr_KeyboardInit( struct tr_Object *kbd, struct tr_Attr *attr,
  struct tr_Object *parent )
{
  if (kbd == NULL)
  {
    kbd = (struct tr_Object*)malloc(sizeof(struct tr_Object));
    kbd->widget.wdata = (struct tr_WidgetKeyboard*)malloc(sizeof(struct tr_WidgetKeyboard));
    kbd->widget.attr = attr;
    kbd->widget.type = TWT_KEYBOARD;
  }
  kbd->widget.sub = gui_NewWidget( );
  _set_gui_object_attr((struct gui_Widget*)kbd->widget.sub,attr);
  kbd->widget.sub->attr.form = FORM_KEYBOARD;
  
  gui_WidgetInit(kbd->widget.sub,parent->widget.sub);
  gui_WidgetMap(kbd->widget.sub);
  form_KeyboardInit(kbd->widget.sub);
  
  return kbd;
}

struct tr_Object *tr_MenuInit( struct tr_Object *menu, struct tr_Attr *attr,
  struct tr_Window *parent )
{
  if (menu == NULL)
  {
    menu = (struct tr_Object*)malloc(sizeof(struct tr_Object));
    menu->widget.wdata = (struct tr_WidgetMenu*)malloc(sizeof(struct tr_WidgetMenu));
    menu->widget.attr = attr; // todo, rather make copy
    menu->widget.type = TWT_MENU;
  }
  menu->widget.sub = gui_NewWidget( );
  _set_gui_object_attr((struct gui_Widget*)menu->widget.sub,attr);
  menu->widget.sub->attr.form = FORM_MENU;
  
  gui_WidgetInit(menu->widget.sub,gui_GetMain());
  gui_WidgetMap(menu->widget.sub);
  form_MenuInit(menu->widget.sub);
  //gui_WidgetSetColor(menu->widget.sub,TR_DEFA_WTBG);
  
  return menu;
}

int tr_MenuSetGrid( struct tr_Object *menu, int gx, int gy )
{
  if (!menu||gx<0||gy<0) return -1;
  form_MenuSetGrid(menu->widget.sub,gx,gy);
  return 0;
}

/**
 * add button to menu, using default grid calculations 
 */
int tr_MenuAddButton( struct tr_Object *menu, struct tr_Object *button )
{ 
  if (!menu || !button) return -1;
  form_MenuAddButton(menu->widget.sub,button->widget.sub);
  return 0;
}

/**  initialize scrollbar
 */
struct tr_Object *tr_ScrollInit( struct tr_Object *scroll, struct tr_Attr *attr,
  struct tr_Window *parent )
{
  if (scroll == NULL)
  {
    scroll = (struct tr_Object*)malloc(sizeof(struct tr_Object));
    scroll->widget.wdata = (struct tr_WidgetScroll*)malloc(sizeof(struct tr_WidgetScroll));
    scroll->widget.attr = attr;
    scroll->widget.type = TWT_SCROLL;
  }
  scroll->widget.sub = gui_NewWidget( );
  _set_gui_object_attr((struct gui_Widget*)scroll->widget.sub,attr); // remove
  
  scroll->widget.sub->attr.form = FORM_SLIDEBAR;
  
  gui_WidgetInit(scroll->widget.sub,gui_GetMain());
  form_SlidebarSetGeometry(gui_GetMain(),scroll->widget.sub);
  scroll->widget.sub->attr.y = gui_GetMain()->attr.h/4;
  scroll->widget.sub->attr.h = gui_GetMain()->attr.h/2;
  form_SlidebarInit(gui_GetMain(),scroll->widget.sub);
  
  gui_WidgetMap(scroll->widget.sub);
  gui_WidgetSetColor(scroll->widget.sub,TR_DEFA_WTBG);
  return scroll;
}

int tr_PanelAddButton( struct tr_Object *panel, struct tr_Object *button )
{
  return 0;
}

/**
 * create panel widget, which in the gui layer is a menu handled by us 
 * pos = positioning with TR_POS_*
 */
struct tr_Object *_tr_panel_init( struct tr_Object *panel, struct tr_Attr *attr,
  struct tr_Window *parent, int pos )
{
  if (pos!=-1)
  {
    if (attr==NULL)
      attr = (struct tr_Attr*)malloc(sizeof(struct tr_Attr*));
    if (pos == TR_POS_BOTTOM)
    {
      attr->h = gui_GetMain()->attr.h/10;
      attr->w = gui_GetMain()->attr.h*0.8;
      attr->x = (gui_GetMain()->attr.w-attr->w)/2;
      attr->y = gui_GetMain()->attr.h-attr->h;
    }
    else if (pos == TR_POS_TOP)
    {
      attr->h = gui_GetMain()->attr.h/10;
      attr->w = gui_GetMain()->attr.h*0.8;
      attr->x = (gui_GetMain()->attr.w-attr->w)/2;
      attr->y = 0;
    }
    else if (pos == TR_POS_LEFT)
    {
      attr->h = gui_GetMain()->attr.h*0.8;
      attr->w = gui_GetMain()->attr.h/10;
      attr->x = 0;
      attr->y = (gui_GetMain()->attr.w-attr->w)/2;
    }
    else if (pos == TR_POS_RIGHT)
    {
      attr->h = gui_GetMain()->attr.h*0.8;
      attr->w = gui_GetMain()->attr.h/10;
      attr->x = gui_GetMain()->attr.w-attr->w;
      attr->y = (gui_GetMain()->attr.w-attr->w)/2;
    }
  }
  if (panel == NULL)
  {
    panel = (struct tr_Object*)malloc(sizeof(struct tr_Object));
    panel->widget.wdata = (struct tr_WidgetPanel*)malloc(sizeof(struct tr_WidgetPanel));
    panel->widget.attr = attr;
    panel->widget.type = TWT_PANEL;
  }
  panel->widget.sub = gui_NewWidget( );
  _set_gui_object_attr((struct gui_Widget*)panel->widget.sub,attr);
  panel->widget.sub->attr.form = FORM_MENU;
  
  gui_WidgetInit(panel->widget.sub,gui_GetMain());
  gui_WidgetMap(panel->widget.sub);
  form_MenuInit(panel->widget.sub);
  return panel;
}

struct tr_Object *tr_PanelInit( struct tr_Object *panel, struct tr_Attr *attr,
  struct tr_Window *parent )
{
  return _tr_panel_init(panel,attr,parent,-1);
}

struct tr_Object *tr_PanelInitEx( struct tr_Object *panel, struct tr_Attr *attr,
  struct tr_Window *parent, int pos )
{
  return _tr_panel_init(panel,attr,parent,pos);
}

int tr_CanvasMove( struct tr_Object *can, int move )
{
  if (!can) return -1;
  form_CanvasMove(can->widget.sub,move);
  return 0;
}

/**
 *  remove objects of canvas recursivelly
 */
int tr_CanvasRemoveAllObj( struct tr_Object *can )
{
  if (!can) return -1;
  form_CanvasRemoveAllObj(can->widget.sub);
  return 0;
}

int tr_CanvasRemoveAllObjRefType( struct tr_Object *can, int type )
{
  if (!can) return -1;
  form_CanvasRemoveAllObjRefType(can->widget.sub,type);
  return 0;
}

/**
 * imput field 
 */
struct tr_Object *tr_EditableInit( struct tr_Object *edit, struct tr_Attr *attr,
  struct tr_Object *parent )
{
  if (!parent) return NULL;
  if (edit==NULL)
  {
    edit = (struct tr_Object*)malloc(sizeof(struct tr_Object));
    edit->widget.wdata = (struct tr_WidgetEditable*)malloc(sizeof(struct tr_WidgetEditable));
    edit->widget.attr = attr;
    edit->widget.type = TWT_EDITABLE; 
  }
  edit->widget.sub = gui_NewWidget( );
  _set_gui_object_attr((struct gui_Widget*)edit->widget.sub,attr);
  edit->widget.sub->attr.form = FORM_EDITABLE;
  
  gui_WidgetInit(edit->widget.sub,gui_GetMain());
  gui_WidgetMap(edit->widget.sub);
  form_EditableInit(edit->widget.sub);
  return edit;
}

int tr_EditableSetValue( struct tr_Object *edit, char *value )
{
  static struct form_Editable *fedit;
  
  if (!edit||!value) return -1;
  
  fedit = (struct form_Editable*)edit->widget.sub->form;
  fedit->vlen = strlen(value);
  strcpy((char*)fedit->value,value);
  
  return 0;
}

/** 
 * Listbox Entry initialize
 */
struct tr_Object *tr_LBEntryInit( struct tr_Object *lbi, struct tr_Attr *attr, struct tr_Window *parent )
{
  if (!parent) return NULL;
  if (lbi==NULL)
  {
    lbi = (struct tr_Object*)malloc(sizeof(struct tr_Object));
    lbi->widget.wdata = (struct tr_WidgetListboxItem*)malloc(sizeof(struct tr_WidgetListboxItem));
    lbi->widget.attr = attr;
    lbi->widget.type = TWT_LBENTRY; 
  }
  lbi->widget.sub = gui_NewWidget( );
  _set_gui_object_attr((struct gui_Widget*)lbi->widget.sub,attr);
  lbi->widget.sub->attr.form = FORM_LBENTRY;
  
  gui_WidgetInit(lbi->widget.sub,gui_GetMain());
  gui_WidgetMap(lbi->widget.sub);
  form_LBEntryInit(lbi->widget.sub);
  return lbi;
}

int tr_LBEntryConfigure( struct tr_Object *lbi, char *str_big, char *str_small )
{
  static struct form_ListboxEntry *e = NULL;
  if (!lbi) return -1;
  form_LBEntryConfigure((struct gui_Widget*)(lbi->widget.sub),str_big,str_small);
  return 0;
}

int tr_LBEntrySetImage( struct tr_Object *lbi, struct tr_Object *image )
{
  if (!lbi||!image) return -1;
  form_LBEntrySetImage(lbi->widget.sub,&((struct tr_WidgetImage*)image->widget.wdata)->sub);
  return 0;
}

/**
 * add object to canvas 
 */
int tr_CanvasAddObj( struct tr_Object *can, struct tr_Object *obj )
{
  if (!can || !obj) return -1;
  static int ret = 0;
  ret = form_CanvasAddObject(can->widget.sub,obj->widget.sub);
  return ret;
}

/** exchange a static widget in a canvas
 */
int tr_CanvasExchangeObjStatic( struct tr_Object *can, struct tr_Object *original, struct tr_Object *obj )
{
  #ifdef _DEBUG
    printf("tr_CanvasExchangeObjStatic\n");
  #endif
  if (!can || !original || !obj)
  {
    #ifdef _DEBUG
      printf("tr_CanvasExchangeObjStatic error, invalid arguments\n");
    #endif
    return 1;
  }
  int ret = form_CanvasExchangeObjectStatic(can->widget.sub,original->widget.sub,obj->widget.sub);
  return ret;
}

int tr_CanvasAddObjStatic( struct tr_Object *can, struct tr_Object *obj )
{
  if (!can || !obj) return -1;
  static int ret = 0;
  ret = form_CanvasAddObjectStatic(can->widget.sub,obj->widget.sub);
  return ret;
}

/**
 * initialize canvas
 */
struct tr_Object *tr_CanvasInit( struct tr_Object *can, struct tr_Attr *attr, struct tr_Window *parent )
{
  if (can==NULL)
  {
    can = (struct tr_Object*)malloc(sizeof(struct tr_Object));
    can->widget.wdata = (struct tr_WidgetCanvas*)malloc(sizeof(struct tr_WidgetCanvas));
    can->widget.attr = attr;
    can->widget.type = TWT_CANVAS;
  }
  can->widget.sub = gui_NewWidget( );
  _set_gui_object_attr((struct gui_Widget*)can->widget.sub,attr);
  can->widget.sub->attr.form = FORM_CANVAS;
  
  gui_WidgetInit(can->widget.sub,gui_GetMain());
  gui_WidgetMap(can->widget.sub);
  form_CanvasInit(can->widget.sub);
  #ifdef _USE_SKIN
    struct mikaSkinNode *node = mikaSkinGlobalGetNode((unsigned char*)"mikaCanvas");
    if (node)
      tr_SetColor(can,(char*)node->color.hex);
  #endif
  return can;
}

int tr_CanvasSetMov( struct tr_Object *can, int movtype )
{
  return form_CanvasSetMov(can->widget.sub,movtype);
}

int tr_ScrollAddListener( struct tr_Object *scroll, struct tr_Object *listener )
{
  if (!scroll || !listener) return -1;
  if (listener->widget.type != TWT_MENU)
  {
    printf("err: cannot add type(%d) as listener\n",listener->widget.type);
    return -1;
  }
  form_SlidebarSetRelay(scroll->widget.sub,listener->widget.sub);
  return 0;
}

int tr_ButtonSetText( struct tr_Object *button, char *str )
{
  if (!button || !str) return -1;
  gui_WidgetAddTextSimple((struct gui_Widget*)button->widget.sub,str);
  return 0;
}

/**
 * add text to default object type 
 */
int tr_SetText( struct tr_Object *parent, char *str )
{
  if (!parent || !str) return -1;
  gui_WidgetAddTextSimple((struct gui_Widget*)parent->widget.sub,str);
  return 0;
}

/** add text to object at a position
 */
int tr_SetTextEx( struct tr_Object *parent, char *str, int placement )
{
  if (!parent || !str) return -1;
  gui_WidgetAddTextSimpleC(
    (struct gui_Widget*)parent->widget.sub,
    str,
    placement);
  return 0;
}

/** change the text at a position
 */
int tr_ChangeTextEx( struct tr_Object *parent, char *str, int placement )
{
  if (!parent || !str)
  {
    #ifdef _DEBUG
      printf("tr_ChangeTextEx ERROR invalid arguments\n");
    #endif
    return -1;
  }
  gui_WidgetChangeText(
    (struct gui_Widget*)parent->widget.sub,
    str,placement
  );
  return 0;
}

/** change the text at a position
 */
int tr_ChangeTextExPos( struct tr_Object *parent, char *str, int x, int y )
{
  if (!parent || !str)
  {
    #ifdef _DEBUG
      printf("tr_ChangeTextExPos ERROR invalid arguments\n");
    #endif
    return -1;
  }
  gui_WidgetChangeTextPos(
    (struct gui_Widget*)parent->widget.sub,
    str,x,y
  );
  return 0;
}

int tr_SetRound( struct tr_Object *parent, int round )
{
  if (!parent || round<0) return -1;
  gui_WidgetSetRound(parent->widget.sub,round);
  return 0;
}

BOOL tr_IsRound( struct tr_Object *parent )
{
  if (!parent) return FALSE;
  return gui_WidgetIsRound(parent->widget.sub);
}

int tr_SetColor( struct tr_Object *parent, char *color )
{
  if (!parent || !color) return -1;
  gui_WidgetSetColor(parent->widget.sub,color);
  return 0;
}

int tr_SetColor2( struct tr_Object *parent, char *color )
{
  if (!parent || !color) return -1;
  gui_WidgetSetColor2(parent->widget.sub,color);
  return 0;
}

int tr_SetColorEx( struct tr_Object *parent, char *color )
{
  return 0;
}

int tr_ResetColor( struct tr_Object *parent )
{
  gui_WidgetResetColor(parent->widget.sub);
  return 0;
}

/**
 * set custom attributes associated with a widget object 
 */
int tr_SetCustomValue( struct tr_Object *parent, int value )
{
  if (!parent) return -1;
  gui_WidgetAddCustomValue(parent->widget.sub,value);
  return 0;
}

int tr_SetCustomPtr( struct tr_Object *parent, void *ptr )
{
  if (!parent||!ptr) return -1;
  gui_WidgetAddCustomPtr(parent->widget.sub,ptr);
  return 0;
}

/**
 * change alpha of image 'applied directly, not through setting' 
 */
int tr_ImageChangeAlpha( struct tr_Object *image, int value )
{
  if (!image) return -1;
  gui_ImageMoveAlpha(&((struct tr_WidgetImage*)image->widget.wdata)->sub,0,value);
  return 0;
}

int tr_ImageSetRound( struct tr_Object *image, int value )
{
  if (!image) return -1;
  gui_ImageSetRound(&((struct tr_WidgetImage*)image->widget.wdata)->sub,value);
  return 0;
}

/**
 * add a gobj that modiffies the alpha of the widget 'when exposing, slow'
 */
int tr_SetBgAlpha( struct tr_Object *parent, int value )
{
  if (!parent) return -1;
  if (
    parent->widget.type != TWT_BUTTON &&
    parent->widget.type != TWT_MENU 
    ) return -1;
  gui_WidgetSetBgAlpha(parent->widget.sub,value);
  return 0;
}

int tr_ButtonSetImage( struct tr_Object *button, struct tr_Object *image )
{
  if (!button || !image) return -1;
  gui_WidgetAddImage(button->widget.sub,&((struct tr_WidgetImage*)image->widget.wdata)->sub);
  return 0;
}

int tr_ButtonSetAction( struct tr_Object *button, void *(*action)(void*data) )
{
  if (!button || !action) return -1;
  button->widget.onclick = action;
  button->widget.sub->attr.onclick = action;
  return 0;
}

int tr_SetAction( struct tr_Object *parent, void *(*action)(void*data), int type )
{
  if (!parent || !action) return -1;
  
  switch(type)
  {
    case TR_ACTION_BUTTON:
      parent->widget.onclick = action;
      parent->widget.sub->attr.onclick = action;
      break;
    case TR_ACTION_BUTTONDOWN:
      parent->widget.onbuttondown = action;
      parent->widget.sub->attr.onbuttondown = action;
      break;
    case TR_ACTION_BUTTONUP:
      parent->widget.onbuttonup = action;
      parent->widget.sub->attr.onbuttonup = action;
      break;
  }

  return 0;
}

/**
 * type 		- physical action (button press, etc)
 */
int tr_SetActionEx(
  struct tr_Object *parent,
  void *(*action)(void*data),
  void *actiondata,
  int type
)
{
  if (!parent || !action) return -1;
  
  switch(type)
  {
    case TR_ACTION_BUTTON:
      parent->widget.onclick = action;
      parent->widget.sub->attr.onclick = action;
      break;
    case TR_ACTION_BUTTONDOWN:
      parent->widget.onbuttondown = action;
      parent->widget.sub->attr.onbuttondown = action;
      break;
    case TR_ACTION_BUTTONUP:
      parent->widget.onbuttonup = action;
      parent->widget.sub->attr.onbuttonup = action;
      break;
  }
  return 0;
}

int tr_PutObject(
  struct tr_Object *parent,
  struct tr_Object *o
)
{
  if (!parent||!o) return -1;
  
  gui_WidgetAddWidget(parent->widget.sub,o->widget.sub);
  
  return 0;
}

/*
  add an object into the gui_Widget obj pool 
*/
int tr_AddGObject(
  
)
{
  return 0;
}

int tr_WindowAddButton( struct tr_Window *w, struct tr_Object *button )
{
  gui_WidgetInit(button->widget.sub,w->sub);
  gui_WidgetSetColor(button->widget.sub,TR_DEFA_BTBG);
  gui_WidgetMap(button->widget.sub);
  return 0;
}

int tr_RelayAdd( struct tr_Object *o, struct tr_Object *r )
{
  gui_WidgetAddRelayEx(o->widget.sub,r->widget.sub);
  return 0;
}

int tr_SetLevel( struct tr_Object *o, int level )
{
  if (!o) return -1;
  
  ((struct gui_Widget*)o->widget.sub)->attr.level = level;
  
  return 0;
}

int tr_SetPositionRelative( 
  struct tr_Object *o,
  struct tr_Object *parent,
  double dx, double dy,
  double dw, double dh
  )
{
  if (!o||!parent) return -1;
  
  gui_WidgetSetPositionRelative(
    (struct gui_Widget*)o->widget.sub,
    (struct gui_Widget*)parent->widget.sub,
    dx, dy,
    dw, dh
  );
  
  return 0;
}

int tr_SetFontColor( 
  struct tr_Object *o, 
  unsigned char r, 
  unsigned char g,
  unsigned char b,
  unsigned char a
  )
{
  if (!o) return -1;
  if (!o->widget.sub) return -1;
  
  struct gui_Widget *w = o->widget.sub;
  if (w->glfont)
  {
    w->glfont->color.rgba[0] = r;
    w->glfont->color.rgba[1] = g;
    w->glfont->color.rgba[2] = b;
    w->glfont->color.rgba[3] = a;
  }
  
  return 0;
}

int tr_SetFontColorEx( 
  struct tr_Object *o, 
  char *color
  )
{
  if (!o) return -1;
  if (!o->widget.sub) return -1;
  
  struct gui_Widget *w = o->widget.sub;
  if (w->glfont)
  {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
  
    _hex2unsignedChar(color+1,&r,&g,&b,&a);
  
    w->glfont->color.rgba[0] = r;
    w->glfont->color.rgba[1] = g;
    w->glfont->color.rgba[2] = b;
    w->glfont->color.rgba[3] = a;
  }
  
  return 0;
}

/** set the font size
 */
int tr_SetFontSize(
  struct tr_Object *o,
  int size
)
{
  if (!o) return -1;
  if (!o->widget.sub) return -1;
  
  struct gui_Widget *w = o->widget.sub;
  if (w->glfont)
  {
    w->glfont->height = size;
  }
  
  return 0;
}

/** change visibility of a widget
 */
int tr_ToggleVisibility( struct tr_Object *o, BOOL visibility )
{
  if (!o)
  {
    #ifdef _DEBUG
      printf("tr_ToggleVisibility error invalid arguments\n");
    #endif
  }
  
  struct gui_Widget *w = o->widget.sub;
  w->attr.bvisible = visibility;
  
  return 0;
}

/** initialize a Slidebar object
 */
struct tr_Object *tr_SlidebarInit( struct tr_Object *slidebar, struct tr_Attr *attr, struct tr_Object *parent )
{
  if (!attr)
  {
    #ifdef _DEBUG
      printf("tr_SlidebarInit error tr_Attr missing\n");
    #endif
  }
  #ifdef _DEBUG
    printf("tr_SlidebarInit\n");
  #endif
  if (!slidebar)
  {
    slidebar = (struct tr_Object*)malloc(sizeof(struct tr_Object));
    slidebar->widget.wdata = (struct tr_WidgetSlidebar*)malloc(sizeof(struct tr_WidgetSlidebar));
    slidebar->widget.attr = (struct tr_Attr*)malloc(sizeof(struct tr_Attr));
    memcpy(slidebar->widget.attr,attr,sizeof(struct tr_Attr));
    slidebar->widget.type = TWT_SLIDEBAR;
  }
  
  slidebar->widget.sub = gui_NewWidget();
  _set_gui_object_attr((struct gui_Widget*)slidebar->widget.sub,attr);
  slidebar->widget.sub->attr.form = FORM_SLIDEBAREX;
  
  gui_WidgetInit(slidebar->widget.sub,gui_GetMain());
  gui_WidgetMap(slidebar->widget.sub);
  form_SlidebarExInit(slidebar->widget.sub);
  
  struct form_SlidebarEx *fslidebar = (struct form_SlidebarEx*)slidebar->widget.sub->form;
  if (fslidebar)
  {
    fslidebar->total = 10;
    fslidebar->position = 0;
    fslidebar->size = 10;
  }
  
  return slidebar;
}

struct tr_Attr tr_GetAttr( int w, int h, int x, int y )
{
  struct tr_Attr attr;
  attr.x = x;
  attr.y = y;
  attr.w = w;
  attr.h = h;
  return attr;
}
