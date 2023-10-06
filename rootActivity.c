#include "rootActivity.h"
#include "config.h"

#define _POS_ONEVENT	0

/** set the root onEvent identifier, after an event on the root window
 */
int rootActivityOnEvent( struct rootActivity *activity )
{
  static struct activityObjects *myActObjects = NULL;
  static struct activityObject oRoot;
  
  #ifdef _DEBUG
    printf("rootActivityOnEvent\n");
  #endif
  
  if (!activity)
  {
    #ifdef _DEBUG
      printf("rootActivityOnEvent ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  if (myActObjects==NULL)
  {
    myActObjects = activityObjectsClone();
    if ( activityObjectRegister(myActObjects,(unsigned char*)"rootActivity") != 0 )
    {
      #ifdef _DEBUG
        printf("rootActivityOnEvent ERROR couldn't register activity object\n");
      #endif
      return 1;
    }
    else
    {
      #ifdef _DEBUG
        printf("rootActivityOnEvent root activity object registered\n");
      #endif
    }
  }
  
  activityObjectClone(myActObjects,(unsigned char*)"rootActivity",&oRoot);
  oRoot.fastData[_POS_ONEVENT] = 1;
  activityObjectSet(myActObjects,(unsigned char*)"rootActivity",&oRoot);
  
  return 0;
}
