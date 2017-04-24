#ifndef PROC_YIN_H
#define PROC_YIN_H

#include <QObject>
#include "../processing/ring_buff.h"

class proc_yin : public QObject
{
    Q_OBJECT
public:
    explicit proc_yin(float threshold,
                      unsigned int window_size, ring_buffer <amplitude_probes> *buff);
private:
    bool _continue;
    double last_found_freq;
    ring_buffer <amplitude_probes>* rb;
    unsigned int window_size;
    float threshold;
public:
    void process(amplitude_probes& amp, float threshold,
                      unsigned int window_size);
signals:
    void freq_is_estimated(double freq);
public slots:
    void process_samples();
    void please_stop();
};

#endif // PROC_YIN_H
