#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QThread>
#include "../processing/ring_buff.h"
#include "../audio/sampling.h"
#include <QPushButton>

struct _yin_audio_pref {
    float threshold;
    unsigned int window_size;
    unsigned int count;
    short int bits_per_sampe;
    unsigned int sampling_freq;
    const char* device;
};

typedef struct _yin_audio_pref yin_audio_pref;

class Controller : public QObject {
    Q_OBJECT
private:
    bool _continue;
    QThread* read_thread;
    QThread* proc_thread;
    QPushButton *m_button;
    QPushButton *exitButton;
    QApplication* apl;
    int buff_size;
    ring_buffer <amplitude_probes> *rb;
    yin_audio_pref *prefs;
    audio_handler *audio;
public:
    Controller(QWidget *parent, yin_audio_pref& prefs, int buff_size, QApplication* app);
    ~Controller();
signals:
    void stop_read();
    void stop_write();
public slots:
    void slot_freq_handel(double);
    void stop ();
};

#endif // CONTROLLER_H
