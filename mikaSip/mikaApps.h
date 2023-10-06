/**  
  Copyright (C) 2012 Milan Kazarka
  milan.kazarka.office@gmail.com
      
  You may distribute under the terms of the Artistic License, 
  as specified in the README file. You can find a copy of the
  license at LICENSE.
*/
#ifndef _MIKA_APPS_H_
#define _MIKA_APPS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <pthread.h>

#include "mikaUID.h"

#define _MIKA_APP_APPS_ROOT "apps"
#define _MIKA_APP_NAMELEN 512
#define _MIKA_APPS_CLOCKTIK 10000

/** handling a list of applications, whereas we don't have
 *  the actual mikaApp objects, just some info
 */
typedef struct appRegisterObject {
  unsigned char uuid[64];
  struct appRegisterObject *next;
} appRegisterObject;

typedef struct mikaAppDelegate {
  void *(*onFinish)( void *appData );
} mikaAppDelegate;

/** defining an application running on a system
 */
class mikaApp {
  public:
    mikaApp *next;
    unsigned char myAppRoot[_MIKA_APP_NAMELEN*2];
    unsigned char myAppName[_MIKA_APP_NAMELEN];
    int pID; // only used by parent process
    mikaUID *uid;
    struct mikaAppDelegate *delegate;
    
    mikaApp( unsigned char *appRoot );
    mikaApp( unsigned char *command, int root );
    ~mikaApp( );
    
    int onFinished();
};

typedef struct mikaAppsDelegate_t {
} mikaAppsDelegate_t;

class mikaAppsDelegate {
  public:
    struct mikaAppsDelegate_t delegate;
    
    mikaAppsDelegate( );
    ~mikaAppsDelegate( );
};

class mikaApps {
  public:
    mikaApp *apps;
    unsigned char myAppDirectory[_MIKA_APP_NAMELEN];
    pthread_mutex_t mutex;
    int parentPID;
  
    /** automatically start a set of applications
     *  residing in a speciffic directory
     */
    mikaApps( unsigned char *appDirectory );
    /** just create
     */
    mikaApps( );
    ~mikaApps( );
    
    int addApp( mikaApp *app, int sync );
    int runAll( );
    int stopAll( );
    int stopChildrenNamed( unsigned char *name );
    int runApp( mikaApp *app );
    int runProcessing( );
    int removeApp( mikaApp *app );
};

#endif
