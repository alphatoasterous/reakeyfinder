#include <cstddef>
#include <keyfinder/audiodata.h>
#pragma once

namespace KeyFinder {
    struct CompatibleKeys {
            key_t relative; // Relative major/minor
            key_t quart;    // Perfect fourth
            key_t quint;    // Perfect fifth
        };

    // Determine if a key is major
    bool is_major(key_t key);

    // Get pitch class from key
    int GetPitchClass(key_t key);

    const char* GetKeyString(key_t key);
    const char* GetCamelotString(key_t key);
    const char* GetKeyInfo(key_t key);

    // Find the key by pitch class and mode
    key_t ResolveKey(int pitch_class, bool major);

    // Compute compatible keys (relative, quart, quint)
    CompatibleKeys GetCompatibleKeys(key_t input);
}

namespace PROJECT_NAME {
    extern "C" {
        KeyFinder::AudioData PrepareAudioData(double* samples, size_t samples_size, int sample_rate, int channels);
        KeyFinder::key_t GetKeyOfAudio(KeyFinder::AudioData audio_data);
    }
}