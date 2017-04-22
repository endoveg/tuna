#include "ring_buff.h"
#include <pthread.h>
#include <unistd.h>
#include <thread>
#include "../audio/sampling.h"
#include <vector>
#include <complex>
#include "dft.h"
#include <fstream>

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
sem_t count_sem, space_sem;

//this file's purpose is only to show
//how to initialize and destroy mutex and semaphores,
//how to use template ring buffer

void* writer (ring_buffer <amplitude_probes>* ptr)
{
    unsigned int sz = 8;
    amplitude_probes* amp =  new amplitude_probes (44100, sz, 2); 
    amp->capture (1);
    while (pthread_mutex_trylock (&amp->is_done) == EBUSY)
    {   
    }
    ptr->write (amp);
    return NULL;
}

void* reader (ring_buffer <amplitude_probes>* ptr)
{
    unsigned int sz = 2048;
    std::vector <std::complex <double> > out (sz);
    ptr->read (fft, out);
    unsigned int discr_freq = 44100;
    std::ofstream display;
    display.open ("variadic.txt");
    std::vector <double> freq (sz / 2); 
    for (unsigned int i = 0; i < sz / 2; i++)
        freq[i] = (double) discr_freq / sz * i;
    for (unsigned int i = 0; i < sz / 2; i++)
        display << freq[i] <<" "<<std::abs (out[i]) <<"\n";
    display.close (); 

    return NULL;
}

int main ()
{
    const unsigned int buff_size = 16;
    pthread_mutex_init (&mtx, NULL);
    sem_init (&count_sem, 0, 0);
    sem_init (&space_sem, 0, buff_size);
    ring_buffer <amplitude_probes>* a = new ring_buffer <amplitude_probes> (buff_size);
    for (unsigned int i = 0; i < 16 * buff_size; i++)
    {
        writer (a);
        reader (a);
        //some more sophisticated tests here
    }
    pthread_mutex_destroy (&mtx);
    delete a;
    return 0;
}
