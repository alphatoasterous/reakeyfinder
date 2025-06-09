#include "integrations.hpp"
#include "keyfinder/keyfinder.h"
#include <keyfinder/constants.h>
#include <map>
#include <iomanip>

namespace KeyFinder {
    bool is_major(key_t key) {
        return key % 2 == 0;
    }

    // Map key to a pitch class (0 = C, 1 = C#, ..., 11 = B)
    std::map<key_t, int> key_to_pitch = {
        {A_MAJOR, 9}, {A_MINOR, 9},
        {B_FLAT_MAJOR, 10}, {B_FLAT_MINOR, 10},
        {B_MAJOR, 11}, {B_MINOR, 11},
        {C_MAJOR, 0}, {C_MINOR, 0},
        {D_FLAT_MAJOR, 1}, {D_FLAT_MINOR, 1},
        {D_MAJOR, 2}, {D_MINOR, 2},
        {E_FLAT_MAJOR, 3}, {E_FLAT_MINOR, 3},
        {E_MAJOR, 4}, {E_MINOR, 4},
        {F_MAJOR, 5}, {F_MINOR, 5},
        {G_FLAT_MAJOR, 6}, {G_FLAT_MINOR, 6},
        {G_MAJOR, 7}, {G_MINOR, 7},
        {A_FLAT_MAJOR, 8}, {A_FLAT_MINOR, 8},
    };

    // Get key from pitch class and major/minor flag
    key_t ResolveKey(int pitch, bool major) {
        for (const auto& [key, p] : key_to_pitch) {
            if (p == pitch && is_major(key) == major) {
                return key;
            }
        }
        return SILENCE;
    }

    key_t TransposeKey(key_t key, int semitone) {
        return ResolveKey((key_to_pitch[key] + semitone) % 12, is_major(key));
    }

    CompatibleKeys GetCompatibleKeys(key_t input) {
        CompatibleKeys result;
        if (input == SILENCE) return {SILENCE, SILENCE, SILENCE};

        int pitch = key_to_pitch[input];
        bool major = is_major(input);

        // Relative key: same pitch, different mode
        result.relative = ResolveKey((major ? pitch + 9 : pitch + 3) % 12, !major);

        // Quart = perfect fourth below (5 semitones down)
        result.quart = ResolveKey((pitch + 5) % 12, major);

        // Quint = perfect fifth up (7 semitones up)
        result.quint = ResolveKey((pitch + 7) % 12, major);

        return result;
    }

    const char* GetKeyString(key_t key) {
        switch (key) {
            case A_MAJOR:         return "A major";
            case A_MINOR:         return "A minor";
            case B_FLAT_MAJOR:    return "B flat major";
            case B_FLAT_MINOR:    return "B flat minor";
            case B_MAJOR:         return "B major";
            case B_MINOR:         return "B minor";
            case C_MAJOR:         return "C major";
            case C_MINOR:         return "C minor";
            case D_FLAT_MAJOR:    return "D flat major";
            case D_FLAT_MINOR:    return "D flat minor";
            case D_MAJOR:         return "D major";
            case D_MINOR:         return "D minor";
            case E_FLAT_MAJOR:    return "E flat major";
            case E_FLAT_MINOR:    return "E flat minor";
            case E_MAJOR:         return "E major";
            case E_MINOR:         return "E minor";
            case F_MAJOR:         return "F major";
            case F_MINOR:         return "F minor";
            case G_FLAT_MAJOR:    return "G flat major";
            case G_FLAT_MINOR:    return "G flat minor";
            case G_MAJOR:         return "G major";
            case G_MINOR:         return "G minor";
            case A_FLAT_MAJOR:    return "A flat major";
            case A_FLAT_MINOR:    return "A flat minor";
            case SILENCE:         return "Silence";
            default:              return "Unknown key";
        }
    }

    const char* GetCamelotString(key_t key) {
        switch (key) {
            case A_MAJOR:         return "11B";
            case A_MINOR:         return "8A";
            case B_FLAT_MAJOR:    return "6B";
            case B_FLAT_MINOR:    return "3A";
            case B_MAJOR:         return "1B";
            case B_MINOR:         return "10A";
            case C_MAJOR:         return "8B";
            case C_MINOR:         return "5A";
            case D_FLAT_MAJOR:    return "3B";
            case D_FLAT_MINOR:    return "12A";
            case D_MAJOR:         return "10B";
            case D_MINOR:         return "7A";
            case E_FLAT_MAJOR:    return "5B";
            case E_FLAT_MINOR:    return "2A";
            case E_MAJOR:         return "12B";
            case E_MINOR:         return "9A";
            case F_MAJOR:         return "7B";
            case F_MINOR:         return "4A";
            case G_FLAT_MAJOR:    return "2B";
            case G_FLAT_MINOR:    return "11A";
            case G_MAJOR:         return "9B";
            case G_MINOR:         return "6A";
            case A_FLAT_MAJOR:    return "4B";
            case A_FLAT_MINOR:    return "1A";
            case SILENCE:         return "Silence";
            default:              return "Unknown";
        }
    }

    const char* GetKeyInfo(key_t key) {
        thread_local std::string result;
        CompatibleKeys keys = GetCompatibleKeys(key);

        std::ostringstream oss;
        oss << "Key: "     << GetKeyString(key)      << " (" << GetCamelotString(key) << ")\n";
        oss << "Relative: " << GetKeyString(keys.relative) << " (" << GetCamelotString(keys.relative) << ")\n";
        oss << "Quart: "   << GetKeyString(keys.quart)    << " (" << GetCamelotString(keys.quart) << ")\n";
        oss << "Quint: "   << GetKeyString(keys.quint)    << " (" << GetCamelotString(keys.quint) << ")\n";

        result = oss.str();
        return result.c_str();
    }

}


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

    std::string BuildInfoString(double project_bpm, const char* take_name, KeyFinder::key_t key, double bpm, double time, int float_precision) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(float_precision);

        oss << "Project BPM: " << project_bpm << "\n";
        oss << "\n========================\n\n";

        oss << "Current take: " << take_name << "\n\n";

        oss << KeyFinder::GetKeyInfo(key) << "\n";

        oss << "\nBPM: " << bpm << "\n";
        oss << "\n========================\n\n";
        oss << "Time elapsed(ms): " << time;

        return oss.str();
    }

}
