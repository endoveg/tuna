#include "mainwindow.h"
#include "controller.h"
#include <QApplication>

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
sem_t count_sem, space_sem;
/*BANG BANG low quality code is here*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    yin_audio_pref pref;
    pref.bits_per_sampe = 2;
    pref.count = 4096;
    pref.threshold = 0.1;
    pref.sampling_freq = 44100;
    pref.device = "plughw:1,0";
    pref.window_size = 1024;

    Controller tuner(&w, pref, 16, &a);
    return a.exec();
}
