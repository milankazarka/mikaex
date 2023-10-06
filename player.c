/** this is used to 
 *
 */
#include "player.h"
#include "config.h"
#include "interface.h"
#include "gui.h"
#include "os.h"

#define _STD_LINE 	1024
#define _PLAYER_TIK 	50000

struct mikaPlayer *mikaPlayerInit( struct mikaPlayer *player )
{
  if (!player)
    player = (struct mikaPlayer*)malloc(sizeof(struct mikaPlayer));
  player->state = _MIKA_PLAYER_STATE_STOP;
  player->filename[0] = 0x00;
  // the widget to be used as the canvas for the movie playback
  //
  player->canvasWidget = NULL;
  player->customw = -1;
  player->customh = -1;
  pthread_create(&player->thread,NULL,mikaPlayerThread,player);
  return player;
}

/** set the widget to which we will commit the movie playback
 */
int mikaPlayerSetCanvasWidget( struct mikaPlayer *player, struct gui_Widget *widget )
{
  if (!player || !widget)
  {
    #ifdef _DEBUG
      printf("mikaPlayerSetCanvasWidget error, invalid arguments\n");
    #endif
    return 1;
  }
  
  player->canvasWidget = widget;
  
  return 0;
}

void *mikaPlayerThread(void*data)
{
  if (!data)
  {
    #ifdef _DEBUG
      printf("mikaPlayerThread invalid arguments\n");
    #endif
    return NULL;
  }
  struct mikaPlayer *player = (struct mikaPlayer*)data;
  // todo, refactoring
  //
  while(1)
  {
    binarySemaphoreWait(bigInterface->activity.sem->id); // move to core scheduller
    if (player->command!=_MIKA_PLAYER_COMMAND_NONE)
    {
      if (player->state==_MIKA_PLAYER_STATE_PLAY)
      {
        #ifdef _DEBUG
          printf("mikaPlayerThread _MIKA_PLAYER_STATE_PLAY\n");
        #endif
        switch(player->command)
        {
          case _MIKA_PLAYER_COMMAND_STOP:
          break;
        }
      }
      else if (player->state==_MIKA_PLAYER_STATE_STOP)
      {
      }
      player->command = _MIKA_PLAYER_COMMAND_NONE;
    }
    binarySemaphorePost(bigInterface->activity.sem->id);
    usleep(_PLAYER_TIK);
  }
}

/** run the playback of the movie. The movie is played onto the
 *  canvas of the application
 */
void *mikaPlayerPlayThread(void*data)
{
  if (!data)
  {
    #ifdef _DEBUG
      printf("mikaPlayerPlayThread error invalid arguments\n");
    #endif
    return NULL;
  }
  struct mikaPlayer *player = (struct mikaPlayer*)data;
  
  unsigned char command[_STD_LINE];
  unsigned char output[_STD_LINE];
  
  if (!player)
  {
    #ifdef _DEBUG
      printf("mikaPlayerPlay error invalid arguments\n");
    #endif
    return 1;
  }
  
  if (player->filename[0]==0x00)
  {
    #ifdef _DEBUG
      printf("mikaPlayerPlay error no filename defined\n");
    #endif
    return 1;
  }
  
  int sizew;
  int sizeh;
  struct gui_Widget *mw = gui_GetMain();
  if (player->customw!=-1)	sizew = player->customw;
  else				sizew = mw->attr.w;
  
  if (player->customh!=-1)	sizeh = player->customh;
  else				sizeh = mw->attr.h;
  
  sprintf((char*)command,"%s -vo x11 -nosound -hardframedrop -zoom -x %d -y %d %s",
    _MPLAYER_BIN,sizew,sizeh,
    (char*)player->filename);
  
  FILE *fdc = popen((char*)command,"r");
  if (fdc)
  {
    binarySemaphoreWait(bigInterface->activity.sem->id);
    player->state = _MIKA_PLAYER_STATE_PLAY;
    binarySemaphorePost(bigInterface->activity.sem->id);
    
    while(fgets((char*)output,_STD_LINE,fdc))
    {
      if (output[strlen((char*)output)-1]=='\n')
        output[strlen((char*)output)-1] = 0x00;
      printf("%s",(char*)output);
    }
    pclose(fdc);
  }
  
}

/** start the playback of a media file
 */
int mikaPlayerPlay( struct mikaPlayer *player )
{
  if (!player)
  {
    #ifdef _DEBUG
      printf("mikaPlayerPlay error invalid arguments\n");
    #endif
    return 1;
  }
  
  pthread_create(&player->threadPlayback,NULL,mikaPlayerPlayThread,player);
  
  return 0;
}
