#include <iostream>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <mutex>
#include <semaphore.h>
#include "../audio/sampling.h"

#ifndef RING_BUFF_H
#define RING_BUFF_H
extern pthread_mutex_t mtx;
extern sem_t count_sem, space_sem;
/*
ATTENTION! mtx, count_sem and space_sem should be declared in the file 
where the ring buffer is used (as global variables)
*/

template <typename type> class ring_buffer 
{
protected:
    type** init;
    unsigned int read_end;
    unsigned int  write_end;
    unsigned int length;
public:
    ring_buffer (unsigned int size);
    ring_buffer (const ring_buffer &to_copy);
    void write (type* x);
    template <typename F, typename ... Args> void read (F& f, Args&&... args);
    ~ring_buffer ();        
};

template <typename type> ring_buffer <type>::ring_buffer (unsigned int size)
{
    length = size;
    init = new type* [length];
    for (unsigned i = 0; i < length; i++)
        init [i] = NULL;
    read_end = write_end = 0;
}

template <typename type> ring_buffer <type>::ring_buffer (const ring_buffer &to_copy)
{
    init = new type* [to_copy.length];
    length = to_copy.length;
    read_end = to_copy.read_end;
    write_end = to_copy.write_end;
}

template <typename type> void ring_buffer <type>::write (type* x)
{
    sem_wait (&space_sem);
    pthread_mutex_lock (&mtx);
    if (init [write_end] != NULL)
        delete init [write_end];
    init [write_end++] = x;
    if (write_end == length)
        write_end = 0;
    pthread_mutex_unlock (&mtx);
    sem_post (&count_sem);
}

template <typename type>
template <typename F, typename ... Args>
void ring_buffer <type>::read (F& f, Args&&... args)
{
    sem_wait (&count_sem);
    pthread_mutex_lock (&mtx);
   /* f (*(init[read_end++]), std::forward<Args>(args)...);*/
    f.process(*(init[read_end++]), std::forward<Args>(args)...);
    if (read_end == length)
        read_end = 0;
    pthread_mutex_unlock (&mtx);
    sem_post (&space_sem);
}

template <typename type> ring_buffer <type>::~ring_buffer ()
{
    for (unsigned int i = 0; i < length; i++)
        if (init [i] != NULL)
            delete init[i];
    delete[] init;
}

#endif
