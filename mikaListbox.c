/** 
 *  this manages a listbox object created using the lowlevel tr_Object &
 *  gui_Widget object
 *
 */
#include "mikaListbox.h"
#include "mikaSkin.h"
#include "mikaSkinEx.h"

void *mikaListboxOnButton(void*data);

typedef struct mikaListboxHolder
{
  mikaListbox *listbox;
  struct mikaListboxHolder *next;
} mikaListboxHolder;

static struct mikaListboxHolder *listboxHolder = NULL;

mikaListboxItem::mikaListboxItem( )
{
  next = NULL;
  last = NULL;
}

/** create standard listbox item, creating it's UI widget
 *
 */
mikaListboxItem::mikaListboxItem( struct tr_Object *canvas, struct tr_Attr *attr )
{
  #ifdef _DEBUG
    printf("mikaListboxItem::mikaListboxItem\n");
  #endif
  
  button = tr_ButtonInitEx2(NULL,attr,canvas);
  mikaSkinEx *skin = getSkin();
  skin->setupWidget(button,(unsigned char*)"mikaCanvasButtons");
  
  tr_SetRound(button,3);
  tr_SetAction(button,mikaListboxOnButton,TR_ACTION_BUTTON);
  
  next = NULL;
  last = NULL;
}

mikaListboxItem::~mikaListboxItem( )
{
}

/** determine if a button belongs to a Listbox
 */
int mikaListboxHolderIsButton( struct mikaListboxHolder *holder, struct gui_Widget *widget )
{
  if (!holder || !widget)
  {
    #ifdef _DEBUG
      printf("mikaListboxHolderIsButton error, invalid arguments\n");
    #endif
    return 0;
  }
  
  if (holder->listbox->nitems>0)
  {
    mikaListboxItem *current = holder->listbox->items;
    while(current)
    {
      if (current->button->widget.sub==widget)
      {
        return 1;
      }
      current = current->next;
    }
  }
  else
  {
    int n;
    for(n = 0; n < _MIKA_LISTBOX_ITEMS; n++)
    {
      struct tr_Object *current = holder->listbox->buttons[n];
      if (current->widget.sub==widget)
      {
        return 1;
      }
    }
  }
  return 0;
}

#define _ONBUTTON_LOADNODE
int mikaListbox::onButton( int nbutton )
{
  #ifdef _DEBUG
    printf("mikaListbox::onButton(%d)\n",nbutton);
  #endif
  int ncurrent = 0;
  struct mikaListboxNode *current = currentNode->children;
  while(current)
  {
    #ifdef _DEBUG
      printf("\tmikaListbox::onButton node\n");
    #endif
    if (ncurrent==nbutton)
    {
      if (delegate)
      {
        if (delegate->onSelection)
          delegate->onSelection(current);
      }
      if (current->children)
      {
        #ifdef _ONBUTTON_LOADNODE
          this->loadNode(current);
        #else
          if (window)
          {
            tr_WindowToTop(window);
            tr_WindowExpose(window);
          }
        #endif
        return 0;
      }
      else
      {
        if (delegate)
        {
          if (delegate->onOrder)
            delegate->onOrder(current);
        }
      }
    }
    current = current->next;
    ncurrent++;
  }
  return 0;
}

/** touched inside of the listbox
 */
void *mikaListboxOnButton(void*data)
{
  struct gui_Event *event = (struct gui_Event*)data;
  struct gui_Widget *widget = event->widget;

  #ifdef _DEBUG
    printf("mikaListboxOnButton event(%dx%d) widget(%dx%d+%d+%d)\n",
      event->x,event->y,
      widget->attr.w,widget->attr.h,
      widget->attr.x,widget->attr.y);
  #endif
  
  struct mikaListboxHolder *holder = listboxHolder;
  struct mikaListbox *listbox = NULL;
  int nitem = 0;
  int n;
  while(holder)
  {
    if (mikaListboxHolderIsButton(holder,widget)==1)
    {
      #ifdef _DEBUG
        printf("mikaListboxOnButton holder found\n");
      #endif
      listbox = holder->listbox;
      
      if (listbox->nitems>0) // dynamic no. of items
      {
        n = 0;
        mikaListboxItem *o = listbox->items;
        while(o)
        {
          if (o->button->widget.sub==widget)
          {
            listbox->onButton(n);
            return NULL;
          }
          o = o->next;
          n++;
        }
      }
      else // static no. of items
      {
        for(n = 0; n < _MIKA_LISTBOX_ITEMS; n++)
        {
          if (listbox->buttons[n]->widget.sub==widget)
          {
            listbox->onButton(n);
            return NULL;
          }
        }
      }
      
    }
    holder = holder->next;
  }
  
  return NULL;
}

int mikaListboxNodeChildren( struct mikaListboxNode *node )
{
  int count = 0;
  struct mikaListboxNode *current = node->children;
  while(current)
  {
    current = current->next;
    count++;
  }
  return count;
}

void *mikaListboxMasterOnButton(void*data)
{
  #ifdef _DEBUG
    printf("mikaListboxMasterOnButton\n");
  #endif
  struct gui_Event *event = (struct gui_Event*)data;
  struct gui_Widget *widget = event->widget;
}

/** setup the static parameters of the Listbox
 *
 */
int mikaListbox::setupStatic( )
{
  #ifdef _DEBUG
    printf("mikaListbox::setupStatic\n");
  #endif
  widget = tr_ButtonInitEx2(NULL,&myAttr,myCanvas);
  
  mikaSkinEx *skin = getSkin();
  skin->setupWidget(widget,(unsigned char*)"mikaListbox");
  tr_SetRound(widget,3);
  tr_SetAction(widget,mikaListboxMasterOnButton,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(myCanvas,widget);
  #ifdef _DEBUG
    printf("mikaListbox::setupStatic finish\n");
  #endif
  
  return 0;
}

mikaListbox::mikaListbox( struct tr_Object *canvas, struct tr_Attr *attr )
{
    int n;
    struct tr_Attr buttonAttr;
    
    myAttr		= *attr;
    myCanvas		= canvas;
    
    start 		= 0;
    mainNode 		= NULL;
    currentNode 	= NULL;
    window 		= NULL;
    delegate 		= NULL;
    // dynamis items
    items		= NULL;
    nitems		= 0;
    
    setupStatic();
    mikaSkinEx *skin = getSkin();
    
    for(n = 0; n < _MIKA_LISTBOX_ITEMS; n++)
    {
        buttonAttr.x = 2;
        buttonAttr.y = 2+((attr->h/_MIKA_LISTBOX_ITEMS)*n);
        buttonAttr.w = attr->w-4;
        buttonAttr.h = (attr->h/_MIKA_LISTBOX_ITEMS)-2;
        
        buttons[n] = tr_ButtonInitEx2(NULL,&buttonAttr,canvas);
        skin->setupWidget(buttons[n],(unsigned char*)"mikaCanvasButtons");
        
        tr_SetFontSize(buttons[n],15);
        tr_SetRound(buttons[n],3);
        tr_SetAction(buttons[n],mikaListboxOnButton,TR_ACTION_BUTTON);
        tr_PutObject(widget,buttons[n]);
    }
    
    // register this listbox for event processing
    if (!listboxHolder)
    {
      listboxHolder = (struct mikaListboxHolder*)malloc(sizeof(struct mikaListboxHolder));
      listboxHolder->next = NULL;
      listboxHolder->listbox = this;
    }
    #ifdef _DEBUG
      printf("mikaListbox::mikaListbox finished\n");
    #endif
}

/** creating a custom listbox with a dynamic number of items
 *
 */
mikaListbox::mikaListbox( struct tr_Object *canvas, struct tr_Attr *attr, int members )
{
  #ifdef _DEBUG
    printf("mikaListbox::mikaListbox items(%d)\n",members);
  #endif
  
  myAttr		= *attr;
  myCanvas		= canvas;
  
  start 		= 0;
  mainNode 		= NULL;
  currentNode 		= NULL;
  window 		= NULL;
  delegate 		= NULL;
  items			= NULL;
  nitems		= members;
  
  setupStatic();
  mikaSkinEx *skin = getSkin();
  
  int n;
  struct tr_Attr buttonAttr;
  for(n = 0; n < nitems; n++)
  {
    buttonAttr.x = 2;
    buttonAttr.y = (2+((attr->h/nitems)*n));
    buttonAttr.w = attr->w-4;
    buttonAttr.h = (attr->h/nitems)-2;
    #ifdef _DEBUG
      printf("mikaListbox::mikaListbox creating item n(%d) attr(%dx%d+%d+%d)\n",
        n,buttonAttr.w,buttonAttr.h,buttonAttr.x,buttonAttr.y);
    #endif
  
    mikaListboxItem *item = new mikaListboxItem(canvas,&buttonAttr);
    tr_PutObject(widget,item->button);
    
    item->next = items;
    items = item;
  }
  
  // register this listbox for event processing
  if (!listboxHolder)
  {
    listboxHolder = (struct mikaListboxHolder*)malloc(sizeof(struct mikaListboxHolder));
    listboxHolder->next = NULL;
    listboxHolder->listbox = this;
  }
  #ifdef _DEBUG
    printf("mikaListbox::mikaListbox finished\n");
  #endif
}

mikaListbox::~mikaListbox( )
{
    
}

/** retrieve an item for a position in list
 *
 */
tr_Object *mikaListbox::itemAt( int pos )
{
  tr_Object *item = NULL;
  
  if (pos<0)
  {
    #ifdef _DEBUG
      printf("mikaListbox::itemAt bad item position\n");
    #endif
    return NULL;
  }
  
  if (nitems>0)
  {
    int n;
    mikaListboxItem *current = items;
    while(current)
    {
      if (n==pos)
      {
        item = current->button;
        break;
      }
      current = current->next;
      n++;
    }
  }
  else
  {
    item = buttons[pos];
  }
  
  return item;
}

/** print out a tree of nodes in this listbox
 */
void mikaListbox::listNodes( struct mikaListboxNode *node )
{
  static int level = 0;
  int n;
  level++;
  #ifdef _DEBUG
    printf("mikaListbox");
    for(n = 0; n < level; n++)
      printf("\t");
    printf("<node>\n");
  #endif
  if (!node)
    this->listNodes(mainNode);
  else
  {
    struct mikaListboxNode *current = node->children;
    while(current)
    {
      this->listNodes(current);
      current = current->next;
    }
  }
  #ifdef _DEBUG
    printf("mikaListbox");
    for(n = 0; n < level; n++)
      printf("\t");
    printf("</node>\n");
  #endif
  level--;
}

int mikaListboxNodeInit( struct mikaListboxNode *node )
{
  #ifdef _DEBUG
    printf("mikaListboxNodeInit\n");
  #endif
  node->parent = NULL;
  node->children = NULL;
  node->next = NULL;
  node->name[0] = 0x00;
  node->descr[0] = 0x00;
  node->image[0] = 0x00;
  return 0;
}

/** setup the attributes of the node from the XML object
 */
int mikaListboxNodeConfigure( struct mikaListboxNode *node, TiXmlElement *element )
{
  if (!node || !element)
  {
    #ifdef _DEBUG
      printf("mikaListboxNodeConfigure error invalid arguments\n");
    #endif
    return 1;
  }
  const char *value = NULL;
  value = element->Attribute("name");
  if (value)
  {
    strcpy((char*)node->name,value);
  }
  return 0;
}

int mikaListboxParseXmlRoot( struct mikaListboxNode *node, TiXmlElement *parent )
{
  if (!node || !parent)
  {
    #ifdef _DEBUG
      printf("mikaListboxParseXmlRoot error invalid arguments\n");
    #endif
    return 1;
  }
  #ifdef _DEBUG
    printf("mikaListboxParseXmlRoot\n");
  #endif
  TiXmlElement *nodes = parent->FirstChildElement("nodes");
  if (nodes)
  {
    TiXmlElement *current = NULL;
    current = nodes->FirstChildElement("node");
    if (current)
    {
      struct mikaListboxNode *subNode = 
        (struct mikaListboxNode*)malloc(sizeof(struct mikaListboxNode));
      mikaListboxNodeInit(subNode);
      mikaListboxNodeConfigure(subNode,current);
      subNode->parent = node;
      node->children = subNode;
      mikaListboxParseXmlRoot(subNode,current);
      
      while((current = current->NextSiblingElement()))
      {
        subNode = (struct mikaListboxNode*)malloc(sizeof(struct mikaListboxNode));
        mikaListboxNodeInit(subNode);
        mikaListboxNodeConfigure(subNode,current);
        subNode->parent = node;
        subNode->next = node->children;
        node->children = subNode;
        mikaListboxParseXmlRoot(subNode,current);
      }
    }
  }
  
  return 0;
}

/** update the graphics according to where in the tree of nodes
 *  we currently are
 */
void mikaListbox::update( )
{
  #ifdef _DEBUG
    printf("mikaListbox::update\n");
  #endif
  int n = 0;
  mikaListboxNode *current = currentNode->children;
  struct gui_Widget *widget = NULL;
  while(current)
  {
    tr_Object *o = itemAt(n);
    
    if (o)
    {
      tr_ChangeTextExPos(o,(char*)current->name,
        5,o->widget.sub->attr.h/2);
    
      widget = o->widget.sub;
    
      #ifdef _DEBUG
        if (widget)
          printf("mikaListbox::update widget(%dx%d)\n",
            widget->attr.w,widget->attr.h);
      #endif
    }
    current = current->next;
    n++;
  }
  if (window)
  {
    tr_WindowToTop(window);
    tr_WindowExpose(window);
  }
  #ifdef _DEBUG
    printf("mikaListbox::update finish\n");
  #endif
}

/** force load a node - the node should only be set as the current
 *  node and the update should be done using update
 *
 */
void mikaListbox::loadNode( struct mikaListboxNode *node )
{ }

int mikaListbox::loadNodeFile( unsigned char *filename )
{
    if (!filename)
    {
        #ifdef _DEBUG
            printf("mikaListbox::loadNodeFile error invalid arguments\n");
        #endif
        return 1;
    }
    #ifdef _DEBUG
      printf("mikaListbox::loadNodeFile\n");
    #endif
    
    TiXmlDocument doc;
    doc.LoadFile((const char*)filename);
    TiXmlElement *root = doc.FirstChildElement("mikaListbox");
    if (root)
    {
      mainNode = (struct mikaListboxNode*)malloc(sizeof(struct mikaListboxNode));
      mikaListboxNodeInit(mainNode);
      mikaListboxParseXmlRoot(mainNode,root);
    }
    else
    {
      #ifdef _DEBUG
        printf("mikaListbox::loadNodeFile error mikaListbox object not found\n");
      #endif
      return 1;
    }
    
    currentNode = mainNode;
    update();
        
    return 0;
}

int mikaListbox::onListDown( )
{
  if (start+_MIKA_LISTBOX_ITEMS<mikaListboxNodeChildren(currentNode))
  {
    start += _MIKA_LISTBOX_ITEMS;
  }
  return 0;
}

int mikaListbox::onListUp( )
{

  return 0;
}

/** move up one node in the tree of nodes
 */
int mikaListbox::onListBack( )
{
  if (currentNode->parent)
  {
    currentNode = currentNode->parent;
    this->loadNode(currentNode);
  }
  return 0;
}

int mikaListbox::loadNavigatorImages( unsigned char *buttonUp, unsigned char *buttonDown )
{
  if (!buttonUp || !buttonDown)
  {
    #ifdef _DEBUG
      printf("mikaListbox::loadNavigatorImages\n");
    #endif
    return 1;
  }
  
  return 0;
}
