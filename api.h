/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _API_H_
#define _API_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class DQObject {
  public:
    DQObject( );
    ~DQObject( );
};

class DQ : public DQObject {
  public: 
    DQ( );
    ~DQ( );
};

class DQWidget : public DQObject {
  protected:
    DQObject *low;
  public:
    DQWidget( );
    ~DQWidget( );
};

class DQButton : public DQWidget
{
  public:
    DQButton( );
    ~DQButton( );
};

class DQCanvas : public DQWidget
{
  public:
    DQCanvas( );
    ~DQCanvas( );
};

#endif
