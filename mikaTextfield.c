#include "mikaTextfield.h"
#include "config.h"

#define _TEST_COLORS

mikaLine::mikaLine( struct tr_Object *canvas, struct tr_Object *parent, struct tr_Attr *attr )
{
  #ifdef _DEBUG
    printf("mikaLine::mikaLine\n");
  #endif
  if (!canvas || !parent || !attr)
  {
    #ifdef _DEBUG
      printf("mikaLine::mikaLine error, invalid arguments\n");
    #endif
  }
  else
  {
    widget = tr_ButtonInitEx2(NULL,attr,canvas);
    tr_SetFontColor(widget,0xfe,0xfe,0xfe,250);
    tr_SetFontSize(widget,17);
    tr_PutObject(parent,widget);
  }
}

mikaLine::~mikaLine( )
{
}

/** create a textfield as a child of 'parent' with as many lines as do fit at the height 'lineheight'
 */
mikaTextfield::mikaTextfield( struct tr_Object *canvas, struct tr_Object *parent, int lineheight )
{
  #ifdef _DEBUG
    printf("mikaTextfield::mikaTextfield\n");
  #endif
  if (!canvas || !parent || lineheight < 1)
  {
    #ifdef _DEBUG
      printf("mikaTextfield::mikaTextfield error, invalid arguments\n");
    #endif
  }
  else
  {
    lines = NULL;
    struct tr_Attr attr;
    struct tr_Attr lineAttr;
    attr.x = 0;
    attr.y = 0;
    attr.w = parent->widget.attr->w;
    attr.h = parent->widget.attr->h;
    widget = tr_ButtonInitEx2(NULL,&attr,canvas);
    #ifdef _TEST_COLORS
      tr_SetColor(widget,(char*)"#202020");
    #endif
    tr_PutObject(parent,widget);
    if (widget)
    {
      lineAttr.x = 0;
      lineAttr.y = 0;
      lineAttr.w = attr.w;
      lineAttr.h = lineheight;
      int nlines = attr.h/lineheight;
      mynlines = nlines;
      int n;
      #ifdef _DEBUG
        printf("mikaTextfield::mikaTextfield lines(%d) size(%d)\n",
          nlines,lineAttr.h);
      #endif
      for(n = 0; n < nlines; n++)
      {
        mikaLine *line = new mikaLine(canvas,widget,&lineAttr);
        addLine(line);
        lineAttr.y += lineAttr.h;
      }
    }
  }
}

mikaTextfield::~mikaTextfield( )
{
}

int mikaTextfield::addLine( mikaLine *line )
{
  if (!line)
  {
    #ifdef _DEBUG
      printf("mikaTextfield::addLine error, invalid arguments\n");
    #endif
    return 1;
  }
  line->next = lines;
  lines = line;
  return 0;
}

int mikaTextfield::fillLine( int nline, unsigned char *text )
{
  int n = mynlines-1;
  mikaLine *line = lines;
  while(line)
  {
    if (n==nline)
    {
      tr_ChangeTextExPos(line->widget,(char*)text,2,10);
      break;
    }
    line = line->next;
    n--;
  }
  return 0;
}

/** clear the lines before we set new text
 */
int mikaTextfield::clear( )
{
  char clear[2];
  strcpy((char*)clear,"");
  int n;
  for(n = 0; n < mynlines; n++)
    fillLine(n,(unsigned char*)clear);
  return 0;
}

/** set the text to be displayed in the textfield
 */
#define _FIX_LINEWIDTH 55
int mikaTextfield::setText( unsigned char *text )
{
  #ifdef _DEBUG
    printf("mikaTextfield::setText\n");
  #endif
  clear();
  char *line = NULL;
  if (!text)
  {
    #ifdef _DEBUG
      printf("mikaTextfield::setText\n");
    #endif
    return 1;
  }
  int nlines = strlen((char*)text)/_FIX_LINEWIDTH;
  int left = strlen((char*)text), n;
  #ifdef _DEBUG
    printf("mikaTextfield::setText lines(%d)\n",nlines);
  #endif
  for(n = 0; n < nlines+1; n++)
  {
    line = (char*)malloc(_FIX_LINEWIDTH+1);
    if (left<_FIX_LINEWIDTH+1)
    {
      strncpy(line,(char*)text+(n*_FIX_LINEWIDTH),left);
      line[left] = 0x00;
    }
    else
    {
      strncpy(line,(char*)text+(n*_FIX_LINEWIDTH),_FIX_LINEWIDTH);
      line[_FIX_LINEWIDTH] = 0x00;
    }
    #ifdef _DEBUG
      printf("mikaTextfield::setText fill line(%s)\n",
        (char*)line);
    #endif
    fillLine(n,(unsigned char*)line);
    left-=_FIX_LINEWIDTH;
  }
  
  return 0;
}
