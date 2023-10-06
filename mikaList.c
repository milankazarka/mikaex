#include "mikaList.h"
#include "mikaSkinEx.h"

#define _CHILDREN_EVENTS_NO

void *mikaListItemOnButton( void *data )
{
    #ifdef _DEBUG
        printf("mikaListItemOnButton\n");
    #endif
    return NULL;
}

mikaListItem::mikaListItem( struct tr_Object *canvas, struct tr_Attr *attr )
{
    button = tr_ButtonInitEx2(NULL,attr,canvas);
    button->widget.sub->id = (long)this;
    mikaSkinEx *skin = getSkin();
    skin->setupWidget(button,(unsigned char*)"mikaCanvasButtons");
  
    tr_SetRound(button,3);
    #ifdef _CHILDREN_EVENTS
        tr_SetAction(button,mikaListItemOnButton,TR_ACTION_BUTTON);
    #endif
  
    next = NULL;
}

mikaListItem::~mikaListItem( )
{
    
}

void *onMikaListNode( struct mikaListDataDelegate *delegate, TiXmlElement *element, int position )
{
    #ifdef _DEBUG
        printf("mikaList - onMikaListNode\n");
    #endif
    if (!delegate || !element || position<0)
    {
        #ifdef _DEBUG
            printf("mikaList - onMikaListNode error in arguments\n");
        #endif
        return NULL;
    }
    
    mikaList *list = (mikaList*)delegate->holder;
    if (list)
    {
        if (list->delegate)
        {
            // use extended, so that our holder knows from which
            // child object this message came
            //
            if (list->delegate->onNodeElement)
                list->delegate->onNodeElement(element,position);
        }
    }
    
    #ifdef _DEBUG
        printf("mikaList - onMikaListNode finish\n");
    #endif
    return NULL;
}

/** we have clicked on an element that has no children
 */
void *onMikaListNodeLowest( struct mikaListDataDelegate *delegate, TiXmlElement *element, int position )
{
    #ifdef _DEBUG
        printf("mikaList onMikaListNodeLowest\n");
    #endif
    if (!element || position < 0)
    {
        #ifdef _DEBUG
            printf("mikaList onMikaListNodeLowest error, invalid arguments\n");
        #endif
        return NULL;
    }
    
    char *name = (char*)element->Attribute("name");
    if (name)
    {
        #ifdef _DEBUG
            printf("mikaList onMikaListNodeLowest name(%s)\n",name);
        #endif
    }
    
    mikaList *list = (mikaList*)delegate->holder;
    if (list->delegate)
    {
        if (list->delegate->onLowest)
        {
            list->delegate->onLowest((void*)list,element,position);
        }
    }
    
    return NULL;
}

void *mikaListOnButton( void *data )
{
    #ifdef _DEBUG
        printf("mikaListOnButton\n");
    #endif
    struct gui_Event *event = (struct gui_Event*)data;
    struct gui_Widget *widget = event->widget;
    #ifdef _DEBUG
        printf("mikaListOnButton listbox ptr:(%d) event(%dx%d)\n",
                widget->id,event->x,event->y);
    #endif
    mikaList *list = (mikaList*)widget->id;
    list->onButton(event);
    return NULL;
}

/** onButton - on the main surface of the listbox - need to
 *  determine the button
 */
int mikaList::onButton( struct gui_Event *event )
{
    if (!event)
        return 1;
    #ifdef _DEBUG
        printf("mikaList::onButton event(%dx%d)\n",
            event->x,event->y);
    #endif
    int n = 0;
    int position;
    mikaListItem *current = items;
    while(current)
    {
        struct tr_Object *button = current->button;
        #ifdef _DEBUG
            printf("mikaList::onButton check button(%dx%d+%d+%d)\n",
                button->widget.sub->attr.w,
                button->widget.sub->attr.h,
                button->widget.sub->attr.x,
                button->widget.sub->attr.y);
        #endif
        if (
            widget->widget.sub->attr.x+button->widget.sub->attr.x < event->x &&
            widget->widget.sub->attr.y+button->widget.sub->attr.y < event->y &&
            widget->widget.sub->attr.x+button->widget.sub->attr.x+ button->widget.sub->attr.w > event->x &&
            widget->widget.sub->attr.y+button->widget.sub->attr.y+ button->widget.sub->attr.h > event->y
        )
        {
            position = nitems-n-1;
            #ifdef _DEBUG
                printf("mikaList::onButton button(%d)\n",position);
            #endif
            // move to an element in tree
            if (delegate)
                if (delegate->onSelection)
                    delegate->onSelection(position); // signalize flushing of the list
            data->moveToElementAtPosition(position);
            return 0;
        }
        current = current->next;
        n++;
    }
    #ifdef _DEBUG
        printf("mikaList::onButton finish\n");
    #endif
    return 0;
}

int mikaList::generateButtons( )
{
    int n;
    struct tr_Attr buttonAttr;
    for(n = 0; n < nitems; n++)
    {
      buttonAttr.x = 2;
      buttonAttr.y = (2+((myAttr.h/nitems)*n));
      buttonAttr.w = myAttr.w-4;
      buttonAttr.h = (myAttr.h/nitems)-2;
      #ifdef _DEBUG
        printf("mikaList::mikaList creating item n(%d) attr(%dx%d+%d+%d)\n",
          n,buttonAttr.w,buttonAttr.h,buttonAttr.x,buttonAttr.y);
      #endif
    
      mikaListItem *item = new mikaListItem(myCanvas,&buttonAttr);
      tr_PutObject(widget,item->button);
      
      item->next = items;
      items = item;
    }
    return 0;
}

mikaList::mikaList( struct tr_Object *canvas, struct tr_Attr *attr, int members )
{
    #ifdef _DEBUG
        printf("mikaList\n");
    #endif
    delegate = NULL;
    data = NULL;
    position = 0;
    items = NULL;
    
    myCanvas = canvas;
    myAttr = *attr;
    nitems = members;
    
    widget = tr_ButtonInitEx2(NULL,&myAttr,myCanvas);
    widget->widget.sub->id = (long)this;
  
    mikaSkinEx *skin = getSkin();
    skin->setupWidget(widget,(unsigned char*)"mikaListbox");
    tr_SetRound(widget,3);
    tr_SetAction(widget,mikaListOnButton,TR_ACTION_BUTTON);
    tr_CanvasAddObjStatic(myCanvas,widget);
    
    generateButtons();
}

mikaList::~mikaList( )
{
    #ifdef _DEBUG
        printf("~mikaList\n");
    #endif
}

int mikaList::loadFile( unsigned char *filename )
{
    data = new mikaListData(filename);
    if (!data)
    {
        #ifdef _DEBUG
            printf("mikaList::loadFile error, data list data couldn't be created\n");
        #endif
        return 1;
    }
    
    dataDelegate.holder = this;
    dataDelegate.onNodeElement = onMikaListNode;
    dataDelegate.onLowestElement = onMikaListNodeLowest;
    data->delegate = &dataDelegate;
    
    return 0;
}

int mikaList::moveUp( )
{
    if (position==0)
        return 1;
    else
        position--;
    
    #ifdef _DEBUG
        printf("mikaList::moveUp position(%d)\n",position);
    #endif
    
    return 0;
}

int mikaList::moveDown( )
{    
    if (position+nitems>=data->numberOfItems())
        return 1;
    else
        position++;
    
    #ifdef _DEBUG
        printf("mikaList::moveDown position(%d)\n",position);
    #endif
    
    return 0;
}

int mikaList::moveBack( )
{
    #ifdef _DEBUG
        printf("mikaList::moveBack\n");
    #endif
    data->back(); // automatically delegates items to us
    return 0;
}

/** update the graphical items - basically flushing the list
 */
int mikaList::update( )
{
    #ifdef _DEBUG
        printf("mikaList::update\n");
    #endif
    
    data->delegateNode();
        
    return 0;
}
