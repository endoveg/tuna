#include "proc_yin.h"
#include "../processing/yin.h"

proc_yin::proc_yin(float t, unsigned int w,
                   ring_buffer <amplitude_probes> *buff, QApplication* a) {
    threshold = t;
    window_size = w;
    _continue = true;
    rb = buff;
    QApplication* apl = a;
}

void proc_yin::process_samples() {
    while(_continue) {
        apl->processEvents ();
        rb->read(*this, threshold, window_size);
    }
}

void proc_yin::process(amplitude_probes& amp, float threshold, unsigned int window_size) {
    double cur_freq = yin(amp, threshold, window_size,0);
    if (cur_freq <= 75.0) {
    } else {
        emit freq_is_estimated(cur_freq);
    }
    for (unsigned int i = 1; i < window_size; i++) {
        cur_freq =  yin(amp, threshold, window_size,i);
        if (cur_freq <= 75.0) {
        } else {
            emit freq_is_estimated(cur_freq);
            last_found_freq = cur_freq;
        }
    }
}

void proc_yin::please_stop() {
    _continue = false;
    emit proc_finished();
}
