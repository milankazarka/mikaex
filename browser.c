/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

extern "C" {
  #include "interface.h"
  #include "os.h"
  #include "system.h"
  #include "global.h"
  #include "wm.h"
}

#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <webkit/webkitdefines.h>
#include <webkit/webkitnetworkrequest.h>
#include <webkit/webkitwebview.h>
#include "mikaHttp.h"

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/un.h>

#define _UDS_PATH "webkitapp.sock"

#define _EMBEDDED_WEBKIT_NO
#ifndef _DEBUG
  #define _DEBUG
#endif

GtkWidget *window = NULL;
WebKitWebView *webview = NULL;
WebKitNetworkRequest *request = NULL;

struct tr_Window *root = NULL;
struct tr_Window *wmain = NULL;

struct tr_Object *cmain = NULL;

void *_inject_event( int x, int y )
{
  #ifdef _DEBUG
    printf("_inject_event x(%d) y(%d)\n",x,y);
  #endif
  #ifdef _EMBEDDED_WEBKIT
    GdkEvent *event;
  
    event = gdk_event_new(GDK_BUTTON_PRESS);
    event->button.window = ((GtkWidget*)webview)->window;
    event->button.x = x;
    event->button.y = y;
    event->button.send_event = (char)true;
    gdk_event_put(event);
    event->button.window = 0;
    gdk_event_free(event);
  
    usleep(10000);
  
    event = gdk_event_new(GDK_BUTTON_RELEASE);
    event->button.window = ((GtkWidget*)webview)->window;
    event->button.x = x;
    event->button.y = y;
    event->button.send_event = (char)true;
    gdk_event_put(event);
    event->button.window = 0;
    gdk_event_free(event);
  #else
    
    int 		sockfd;
    struct sockaddr_un 	address;
    size_t 		address_len;
  
    sockfd = socket(PF_UNIX,SOCK_STREAM,0);
    if (sockfd<0)
    {
      printf("webkitappclient couldn't create socket\n");
      return NULL;
    }
    address.sun_family = AF_UNIX;
    address_len = sizeof(address.sun_family) + sprintf(address.sun_path,_UDS_PATH);
    if (connect(sockfd,(struct sockaddr*)&address,address_len) != 0)
    {
      printf("webkitappclient couldn't connect\n");
      return NULL;
    }
  
    unsigned char msg[128];
    sprintf((char*)msg,"%d %d\n",x,y);
    #ifdef _DEBUG
      printf("webkitappclient msg(%s)\n",(char*)msg);
    #endif
    write(sockfd,(char*)msg,strlen((char*)msg));
  
    close(sockfd);
  #endif
}

void *_gtk_thread(void*data)
{
  gtk_main();
}

void *_assign_injection(void*data)
{
  sleep(5);
  
  #ifdef _DEBUG
    printf("_assign_injection shm(%d)\n",bigInterface->activity.shm->id);
  #endif
  if (1)
  {
    FILE *fd;
    if ((fd = fopen("bridgeShm.log","w")))
    {
      unsigned char tmp[64];
      sprintf((char*)tmp,"%d\n",bigInterface->activity.shm->id);
      fwrite((void*)tmp,strlen((char*)tmp),1,fd);
      fclose(fd);
    }
  }
  
  bigInterface->onButtonDown = _inject_event;
  bigInterface->onButtonUp = _inject_event;
  sleep(2);
  system("./webkitapp");
}

int main( int argc, char *argv[] )
{
  struct tr_Attr	mainAttr;
  struct tr_Attr	buttonAttr;
  struct tr_Attr	imageAttr;

  struct tr_Object	*bPoster;
  struct tr_Object	*bimage;
  char			*text;

  interfaceSetName((unsigned char*)"BROWSER");

  osInit();
  trop_Init();
  gui_Init();
  tr_WmInit();
  
  layout = NULL;
  rotation = ROT_NONE;
  
  mainAttr.x = 0;
  mainAttr.y = 0;
  mainAttr.w = 360;
  mainAttr.h = 360;

  buttonAttr.x = 0;
  buttonAttr.y = 0;
  buttonAttr.w = 340;
  buttonAttr.h = 340;

  imageAttr.x = 0;
  imageAttr.y = 0;
  imageAttr.w = 340;
  imageAttr.h = 340;
  
  root = tr_WindowInit(NULL,root,&mainAttr);
  wmain = tr_WindowInit(NULL,root,&mainAttr);
  
  tr_WindowMap(wmain);
  tr_WindowToTop(wmain);
  
  pthread_t t;
  pthread_create(&t,NULL,_assign_injection,NULL);
  
  cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
  tr_SetRound(cmain,3);
  tr_SetColor(cmain,(char*)"#101010");
  form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);
  
  bPoster = tr_ButtonInitEx2(NULL,&buttonAttr,cmain);
  tr_SetRound(bPoster,4);
  tr_SetColor(bPoster,(char*)"#101010");
  tr_CanvasAddObj(cmain,bPoster);
  
  tr_WindowToTop(wmain);
  
  #ifdef _EMBEDDED_WEBKIT
    g_thread_init(NULL);
    gtk_init(&argc,&argv);
  
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window,360,360);
    webview = (WebKitWebView*)webkit_web_view_new();
  
    struct mikaHttp http;
    mikaHttpGet(&http,(unsigned char*)"192.168.0.1");
    unsigned char *data = mikaHttpData(&http,NULL);
    if (data)
    {
      webkit_web_view_load_html_string(webview,(const gchar*)data,"http://192.168.0.1");
      free(data);
    }
  
    gtk_container_add((GtkContainer*)window,(GtkWidget*)webview);
    gtk_widget_show_all(window);
  
    pthread_t t;
  
    pthread_create(&t,NULL,_gtk_thread,NULL);

    tr_WindowQueue(root);
  
    while(1)
    {
      sleep(1);
    }
  #else
    tr_WindowQueue(root);
  #endif
  
  return 0;
}
