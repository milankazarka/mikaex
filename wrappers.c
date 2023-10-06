#include "wrappers.h"

int wrapperInitApp( )
{
    #ifdef _DEBUG
        printf("wrapperInitApp\n");
    #endif
    osInit();
    trop_Init();
    gui_Init();
    tr_WmInit();
  
    layout = NULL;
    rotation = ROT_NONE;
    
    return 0;
}
