#include "integrations.hpp"
#include "keyfinder/keyfinder.h"


namespace PROJECT_NAME {
    KeyFinder::AudioData PrepareAudioData(double* samples, size_t samples_size, int sample_rate, int channels) {
        KeyFinder::AudioData audio_data;
        audio_data.setFrameRate(sample_rate);
        audio_data.setChannels(channels);
        audio_data.addToSampleCount(samples_size);

        for (int current_sample = 0; current_sample < samples_size; ++current_sample) {
            audio_data.setSample(current_sample, samples[current_sample]);

        }
        
        return audio_data;
    }

    KeyFinder::key_t GetKeyOfAudio(KeyFinder::AudioData audio_data) {
        KeyFinder::KeyFinder k;
        KeyFinder::key_t key = k.keyOfAudio(audio_data);

        return key;
    }
}
