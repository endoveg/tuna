#ifndef PROC_YIN_H
#define PROC_YIN_H

#include <QObject>
#include <QApplication>
#include "../processing/ring_buff.h"

class proc_yin : public QObject
{
    Q_OBJECT
public:
    explicit proc_yin(float threshold,
                      unsigned int window_size, ring_buffer <amplitude_probes> *buff, QApplication* a);
private:
    bool _continue;
    double last_found_freq;
    ring_buffer <amplitude_probes>* rb;
    unsigned int window_size;
    float threshold;
    QApplication* apl;
public:
    void process(amplitude_probes& amp, float threshold,
                      unsigned int window_size);
signals:
    void freq_is_estimated(double freq);
    void proc_finished ();
public slots:
    void process_samples();
    void please_stop();
};

#endif // PROC_YIN_H
