#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define _STATIC

typedef unsigned short int DQINT;

typedef struct sDQPoint
{
  DQINT		x;
  DQINT		y;
} sDQPoint;
typedef struct sDQPoint DQPoint;

typedef struct sDQSize
{
  DQINT		width;
  DQINT		height;
} sDQSize;
typedef struct sDQSize DQSize;

typedef struct sDQRect
{
  DQPoint	origin;
  DQSize	size;
} sDQRect;
typedef struct sDQRect DQRect;

DQRect DQRectMake( DQINT x, DQINT y, DQINT width, DQINT height )
{
  DQRect rect;
  rect.origin.x 	= x;
  rect.origin.y 	= y;
  rect.size.width 	= width;
  rect.size.height 	= height;
  return rect;
}

class DQNode
{
  public:
    unsigned char 	*mName;
    unsigned short int 	mNameLen;
  
    DQNode( );
    DQNode( unsigned char *name, unsigned short int nameLen );
    ~DQNode( );
  protected:
    int onError( int code, unsigned char *message );
};

class DQUINodePool
{
  public:
    DQUINodePool( );
    ~DQUINodePool( );
  protected:
    int addNode( DQNode *node, bool managed );
    int removeNode( DQNode *node );
};

typedef struct sDQUIEventExpose
{
  DQRect		rect;
} sDQUIEventExpose;
typedef struct sDQUIEventExpose DQUIEventExpose;

typedef struct sDQUIEventButton
{
  DQPoint		rect;
} sDQUIEventButton;
typedef struct sDQUIEventButton DQUIEventButton;

typedef struct sDQUIEvent
{
  DQUIEventExpose	expose;
  DQUIEventButton	button;
} sDQUIEvent;
typedef struct sDQUIEvent DQUIEvent;

class DQUINode
{
  public:
    DQRect		mFrame;
    
    DQUINode( );
    DQUINode( DQRect rect );
    ~DQUINode( );
  protected:
    int onDraw( DQUIEvent *event );
    int onButtonDown( DQUIEvent *event );
    int onButtonUp( DQUIEvent *event );
    int onMotion( DQUIEvent *event );
};

DQNode::DQNode( )
{
  mName = NULL;
  mNameLen = 0;
}

DQNode::DQNode( unsigned char *name, unsigned short int nameLen )
{
  mNameLen = nameLen;
  if (!name||nameLen<1)
  {
    mNameLen = NULL;
    return;
  }
  mName = (unsigned char *)malloc(nameLen+1);
  memcpy(mName,name,nameLen);
  mName[nameLen] = 0x00;
}

DQNode::~DQNode( )
{
  if (mNameLen&&mName)
  {
    free(mName);
  }
}

int DQNode::onError( int error, unsigned char *message ) { return 0; }

DQUINode::DQUINode( ) 
{ mFrame = DQRectMake(0,0,0,0); }

DQUINode::DQUINode( DQRect rect )
{
  mFrame = rect;
}

DQUINode::~DQUINode( )
{
}

int DQUINode::onDraw( DQUIEvent *event )
{
  if (!event) return -1;
  return 0;
}

int DQUINode::onButtonDown( DQUIEvent *event )
{
  if (!event) return -1;
  return 0;
}

int DQUINode::onButtonUp( DQUIEvent *event )
{
  if (!event) return -1;
  return 0;
}

int DQUINode::onMotion( DQUIEvent *event )
{
  if (!event) return -1;
  return 0;
}

#ifdef _STATIC
  int main( )
  {
    return 0;
  }
#endif
