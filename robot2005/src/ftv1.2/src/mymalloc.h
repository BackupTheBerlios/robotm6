#ifndef _MYMALLOC_H
#define _MYMALLOC_H

void        *mymalloc          (int size);
void        *myrealloc         (void *ptr, int size);
void         myfree            (void *ptr);
void         myexit            (int n);
extern int          malloc_status;

#endif /* _MYMALLOC_H */
