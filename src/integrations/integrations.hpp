#include <cstddef>
#include <keyfinder/audiodata.h>

namespace PROJECT_NAME {
    extern "C" {
        KeyFinder::AudioData PrepareAudioData(double* samples, size_t samples_size, int sample_rate, int channels);
        KeyFinder::key_t GetKeyOfAudio(KeyFinder::AudioData audio_data);
    }
}