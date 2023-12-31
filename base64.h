/*

	   Encode or decode file as MIME base64 (RFC 1341)

			    by John Walker
		       http://www.fourmilab.ch/

		This program is in the public domain.

*/

//#define BASE64LEN 768 // always % 12, read by 576 'x/4*3'
#define BASE64LEN 32 // alyc requires %4

struct buffer_st {
  char *data;
  int length;
  char *ptr;
  int offset;
};

void buffer_new(struct buffer_st *b);
void buffer_add(struct buffer_st *b, char c);
void buffer_delete(struct buffer_st *b);

void base64_encode(struct buffer_st *b, const char *source, int length);
void base64_decode(struct buffer_st *b, const char *source, int length);

/*
#define DEBUG_MALLOC
 */

#ifdef DEBUG_MALLOC
void *_malloc_real(size_t s, char *file, int line);
void _free_real(void *p, char *file, int line);

#define malloc(s)	_malloc_real(s,__FILE__,__LINE__)
#define free(p)		_free_real(p, __FILE__,__LINE__)
#endif

