#include "mikaKit.h"
#include "mikaWindow.h"
#include "mikaScene.h"
#include "mikaCanvas.h"
#include "mikaWidget.h"
#include "mikaImage.h"

int main( )
{
  mikaKit *kit = new mikaKit((unsigned char*)"kitTest");
  if (kit)
  {
    struct mikaFrame frame;
    frame.x = 0;
    frame.y = 0;
    frame.w = 320;
    frame.h = 320;
    mikaWindow *window = new mikaWindow(&frame);
    mikaScene *mainScene = new mikaScene((dhandle)window);
    window->addScene(mainScene);
    mikaCanvas *canvas = new mikaCanvas((dhandle)mainScene);
    
    frame.x = 10;
    frame.y = 10;
    frame.w = 120;
    frame.h = 120;
    mikaWidget *widget = new mikaWidget(canvas,&frame);
    canvas->addWidget(widget);
    
    frame.x = 0;
    frame.y = 0;
    frame.w = 120;
    frame.h = 120;
    mikaImage *image = new mikaImage((unsigned char*)"img/whisky.jpg",&frame);
    widget->addImage(image);
    
    window->eventLoop();
  }
  return 0;
}
