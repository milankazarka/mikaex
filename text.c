/**
 *  Milan Kazarka 2010, all rights reserved
 *  Ul.M.R.Stefanika 44/29, Ziar nad Hronom, Slovakia
 *
 */

#include "system.h"
#include "gui.h"
#include "xml.h"

#ifdef USEX
  
  #define _TEXT_MARGIN	4
  
  int form_TextRelease( struct gui_Widget *w )
  {
    return 0;
  }
  
  int form_TextInit( struct gui_Widget *w )
  {
    static struct form_Text 	*text;
    if (!w) return -1;
    w->form = (struct form_Text*)malloc(sizeof(struct form_Text));
    text = (struct form_Text*)w->form;
    text->texts = NULL;
    text->margin = _TEXT_MARGIN;
    return 0;
  }
  
  int form_TextAddText( struct gui_Widget *w, char *str )
  {
    static struct form_Text 	*text;
    static int			width;
    static int			height;
    static int			strw = 0, strh = 0;
    static int			lineh = 0;
    static int			sepaw = 0;
    static int			sepah = 0;
    static int			linesh = 0; // absolute h/w of lines
    static int			linesw = 0;
    static int			i, ii;
    static char			chunk[32768];
    static struct gui_TextSim	*ts = NULL;
    
    if (!w||!str) return -1;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":form_TextAddText (%s)\n",str);
    #endif
    
    text = (struct form_Text*)w->form;
    width = w->attr.w;
    height = w->attr.h; // will be adjusted to be able to display text
    #ifdef USEFT
      lineh = gui_GetMain()->attr.ftsize*1.3;
    #else
      lineh = 20; // mika - todo
    #endif
    
    ii 		= 0;
    linesw	= text->margin;
    linesh	= lineh+text->margin;
    
    gui_TextGetSize( "/",&sepaw,&sepah );
    
    for (i = 0; i < strlen(str); i++)
    {
      if (str[i] != ' ')
      {
        chunk[ii] = str[i];
        ii++;
        if (str[i+1]==0x00)
        {
          chunk[ii] = 0x00;
          if (strlen((char*)chunk) > 0)
          {
            strw = 0; 
            strh = 0;
            #ifdef _DEBUG
              if (DEBUG==TRUE) printf("before gui_TextGetSize _ft DEBUG(%d) TRUE(%d)\n",DEBUG,TRUE);
            #endif
            gui_TextGetSize( (char*)chunk,&strw,&strh );
            #ifdef _DEBUG
              if (DEBUG==TRUE)
                printf("__text.chunk(%s) size(%d/%d)\n",(char*)chunk,strw,strh);
            #endif
          
            ts = (struct gui_TextSim*)malloc(sizeof(struct gui_TextSim));
            ts->str = NULL;
            gui_WidgetAddTextSimpleD(w,ts,(char*)chunk,PLACE_LEFT);
        
            if (linesw+strw+text->margin > width)
            {
              linesw = strw+sepaw+text->margin;
              linesh+=lineh;
            }
            else
            {
              linesw += strw+sepaw;
            }
            ts->pos.w = strw;
            ts->pos.h = strh;
            ts->pos.x = linesw-strw-sepaw;
            ts->pos.y = linesh-lineh;
          }
        }
      } 
      else if ( str[i] == ' ' )
      {
        chunk[ii] = 0x00;
        if (strlen((char*)chunk) > 0)
        {
          strw = 0; 
          strh = 0;
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("before gui_TextGetSize _ft\n");
          #endif
          gui_TextGetSize( (char*)chunk,&strw,&strh );
          if (DEBUG==TRUE)
            printf("__text.chunk(%s) size(%d/%d)\n",(char*)chunk,strw,strh);
          
          ts = (struct gui_TextSim*)malloc(sizeof(struct gui_TextSim));
          ts->str = NULL;
          gui_WidgetAddTextSimpleD(w,ts,(char*)chunk,PLACE_LEFT);
        
          if (linesw+strw+text->margin > width)
          {
            linesw = strw+sepaw+text->margin;
            linesh+=lineh;
          }
          else
          {
            linesw += strw+sepaw;
          }
          ts->pos.w = strw;
          ts->pos.h = lineh;
          ts->pos.x = linesw-strw-sepaw;
          ts->pos.y = linesh-lineh;
        }
        ii = 0;
      }
    }
    if (linesh>height) w->attr.h = linesh;
    
    return 0;
  }
  
  #define _HTML_BITSIZE 4096
  
  typedef struct _form_text_html_img
  {
    char src[_HTML_BITSIZE];
  } _form_text_html_img;
  
  typedef struct _form_text_html_feed
  {
    int 			id;
    BOOL 			active;
    struct _form_text_html_img	img;
  } _form_text_html_feed;
  
  struct _form_text_html_feed feeds[128];
  
  int __download( char *url, char *dir, char *local )
  {
    char *command;
    
    if (!url||!dir) return -1;
    if (strlen(url)==0) return -1;
    
    command = (char*)malloc(strlen(url)+strlen(dir)+64);
    sprintf(command,"wget -O%s/%s \"%s\"",dir,local,url);
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("__download command(%s)\n",command);
    #endif
    system(command);
    free(command);
    
    return 0;
  }
  
  /*
    RSS feed content
  */
  int _form_text_html_feed_init( struct _form_text_html_feed *feed )
  {
    if (!feed) return -1;
    
    feed->id = -1;
    feed->active = -1;
    
    return 0;
  }
  
  int _form_text_html_img_src_process( struct _form_text_html_img *fimg, char *str )
  {
    static int n;
    static int _n;
    static int len;
    static int from;
    static int to;
    
    if (!str) return -1;
    if (!fimg) return -1;
    if (strlen(str)==0) return -1;
    
    fimg->src[0] = 0x00;
    
    len = strlen(str);
    from = -1;
    to = -1;
    
    for (n = 0; n < len; n++)
    {
      if (n+4<=len)
      {
        if (strncmp(str+n,"src=",4)==0)
        {
          for (_n=n+4; _n < len; _n++)
          {
            if (str[_n]=='"')
            {
              if (from==-1)
              {
                from = _n;
              }
              else if (to==-1)
              {
                to = _n;
              }
              else
              {
                goto FORM_THTML_IMG_SRC_FINISH;
              }
            }
          }
        }
      }
    }
  FORM_THTML_IMG_SRC_FINISH:
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("_form_text_html_img_src_process from(%d) to(%d)\n",
        from,to);
    #endif
    if (from!=-1&&to!=-1)
    {
      from++;
      if (to-from>=_HTML_BITSIZE)
      {
        to = from+_HTML_BITSIZE-1;
      }
      strncpy((char*)fimg->src,str+from,to-from);
      fimg->src[to-from] = 0x00;
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("_form_text_html_img_src_process src=(%s)\n",
          (char*)fimg->src);
      #endif
    }
    
    return 0;
  }
  
  int _form_text_html_img_process( struct _form_text_html_img *fimg, char *str )
  {
    int n;
    
    if (!fimg||!str) return -1;
    
    fimg->src[0] = 0x00;
    
    _form_text_html_img_src_process(fimg,str);
    __download((char*)fimg->src,"./","local.jpg");
    
    return 0;
  }
  
  int form_TextAddHtmlText( struct gui_Widget *w, char *str )
  {
    static int n;
    static int _n;
    static int __n;
    static int nn;
    static int len;
    static int mode;
    static char *_str;
    static char tag[_HTML_BITSIZE];
    
    if (!w||!str) return -1;
    if (strlen(str)==0) return -1;
    
    _str = (char*)malloc(strlen(str)+1);
    
    nn = 0;
    mode = 0;
    len = strlen(str);
    for(n = 0; n < len; n++)
    {
      switch(mode)
      {
        case 0:
          if (
            str[n]=='<'
          )
          {
            if (n+4<=len)
            {
              if (
                strncmp(str+n+1,"img",3)==0 ||
                strncmp(str+n+1,"IMG",3)==0
              )
              {
                #ifdef _DEBUG
                  if (DEBUG==TRUE) printf("form_TextAddHtmlText tag:img\n");
                #endif
                for(_n = n; _n < len; _n++) // get IMG tag content
                {
                  if (_n+4<=len)
                  {
                    if (
                      strncmp(str+_n,"src=",4)==0
                    )
                    {
                      #ifdef _DEBUG
                        if (DEBUG==TRUE) printf("form_TextAddHtmlText tag:img:src\n");
                      #endif
                      struct _form_text_html_img fimg;
                      _form_text_html_img_process( &fimg, str+_n );
                    }
                  }
                  if (str[_n]=='>')
                  {
                    strncpy((char*)tag,str+n,(_n-n)+1);
                    tag[(_n-n)+1] = 0x00;
                    #ifdef _DEBUG
                      if (DEBUG==TRUE) printf("form_TextAddHtmlText tag:img (%s)\n",
                        (char*)tag);
                    #endif
                    break;
                  }
                }
              }
            }
            mode = 1;
            break;
          }
          _str[nn] = str[n];
          nn++;
        break;
        case 1:
          if (
            str[n]=='>'
          )
          {
            mode = 0;
            break;
          }
        break;
      }
    }
    _str[nn] = 0x00;
    strcpy(str,_str);
    free(_str);
    
    form_TextAddText(w,str);
    
    return 0;
  }
  
#endif
