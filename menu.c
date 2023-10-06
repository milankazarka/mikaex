/**
 *  Milan Kazarka 2010, all rights reserved
 *  Ul.M.R.Stefanika 44/29, Ziar nad Hronom, Slovakia
 *
 */
extern "C" {
  #include "os.h"
  #include "system.h"
  #include "global.h"
  #include "xml.h"
  #include "wm.h"
  #include "dom.h"
  #include "mikaRss.h"
  #include "mikaPosClient.h"
  #include "mikaSerial.h"
  #include "config.h"
}

#include "layout.h"
#include "setup.h"
  
  dom *dom_InitNamed(unsigned char *filename);
 
  struct mikaPosClient client;
  struct mikaPosOrder order;
  
  struct tr_Window *root = NULL;	// root Window
  struct tr_Window *mmain = NULL;	// main menu
  struct tr_Window *msoft = NULL;	// soft drinks menu
  struct tr_Window *mwiskey = NULL;	// wiskey menu
  struct tr_Window *mspirits = NULL;	// spirits menu
  struct tr_Window *mliguere = NULL;	// liguere menu
  struct tr_Window *mbeer = NULL;	// beer menu

  // product manipulation:  
  struct tr_Window *morder = NULL;

  // product's:
  struct tr_Window *mcocacola = NULL;
  
  struct tr_Object *cmain = NULL;	// main menu canvas
  struct tr_Object *imain = NULL;	// main menu image
  
  struct tr_Object *csoft = NULL;	// soft drinks canvas
  struct tr_Object *cwiskey = NULL;	// wiskey canvas
  struct tr_Object *cspirits = NULL;	// spirits canvas
  struct tr_Object *cliguere = NULL;	// liguere canvas
  struct tr_Object *cbeer = NULL;	// beer canvas
  
  struct tr_Object *corder = NULL;
  //
  struct tr_Object *ccocacola = NULL;
  
  void *onbutton(void*data)
  {
    if (DEBUG==TRUE) printf("\t:click\n");
  }

  void *onbutton_home(void*data)
  {
    tr_WindowToTop(mmain);
    tr_WindowExpose(mmain);
  }
  
  void *onbutton_wiskey(void*data)
  {
    tr_WindowToTop(mwiskey);
    tr_WindowExpose(mwiskey);
  }
  
  void *onbutton_soft(void*data)	// to softdrinks canvas
  {
    tr_WindowToTop(msoft);
    tr_WindowExpose(msoft);
  }
  
  void *onbutton_spirits(void*data)
  {
    tr_WindowToTop(mspirits);
    tr_WindowExpose(mspirits);
  }
  
  void *onbutton_liguere(void*data)
  {
    tr_WindowToTop(mliguere);
    tr_WindowExpose(mliguere);
  }
  
  void *onbutton_beer(void*data)
  {
    tr_WindowToTop(mbeer);
    tr_WindowExpose(mbeer);
  }
  
  void *onbutton_order(void*data)
  {
    tr_WindowToTop(morder);
    tr_WindowExpose(morder);
  }
  
  void *onbutton_cocacola(void*data)
  {
    tr_WindowToTop(mcocacola);
    tr_WindowExpose(mcocacola);
  }

  /** order speciffic 
  */

  void *order_mattoni(void*data)
  {
    strcpy((char*)order.value,"mattoni #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_radenska(void*data)
  {
    strcpy((char*)order.value,"radenska #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_bonaqua(void*data)
  {
    strcpy((char*)order.value,"bonaqua #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_kinley(void*data)
  {
    strcpy((char*)order.value,"kinley #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_cocacola(void*data)
  {
    strcpy((char*)order.value,"cocacola #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_sprite(void*data)
  {
    strcpy((char*)order.value,"sprite #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_tonic(void*data)
  {
    strcpy((char*)order.value,"tonic #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_fanta(void*data)
  {
    strcpy((char*)order.value,"fanta #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_johniewalker(void*data)
  {
    strcpy((char*)order.value,"johniewalker #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_jameson(void*data)
  {
    strcpy((char*)order.value,"jameson #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_jimbeam(void*data)
  {
    strcpy((char*)order.value,"jimbeam #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_jameson12(void*data)
  {
    strcpy((char*)order.value,"jameson12 #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_chivasregal(void*data)
  {
    strcpy((char*)order.value,"chivasregal #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_tequilla(void*data)
  {
    strcpy((char*)order.value,"tequilla #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_gin(void*data)
  {
    strcpy((char*)order.value,"gin #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_vodka(void*data)
  {
    strcpy((char*)order.value,"vodka #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_slivovica(void*data)
  {
    strcpy((char*)order.value,"slivovica #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_hruskovica(void*data)
  {
    strcpy((char*)order.value,"hruskovica #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_havana(void*data)
  {
    strcpy((char*)order.value,"havana #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_malibu(void*data)
  {
    strcpy((char*)order.value,"malibu #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_fernet(void*data)
  {
    strcpy((char*)order.value,"fernet #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_becherovka(void*data)
  {
    strcpy((char*)order.value,"becherovka #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_demanovka(void*data)
  {
    strcpy((char*)order.value,"demanovka #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_vajecnyliker(void*data)
  {
    strcpy((char*)order.value,"vajecny liker #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_zlatybazant(void*data)
  {
    strcpy((char*)order.value,"zlatybazant #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  void *order_heineken(void*data)
  {
    strcpy((char*)order.value,"heineken #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    onbutton_order(NULL);
  }

  #define _TEST_LAYOUT_NO
  #define _PANEL_NO
  
  int main( )
  {
    int i;
    
    struct tr_Attr	attr;			// main attributes
    struct tr_Attr	attr_button;		// button attributes
    struct tr_Attr	attr_lbentry;		// listbox entry attributes
    struct tr_Attr	attr_home;		// home button attributes
    struct tr_Attr	attr_bar_right;		
    struct tr_Attr	attr_info;		// info multiline text
    struct tr_Attr	attr_orderyes;		// order yes
    struct tr_Attr	attr_orderno;		// order no
    struct tr_Attr	attr_panel;		// panel attributes
    struct tr_Attr	attr_panel_item;	// panel item attributes
    //
    struct tr_Attr	attr_order;		// in order window 'order' <- used
    struct tr_Attr	attr_cancel;		// in order window 'cancel'
    struct tr_Attr	attr_order_button;	// graphics in these buttons
    
    struct tr_Object	*image = NULL;		// general image
      
    char		*btext = NULL;		// button text
    struct tr_Object	*bimage = NULL;		// button image
    struct tr_Object	*ihome = NULL;		// home image
    struct tr_Object	*iorder = NULL;
    struct tr_Object	*icancel = NULL;
    struct tr_Object	*button = NULL;		// button
    struct tr_Object	*home = NULL;		// home button
    struct tr_Object	*bar_right = NULL;	// right bar object
    struct tr_Object	*lbentry = NULL;	// listbox entry

    struct tr_Object	*border = NULL;
    struct tr_Object	*bcancel = NULL;

    struct tr_Object	*panel = NULL;		// bottom panel
    struct tr_Object	*panel_button1 = NULL;	// panel's button1
    struct tr_Object	*panel_button2 = NULL;
    struct tr_Object	*panel_button3 = NULL;

    #ifdef _TEST_LAYOUT
      trop_Init();
      gui_Init();
      tr_WmInit();
      rotation = ROT_NONE;
      layout = NULL;
      
      struct tr_Layout *custom = NULL;
      custom = tr_LayoutInit(custom,"main");
    #else

    //_test_jpeg_handling("testBg.jpg");
  
    osInit();
    trop_Init();
    printf("trop_init.end\n");
    gui_Init();
    printf("gui_init.end\n");
    tr_WmInit();
    printf("tr_wminit.end\n");
    layout = NULL;
    rotation = ROT_NONE;

    mikaPosClientInit(&client);

    //imain = tr_ImageInitEx(NULL,&attr,"testBg2.jpg");


/*    struct trop_Conn conn;
    trop_ConnInitPrepare(&conn);
    strcpy((char*)conn.remote,"www.root.cz");
    conn.port = 80;
    trop_ConnInit(&conn);
    trop_ConnSendHttpRequest(&conn,"/");
    printf("sent HTTP request:\n%s\n",(char*)conn.out);
    trop_ConnGetEx(&conn);
    printf("HTTP:(%s)\n",(char*)conn.in);
    
    struct tr_Layout *custom = NULL;
    custom = tr_LayoutInit(custom,"main");
*/    


    attr.x = 0;
    attr.y = 0;
    attr.w = 360; // 480;
    attr.h = 360; // 320;
    
    attr_button.x = 0;
    attr_button.y = 0;
    attr_button.w = 130;
    attr_button.h = 130;
    
    attr_order_button.x = 0;
    attr_order_button.y = 0;
    attr_order_button.w = 90;
    attr_order_button.h = 30;
    
    attr_info.x = 0;
    attr_info.y = 0;
    attr_info.w = 400;
    attr_info.h = 250;
    
    attr_lbentry.x = 0;
    attr_lbentry.y = 0;
    attr_lbentry.w = 0;
    attr_lbentry.h = 100;
    
    //attr_home.w = 60;
    //attr_home.h = 60;
    //attr_home.x = attr.w-70;
    //attr_home.y = attr.h-70;

    attr_orderyes.w = 150;
    attr_orderyes.h = 60;
    attr_orderyes.x = 0;
    attr_orderyes.y = 0;
    
    attr_orderno.w = 150;
    attr_orderno.h = 60;
    attr_orderno.x = 0;
    attr_orderno.y = 0;
    
    // testing the Xml DOM handling functionality
    //
    //dom *xd = dom_InitNamed((unsigned char*)"testxml.xml");
    //
    //
    
    printf(":before init Windows\n");
    root = tr_WindowInit(NULL,NULL,&attr);
    printf(":after init Root Window\n");
    
    //attr.w = 240;
    mmain = tr_WindowInit(NULL,root,&attr);
    tr_SetColor((tr_Object*)mmain,(char*)"#dfdfdf");
    //attr.w = 480;
    
    printf("_creating_scenes (%d/%d)\n",attr.x,attr.y);
    
    msoft = tr_WindowInit(NULL,root,&attr);
    mwiskey = tr_WindowInit(NULL,root,&attr);
    mspirits = tr_WindowInit(NULL,root,&attr);
    mliguere = tr_WindowInit(NULL,root,&attr);
    mbeer = tr_WindowInit(NULL,root,&attr);
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":after init Windows\n");
    #endif
    
    morder = tr_WindowInit(NULL,root,&attr);
    mcocacola = tr_WindowInit(NULL,root,&attr);
    
    tr_WindowMap(mmain);  
    tr_WindowToTop(mmain);
    
    ihome = tr_ImageInitEx(NULL,&attr_button,(char*)"img/home_small_new.jpg");
    tr_ImageSetRound(ihome,2);
    
    iorder = tr_ImageInitEx(NULL,&attr_order_button,(char*)"img/buttonOrder.jpg");
    tr_ImageSetRound(iorder,2);
    icancel = tr_ImageInitEx(NULL,&attr_order_button,(char*)"img/buttonCancel.jpg");
    tr_ImageSetRound(icancel,2);
    
    // main canvas:
    //imain = tr_ImageInitEx(NULL,&attr,"bgTest00.jpg"); // testBg2.jpg
    
    //attr.w = 240;
    //attr.w = 440;
    //attr.h = 380;
    cmain = tr_CanvasInit(NULL,&attr,mmain);
    //attr.w = 480;
    
    attr.x = 0;
    attr.y = 0;
    attr.w = 360;
    #ifdef _PANEL
      attr.h = 380;
    #else
      attr.h = 360;
    #endif
    
    attr_home.w = 60;
    attr_home.h = 60;
    attr_home.x = attr.w-70;
    attr_home.y = attr.h-70;
    
    // in case that we don't want navigation objects on main view
    if (0)
      form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);
    
    //tr_AddImage(cmain,imain);
    tr_SetRound(cmain,3);
    tr_SetColor(cmain,(char*)"#b9b9b9");
    form_CanvasSetMov(cmain->widget.sub,CAN_MOV_SCROLL);
    
    tr_WindowToTop(msoft);
    // soft drinks canvas:
    csoft = tr_CanvasInit(NULL,&attr,msoft);
    tr_SetRound(csoft,2);
    form_CanvasSetMov(csoft->widget.sub,CAN_MOV_SCROLL);
    tr_SetColor(csoft,(char*)"#303030");
    //tr_AddImage(csoft,imain);
    
    tr_WindowToTop(mwiskey);
    // wiskey canvas:
    cwiskey = tr_CanvasInit(NULL,&attr,mwiskey);
    tr_SetRound(cwiskey,2);
    form_CanvasSetMov(cwiskey->widget.sub,CAN_MOV_SCROLL);
    tr_SetColor(cwiskey,(char*)"#303030");
    //tr_AddImage(cwiskey,imain);
    //tr_SetRound(cwiskey,5);
    
    tr_WindowToTop(mspirits);
    // wiskey canvas:
    cspirits = tr_CanvasInit(NULL,&attr,mspirits);
    tr_SetRound(cspirits,2);
    form_CanvasSetMov(cspirits->widget.sub,CAN_MOV_SCROLL);
    tr_SetColor(cspirits,(char*)"#303030");
    //tr_AddImage(cspirits,imain);
    
    tr_WindowToTop(mliguere);
    // liguere canvas:
    cliguere = tr_CanvasInit(NULL,&attr,mliguere);
    tr_SetRound(cliguere,2);
    form_CanvasSetMov(cliguere->widget.sub,CAN_MOV_SCROLL);
    tr_SetColor(cliguere,(char*)"#303030");
    //tr_AddImage(cliguere,imain);
    
    tr_WindowToTop(mbeer);
    // beer canvas:
    cbeer = tr_CanvasInit(NULL,&attr,mbeer);
    tr_SetRound(cbeer,2);
    form_CanvasSetMov(cbeer->widget.sub,CAN_MOV_SCROLL);
    tr_SetColor(cbeer,(char*)"#303030");
    //tr_AddImage(cbeer,imain);
    
    tr_WindowToTop(morder);
    // cocacola canvas:
    corder = tr_CanvasInit(NULL,&attr,morder);
    tr_SetRound(corder,2);
    form_CanvasSetMov(corder->widget.sub,CAN_MOV_NONE);
    tr_SetColor(corder,(char*)"#505050");
    
    tr_WindowToTop(mcocacola);
    // cocacola canvas:
    ccocacola = tr_CanvasInit(NULL,&attr,mcocacola);
    tr_SetRound(ccocacola,2);
    form_CanvasSetMov(ccocacola->widget.sub,CAN_MOV_NONE);
    tr_SetColor(ccocacola,(char*)"#dddddd");
    
    tr_WindowToTop(morder);
    // cocacola canvas:
    corder = tr_CanvasInit(NULL,&attr,morder);
    tr_SetRound(corder,2);
    form_CanvasSetMov(corder->widget.sub,CAN_MOV_NONE);
    tr_SetColor(corder,(char*)"#efefef");
    
    // main menu:
    tr_WindowToTop(mmain);
    home = tr_ButtonInitEx2(NULL,&attr_home,cmain);
    tr_SetRound(home,2);
    tr_SetColor(home,(char*)"#707070");
    tr_SetAction(home,onbutton_home,TR_ACTION_BUTTON);
    btext = (char*)malloc(32);
    strcpy(btext,(char*)"Main");
    //tr_SetTextEx(home,btext,PLACE_CENTER);
    tr_AddImage(home,ihome);
    //tr_CanvasAddObjStatic(cmain,home);
    bimage = tr_ImageInitEx(NULL,&attr_button,(char*)"icon2.xpm");
    
    /*
    tr_WindowToTop(morder);
    home = tr_ButtonInitEx2(NULL,&attr_home,corder);
    tr_SetRound(home,2);
    tr_SetColor(home,(char*)"#ffffff");
    tr_SetAction(home,onbutton_home,TR_ACTION_BUTTON);
    btext = (char*)malloc(32);
    strcpy(btext,(char*)"Main");
    tr_SetTextEx(home,btext,PLACE_CENTER);
    tr_AddImage(home,ihome);
    tr_PutObject(csoft,home);
    tr_WindowToTop(mmain);
    */
    
    attr_panel.x = 0;
    attr_panel.y = 380;
    attr_panel.w = 440;
    attr_panel.h = 60;
    panel = tr_ButtonInitEx2(NULL,&attr_panel,cmain);
    tr_SetColor(panel,"#070707");
    tr_SetRound(panel,2);
    //tr_CanvasAddObjStatic(cmain,panel);
    
    attr_panel_item.x = 2;
    attr_panel_item.y = attr_panel.y+2;
    attr_panel_item.w = 56;
    attr_panel_item.h = 56;
    
    panel_button1 = tr_ButtonInitEx2(NULL,&attr_panel_item,cmain);
    attr_panel_item.x+=58;
    panel_button2 = tr_ButtonInitEx2(NULL,&attr_panel_item,cmain);
    attr_panel_item.x+=58;
    panel_button3 = tr_ButtonInitEx2(NULL,&attr_panel_item,cmain);
    
    tr_SetColor(panel_button1,"#141414");
    tr_SetColor(panel_button2,"#141414");
    tr_SetColor(panel_button3,"#141414");
    
    tr_SetRound(panel_button1,1);
    tr_SetRound(panel_button2,1);
    tr_SetRound(panel_button3,1);
    
    for (i = 0; i < 5; i++)
    {
      button = tr_ButtonInitEx2(NULL,&attr_button,cmain);
      tr_SetFontColor(button,0x30,0x30,0x30,220);
      tr_SetRound(button,4);
      //tr_SetColor(button,"#trans");
      tr_SetColor(button,(char*)"#ffffff");
      //gui_WidgetSetGradient(button->widget.sub,-40);
      tr_CanvasAddObj(cmain,button);
      if (i==0) // soft drinks button
      {
        btext = (char*)malloc(32);
        strcpy(btext,(char*)"Soft Drinks");
        tr_SetTextEx(button,btext,PLACE_CENTER);
        tr_SetAction(button,onbutton_soft,TR_ACTION_BUTTON);
        bimage = tr_ImageInitEx(NULL,&attr_button,(char*)"img/soft.jpg");
        tr_ImageSetRound(bimage,2);
        tr_AddImage(button,bimage);
        tr_ImagePosCenter(bimage,button);
      }
      else if (i==1)
      {
        btext = (char*)malloc(32);
        strcpy(btext,(char*)"Whiskey");
        tr_SetTextEx(button,btext,PLACE_CENTER);
        tr_SetAction(button,onbutton_wiskey,TR_ACTION_BUTTON);
        bimage = tr_ImageInitEx(NULL,&attr_button,(char*)"img/whisky.jpg");
        tr_ImageSetRound(bimage,2);
        tr_AddImage(button,bimage);
        tr_ImagePosCenter(bimage,button);
      }
      else if (i==2)
      {
        btext = (char*)malloc(32);
        strcpy(btext,(char*)"Spirits");
        tr_SetTextEx(button,btext,PLACE_CENTER);
        tr_SetAction(button,onbutton_spirits,TR_ACTION_BUTTON);
        bimage = tr_ImageInitEx(NULL,&attr_button,(char*)"img/spirits.jpg");
        tr_ImageSetRound(bimage,2);
        tr_AddImage(button,bimage);
        tr_ImagePosCenter(bimage,button);
      }
      else if (i==3)
      {
        btext = (char*)malloc(32);
        strcpy(btext,(char*)"Liguere");
        tr_SetTextEx(button,btext,PLACE_CENTER);
        tr_SetAction(button,onbutton_liguere,TR_ACTION_BUTTON);
        bimage = tr_ImageInitEx(NULL,&attr_button,(char*)"img/liguere.jpg");
        tr_ImageSetRound(bimage,2);
        tr_AddImage(button,bimage);
        tr_ImagePosCenter(bimage,button);
      }
      else if (i==4)
      {
        btext = (char*)malloc(32);
        strcpy(btext,(char*)"Beer");
        tr_SetTextEx(button,btext,PLACE_CENTER);
        tr_SetAction(button,onbutton_beer,TR_ACTION_BUTTON);
        bimage = tr_ImageInitEx(NULL,&attr_button,(char*)"img/beer.jpg");
        tr_ImageSetRound(bimage,2);
        tr_AddImage(button,bimage);
        tr_ImagePosCenter(bimage,button);
      }
    }
    
    #ifdef _PANEL
      tr_CanvasAddObjStatic(cmain,panel);
      tr_CanvasAddObjStatic(cmain,panel_button1);
      tr_CanvasAddObjStatic(cmain,panel_button2);
      tr_CanvasAddObjStatic(cmain,panel_button3);
    #endif
    
    // soft drinks canvas
    tr_WindowToTop(msoft);
    home = tr_ButtonInitEx2(NULL,&attr_home,csoft);
    tr_SetRound(home,2);
    tr_SetColor(home,(char*)"#ffffff");
    tr_SetAction(home,onbutton_home,TR_ACTION_BUTTON);
    btext = (char*)malloc(32);
    strcpy(btext,(char*)"Main");
    //tr_SetTextEx(home,btext,PLACE_CENTER);
    tr_AddImage(home,ihome);
    tr_CanvasAddObjStatic(csoft,home);

    //tr_PutObject(csoft,home);

    //while(1)
    {

    for (i = 0; i < 8; i++)
    {
      if (i==0)
      {
        lbentry = tr_LBEntryInit(NULL,&attr_lbentry,msoft);
        tr_LBEntryConfigure(lbentry,(char*)"Mattoni fl.",(char*)"0.33l");
        bimage = tr_ImageInitEx(NULL,&attr_button,(char*)"img/mattoni.jpg");
        tr_ImageSetRound(bimage,2);
        tr_LBEntrySetImage(lbentry,bimage);
        if (lbentry)
          tr_CanvasAddObj(csoft,lbentry);
        tr_SetAction(lbentry,order_mattoni,TR_ACTION_BUTTON);
      }
      else if (i==1)
      {
        lbentry = tr_LBEntryInit(NULL,&attr_lbentry,msoft);
        tr_LBEntryConfigure(lbentry,(char*)"Radenska fl.",(char*)"0.25l");
        bimage = tr_ImageInitEx(NULL,&attr_button,(char*)"img/radenska.jpg");
        tr_ImageSetRound(bimage,2);
        tr_LBEntrySetImage(lbentry,bimage);
        if (lbentry)
          tr_CanvasAddObj(csoft,lbentry);
        tr_SetAction(lbentry,order_radenska,TR_ACTION_BUTTON);
      }
      else if (i==2)
      {
        lbentry = tr_LBEntryInit(NULL,&attr_lbentry,msoft);
        tr_LBEntryConfigure(lbentry,(char*)"Bonaqua fl.",(char*)"0.25l");
        bimage = tr_ImageInitEx(NULL,&attr_button,(char*)"img/bonaqua.jpg");
        tr_ImageSetRound(bimage,2);
        tr_LBEntrySetImage(lbentry,bimage);
        if (lbentry)
          tr_CanvasAddObj(csoft,lbentry);
        tr_SetAction(lbentry,order_bonaqua,TR_ACTION_BUTTON);
      }
      else if (i==3)
      {
        lbentry = tr_LBEntryInit(NULL,&attr_lbentry,msoft);
        tr_LBEntryConfigure(lbentry,(char*)"CocaCola fl.",(char*)"0.33l");
        bimage = tr_ImageInitEx(NULL,&attr_button,(char*)"img/cocacola.jpg");
        tr_ImageSetRound(bimage,2);
        tr_LBEntrySetImage(lbentry,bimage);
        if (lbentry)
          tr_CanvasAddObj(csoft,lbentry);
        //tr_SetAction(lbentry,onbutton_cocacola,TR_ACTION_BUTTON);
        //tr_SetColor(lbentry,(char*)"#e0e0e0");
        //gui_WidgetSetGradient(lbentry->widget.sub,-180);
        tr_SetAction(lbentry,order_cocacola,TR_ACTION_BUTTON);
      }
      else if (i==4)
      {
        lbentry = tr_LBEntryInit(NULL,&attr_lbentry,msoft);
        tr_LBEntryConfigure(lbentry,(char*)"Sprite fl.",(char*)"0.33l");
        bimage = tr_ImageInitEx(NULL,&attr_button,(char*)"img/sprite.jpg");
        tr_ImageSetRound(bimage,2);
        tr_LBEntrySetImage(lbentry,bimage);
        if (lbentry)
          tr_CanvasAddObj(csoft,lbentry);
        tr_SetAction(lbentry,order_sprite,TR_ACTION_BUTTON);
      }
      else if (i==5)
      {
        lbentry = tr_LBEntryInit(NULL,&attr_lbentry,msoft);
        tr_LBEntryConfigure(lbentry,(char*)"Tonic fl.",(char*)"0.25l");
        bimage = tr_ImageInitEx(NULL,&attr_button,(char*)"img/kinley.jpg");
        tr_ImageSetRound(bimage,2);
        tr_LBEntrySetImage(lbentry,bimage);
        if (lbentry)
          tr_CanvasAddObj(csoft,lbentry);
        tr_SetAction(lbentry,order_kinley,TR_ACTION_BUTTON);
      }
      else if (i==6)
      {
        lbentry = tr_LBEntryInit(NULL,&attr_lbentry,msoft);
        tr_LBEntryConfigure(lbentry,(char*)"Fanta fl.",(char*)"0.33l");
        bimage = tr_ImageInitEx(NULL,&attr_button,(char*)"img/fanta.jpg");
        tr_ImageSetRound(bimage,2);
        tr_LBEntrySetImage(lbentry,bimage);
        if (lbentry)
          tr_CanvasAddObj(csoft,lbentry);
        tr_SetAction(lbentry,order_fanta,TR_ACTION_BUTTON);
      }
      //tr_SetAction(lbentry,onbutton_order,TR_ACTION_BUTTON);
      tr_SetFontColor(lbentry,0x30,0x30,0x30,220);
    }
    usleep(100000);
    }
    //form_CanvasRemoveAllObjRefType(csoft->widget.sub,FORM_LBENTRY); // test - ok
    //form_CanvasRemoveAllObjRef(csoft->widget.sub);
    
    // soft drinks canvas
    tr_WindowToTop(mwiskey);
    home = tr_ButtonInitEx2(NULL,&attr_home,cwiskey);
    tr_SetRound(home,2);
    tr_SetColor(home,(char*)"#ffffff");
    tr_SetAction(home,onbutton_home,TR_ACTION_BUTTON);
    btext = (char*)malloc(32);
    strcpy(btext,(char*)"Main");
    //tr_SetTextEx(home,btext,PLACE_CENTER);
    tr_AddImage(home,ihome);
    tr_CanvasAddObjStatic(cwiskey,home);

    attr_bar_right.x = 370;
    attr_bar_right.y = 0;
    attr_bar_right.w = 100;
    attr_bar_right.h = 640;
    bar_right = tr_ButtonInitEx2(NULL,&attr_bar_right,cwiskey);
    tr_SetPositionRelative(bar_right,cwiskey,0.83,0.0,0.20,1.0);
    tr_SetLevel(bar_right,0);
    tr_SetColor(bar_right,(char*)"#161616");
    //tr_CanvasAddObjStatic(cwiskey,bar_right);
    
    for (i = 0; i < 5; i++)
    {
      switch(i)
      {
        case 0:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mwiskey);
          tr_LBEntryConfigure(lbentry,(char*)"Johnie Walker Red 0.04l",(char*)"2 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,(char*)"img/johniewalker.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cwiskey,lbentry);
          tr_SetAction(lbentry,order_johniewalker,TR_ACTION_BUTTON);
        break;
        case 1:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mwiskey);
          tr_LBEntryConfigure(lbentry,(char*)"Jameson 0.04l",(char*)"2.10 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,(char*)"img/jameson.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cwiskey,lbentry);
          tr_SetAction(lbentry,order_jameson,TR_ACTION_BUTTON);
        break;
        case 2:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mwiskey);
          tr_LBEntryConfigure(lbentry,"Jameson 12r. 0.04l","4 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,"img/jameson.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cwiskey,lbentry);
          tr_SetAction(lbentry,order_jameson12,TR_ACTION_BUTTON);
        break;
        case 3:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mwiskey);
          tr_LBEntryConfigure(lbentry,"Chivas Regal 12r. 0.04l","3.50 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,"img/chivasregal.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cwiskey,lbentry);
          tr_SetAction(lbentry,order_chivasregal,TR_ACTION_BUTTON);
        break;
        case 4:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mwiskey);
          tr_LBEntryConfigure(lbentry,"Jim Beam 0.04l","2.10 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,"img/jimbeam.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cwiskey,lbentry);
          tr_SetAction(lbentry,order_jimbeam,TR_ACTION_BUTTON);
        break;
      }
      //tr_SetAction(lbentry,onbutton_order,TR_ACTION_BUTTON);
      tr_SetFontColor(lbentry,0x30,0x30,0x30,220);
    }
    
    // spirits canvas
    tr_WindowToTop(mspirits);
    home = tr_ButtonInitEx2(NULL,&attr_home,cspirits);
    tr_SetRound(home,2);
    tr_SetColor(home,"#ffffff");
    tr_SetAction(home,onbutton_home,TR_ACTION_BUTTON);
    btext = (char*)malloc(32);
    strcpy(btext,"Main");
    //tr_SetTextEx(home,btext,PLACE_CENTER);
    tr_AddImage(home,ihome);
    tr_CanvasAddObjStatic(cspirits,home);
    for (i = 0; i < 6; i++)
    {
      switch(i)
      {
        case 0:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mspirits);
          tr_LBEntryConfigure(lbentry,"Tequilla 0.04","2.65 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,"img/tequila.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cspirits,lbentry);
          tr_SetAction(lbentry,order_tequilla,TR_ACTION_BUTTON);
        break;
        case 1:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mspirits);
          tr_LBEntryConfigure(lbentry,"Gin Beefeater 0.04","2 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,"img/gintonic.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cspirits,lbentry);
          tr_SetAction(lbentry,order_gin,TR_ACTION_BUTTON);
        break;
        case 2:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mspirits);
          tr_LBEntryConfigure(lbentry,"Vodka Absolut 0.04","2 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,"img/absolutvodka.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cspirits,lbentry);
          tr_SetAction(lbentry,order_vodka,TR_ACTION_BUTTON);
        break;
        case 3:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mspirits);
          tr_LBEntryConfigure(lbentry,"Slivovica Bosacka 0.04","2 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,"img/slivovica.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cspirits,lbentry);
          tr_SetAction(lbentry,order_slivovica,TR_ACTION_BUTTON);
        break;
        case 4:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mspirits);
          tr_LBEntryConfigure(lbentry,"Hruskovica Vilmos 0.04","2.30 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,"img/hruskovica.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cspirits,lbentry);
          tr_SetAction(lbentry,order_hruskovica,TR_ACTION_BUTTON);
        break;
        case 5:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mspirits);
          tr_LBEntryConfigure(lbentry,"Havana Club 0.04","2 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,"img/havanaclub.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cspirits,lbentry);
          tr_SetAction(lbentry,order_havana,TR_ACTION_BUTTON);
        break;
      }
      //tr_SetAction(lbentry,onbutton_order,TR_ACTION_BUTTON);
      tr_SetFontColor(lbentry,0x30,0x30,0x30,220);
    }
    
    // spirits canvas
    tr_WindowToTop(mliguere);
    home = tr_ButtonInitEx2(NULL,&attr_home,cliguere);
    tr_SetRound(home,2);
    tr_SetColor(home,"#ffffff");
    tr_SetAction(home,onbutton_home,TR_ACTION_BUTTON);
    btext = (char*)malloc(32);
    strcpy(btext,"Main");
    //tr_SetTextEx(home,btext,PLACE_CENTER);
    tr_AddImage(home,ihome);
    tr_CanvasAddObjStatic(cliguere,home);
    for (i = 0; i < 5; i++)
    {
      switch(i)
      {
        case 0:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mliguere);
          tr_LBEntryConfigure(lbentry,"Malibu 0.04","2 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,"img/malibu.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cliguere,lbentry);
          tr_SetAction(lbentry,order_malibu,TR_ACTION_BUTTON);
        break;
        case 1:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mliguere);
          tr_LBEntryConfigure(lbentry,"Fernet Stock 0.04","1.65 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,"img/fernetstock.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cliguere,lbentry);
          tr_SetAction(lbentry,order_fernet,TR_ACTION_BUTTON);
        break;
        case 2:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mliguere);
          tr_LBEntryConfigure(lbentry,"Becherovka 0.04","1.65 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,"img/becherovka.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cliguere,lbentry);
          tr_SetAction(lbentry,order_becherovka,TR_ACTION_BUTTON);
        break;
        case 3:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mliguere);
          tr_LBEntryConfigure(lbentry,"Demenovka 0.04","1.65 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,"img/demanovka.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cliguere,lbentry);
          tr_SetAction(lbentry,order_demanovka,TR_ACTION_BUTTON);
        break;
        case 4:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mliguere);
          tr_LBEntryConfigure(lbentry,"Vajecny Liker 0.04","1.30 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,"img/vajecnyliker.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cliguere,lbentry);
          tr_SetAction(lbentry,order_vajecnyliker,TR_ACTION_BUTTON);
        break;
      }
      //tr_SetAction(lbentry,onbutton_order,TR_ACTION_BUTTON);
      tr_SetFontColor(lbentry,0x30,0x30,0x30,220);
    }
    
    // beer canvas
    tr_WindowToTop(mbeer);
    home = tr_ButtonInitEx2(NULL,&attr_home,cbeer);
    tr_SetRound(home,2);
    tr_SetColor(home,"#ffffff");
    tr_SetAction(home,onbutton_home,TR_ACTION_BUTTON);
    btext = (char*)malloc(32);
    strcpy(btext,"Main");
    //tr_SetTextEx(home,btext,PLACE_CENTER);
    tr_AddImage(home,ihome);
    tr_CanvasAddObjStatic(cbeer,home);
    for (i = 0; i < 2; i++)
    {
      switch(i)
      {
        case 0:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mbeer);
          tr_LBEntryConfigure(lbentry,"Zlaty Bazant","1.5 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,"img/zlatybazant.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cbeer,lbentry);
          tr_SetAction(lbentry,order_zlatybazant,TR_ACTION_BUTTON);
        break;
        case 1:
          lbentry = tr_LBEntryInit(NULL,&attr_lbentry,mbeer);
          tr_LBEntryConfigure(lbentry,"Heineken 0.33l","1.65 euro");
          bimage = tr_ImageInitEx(NULL,&attr_button,"img/heineken.jpg");
          tr_ImageSetRound(bimage,2);
          tr_LBEntrySetImage(lbentry,bimage);
          if (lbentry)
            tr_CanvasAddObj(cbeer,lbentry);
          tr_SetAction(lbentry,order_heineken,TR_ACTION_BUTTON);
        break;
      }
      //tr_SetAction(lbentry,onbutton_order,TR_ACTION_BUTTON);
      tr_SetFontColor(lbentry,0x30,0x30,0x30,220);
    }
    
    // products
    tr_WindowToTop(mcocacola);
    home = tr_ButtonInitEx2(NULL,&attr_home,ccocacola);
    tr_SetRound(home,2);
    tr_SetColor(home,"#202020");
    tr_SetAction(home,onbutton_home,TR_ACTION_BUTTON);
    btext = (char*)malloc(32);
    strcpy(btext,"Main");
    //tr_SetTextEx(home,btext,PLACE_CENTER);
    tr_AddImage(home,ihome);
    tr_CanvasAddObjStatic(ccocacola,home);

    bimage = tr_ImageInitEx(NULL,&attr_info,"img/cocacola_poster_small.jpg");
    button = tr_ButtonInitEx2(NULL,&attr_info,ccocacola);
    tr_ImageSetRound(bimage,2);
    tr_SetRound(button,2);
    tr_AddImage(button,bimage);
    tr_CanvasAddObj(ccocacola,button);
    
    attr_info.h = 120;
    button = tr_TextInit(NULL,&attr_info,ccocacola);
    tr_SetColor(button,"#ffffff");
    tr_SetRound(button,2);
    form_TextAddText(button->widget.sub,"CocaCoca is a drink of gods created by a \
      Cocaine baron in the 17th century");
    tr_CanvasAddObj(ccocacola,button);

    tr_WindowToTop(morder);
    home = tr_ButtonInitEx2(NULL,&attr_home,corder);
    tr_SetRound(home,2);
    tr_SetColor(home,"#202020");
    tr_SetAction(home,onbutton_home,TR_ACTION_BUTTON);
    btext = (char*)malloc(32);
    strcpy(btext,"Main");
    //tr_SetTextEx(home,btext,PLACE_CENTER);
    tr_AddImage(home,ihome);
    tr_CanvasAddObjStatic(corder,home);
    attr_order.x = 20;
    attr_order.y = 20;
    attr_order.w = 120;
    attr_order.h = 80;
    border = tr_ButtonInitEx2(NULL,&attr_order,corder);
    tr_SetRound(border,2);
    tr_SetColor(border,"#ddffdd");
    btext = (char*)malloc(32);
    strcpy(btext,"Order");
    //tr_AddImage(border,iorder);
    tr_SetTextEx(border,btext,PLACE_CENTER);
    //tr_CanvasAddObjStatic(corder,border);
    attr_order.x = 160;
    bcancel = tr_ButtonInitEx2(NULL,&attr_order,corder);
    tr_SetRound(bcancel,2);
    tr_SetColor(bcancel,"#ffdddd");
    btext = (char*)malloc(32);
    strcpy(btext,"Cancel");
    tr_SetTextEx(bcancel,btext,PLACE_CENTER);
    //tr_CanvasAddObjStatic(corder,bcancel);
    //tr_AddImage(bcancel,icancel);
    
    attr_info.h = 120;
    attr_info.w = 250;
    button = tr_TextInit(NULL,&attr_info,corder);
    tr_SetColor(button,"#ffffff");
    tr_SetRound(button,2);
    form_TextAddText(button->widget.sub,"Order Sent");
    tr_CanvasAddObj(corder,button);
    
    struct setup s;
    setupInit(&s,(unsigned char*)"setup.xml");
    unsigned char title[128];
    setupGet(&s,(unsigned char*)"title",(unsigned char*)"value",(unsigned char*)title);
    printf("menu_title (%s)\n",(char*)title);
    
    tr_WindowToTop(mmain);
    
    // add tests here
    /**
    struct mikaSerial *serial = mikaSerialInit(NULL);
    struct mikaSerial *serialClone = mikaSerialClone();
    mikaSerialEventLoopRun(serial);
    mikaSerialAddEvent(serial,1,NULL);
    while(1)
    {
      mikaSerialAddEvent(serial,1,NULL);
      mikaSerialAddEvent(serial,1,NULL);
      mikaSerialAddEvent(serial,1,NULL);
      sleep(1);
    }
    */
    //
    
    tr_WindowQueue(root);
    
    //tr_WindowToTop(custom->start->window);
    //tr_WindowToTop(custom->start->window);
    #endif

    #ifdef _TEST_LAYOUT
      if (!custom)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf(":err no layout\n");
        #endif
      }
      if (!custom->root)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf(":err no root in layout\n");
        #endif
      }
      tr_WindowQueue(custom->root->window);
    #endif
    
    return 0;
  }

