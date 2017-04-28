#include "audio_reader.h"
#include "../processing/ring_buff.h"
#include <../processing/filter.h>
#include <fstream>

audio_reader::audio_reader(ring_buffer <amplitude_probes> *buffer,
                           unsigned int c, unsigned int fs, short int bd, audio_handler *au) {
    rb = buffer;
    audio = au;
    count = c;
    _continue = true;
    sampling_freq = fs;
    bits_per_sample = bd;
    std::ifstream filter_coefs;
    filter_coefs.open("../processing/coef");
    F = new filter(filter_coefs);
}

void audio_reader::read_samples() {
    while(_continue) {
        amplitude_probes* amp = new amplitude_probes(sampling_freq, count, bits_per_sample);
        amp->capture (*audio);
        F->apply(amp);
        rb->write(amp);
    }
}

void audio_reader::please_stop() {
    _continue = false;
    emit reader_finished();
}
