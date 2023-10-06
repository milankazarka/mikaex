/** Copyright Mika Limited
 *  developed by Milan Kazarka
 */
#include "mikaKit.h"
#include "config.h"

extern "C" {
    #include "interface.h"
    #include "common.h"
    #include "config.h"
    #include "os.h"
    #include "system.h"
    #include "systemip.h"
    #include "global.h"
    #include "xml.h"
    #include "wm.h"
    #include "dom.h"
    #include "activityObject.h"
    #include "rootControllerSync.h"
    #include "basic.h"
}

mikaKit *globalMikaKit = NULL;

mikaKit::mikaKit( unsigned char *appName )
{
    #ifdef _DEBUG
        printf("mikaKit::mikaKit\n");
    #endif
    if (globalMikaKit)
    {
        printf("mikaKit warning MikaKit already initialized\n");
    }
    globalMikaKit = this;
    osInit();
    trop_Init();
    gui_Init();
    tr_WmInit();
    
    layout = NULL;
    rotation = ROT_NONE;
}

mikaKit::~mikaKit( )
{
}
