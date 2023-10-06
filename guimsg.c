/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "system.h"
#include "gui.h"
#include "xml.h"

#ifdef USEX

  int gui_MsgNetworkSetConnection(
    struct gui_Msg *msg,
    struct trop_Conn *conn
  )
  {
    static struct gui_MsgNetwork *mnet = NULL;
    
    if (!msg||!conn) return -1;
    if (!msg->descr) return -1;
    
    mnet = msg->descr;
    
    if (DEBUG==TRUE)
    {
      if (mnet->conn!=NULL)
      {
        printf("gui_MsgNetworkSetConnection change existing connection\n");
      }
    }
    mnet->conn = conn;
    
    return 0;
  }

  /** directly set the message string 
   */
  int gui_MsgSetStringSimple(
    struct gui_Msg *msg,
    char *str
  )
  {
    if (!msg||!str) return 1;
    if (strlen(str)<1) return -1;
    
    msg->str = (char*)malloc(strlen(str)+1);
    strcpy(msg->str,str);
    
    return 0;
  }

  /** sets the message string by parameters
   * format <name=name from=id body=sent_message> 
   */
  int gui_MsgSetXString(
    struct gui_Msg *msg,
    char *name,
    char *from,
    char *body
  )
  {
    static int i;
    static char *_body;
      
    if (!msg||!name||!from||!body)
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("gui_MsgSetXString missing param\n");
      #endif
      return -1;
    }
    
    _body = (char*)malloc(strlen(body)+1);
    strcpy(_body,body);
    
    for (i = 0; i < strlen(_body); i++)
    {
      if (_body[i]==' '||_body[i]=='\t'||_body[i]=='\n') _body[i] = '_';
    }
    
    msg->str = (char*)malloc(20+strlen(name)+strlen(from)+strlen(_body));
    sprintf(msg->str,"<name=%s from=%s body=%s>",
      name,from,_body);
    
    return 0;
  }

  int gui_MsgSetXStringBitStr(
    struct gui_Msg *msg,
    trop_List *bits // list->ptr(char[name256][value256])
  )
  {
    static struct trop_List *current;
    static char bit[20][2][GUIMSGBIT_LEN];
    
    if (!msg||!bits) return -1;
    
    int i;
    for(i = 0; i < 20; i++)
    {
      bit[i][0][0] = 0x00;
      bit[i][1][0] = 0x00;
    }
    
    i = 0;
    current = bits;
    while(current!=NULL)
    {
      strcpy((char*)bit[i][0],current->ptr);
      strcpy((char*)bit[i][1],current->ptr+GUIMSGBIT_LEN);
      i++;
      current = current->next;
    }
    
    return 0;
  }

  int gui_MsgBitsFree( struct trop_List *bits )
  {
    if (!bits) return -1;
    trop_ListDeleteRecursive(bits); 
    return 0;
  }

  /** add a message bit structure to a list 
   */
  struct trop_List *gui_MsgBitsAddBit( 
    struct trop_List *bits,
    char *name,
    char *value 
  )
  {
    static int i;
    
    if (!name || !value) return bits;
    if ( strlen(name)==0 || strlen(value)==0 ||
      strlen(name)>GUIMSGBIT_LEN || strlen(value)>GUIMSGBIT_LEN ) return bits;
    
    char *bit = (char*)malloc(GUIMSGBIT_LEN*2);
    for (i = 0; i < GUIMSGBIT_LEN*2; i++) bit[i] = 0x00;
    
    strcpy(bit,name);
    strcpy(bit+GUIMSGBIT_LEN,value);
    
    bits = trop_ListAdd(bits,(void*)bit);
    return bits;
  }

  /*
    set message bits from a list of (name,value) structures 
  */
  int gui_MsgSetXStringBitStrEx(
    struct gui_Msg *msg,
    trop_List *bits // list->ptr(char[name[256]][value[256]])
  )
  {
    static struct trop_List *current;
    static int msgsize;
    
    if (!msg||!bits) return -1;
    
    msgsize = 2; // '<>'
    current = bits;
    while(current!=NULL)
    {
      if (current->ptr)
      {
        msgsize += strlen(current->ptr);
        msgsize += strlen(current->ptr+GUIMSGBIT_LEN);
        msgsize += 3; // '=' ' ' '\0'
      }
      current = current->next;
    }
    
    msg->str = (char*)malloc(msgsize);
    msg->str[0] = 0x00;
    strcat(msg->str,"<");
    current = bits;
    while(current!=NULL)
    {
      if (current->ptr)
      {
        strcat(msg->str,current->ptr);
        strcat(msg->str,"=");
        strcat(msg->str,current->ptr+GUIMSGBIT_LEN);
        if (current->next!=NULL)
          strcat(msg->str," ");
      }
      current = current->next;
    }
    strcat(msg->str,">");
    
    return 0;
  }

  int _gui_msg_init( struct gui_Msg *msg )
  {
    if (!msg) return -1;
    msg->str 	= NULL;
    msg->descr 	= NULL;
    msg->type	= GMSG_NONE; 
    return 0;
  }

  struct gui_Msg *gui_MsgNetworkInit(
    struct gui_Msg *msg
  )
  {
    static struct gui_MsgNetwork *mnet = NULL;
    
    if (msg==NULL)
    {
      msg = (struct gui_Msg*)malloc(sizeof(struct gui_Msg));
      _gui_msg_init(msg);
    }
    
    if (msg->descr==NULL)
    {
      msg->descr = (struct gui_MsgNetwork*)malloc(sizeof(struct gui_MsgNetwork));
    }
    if (msg->type==GMSG_NONE)
    {
      msg->type = GMSG_NETWORK;
    }
    
    mnet = msg->descr;
    mnet->conn = NULL;
    
    return msg;
  }

  struct gui_Msg *gui_MsgInit(
    struct gui_Msg *msg,
    int type
  )
  {
    static struct gui_MsgNetwork *msgn = NULL;
  
    if (msg==NULL)
    {
      msg = (struct gui_Msg*)malloc(sizeof(struct gui_Msg));
      _gui_msg_init(msg);
    }
    msg->type = type;
    
    switch(type)
    {
      case GMSG_NETWORK:
        msg->descr = (struct gui_MsgNetwork*)malloc(sizeof(struct gui_MsgNetwork));
        msgn = msg->descr;
      
      break;
    }
    
    return msg;
  }

  int gui_MsgNetworkDoMsg( struct gui_Msg *msg )
  {
    static struct gui_MsgNetwork *msgn = NULL;
    
    if (!msg) return -1;
    
    msgn = msg->descr;
    if (msgn==NULL) return -1;
    
    if (DEBUG==TRUE) printf("gui_MsgNetworkDoMsg\n");
    
    trop_ConnInit(msgn->conn); // connect
    
    if (msg->str!=NULL)
    {
      strcpy((char*)msgn->conn->out,msg->str);
      msgn->conn->fd_out = strlen(msg->str);
      trop_ConnSend(msgn->conn);
    }
    
    trop_ConnDeinit(msgn->conn); // disconnect
    
    return 0;
  }

  int gui_MsgDoMsg( struct gui_Msg *msg )
  {
    if (!msg) return -1;

    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("gui_MsgDoMsg\n");
    #endif
    
    switch(msg->type)
    {
      case GMSG_NETWORK:
        gui_MsgNetworkDoMsg(msg);
      break;
    }
    
    return 0;
  }

  int gui_WidgetAddMsg( struct gui_Widget *wi, struct gui_Msg *msg )
  {
    static struct gui_object *current;
    if (!wi||!msg) return -1;
    
    current = (struct gui_object*)malloc(sizeof(struct gui_object));
    current->next = wi->obj;
    wi->obj = current;
    
    current->id = GOID_MSG;
    current->data = msg;
    
    return 0;
  }

#endif
