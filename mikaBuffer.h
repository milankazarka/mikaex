#ifndef _MIKA_BUFFER_H_
#define _MIKA_BUFFER_H_

#define _MIKA_BUFFER_SMALL 1024
#define _MIKA_BUFFER_SIZE 4096

typedef struct mikaBuffer
{
  unsigned char data[_MIKA_BUFFER_SIZE];
  int size;
  struct mikaBuffer *last;
  struct mikaBuffer *next;
} mikaBuffer;

#endif
