#include "api.h"

extern "C" {
  #include "os.h"
  #include "system.h"
}

DQObject::DQObject( )
{
}

DQObject::~DQObject( )
{
}

DQ::DQ( )
{
}

DQ::~DQ( )
{
}

DQWidget::DQWidget( )
{
  low = new DQObject();
}

DQWidget::~DQWidget( )
{
  free(low);
}

DQButton::DQButton( )
{
}

DQButton::~DQButton( )
{
}

DQCanvas::DQCanvas( )
{
}

DQCanvas::~DQCanvas( )
{
}
