#ifndef _PHONE_H_
#define _PHONE_H_

extern "C" {
  #include "interface.h"
  #include "common.h"
  #include "config.h"
  #include "os.h"
  #include "system.h"
  #include "systemip.h"
  #include "xml.h"
  #include "wm.h"
  #include "dom.h"
  #include "activityObject.h"
  #include "rootControllerSync.h"
  #include "basic.h"
  #include "mikaPosClient.h"
  #include "global.h"
}
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"

class mikaPhoneWindow {
    public:
        mikaPhoneWindow        *myParent;    
        
        struct tr_Window	*root;
        struct tr_Window	*window;
        struct tr_Object	*canvas;
        struct tr_Attr		attr;
        
        mikaPhoneWindow( mikaPhoneWindow *parent );
        ~mikaPhoneWindow( );
        
        int initCanvas( );
        int eventLoop( );
};

class mikaPhoneButton {
    public:
        mikaPhoneButton *next;
        
        mikaPhoneWindow *myParent;    
        struct tr_Object *button;   
        struct tr_Attr myAttr;
        void *(*onButtonHandler)( mikaPhoneButton *button );
        void *receiver;
        unsigned char id[64];
        
        mikaPhoneButton( mikaPhoneWindow *parent, struct tr_Attr attr );
        ~mikaPhoneButton( );
};

mikaPhoneButton *buttons = NULL;
void *onKeyboardButton( void *data );

class mikaPhoneInputField {
    public:
        mikaPhoneWindow *myParent;
        struct tr_Object *field;
        struct tr_Attr myAttr;
        unsigned char text[512];
        
        mikaPhoneInputField( mikaPhoneWindow *parent, struct tr_Attr attr );
        ~mikaPhoneInputField( );
        
        int updateText( );
};

class mikaPhoneKeyboard {
    public:
        mikaPhoneWindow *myWindow;
        mikaPhoneInputField *field;
        mikaPhoneButton *buttons[10];    
        mikaPhoneButton *call;
        
        mikaPhoneKeyboard( mikaPhoneWindow *window );
        ~mikaPhoneKeyboard( );
};

class mikaPhoneRoot {
    public:
        mikaPhoneRoot( );
        ~mikaPhoneRoot( );
};

class mikaPhone {
    public:
        mikaPhone( );
        ~mikaPhone( );
};

#endif
