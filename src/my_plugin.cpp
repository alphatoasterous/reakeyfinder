#include "my_plugin.h"
#include "reaper_plugin.h"
#include "reaper_plugin_functions.h"
#include "reaper_vararg.hpp"

#include "integrations/integrations.hpp"
#include "utils/utils.hpp"
#include "json.hpp"

#include <cstdio>
#include <gsl/gsl>
#include <keyfinder/constants.h>
#include <MiniBpm.h>

#include <thread>
#include <chrono>

#define STRINGIZE_DEF(x) #x
#define STRINGIZE(x) STRINGIZE_DEF(x)
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

// confine my plugin to namespace
namespace PROJECT_NAME
{

// some global non-const variables
// the necessary 'evil'
int command_id{0};
constexpr auto command_name = "AK5K_" STRINGIZE(PROJECT_NAME) "_COMMAND";
constexpr auto action_name = STRINGIZE(PROJECT_NAME) ": " "Analyze key/BPM of selected media item";
custom_action_register_t action = {0, command_name, action_name, nullptr};

// hInstance is declared in header file my_plugin.hpp
// defined here
REAPER_PLUGIN_HINSTANCE hInstance{nullptr}; // used for dialogs, if any

// the main function of my plugin
// gets called via callback or timer
void ReaKeyfinder()
{

    TimePoint seq_start = Clock::now();

    

    // Access audio from a current item's take #0. Write it to a sample_buffer.
    MediaItem* current_item = GetSelectedMediaItem(0, 0);
    if (!(IsMediaItemSelected(current_item))) {
        ShowConsoleMsg("Айтем мне выбери блядь\n");
        return;
    }
    MediaItem_Take* current_take = GetMediaItemTake(current_item, 0);
    AudioAccessor* audio_accessor = CreateTakeAudioAccessor(current_take);
    
    char* item_metadata = new char[65536];
    bool metadata_empty = false;
    nlohmann::json js;
    if (GetSetMediaItemInfo_String(current_item, "P_EXT:reakeyfinder", item_metadata, false))
    {
        if (isStringNotEmpty(item_metadata)) {
            try {
                nlohmann::json js = nlohmann::json::parse(item_metadata);
                // Successfully parsed
            } catch (const nlohmann::json::parse_error&) {
                // Parsing failed
                metadata_empty = true;
            }
        }
    }
    if (metadata_empty) {
        ShowConsoleMsg("Metadata is empty\n");
    }
    
    
    double start_time = GetAudioAccessorStartTime(audio_accessor);
    double end_time = GetAudioAccessorEndTime(audio_accessor);
    int length = end_time - start_time;
    int sample_rate = 48000;
    int numchannels = 2;
    size_t samples_size = (length * sample_rate * numchannels);
    double* samples = new double[samples_size];
    int samples_created = GetAudioAccessorSamples(audio_accessor, sample_rate, numchannels, 0.0, length * sample_rate, samples); // 0.0 - ???
    
    
    // Verify that samples are indeed created.
    if (samples_created == 1){
        //ShowConsoleMsg("Ну вот тебе буфер, карочи: ");
        //ShowConsoleMsg(StringizeSampleBuffer(sample_buffer, sample_buffer_size).c_str());
    } else if (samples_created == 0){
        ShowConsoleMsg("А где звук ёпта\n");
        return;
    } else if (samples_created == -1){
        ShowConsoleMsg("пиздец\n"); return;
    } else { ShowConsoleMsg("уберпиздец\n"); return;}

    
    // Get project bpm
    double project_bpm = 0.0f;
    GetProjectTimeSignature2(GetItemProjectContext(current_item), &project_bpm, nullptr);

    // Get key
    KeyFinder::key_t key;
    std::thread key_thread([&]() {
        key = GetKeyOfAudio(PrepareAudioData(samples, samples_size, sample_rate, numchannels));
    });
    
    // Get bpm
    double bpm;
    std::thread bpm_thread([&]() {
        bpm = breakfastquay::MiniBPM(sample_rate).estimateTempoOfSamples(samples, samples_size);
    });

    bpm_thread.join();
    key_thread.join();
    
    // Get elapsed time
    TimePoint seq_end = Clock::now();
    double seq_time = std::chrono::duration<double, std::milli>(seq_end - seq_start).count();

    // Show console output
    ShowConsoleMsg(BuildInfoString(project_bpm, GetTakeName(current_take), key, bpm, seq_time, 3).c_str());

    // Free allocated stuff
    DestroyAudioAccessor(audio_accessor);
    delete[] samples;
    delete[] item_metadata;
}

// this gets called when my plugin action is run (e.g. from action list)
bool OnAction(KbdSectionInfo* sec, int command, int val, int valhw, int relmode, HWND hwnd)
{
    // treat unused variables 'pedantically'
    (void)sec;
    (void)val;
    (void)valhw;
    (void)relmode;
    (void)hwnd;

    // check command
    if (command != command_id)
        return false;

    ReaKeyfinder();

    return true;
}

// when my plugin gets loaded
// function to register my plugins 'stuff' with REAPER
void Register()
{
    // register action name and get command_id
    command_id = plugin_register("custom_action", &action);

    // register run action/command
    plugin_register("hookcommand2", (void*)OnAction);

}

// shutdown, time to exit
// modern C++11 syntax
auto Unregister() -> void
{
    plugin_register("-custom_action", &action);
    plugin_register("-hookcommand2", (void*)OnAction);
}

} // namespace PROJECT_NAME
