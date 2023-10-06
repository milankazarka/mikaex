#include <stdio.h>
#include <stdlib.h>

extern "C" {
  #include "system.h"
}

#define STATIC

class MikaObj
{
public:
  MikaObj( ) { }
  ~MikaObj( ) { }
  
  MikaObj	*next;
};

class MikaWAttr
{
public:
  MikaWAttr( );
  MikaWAttr( int _w, int _h, int _x, int _y );
  ~MikaWAttr( );
  
  int		x;
  int		y;
  int		w;
  int		h;
};

class MikaWidget : public MikaObj
{
public:
  MikaWidget( MikaWidget *parent );
  MikaWidget( );
  ~MikaWidget( );

  MikaWidget	*parent;
};

class Mika 
{
public:
  Mika( );
  ~Mika( );
};

Mika	*_main;

Mika::Mika( )
{
}

Mika::~Mika( )
{
}

MikaWAttr::MikaWAttr( )
{
  w = h = x = y = 0;
}

MikaWAttr::MikaWAttr( 
  int _w, int _h, 
  int _x, int _y )
{
  w = _w;
  h = _h;
  x = _x;
  y = _y;
}

void _mika_widget( )
{
  MikaObj();
}

MikaWidget::MikaWidget( )
{
  _mika_widget();
  parent = NULL;
}

MikaWidget::MikaWidget( MikaWidget *_parent )
{
  _mika_widget();
  parent = _parent;
}

MikaWidget::~MikaWidget( )
{
}

#ifdef STATIC
  int main( )
  {
    _main = new Mika();
    return 0;
  }
#endif
