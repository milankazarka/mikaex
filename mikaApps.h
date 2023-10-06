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

/** handling a list of applications, whereas we don't have
 *  the actual mikaApp objects, just some info
 */
typedef struct appRegisterObject {
  unsigned char uuid[64];
  struct appRegisterObject *next;
} appRegisterObject;

/** defining an application running on a system
 */
class mikaApp {
  public:
    mikaApp *next;
    unsigned char myAppRoot[_MIKA_APP_NAMELEN*2];
    unsigned char myAppName[_MIKA_APP_NAMELEN];
    int pID; // only used by parent process
    mikaUID *uid;
    
    mikaApp( unsigned char *appRoot );
    ~mikaApp( );
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
  
    /** automatically start a set of applications
     *  residing in a speciffic directory
     */
    mikaApps( unsigned char *appDirectory );
    mikaApps( );
    ~mikaApps( );
    int runAll( );
    int runProcessing( );
    int removeApp( mikaApp *app );
};

#endif
