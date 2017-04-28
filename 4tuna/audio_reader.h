#ifndef AUDIO_READER_H
#define AUDIO_READER_H

#include <QObject>
#include "../processing/ring_buff.h"
#include "../processing/filter.h"
#include "../audio/sampling.h"

class audio_reader : public QObject
{
    Q_OBJECT
private:
    bool _continue;
    short bits_per_sample;
    unsigned int count;
    unsigned int sampling_freq;
    ring_buffer <amplitude_probes> *rb;
    filter *F;
    audio_handler *audio;
public:
    explicit audio_reader(ring_buffer <amplitude_probes> *buffer,
                          unsigned int c, unsigned int fs, short int bd, audio_handler *audio);

signals:
    void reader_finished();
public slots:
    void read_samples();
    void please_stop();
};

#endif // AUDIO_READER_H
