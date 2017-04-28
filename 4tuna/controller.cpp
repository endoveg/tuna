#include <QCloseEvent>
#include "controller.h"
#include "proc_yin.h"
#include "audio_reader.h"
#include "proc_yin.h"
#include <QObject>
#include <QMainWindow>

Controller::Controller(QWidget *parent, yin_audio_pref& pref, int bfz) {
    m_button = new QPushButton(parent);
    m_button->setGeometry(10, 10, 80, 30);
    m_button->setText("440");
    m_button->show();
    setParent(parent);
    buff_size = bfz;
    pthread_mutex_init (&mtx, NULL);
    sem_init (&count_sem, 0, 0);
    sem_init (&space_sem, 0, buff_size);
    audio = new audio_handler(pref.sampling_freq, pref.bits_per_sampe, pref.device);
    audio->open();
    rb = new ring_buffer <amplitude_probes> (buff_size);
    audio_reader *ar = new audio_reader(rb, pref.count, pref.sampling_freq,
                                        pref.bits_per_sampe, audio);
    proc_yin *py = new proc_yin(pref.threshold, pref.window_size, rb);
    ar->moveToThread(&read_thread);
    py->moveToThread(&proc_thread);
    connect(&read_thread, &QThread::finished, ar, &QObject::deleteLater);
    connect(&proc_thread, &QThread::finished, py, &QObject::deleteLater);
    connect(&read_thread, SIGNAL(started()), ar, SLOT(read_samples()));
    connect(&proc_thread, SIGNAL(started()), py, SLOT(process_samples()));
    connect(py, SIGNAL (freq_is_estimated(double)),
            this, SLOT (slot_freq_handel(double)));
    read_thread.start();
    proc_thread.start();
}

void Controller::slot_freq_handel(double d) {
    m_button->setText(QString::number(d));
}
