/** Copyright Mika Limited
 *
 */
#include "mikaCanvas.h"
#include "mikaWindow.h"
#include "mikaScene.h"
#include "mikaWidget.h"

#include "lowCommon.h"
#include "lowObjects.h"

/** create a canvas of the same size as the holding scene
 */
mikaCanvas::mikaCanvas( dhandle scene )
{
  #ifdef _DEBUG
    printf("mikaCanvas::mikaCanvas\n");
  #endif
  if (!scene)
  {
    #ifdef _DEBUG
      printf("mikaCanvas::mikaCanvas invalid argument\n");
    #endif
  }
  mikaScene *dScene = (mikaScene*)scene;
  struct mikaSceneLow *dSceneLow = (struct mikaSceneLow*)dScene->lowScene;
  lowCanvas = malloc(sizeof(struct mikaCanvasLow));
  struct mikaCanvasLow *dCanvasLow = (struct mikaCanvasLow*)lowCanvas;
  tr_WindowToTop(dSceneLow->window);
  
  dCanvasLow->attr = dSceneLow->attr;
  dCanvasLow->canvas = tr_CanvasInit(NULL,&dCanvasLow->attr,dSceneLow->window);
  if (!dCanvasLow->canvas)
  {
    #ifdef _DEBUG
      printf("mikaCanvas::mikaCanvas error low level canvas couldn't be created\n");
    #endif
  }
  dCanvasLow->parentWindow = dSceneLow->window;
  form_CanvasSetMov(dCanvasLow->canvas->widget.sub,CAN_MOV_NONE);
  tr_SetColor(dCanvasLow->canvas,(char*)"#f0f0f0");
}

mikaCanvas::~mikaCanvas( )
{
}

/** set the way the objects inside of the canvas are moved
 *  
 *  todo: add error checks
 */
int mikaCanvas::setMovement( int movementType )
{
  myMovementType = movementType;
  struct mikaCanvasLow *dCanvasLow = (struct mikaCanvasLow*)lowCanvas;
  tr_WindowToTop(dCanvasLow->parentWindow);
  
  switch(myMovementType)
  {
    case _CANVAS_MOVEMENT_TYPE_NONE:
      form_CanvasSetMov(dCanvasLow->canvas->widget.sub,CAN_MOV_NONE);
      break;
    default:
      printf("mikaCanvas::setMovement warning type not recognized\n");
      break;
  }
  return 0;
}

/** set the color of the canvas (unsigned char*)"#RRGGBB"
 */
int mikaCanvas::setColor( unsigned char *colorCode )
{
  if (!colorCode)
    return 1;
  
  if (
    strlen((char*)colorCode)!=7
    )
  {
    printf("mikaCanvas::setColor error colorCode form(#RRGGBB) incorrect\n");
    return 1;
  }
  
  struct mikaCanvasLow *dCanvasLow = (struct mikaCanvasLow*)lowCanvas;
  tr_WindowToTop(dCanvasLow->parentWindow);
  tr_SetColor(dCanvasLow->canvas,(char*)colorCode);
  
  return 0;
}

/** add a widget to the canvas, the object is placed dynamicaly
 *
 */
int mikaCanvas::addWidget( dhandle widget )
{
  #ifdef _DEBUG
    printf("mikaCanvas::addWidget\n");
  #endif
  if (!widget)
    return 1;
  
  struct mikaCanvasLow *dCanvasLow = (struct mikaCanvasLow*)lowCanvas;
  mikaWidget *dWidget = (mikaWidget*)widget;
  struct mikaWidgetLow *dWidgetLow = (struct mikaWidgetLow*)dWidget->lowWidget;
  tr_WindowToTop(dCanvasLow->parentWindow);
  tr_CanvasAddObj(dCanvasLow->canvas,dWidgetLow->widget);
  
  return 0;
}
