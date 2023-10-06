#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "wm.h"
#define _MPLAYER_BIN "mplayer"

enum mikaPlayerStates
{
  _MIKA_PLAYER_STATE_PLAY,
  _MIKA_PLAYER_STATE_PAUSE,
  _MIKA_PLAYER_STATE_STOP
};

enum mikaPlayerCommand
{
  _MIKA_PLAYER_COMMAND_NONE,
  _MIKA_PLAYER_COMMAND_PLAY,
  _MIKA_PLAYER_COMMAND_PAUSE,
  _MIKA_PLAYER_COMMAND_STOP
};

typedef struct mikaPlayer
{
  // custom size of the canvas
  int customw;
  int customh;
  int state;
  int command;
  pthread_t thread;
  pthread_t threadPlayback;
  unsigned char filename[1024];
  // widget to be used as the canvas for the movie playback
  gui_Widget *canvasWidget;
} mikaPlayer;

struct mikaPlayer *mikaPlayerInit( struct mikaPlayer *player );
int mikaPlayerPlay( struct mikaPlayer *player );
void *mikaPlayerThread(void*data);
