#include "listbox.h"
#include "system.h"
#include "gui.h"
#include "xml.h"
#include "interface.h"
#include "global.h"
#include "config.h"
#include "ai.h"

#define _LISTBOX_DEFAULT_HEIGHT 80
#define _LISTBOX_DEFAULT_SIZE	5
#define _LISTBOX_AUTOMATIC_ITEMS	// automatically create a default number of items
#define _LISTBOX_TEST_COLORS		// set colors automatically
#define _LISTBOX_CREATE_DELEGATE_NO
#define _LISTBOX_TEST_DELEGATE
#define _LISTBOX_EFFECTS_NO

void *form_ListboxOnExposeEx( struct gui_Widget *widget, int x, int y, int w, int h );
int _form_listbox_create_items( struct gui_Widget *w, int nitems );
struct form_ListboxItem *_form_listbox_position_to_item( struct gui_Widget *w, int x, int y );

struct form_ListboxDelegate _test_delegate;
// currently processed objects:
struct form_ListboxItem *currentListboxItem = NULL;
struct form_Listbox *currentListbox = NULL;
int currentListboxItemRow = 0;

/** test delegate methods
 *
 *  fetch the number of real items in a listbox
 */
#define _TEST_NUMBER_OF_ITEMS
#define _TEST_NUMBER_OF_ITEMS_COUNT	16
void *_test_number_of_items( int *nitems )
{
  #ifdef _DEBUG
    printf("_test_number_of_items form_Listbox\n");
  #endif
  if (!nitems)
  {
    #ifdef _DEBUG
      printf("_test_number_of_items ERROR invalid argument\n");
    #endif
    return NULL;
  }
  #ifdef _TEST_NUMBER_OF_ITEMS
    *nitems = _TEST_NUMBER_OF_ITEMS_COUNT;
  #else
    *nitems = 0;
  #endif
}

/** fetch the setup of a listbox item at a position
 */
#define _ITEM_TEST_SETUP
struct listboxItemSetup *_test_listbox_item_setup( int nitem )
{
  #ifdef _DEBUG
    printf("_test_listbox_item_setup form_Listbox\n");
  #endif
  if (nitem<0)
  {
    #ifdef _DEBUG
      printf("_test_listbox_item_setup ERROR invalid argument\n");
    #endif
    return NULL;
  }
  
  struct listboxItemSetup *lsetup = NULL;
  
  #ifdef _ITEM_TEST_SETUP
    static struct listboxItemSetup *staticSetup = NULL;
    if (!staticSetup)
    {
      staticSetup = (struct listboxItemSetup*)malloc(sizeof(struct listboxItemSetup));
      staticSetup->str_big[0] = 0x00;
      staticSetup->str_small[0] = 0x00;
      staticSetup->str_right[0] = 0x00;
      staticSetup->color_bg[0] = 0x00;
    }  
    
    int nitems = 0;
    if (currentListbox->delegate)
    {
      if (currentListbox->delegate->numberOfItems)
      {
        currentListbox->delegate->numberOfItems(&nitems);
      }
    }
    
    if (nitem<nitems)
    {
      sprintf((char*)staticSetup->str_big,"test_%d",nitem);
      strcpy((char*)staticSetup->str_small,"small text");
      strcpy((char*)staticSetup->str_right,"right");
    
      lsetup = staticSetup;
    }
  #endif
  
  return lsetup;
}

/** initialize a listbox item being held by Widget w
 */
int form_ListboxInit( struct gui_Widget *w )
{
  if (!w)
  {
    #ifdef _DEBUG
      printf("form_ListboxInit ERROR invalid argument !w\n");
    #endif
    return 1;
  }
  #ifdef _DEBUG
    printf("form_ListboxInit\n");
  #endif
  
  struct form_Listbox *lb = (struct form_Listbox*)malloc(sizeof(struct form_Listbox));
  w->form = lb;
  w->attr.form = FORM_LISTBOX;
  w->attr.onbuttondown = form_ListboxButtonDown;
  w->attr.onbuttonup = form_ListboxButtonUp;
  w->attr.onexposeEx = form_ListboxOnExposeEx;
  
  #ifdef _LISTBOX_CREATE_DELEGATE
    lb->delegate = (struct form_ListboxDelegate*)malloc(sizeof(struct form_ListboxDelegate));
    lb->delegate->numberOfItems = NULL;
  #else
    lb->delegate = NULL;
  #endif
  
  #ifdef _LISTBOX_TEST_DELEGATE
    lb->delegate = &_test_delegate;
    _test_delegate.numberOfItems = _test_number_of_items;
    _test_delegate.listboxItemSetup = _test_listbox_item_setup;
  #endif
  
  lb->items = NULL;
  lb->canvas = NULL;
  lb->move = 0;
  
  #ifdef _LISTBOX_AUTOMATIC_ITEMS
    _form_listbox_create_items( w, _LISTBOX_DEFAULT_SIZE );
  #else
  #endif
  
  return 0;
}

/** retrieve the number of visible items of listbox
 */
int form_ListboxVisibleItems( struct gui_Widget *w )
{
  #ifdef _DEBUG
    printf("form_ListboxVisibleItems\n");
  #endif
  
  if (!w)
  {
    #ifdef _DEBUG
      printf("form_ListboxVisibleItems ERROR invalid argument\n");
    #endif
    return 1;
  }
  
  int visible = 0;
  
  struct form_Listbox *lb = (struct form_Listbox*)w->form;
  if (lb)
  {
    struct form_ListboxItem *current = lb->items;
    while(current)
    {
      visible++;
      current = current->next;
    }
  }
  
  return visible;
}

/** set the font color of all the items in the Listbox
 *
 */
int form_ListboxSetTextColor( struct gui_Widget *widget, unsigned char r, unsigned char g, unsigned char b, unsigned char a )
{
  #ifdef _DEBUG
    printf("form_ListboxSetTextColor\n");
  #endif
  if (!widget)
  {
    #ifdef _DEBUG
      printf("form_ListboxSetTextColor ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  struct form_Listbox *lb = (struct form_Listbox*)widget->form;
  struct form_ListboxItem *current = lb->items;
  struct gui_Widget *holder = NULL;
  while(current)
  {
    holder = current->holder;
    if (holder)
    {
      if (holder->glfont)
      {
        holder->glfont->color.rgba[0] = r;
        holder->glfont->color.rgba[1] = g;
        holder->glfont->color.rgba[2] = b;
        holder->glfont->color.rgba[3] = a;
      }
    }
    current = current->next;
  }
  
  return 0;
}

/** set the delegate object of the Listbox - the delegate holds the methods to
 *  call on certain events in the Listbox
 */
int form_ListboxSetDelegate( struct gui_Widget *w, struct form_ListboxDelegate *delegate )
{
  if (!w||!delegate)
  {
    #ifdef _DEBUG
      printf("form_ListboxSetDelegate invalid arguments\n");
    #endif
    return 1;
  }
  struct form_Listbox *lb = (struct form_Listbox*)w->form;
  if (lb)
    lb->delegate = delegate;
    
  return 0;
}

/** retrieve the n'th Listbox Item
 */
struct form_ListboxItem *form_ListboxGetItem( struct gui_Widget *w, int n )
{
  if (!w||n<0)
  {
    #ifdef _DEBUG
      printf("form_ListboxGetItem ERROR invalid arguments\n");
    #endif
    return NULL;
  }

  struct form_ListboxItem *lbItem = NULL;
  int count = 0;
  struct form_Listbox *lb = (struct form_Listbox*)w->form;
  struct form_ListboxItem *current = lb->items;
  while(current)
  {
    if (count==n)
    {
      #ifdef _DEBUG
        printf("form_ListboxGetItem hit\n");
      #endif
      lbItem = current;
      break;
    }
    count++;
    current = current->next;
  }
  
  return lbItem;
}

/** Listbox speciffic exposure call
 *
 */
int form_ListboxExpose( struct gui_Widget *w )
{
  if (!w)
  {
    #ifdef _DEBUG
      printf("form_CanvasExpose ERROR invalid argument\n");
      return 1;
    #endif
  }
  
  gui_WidgetExpose(w,
    w->attr.x,
    w->attr.y,
    w->attr.w,
    w->attr.h
  );
  
  int count = 0;
  struct listboxItemSetup *setup = NULL;
  struct form_Listbox *lb = (struct form_Listbox*)w->form;
  struct form_ListboxItem *current = lb->items;
  struct gui_Widget *currentWidget = NULL;
  while(current)
  {
    currentListboxItem = current;
    currentListboxItemRow = count;
    currentListbox = lb;
    
    currentWidget = current->holder;
    #ifdef _DEBUG
      printf("form_CanvasExpose item\n");
    #endif
    
    if (lb->delegate)
    {
      setup = lb->delegate->listboxItemSetup(count+lb->first);
      if (setup)
      {
      }
    }
    
    if (currentWidget->attr.onexposeEx)
    {
      currentWidget->attr.onexposeEx(
        currentWidget,
        currentWidget->attr.x,
        currentWidget->attr.y,
        currentWidget->attr.w,
        currentWidget->attr.h
        );
    }
    else
    {
      gui_WidgetExpose(
        currentWidget,
        currentWidget->attr.x,
        currentWidget->attr.y,
        currentWidget->attr.w,
        currentWidget->attr.h
      );
    }
    current = current->next;
    count++;
  }
  
  return 0;
}

/** w->attr.onexposeEx
 */
void *form_ListboxOnExposeEx( struct gui_Widget *widget, int x, int y, int w, int h )
{
  #ifdef _DEBUG
    printf("form_ListboxOnExposeEx\n");
  #endif
  if (!widget)
  {
    #ifdef _DEBUG
      printf("form_CanvasOnExposeEx ERROR invalid argument !w\n");
    #endif
    return NULL;
  }
  
  form_ListboxExpose(widget);
  
  return NULL;
}

void *form_ListboxItemExposeEx( struct gui_Widget *widget, int x, int y, int w, int h )
{
  #ifdef _DEBUG
    printf("form_ListboxItemExposeEx\n");
  #endif
  if (!widget)
  {
    #ifdef _DEBUG
      printf("form_ListboxItemExposeEx ERROR invalid argument !w\n");
    #endif
    return NULL;
  }
  
  gui_WidgetExpose(
    widget,x,y,w,h
  );
  
  static struct listboxItemSetup *setup = NULL;
  static int realx = 0;
  static int realy = 0;
  
  gui_WidgetGetRealPosition(widget,&realx,&realy);
  
  if (currentListbox)
  {
    if (currentListbox->delegate)
    {
      setup = currentListbox->delegate->listboxItemSetup(
        currentListboxItemRow+currentListbox->first-1);
      if (setup)
      {
        #ifdef _DEBUG
          printf("form_ListboxItemExposeEx listboxItemSetup fetched %d\n",
            currentListboxItemRow+currentListbox->first-1);
        #endif
        if (setup->str_big[0]!=0x00)
        {
          gui_DrawTextEx(
            (char*)setup->str_big,
            10+realx,30+realy,
            150,h-10,
            DEFA_TXTR
          );
        }
        if (setup->str_right[0]!=0x00)
        {
          gui_DrawTextEx(
            (char*)setup->str_right,
            w-(10+realx+50),30+realy,
            50,h-10,
            DEFA_TXTR
          );
        }
      }
    }
  }
  
  return NULL;
}

/** form_CanvasDelegate, relays messages from the canvas here
 *
 *  move up in the list of items
 *  first & last are index+1
 */
void *_canvas_delegate_on_up( void *obj )
{
  if (!obj)
  {
    #ifdef _DEBUG
      printf("_canvas_delegate_on_up form_Listbox ERROR invalid argument\n");
    #endif
    return NULL;
  }
  
  //#ifdef _DEBUG
    printf("_canvas_delegate_on_up form_Listbox\n");
  //#endif
  struct gui_Widget *w = (struct gui_Widget*)obj;
  struct form_Listbox *lb = (struct form_Listbox*)w->form;
  
  int visible = 0;
  int nitems = 0; // total of items in listbox to scroll through
  int moveby = 0;
  
  if (lb->delegate)
  {
    if (lb->delegate->numberOfItems)
    {
      lb->delegate->numberOfItems(&nitems);
      visible = form_ListboxVisibleItems(w);
      moveby = _LISTBOX_DEFAULT_SIZE-1;
      
      if (lb->first-moveby<1)
        moveby = lb->first-1;
      
      if (moveby>0)
      {
        lb->first -= moveby;
        lb->last -= moveby;
        #ifdef _DEBUG
          printf("_canvas_delegate_on_up form_Listbox first(%d) moveby(%d)\n",
            lb->first,moveby);
        #endif
      }
      else
      {
        #ifdef _DEBUG
          printf("_canvas_delegate_on_up form_Listbox unable to move\n");
        #endif
      }
    }
  }
}

/** move down in the list of items
 */
void *_canvas_delegate_on_down( void *obj )
{
  if (!obj)
  {
    #ifdef _DEBUG
      printf("_canvas_delegate_on_down form_Listbox ERROR invalid argument\n");
    #endif
    return NULL;
  }
  //#ifdef _DEBUG
    printf("_canvas_delegate_on_down\n");
  //#endif
  struct gui_Widget *w = (struct gui_Widget*)obj;
  struct form_Listbox *lb = (struct form_Listbox*)w->form;

  int visible = 0;
  int nitems = 0; // total of items in listbox to scroll through
  int moveby = 0;
  
  if (lb->delegate)
  {
    if (lb->delegate->numberOfItems)
    {
      lb->delegate->numberOfItems(&nitems);
      visible = form_ListboxVisibleItems(w);
      moveby = _LISTBOX_DEFAULT_SIZE-1;
      
      if (lb->last+moveby>nitems)
        moveby = nitems-lb->last;
      
      if (moveby>0)
      {
        lb->first += moveby;
        lb->last += moveby;
        #ifdef _DEBUG
          printf("_canvas_delegate_on_down form_Listbox last(%d) moveby(%d)\n",
            lb->last,moveby);
        #endif
      }
      else
      {
        #ifdef _DEBUG
          printf("_canvas_delegate_on_down form_Listbox unable to move\n");
        #endif
      }
    }
  }
}

/** sets the canvas, which we can manipulate and also set's the canvas's delegate
 *  so that we receive messages from the canvas
 */
int form_ListboxSetCanvasDelegate( struct gui_Widget *w, struct gui_Widget *c )
{
  #ifdef _DEBUG
    printf("form_ListboxSetCanvasDelegate\n");
  #endif
  
  if (!w||!c)
  {
    #ifdef _DEBUG
      printf("form_ListboxSetCanvasDelegate ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  struct form_Listbox *lb = w->form;
  struct form_Canvas *canvas = c->form;
  lb->canvas = canvas; 
  canvas->delegate = (struct form_CanvasDelegate*)malloc(sizeof(struct form_CanvasDelegate));
  canvas->delegate->obj = (void*)w;
  canvas->delegate->onUp = _canvas_delegate_on_up;
  canvas->delegate->onDown = _canvas_delegate_on_down;
  
  return 0;
}

int _form_listbox_add_item( struct gui_Widget *w, struct gui_Widget *item )
{
  if (!w||!item)
    return 1;
  
  #ifdef _DEBUG
    printf("_form_listbox_add_item\n");
  #endif
  
  struct form_Listbox *lb = (struct form_Listbox*)w->form;
  struct form_ListboxItem *lbitem = (struct form_ListboxItem*)item->form;
  if (!lb||!lbitem)
    return 1;
  
  lbitem->next = NULL;
  
  if (!lb->items)
  {
    lb->items = lbitem;
    return 0;
  }
  
  struct form_ListboxItem *current = lb->items;
  while(current)
  {
    if (!current->next)
    {
      current->next = lbitem;
      break;
    }
    current = current->next;
  }
  
  return 0;
}

/** find an item in the listbox according to an x,y position
 *
 */
struct form_ListboxItem *_form_listbox_position_to_item( struct gui_Widget *w, int x, int y )
{
  if (!w)
    return NULL;
  
  struct form_ListboxItem *item = NULL;
  struct form_Listbox *lb = w->form;
  struct gui_Widget *holder = NULL;
  struct form_ListboxItem *current = lb->items;
  while(current)
  {
    holder = current->holder;
    if (
      holder->attr.x<x &&
      holder->attr.y<y &&
      holder->attr.x+holder->attr.w>x &&
      holder->attr.y+holder->attr.h>y
    )
    {
      #ifdef _DEBUG
        printf("_form_listbox_position_to_item found item\n");
      #endif
      item = current;
    }
    current = current->next;
  }
  
  return item;
}

/** perform an event after a time period after a button down event
 *
 */
#define _LISTBOX_BUTTONDOWN_TIMEOUT 300000 // how long to blink listbox item in microseconds
void *_listbox_button_down_thread(void*data)
{
  #ifdef _DEBUG
    printf("_listbox_button_down_thread form_Listbox\n");
  #endif
  if (!data)
  {
    #ifdef _DEBUG
      printf("_listbox_button_down_thread form_Listbox ERROR invalid argument\n");
    #endif
    return NULL;
  }
  
  usleep(_LISTBOX_BUTTONDOWN_TIMEOUT);
  
  struct form_ListboxItem *lbitem = (struct form_ListboxItem*)data;
  binarySemaphoreWait(bigInterface->controller.SEMID);
  #ifdef _DEBUG
    printf("_listbox_button_down_thread form_Listbox locked\n");
  #endif
  #ifdef _LISTBOX_EFFECTS
    #ifdef _LISTBOX_TEST_COLORS
      gui_WidgetSetColor(lbitem->holder,"#dcdcdc");
    #endif
  #endif
  binarySemaphorePost(bigInterface->controller.SEMID);
  #ifdef _DEBUG
    printf("_listbox_button_down_thread form_Listbox unlocked\n");
  #endif
  #ifdef _LISTBOX_TEST_COLORS
    gui_TriggerExpose(gui_GetMain());
  #endif
}

/** Button Down event
 */
void *form_ListboxButtonDown( void *wdata )
{
  if (!wdata)
  {
    #ifdef _DEBUG
      printf("form_ListboxButtonDown ERROR wdata invalid\n");
    #endif
    return NULL;
  }
  struct gui_Event *event = (struct gui_Event*)wdata;
  struct gui_Widget *w = event->widget;
  event->x -= w->attr.x;
  event->y -= w->attr.y;
  if (event->x<0||event->y<0) return NULL;
  #ifdef _DEBUG
    printf("form_ListboxButtonDown event(%d,%d)\n",event->x,event->y);
  #endif
  struct form_ListboxItem *item = _form_listbox_position_to_item(w,event->x,event->y);
  if (item)
  {
    #ifdef _DEBUG
      printf("form_ListboxButtonDown item\n");
    #endif
    struct gui_Widget *holder = item->holder;
    if (holder)
    {
      #ifdef _LISTBOX_EFFECTS
        #ifdef _LISTBOX_TEST_COLORS
          gui_WidgetSetColor(holder,"#ffffff");
          struct interface_event_t ievent;
          ievent.type = INTERFACE_EVENT_EXPOSE;
          ievent.x = 0;
          ievent.y = 0;
          ievent.width = 640;
          ievent.height = 480;
          gui_WidgetPushEvent(&ievent);
        #endif
      #endif
      if (holder->attr.onbuttondown)
      {
        event->x -= holder->attr.x;
        event->y -= holder->attr.y;
        event->widget = holder;
        holder->attr.onbuttondown(event);
      }
    }
    #ifdef _LISTBOX_EFFECTS
      pthread_t t;
      pthread_create(&t,NULL,_listbox_button_down_thread,item);
    #endif
  }
}

/** Button Up event
 */
void *form_ListboxButtonUp( void *wdata )
{
  if (!wdata)
  {
    #ifdef _DEBUG
      printf("form_ListboxButtonUp ERROR wdata invalid\n");
    #endif 
    return NULL;
  }
  struct gui_Event *event = (struct gui_Event*)wdata;
  struct gui_Widget *w = event->widget;
  event->x -= w->attr.x;
  event->y -= w->attr.y;
  if (event->x<0||event->y<0) return NULL;
  #ifdef _DEBUG
    printf("form_ListboxButtonUp event(%d,%d)\n",event->x,event->y);
  #endif
  struct form_ListboxItem *item = _form_listbox_position_to_item(w,event->x,event->y);
  if (item)
  {
    #ifdef _DEBUG
      printf("form_ListboxButtonUp item\n");
    #endif
    struct gui_Widget *holder = item->holder;
    if (holder)
    {
      #ifdef _LISTBOX_EFFECTS
        #ifdef _LISTBOX_TEST_COLORS
          if (0)
          {
            gui_WidgetSetColor(holder,"#dcdcdc");
            gui_TriggerExpose(gui_GetMain());
          }
        #endif
      #endif
      if (holder->attr.onbuttonup)
      {
        event->x -= holder->attr.x;
        event->y -= holder->attr.y;
        event->widget = holder;
        holder->attr.onbuttonup(event);
      }
    }
  }
}

/** initialize a number of items for the listbox
 *
 */
int _form_listbox_create_items( struct gui_Widget *w, int nitems )
{
  if (!w||nitems<1)
    return 0;
  
  #ifdef _DEBUG
    printf("_form_listbox_create_items nitems(%d)\n",nitems);
  #endif
  
  struct form_Listbox *lb = (struct form_Listbox*)w->form;
  if (!lb)
  {
    #ifdef _DEBUG
      printf("_form_listbox_create_items ERROR w->form invalid\n");
    #endif
    return 1;
  }
  
  int n;
  int y = 0;
  int height = w->attr.h/nitems;
  struct gui_Widget *guiItem = NULL;
  struct form_ListboxItem *item = NULL;
  int pair = 0;
  for(n = 0; n < nitems; n++)
  {
    guiItem = gui_NewWidget();
    if (pair==0)
    {
      #ifdef _LISTBOX_TEST_COLORS
        gui_WidgetSetColor(guiItem,"#dcdcdc");
      #endif
      pair = 1;
    }
    else
    {
      #ifdef _LISTBOX_TEST_COLORS
        gui_WidgetSetColor(guiItem,"#dcdcdc");
      #endif
      pair = 0;
    }
    gui_WidgetInit(guiItem,w);
    guiItem->attr.x = 0;
    guiItem->attr.y = y;
    guiItem->attr.w = w->attr.w;
    guiItem->attr.h = height;
    guiItem->attr.form = FORM_LISTBOXITEM;
    guiItem->attr.onexposeEx = form_ListboxItemExposeEx;
    item = (struct form_ListboxItem*)malloc(sizeof(struct form_ListboxItem));
    guiItem->form = item;
    item->holder = guiItem;
    y+=height;
    
    _form_listbox_add_item(w,guiItem);
  }

  lb->first = 1;
  lb->last = nitems;
  
  #ifdef _LISTBOX_TEST_COLORS
    form_ListboxSetTextColor(w,0x00,0x00,0x00,0xff);
  #endif
    
  return 0;
}
