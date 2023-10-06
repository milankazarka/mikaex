#include "desktop.h"
#include "window.h"
#include "canvas.h"
#include "widget.h"

milanDesktop::milanDesktop( )
{
}

milanDesktop::~milanDesktop( )
{
}

int main( )
{
  printf("desktop\n");
  milanWindow *window = new milanWindow(640,480);
  milanCanvas *canvas = new milanCanvas((object*)window);
  window->addCanvas(canvas);
  window->pushCanvas(canvas);
  
  milanWidget *widget = new milanWidget( 30, 30, 80, 80 );
  widget->setRound(7);
  widget->setColor(0x20,0x20,0x20,0xde);
  canvas->addWidget((object*)widget);
  milanWidget *cw = (milanWidget*)canvas->canvasWidget;
  cw->setColor(0x70,0x70,0x70,0xff);
  milanImage *image = new milanImage((unsigned char*)"bg.jpg");
  cw->addImage(image);
  image = new milanImage((unsigned char*)"image.jpg");
  image->mx = 6;
  image->my = 6;
  widget->addImage(image);
  window->triggerExpose();
  
  window->eventLoop();
  return 0;
}

