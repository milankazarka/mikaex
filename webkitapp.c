#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <webkit/webkitdefines.h>
#include <webkit/webkitnetworkrequest.h>
#include <webkit/webkitwebview.h>
#include "mikaHttp.h"
#include <pthread.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/un.h>

#define _UDS_PATH "./webkitapp.sock"

#ifndef _DEBUG
  #define _DEBUG
#endif

GtkWidget *window = NULL;
WebKitWebView *webview = NULL;
WebKitNetworkRequest *request = NULL;

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

  printf("after event\n");
}

/** testing injection of events
 */
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

void *_event_thread_uds(void*data)
{
  int 			sockfd;
  struct sockaddr_un 	address;
  size_t 		address_len;
  
  sockfd = socket(PF_UNIX,SOCK_STREAM,0);
  if (sockfd<0)
  {
    printf("webkitapp couldn't create socket\n");
    return NULL;
  }
  unlink(_UDS_PATH);
  address.sun_family = AF_UNIX;
  address_len = sizeof(address.sun_family) + sprintf(address.sun_path,_UDS_PATH);
  if (bind(sockfd,(struct sockaddr*)&address,address_len) != 0)
  {
    printf("webkitapp couldn't bind socket\n");
    return NULL;
  }
  if (listen(sockfd,5) != 0)
  {
    printf("webkitapp couldn't listen on socket\n");
    return NULL;
  }
  int connfd;
  int bytes;
  int x = -1, y = -1;
  unsigned char msg[256];
  while(
    (connfd = accept(sockfd,(struct sockaddr*)&address,&address_len)) > -1
  )
  {
    printf("\twebkitapp connection\n");
    msg[0] = 0x00;
    bytes = read(connfd,(char*)msg,255);
    msg[bytes] = 0x00;
    if (strlen((char*)msg)>0)
    {
      printf("\twebkitapp msg(%s)\n",(char*)msg);
      int n;
      int pos = 0;
      unsigned char tmp[64];
      for(n = 0; n < strlen((char*)msg); n++)
      {
        if (msg[n]==' ')
        {
          strncpy((char*)tmp,(char*)msg,n);
          tmp[n] = 0x00;
          x = atoi((char*)tmp);
          printf("\twebkitapp coordinate_x(%d)\n",x);
          pos = n+1;
        }
        else if (msg[n]=='\n')
        {
          strncpy((char*)tmp,(char*)msg+pos,n-pos);
          tmp[n-pos] = 0x00;
          y = atoi((char*)tmp);
          printf("\twebkitapp coordinate_y(%d)\n",y);
        }
      }
      if (x!=-1 && y!=-1)
        _inject_event(x,y);
    }
    close(connfd);
    usleep(10000);
  }
  unlink(_UDS_PATH);
}

void *_gtk_thread(void*data)
{
  gtk_main();
}

void *_iconify_thread(void*data)
{
  sleep(5);
  gtk_window_iconify((GtkWindow*)window);
}

int main( int argc, char *argv[] )
{
  g_thread_init(NULL);
  gtk_init(&argc,&argv);
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request(window,360,360);
  webview = (WebKitWebView*)webkit_web_view_new();

  struct mikaHttp http;
  mikaHttpGet(&http,(unsigned char*)"127.0.0.1/mikaMobile.com/index.php");
  unsigned char *data = mikaHttpData(&http,NULL);

  if (data)
  {
    webkit_web_view_load_html_string(webview,(const gchar*)data,"http://127.0.0.1/mikaMobile.com/");
    free(data);
  }
  
  gtk_container_add((GtkContainer*)window,(GtkWidget*)webview);
  gtk_widget_show_all(window);
  
  pthread_t t;
  
  pthread_create(&t,NULL,_iconify_thread,NULL);
  pthread_create(&t,NULL,_event_thread_uds,NULL);
  pthread_create(&t,NULL,_gtk_thread,NULL);
  #ifdef _DEBUG
    printf("after _gtk_thread\n");
  #endif
  
  int run = 0;
  while(1)
  {
    run++;
    sleep(1);
  }
  
  return 0;
}
