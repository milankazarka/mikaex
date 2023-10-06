/** Copyright Mika Limited
 *
 */
#include "mikaScene.h"
#include "mikaWindow.h"

#include "lowCommon.h"
#include "lowObjects.h"

mikaScene::mikaScene( dhandle window )
{
  #ifdef _DEBUG
    printf("mikaScene::mikaScene\n");
  #endif
  static int mapped = 0;
  if (!window)
  {
    #ifdef _DEBUG
      printf("mikaScene::mikaScene invalid argument\n");
    #endif
  }
  mikaWindow *dWindow = (mikaWindow*)window;
  struct mikaWindowLow *dWindowLow = (struct mikaWindowLow*)dWindow->lowWindow;
  lowScene = malloc(sizeof(struct mikaSceneLow));
  struct mikaSceneLow *dSceneLow = (struct mikaSceneLow*)lowScene;
  dSceneLow->attr = dWindowLow->attr;
  #ifdef _DEBUG
    printf("mikaScene::mikaScene creating low level window frame(%dx%d+%d+%d) parent(%p)\n",
      dSceneLow->attr.w,dSceneLow->attr.h,
      dSceneLow->attr.x,dSceneLow->attr.y,
      dWindowLow->window);
  #endif
  
  /** initialize the low level Window to host this scene
   */
  dSceneLow->window = tr_WindowInit(NULL,dWindowLow->window,&dSceneLow->attr);
  if (!dSceneLow->window)
  {
    #ifdef _DEBUG
      printf("mikaScene::mikaScene error couldn't initialize low level Window\n");
    #endif
  }
  
  /** historical, needs refactoring
   */
  if (mapped==0)
  {
    #ifdef _DEBUG
      printf("mikaScene::mikaScene mapping window\n");
    #endif
    tr_WindowMap(dSceneLow->window);
    mapped = 1;
  }
  tr_WindowToTop(dSceneLow->window);
}

mikaScene::~mikaScene( )
{
}
