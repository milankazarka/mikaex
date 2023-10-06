#include "object.h"
#include "widget.h"
#include "config.h"
#include "mikaGl/mikaGlWindow.h"
#include "mikaGl/mikaGl.h"

milanWidget::milanWidget( int x, int y, int w, int h )
{
  #ifdef _DEBUG
    printf("milanWidget::milanWidget (%dx%d+%d+%d)\n",
      w,h,x,y);
  #endif
  mx = x;
  my = y;
  mw = w;
  mh = h;
  objects = NULL;
  delegate = NULL;
  round = 0;
  setColor(0xff,0xff,0xff,0xff);
}

milanWidget::~milanWidget( )
{
}

int milanWidget::redraw( )
{
  #ifdef _DEBUG
    printf("milanWidget::redraw\n");
  #endif
  return 0;
}

int milanWidget::onButtonDown( struct eventButton *event )
{
  #ifdef _DEBUG
    printf("milanWidget::onButtonDown\n");
  #endif
  if (!event)
  {
    return 1;
  }
  if (delegate)
  {
    if (delegate->onButtonDown)
    {
      delegate->onButtonDown((object*)this,event);
    }
  }
  return 0;
}

int milanWidget::onButtonUp( struct eventButton *event )
{
  #ifdef _DEBUG
    printf("milanWidget::onButtonUp\n");
  #endif
  if (!event)
  {
    return 1;
  }
  if (delegate)
  {
    if (delegate->onButtonUp)
    {
      delegate->onButtonUp((object*)this,event);
    }
  }
  return 0;
}

int milanWidget::addText( milanText *text )
{
  if (!text)
  {
    return 1;
  }
  struct milanWidgetObject *wobject = (struct milanWidgetObject*)malloc(sizeof(struct milanWidgetObject));
  wobject->next = objects;
  objects = wobject;
  wobject->data = text;
  return 0;
}

int milanWidget::addImage( milanImage *image )
{
  if (!image)
  {
    return 1;
  }
  
  struct milanWidgetObject *wo = (struct milanWidgetObject*)malloc(sizeof(struct milanWidgetObject));
  wo->next = objects;
  objects = wo;
  wo->type = _WIDGET_TYPE_IMAGE;
  wo->data = (object*)image;
  
  return 0;
}

int milanWidget::setRound( int r )
{
  round = r;
  return 0;
}

int milanWidget::setColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
{
  #ifdef _DEBUG
    printf("milanWidget::setColor\n");
  #endif
  myColor[0] = r;
  myColor[1] = g;
  myColor[2] = b;
  myColor[3] = a;
  return 0;
}

int milanWidget::drawOnWindow( milanWindow *window )
{
  #ifdef _DEBUG
    printf("milanWidget::drawOnWindow widget(%p) (%dx%d+%d+%d)\n",
      this,mw,mh,mx,my);
  #endif
  mikaGl *gl = window->glWindow->gl;
  gl->setColor(myColor[0],myColor[1],myColor[2],myColor[3]);
  gl->drawRoundRect(mx,my,mw+mx,mh+my,round);
  struct milanWidgetObject *current = objects;
  while(current)
  {
    if (current->type == _WIDGET_TYPE_IMAGE)
    {
      milanImage *mi = (milanImage*)current->data;
      gl->drawImage(mx+mi->mx,my+mi->my,mi->glImage);
    }
    current = current->next;
  }
  
  return 0;
}
