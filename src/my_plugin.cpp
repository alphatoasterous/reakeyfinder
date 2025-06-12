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

#define API_ID REAKEYFINDER_API
#define SCRIPT_ID "_RS15ad79b6e0c8e720c9d6353d14bbf0bd77753369"
#define NOECHO "!SHOW:"
#define STRINGIZE_DEF(x) #x
#define STRINGIZE(x) STRINGIZE_DEF(x)
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

const char* reakeyfinder_version = STRINGIZE(REAKEYFINDER_VERSION);
const char* reakeyfinder_buildtype = STRINGIZE(PROJECT_BUILD_TYPE);

#if PROJECT_BUILD_TYPE == Debug
#include <chrono>
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;
#endif

// confine my plugin to namespace
namespace PROJECT_NAME
{

// some global non-const variables
// the necessary 'evil'
int command_id{0};
int script_id{0};
custom_action_register_t analyze_action = {
    0,
    STRINGIZE(PROJECT_NAME) "_COMMAND",
    STRINGIZE(PROJECT_NAME) ": " "Analyze key/BPM of selected media item",
    nullptr};

enum Commands {
    RKF_ANALYZE,
    RKF_FORCE_REANALYZE
};

// hInstance is declared in header file my_plugin.hpp
// defined here
REAPER_PLUGIN_HINSTANCE hInstance{nullptr}; // used for dialogs, if any

// the main function of my plugin
// gets called via callback or timer
void ReaKeyfinder()
{
    
    #if PROJECT_BUILD_TYPE == Debug
    TimePoint seq_start = Clock::now();
    #endif
    
    // Access audio from a current item's take #0. Write it to a sample_buffer.
    MediaItem* current_item = GetSelectedMediaItem(0, 0);
    if (!(IsMediaItemSelected(current_item))) {
        ShowConsoleMsg(NOECHO"No item selected. Please select an item to analyze!\n");
        return;
    }
    MediaItem_Take* current_take = GetMediaItemTake(current_item, 0);
    
    // Get project bpm
    //double project_bpm = 0.0f;
    //GetProjectTimeSignature2(GetItemProjectContext(current_item), &project_bpm, nullptr);

    double bpm;

    double take_pitch = GetMediaItemTakeInfo_Value(current_take, "D_PITCH");
    double take_playrate = GetMediaItemTakeInfo_Value(current_take, "D_PLAYRATE");
    
        bool json_parse_error = false;
    bool json_correct = false; // preemptively assuming that JSON is not correct 
    nlohmann::json js;
    char* item_metadata = new char[1024];
    // Check if JSON exists in item metadata
    if (GetSetMediaItemTakeInfo_String(current_take, "P_EXT:reakeyfinder", item_metadata, false))
    {
        if (isStringNotEmpty(item_metadata)) {
            //std::string unwrapped_item_metadata = unwrapLuaBracketString(item_metadata);
            try {
                js = nlohmann::json::parse(item_metadata);
            } catch (const nlohmann::json::parse_error&) {
                // Parsing failed
                json_parse_error = true;
                ShowConsoleMsg(NOECHO"JSON parsing failed.\n");
                json_correct = false;
            }
        }
    }
    // Free allocated stuff
    delete[] item_metadata;

    // JSON validation: logical
    if (json_parse_error == false ) {
        if (js.contains("bpm") && 
            js.contains("key") &&
            js.contains("key_relative") &&
            js.contains("key_quart") &&
            js.contains("key_quint") && 
            js.contains("take_info") &&
            js.contains("rkf_ver")
        ) {
            if ((js["take_info"]["name"] == GetTakeName(current_take)) &&
            (js["take_info"]["playrate"] == take_playrate) &&
            (js["take_info"]["pitch"] == take_pitch)
            // (js["rkf_ver"] == reakeyfinder_version) can be checked in future, if algorithm changes without changing object's structure
            ) json_correct = true; else {
                ShowConsoleMsg(NOECHO"Take metadata mismatch. Reanalyzing!\n");
                json_correct = false;
            }
        } else {
            ShowConsoleMsg(NOECHO"JSON Structure invalid. Reanalyzing!\n");
            json_correct = false;
        }

    }
    if (json_correct == true) {
        std::string take_name = js["take_info"]["name"];
        bpm = js["bpm"];
        std::string key_string = js["key"];
        std::string key_relative_string = js["key_relative"];
        std::string key_quart_string = js["key_quart"];
        std::string key_quint_string = js["key_quint"];

       ShowConsoleMsg((NOECHO + BuildInfoStringWithKeyStringed(
                                                GetTakeName(current_take), 
                                                        key_string.c_str(), 
                                                key_relative_string.c_str(), 
                                                key_quart_string.c_str(), 
                                                key_quint_string.c_str(), 
                                                            bpm, 
                                                            3)).c_str());

    } else {
        AudioAccessor* audio_accessor = CreateTakeAudioAccessor(current_take);
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
            // Samples are indeed created.
        } else if (samples_created == 0){
            ShowConsoleMsg(NOECHO"Could not find any sound. Are you messing with me?\n");
            return;
        } else if (samples_created == -1){
            ShowConsoleMsg(NOECHO"Sound extraction failed.\n"); return;
        } else { ShowConsoleMsg(NOECHO"Sound extraction failed *miserably*.\n"); return;}

        // Get key
        KeyFinder::key_t key;
        KeyFinder::CompatibleKeys compatible_keys;
        std::thread key_thread([&]() {
           key = GetKeyOfAudio(PrepareAudioData(samples, samples_size, sample_rate, numchannels));
           compatible_keys = GetCompatibleKeys(key);
        });
        
        
        // Get bpm
        std::thread bpm_thread([&]() {
            bpm = breakfastquay::MiniBPM(sample_rate).estimateTempoOfSamples(samples, samples_size);
        });

        bpm_thread.join();
        key_thread.join();

        
        delete[] samples;
        DestroyAudioAccessor(audio_accessor);

        // Save analysis reports and necessary metadata into JSON
        js = {
            {"bpm", bpm},
            {"key", std::string(GetKeyString(key)) + " (" + std::string(KeyFinder::GetCamelotString(key))+ ")"},
            {"key_relative", std::string(GetKeyString(compatible_keys.relative)) + " (" + std::string(KeyFinder::GetCamelotString(compatible_keys.relative))+ ")"},
            {"key_quart", std::string(GetKeyString(compatible_keys.quart)) + " (" + std::string(KeyFinder::GetCamelotString(compatible_keys.quart))+ ")"},
            {"key_quint", std::string(GetKeyString(compatible_keys.quint)) + " (" + std::string(KeyFinder::GetCamelotString(compatible_keys.quint)) + ")"},

            {"take_info", 
                { 
                    {"name",GetTakeName(current_take)},
                    {"playrate", take_playrate},
                    {"pitch", take_pitch }
            },},
            {"rkf_ver", reakeyfinder_version }
        };

        // Show console output
        ShowConsoleMsg((NOECHO +BuildInfoString(GetTakeName(current_take), key, bpm, 3)).c_str());
        
        // Dump JSON in a Media item
        std::string dumped = js.dump();
        #if PROJECT_BUILD_TYPE == Debug
        ShowConsoleMsg((NOECHO + dumped).c_str());
        #endif
        char* cstr = new char[dumped.size() + 1];
        std::strcpy(cstr, dumped.c_str());
        if (GetSetMediaItemTakeInfo_String(current_take, "P_EXT:reakeyfinder", cstr, true)) {
            ShowConsoleMsg(NOECHO"\nAnalyzed data is cached into the item parameters.");
        }
        delete[] cstr;
    }
    // Get elapsed time
    #if PROJECT_BUILD_TYPE == Debug
    TimePoint seq_end = Clock::now();
    double seq_time = std::chrono::duration<double, std::milli>(seq_end - seq_start).count();
    ShowConsoleMsg(NOECHO"\nElapsed time (ms): ");
    ShowConsoleMsg((NOECHO + PROJECT_NAME::FloatToString(seq_time)).c_str());
    #endif

    script_id = NamedCommandLookup(SCRIPT_ID);
    if (script_id > 0) {
        Main_OnCommandEx(script_id,0,0);
    } else {
        ShowConsoleMsg("\n\n");
    }

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

    if (command != command_id)
        return false;

    ReaKeyfinder();

    return true;
}

// definition string for example API function
auto reakeyfinder_registered_defstring =
    "bool" // return type
    "\0"  // delimiter ('separator')
    // input parameter types
    "\0"
    // input parameter names
    "\0"
    "Internal function to verify that ReaKeyfinder is indeed registered.\n"
    "Always returns true.";

bool ReaKeyfinderRegistered() {
    return true;
}

// when my plugin gets loaded
// function to register my plugins 'stuff' with REAPER
void Register()
{
    // register action name and get command_id
    command_id = plugin_register("custom_action", &analyze_action);

    // register run action/command
    plugin_register("hookcommand", (void*)OnAction);
    plugin_register("hookcommand2", (void*)OnAction);

    // register API entry
    plugin_register("API_" STRINGIZE(API_ID)"_ReaKeyfinderRegistered", (void*)ReaKeyfinderRegistered);
    plugin_register(
        "APIdef_" STRINGIZE(API_ID)"_ReaKeyfinderRegistered", (void*)reakeyfinder_registered_defstring
    );
    plugin_register("APIvararg_" STRINGIZE(API_ID)"_ReaKeyfinderRegistered", (void*)&InvokeReaScriptAPI<&ReaKeyfinderRegistered>);
}

// shutdown, time to exit
// modern C++11 syntax
auto Unregister() -> void
{
    plugin_register("-custom_action", &analyze_action);
    plugin_register("-hookcommand", (void*)OnAction);
}

} // namespace PROJECT_NAME
