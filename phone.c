#include "phone.h"

#define _APP_SIZE_W 520
#define _APP_SIZE_H 520
#define _APP_FONT 34
#define _APP_UI_FRAME 10

mikaPhoneWindow::mikaPhoneWindow( mikaPhoneWindow *parent )
{
    myParent = parent;    
    
    attr.x = 0;
    attr.y = 0;
    attr.w = _APP_SIZE_W;
    attr.h = _APP_SIZE_H;
    
    root    = tr_WindowInit(NULL,NULL,&attr);
    window   = tr_WindowInit(NULL,root,&attr);
  
    tr_WindowMap(window);
    tr_WindowToTop(window);
    canvas = tr_CanvasInit(NULL,&attr,window);
    
    form_CanvasSetMov(canvas->widget.sub,CAN_MOV_NONE);
    tr_SetRound(canvas,2);
    initCanvas();
}

mikaPhoneWindow::~mikaPhoneWindow( )
{
    
}

int mikaPhoneWindow::initCanvas( )
{
    tr_SetColor(canvas,(char*)"#ffffff");
    
    struct tr_Attr battr;
    battr = tr_GetAttr(65,40,5,5);
    struct tr_Object *title = tr_ButtonInitEx2(NULL,&battr,canvas);
    tr_SetColor(title,(char*)"#ffffff");
    tr_SetRound(title,2);
    tr_CanvasAddObjStatic(canvas,title);
    struct tr_Object *titleBg = tr_ImageInitEx(NULL,&battr,(char*)"imgTelgo/logo.jpg");
    tr_AddImage(title,titleBg);
    tr_ImagePosCenter(titleBg,title);
  
    return 0;
}

int mikaPhoneWindow::eventLoop( )
{
    interfaceSetBackplaneSize(0,0);
    activitySetMovable(&bigInterface->activity,1,0);
  
    tr_WindowToTop(window);
    tr_WindowQueue(root);    
    
    return 0;
}

mikaPhoneButton::mikaPhoneButton( mikaPhoneWindow *parent, struct tr_Attr attr )
{   
    myParent = parent;
    myAttr = attr;
    
    next = buttons;
    buttons = this;
    onButtonHandler = NULL;
    receiver = NULL;
    
    button = tr_ButtonInitEx2(NULL,&myAttr,myParent->canvas);
    tr_SetColor(button,(char*)"#e0e0e0");
    tr_SetFontColorEx(button,(char*)"#ffffff");
    tr_SetFontSize(button,23);
    tr_SetRound(button,2);
    tr_CanvasAddObjStatic(myParent->canvas,button);
    tr_SetAction(button,onKeyboardButton,TR_ACTION_BUTTON);
}

mikaPhoneButton::~mikaPhoneButton( )
{
    
}

void *onKeyboardButton( void *data )
{
    if (!data)
        return NULL;

    int n = 0;
    struct gui_Event *event = (struct gui_Event*)data;
    struct gui_Widget *widget = event->widget;
    mikaPhoneButton *current = buttons;
    while(current)
    {
        if (current->button->widget.sub==widget)
        {
            printf("onKeyboardButton hit %d\n",n);
            if (current->receiver)
            {
                mikaPhoneInputField *field = (mikaPhoneInputField*)current->receiver;
                char *text = (char*)field->text;
                strcat(text,(char*)current->id);
                field->updateText();
            }
            if (current->onButtonHandler)
            {
                current->onButtonHandler(current);
            }
        }
        current = current->next;
        n++;
    }
    
    return NULL;
}

void *onCallButton( void *data )
{
    if (!data)
        return NULL;
    printf("onCallButton\n");
    return NULL;
}

mikaPhoneInputField::mikaPhoneInputField( mikaPhoneWindow *parent, struct tr_Attr attr )
{
    myParent = parent;
    myAttr = attr;
    strcpy((char*)text,"");
    
    field = tr_ButtonInitEx2(NULL,&myAttr,myParent->canvas);
    tr_SetColor(field,(char*)"#e0e0e0");
    tr_SetFontColorEx(field,(char*)"#ffffff");
    tr_SetFontSize(field,23);
    tr_SetRound(field,2);
    tr_CanvasAddObjStatic(myParent->canvas,field);
}

mikaPhoneInputField::~mikaPhoneInputField( )
{
   
}

int mikaPhoneInputField::updateText( )
{
    printf("updateText %s\n",(char*)text);
    tr_ChangeTextExPos(field,(char*)text,10,(myAttr.h/2)+3);
    tr_WindowToTop(myParent->window);
    tr_WindowExpose(myParent->window);
    return 0;
}

mikaPhoneKeyboard::mikaPhoneKeyboard( mikaPhoneWindow *window )
{
    myWindow = window;
    struct tr_Object *canvas = window->canvas;
    
    int n;
    struct tr_Attr battr;

    battr.x = _APP_UI_FRAME;
    battr.y = 60;
    battr.w = _APP_SIZE_W-(battr.x*2);
    battr.h = 50;
    field = new mikaPhoneInputField(myWindow,battr);    
    
    battr.x = _APP_UI_FRAME;
    battr.y = 120;
    battr.w = 70;
    battr.h = battr.w;
    char *keyid = NULL;
    for(n = 0; n < 10; n++)
    {
        printf("mikaPhoneKeyboard::mikaPhoneKeyboard %dx%d+%d+%d (%d)\n",
            battr.w,battr.h,battr.x,battr.y,n);
        mikaPhoneButton *button = new mikaPhoneButton(myWindow,battr);
        button->receiver = field;
        tr_SetAction(button->button,onKeyboardButton,TR_ACTION_BUTTON);
        keyid = (char*)malloc(3);
        if (n!=9)
            sprintf(keyid,"%d",n+1);
        else
            strcpy(keyid,"0");
        strcpy((char*)button->id,keyid);
        tr_ChangeTextExPos(button->button,(char*)keyid,(battr.w/2)-(_APP_FONT/2)-5,
                           battr.h/2); 
        free(keyid);        
        buttons[n] = button;
        if (n==2||n==5||n==8)
        {
            battr.y+=battr.w+2;
            battr.x = 10;
        }
        else
            battr.x+=battr.w+2;
    }
    
    battr.w *= 3;
    battr.w+=4;
    battr.x = 10;
    battr.y += battr.h+2;
    call = new mikaPhoneButton(myWindow,battr);
    tr_SetColor(call->button,(char*)"#88d48f");
    tr_SetAction(call->button,onCallButton,TR_ACTION_BUTTON);    
}

mikaPhoneKeyboard::~mikaPhoneKeyboard( )
{
    
}

mikaPhoneRoot::mikaPhoneRoot( )
{
    
}

mikaPhoneRoot::~mikaPhoneRoot( )
{
    
}

mikaPhone::mikaPhone( )
{
    
}

mikaPhone::~mikaPhone( )
{
    
}

int main( )
{
    interfaceSetName((unsigned char*)"PHONE");

    osInit();
    trop_Init();
    gui_Init();
    tr_WmInit();
  
    layout = NULL;
    rotation = ROT_NONE;

    mikaPhoneWindow *pw = new mikaPhoneWindow(NULL);
    mikaPhoneKeyboard *pk = new mikaPhoneKeyboard(pw);
    pw->eventLoop();  
  
    return 0;
}
