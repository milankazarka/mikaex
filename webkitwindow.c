#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <webkit/webkitdefines.h>
#include <webkit/webkitnetworkrequest.h>
#include <webkit/webkitwebview.h>
#include "mikaHttp.h"
#include "xipc.h"
#include <pthread.h>

#ifndef _DEBUG
  #define _DEBUG
#endif

static GtkWidget 	*window = NULL;
static WebKitWebView 	*webview = NULL;
static WebKitNetworkRequest *request = NULL;
static ipc_t 		*ipc = NULL;

static ipc_module_t 	*ipcModule = NULL;
static ipc_module_t	*ipcModuleTouch = NULL;

void _inject_event( int x, int y )
{
  GdkEvent *event;
  // GDK_BUTTON_PRESS
  // GDK_BUTTON_RELEASE
  
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

  #ifdef _DEBUG
    printf("after event\n");
  #endif
}

void _repaint( )
{
  gtk_widget_queue_draw(((GtkWidget*)webview));
}

void *_event_thread(void*data)
{
  sleep(5);
  while(1)
  {
    _inject_event(30,30);
    sleep(1);
    _inject_event(100,200);
    sleep(1);
  }
}

void *_gtk_thread(void*data)
{
  #ifdef _DEBUG
    printf("webkitwindow gtk_main()\n");
  #endif
  gtk_main();
}

/** command for the Webkit handler
 */
void *onWebkitCommand( void *ipc, unsigned char *command )
{
  #ifdef _DEBUG
    printf("onWebkitCommand command(%s)\n",(char*)command);
  #endif
  
  char *section = strstr((char*)command,"touch");
  if (section)
  {
    int n,run,x,y;
    unsigned char tmp[8];
    char *xstr = strstr(section,"x");
    char *ystr = strstr(section,"y");
    run = 0;
    for(n = 1; n < 6; n++)
    {
      if (xstr[n]==',')
      {
        tmp[run] = 0x00;
        break;
      }
      tmp[run] = xstr[n];
      run++;
    }
    x = atoi((char*)tmp);
    run = 0;
    for(n = 1; n < 6; n++)
    {
      if (ystr[n]==',')
      {
        tmp[run] = 0x00;
        break;
      }
      tmp[run] = ystr[n];
      run++;
    }
    y = atoi((char*)tmp);
    #ifdef _DEBUG
      printf("onWebkitCommand touch(%dx%d)\n",x,y);
    #endif
    _inject_event(x,y);
  }
  else
  {
    #ifdef _DEBUG
      printf("onWebkitCommand expose\n");
    #endif
    _repaint();
  }
}

void *onWebkitCommandTouch( void *ipc, unsigned char *command )
{
  #ifdef _DEBUG
    printf("onWebkitCommandTouch\n");
  #endif
}

int main( int argc, char *argv[] )
{
  #ifdef _DEBUG
    printf("webkitwindow init\n");
  #endif
  
  g_thread_init(NULL);
  gtk_init(&argc,&argv);
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  if (window)
  {
    #ifdef _DEBUG
      printf("webkitwindow window created\n");
    #endif
  }
  gtk_widget_set_size_request(window,480,480);
  webview = (WebKitWebView*)webkit_web_view_new();
  if (webview)
  {
    #ifdef _DEBUG
      printf("webkitwindow webview created\n");
    #endif
  }

  //request = webkit_network_request_new("192.168.0.1");

  struct mikaHttp http;
  // http://iposcash.com/mob/app/demo/
  mikaHttpGet(&http,(unsigned char*)"http://www.simplestrawberry.com");
  unsigned char *data = mikaHttpData(&http,NULL);
  //webkit_web_view_load_html_string(webview,"<p>Test<a href=\"http://www.google.com\">google</a></p>","file:///");
  if (data)
  {
    webkit_web_view_load_html_string(webview,(const gchar*)data,"http://www.simplestrawberry.com/");
    free(data);
  }
  //webkit_web_view_load_request(webview,request);
  //webkit_web_view_load_uri(webview,"192.168.0.1");
  
  gtk_container_add((GtkContainer*)window,(GtkWidget*)webview);
  gtk_widget_show_all(window);
  #ifdef _DEBUG
    printf("webkitwindow window shown\n");
  #endif
  
  gtk_window_iconify((GtkWindow*)window);
  
  pthread_t t;
  //pthread_create(&t,NULL,_gtk_thread,NULL); // gtk_main();
  //gtk_main();
  #ifdef _DEBUG
    printf("after _gtk_thread\n");
  #endif
  
  //pthread_create(&t,NULL,_event_thread,NULL);
  
  ipc = ipcInit(NULL);
  if (ipc)
  {
    // initialize the listener for redrawing
    ipc->listenport = 46200;
    ipcModule = (struct ipc_module_t*)malloc(sizeof(struct ipc_module_t));
    strcpy((char*)ipcModule->keyword,"webkitcommand");
    ipcModule->oncommand = onWebkitCommand;
    ipcModuleRegister(ipc,ipcModule);
    ipcRun(ipc);
  }
  else
  {
    #ifdef _DEBUG
      printf("webkitwindow error couldn't initialize IPC\n");
    #endif
    exit(2);
  }
  
  gtk_main();
  
  while(1)
  {
    int run = 0;
    #ifdef _DEBUG
      printf("_main_thread run(%d)\n",run);
    #endif
    run++;
    sleep(1);
  }
  
  return 0;
}
