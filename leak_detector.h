#ifndef  LEAK_DETECTOR_H
#define  LEAK_DETECTOR_H

void * xrealloc (void *str,int size,const char *file,unsigned int line);
void * xmalloc(unsigned int size, const char * file, unsigned int line);
void * xcalloc(unsigned int elements, unsigned int size, const char * file, unsigned int line);
char * xstrdup (const char *str, const char * file, unsigned int line);
void xfree(void * mem_ref, const char * file, unsigned int linesss);

#define  realloc(str,size)			xrealloc(str,size, __FILE__, __LINE__)
#define  strdup(str)				xstrdup(str, __FILE__, __LINE__)
#define  malloc(size)				xmalloc(size, __FILE__, __LINE__)
#define  calloc(elements, size)		xcalloc(elements, size, __FILE__, __LINE__)
#define  free(mem_ref)				xfree(mem_ref,  __FILE__, __LINE__)

void test1();
#endif

