#include "mikaWidgetDelegate.h"

mikaWidgetDelegate::mikaWidgetDelegate( )
{
}

mikaWidgetDelegate::~mikaWidgetDelegate( )
{
}

void mikaWidgetDelegate::onWidgetDelegate( struct delegateData *data )
{
  #ifdef _DEBUG
    printf("mikaWidgetDelegate::onWidgetDelegate\n");
  #endif
}

