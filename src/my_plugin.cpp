#include "my_plugin.h"
#include "keyfinder/keyfinder.h"
#include <keyfinder/audiodata.h>
#include "reaper_vararg.hpp"
#include "reakeyfinder_dbgtools.h"

#include <gsl/gsl>

#define STRINGIZE_DEF(x) #x
#define STRINGIZE(x) STRINGIZE_DEF(x)
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
// register main function on timer
// true or false
#define API_ID MYAPI
#define RUN_ON_TIMER false 

// confine my plugin to namespace
namespace PROJECT_NAME
{

// some global non-const variables
// the necessary 'evil'
int command_id{0};
bool toggle_action_state{false};
constexpr auto command_name = "AK5K_" STRINGIZE(PROJECT_NAME) "_COMMAND";
constexpr auto action_name = "ak5k: " STRINGIZE(PROJECT_NAME);
custom_action_register_t action = {0, command_name, action_name, nullptr};

// hInstance is declared in header file my_plugin.hpp
// defined here
REAPER_PLUGIN_HINSTANCE hInstance{nullptr}; // used for dialogs, if any

// the main function of my plugin
// gets called via callback or timer
void MainFunctionOfMyPlugin()
{

    // Access audio from a current item's take #0. Write it to a sample_buffer.
    MediaItem* current_item = GetSelectedMediaItem(0, 0);
    if (!(IsMediaItemSelected(current_item))) {
        ShowConsoleMsg("Айтем мне выбери блядь\n");
        return;
    }
    MediaItem_Take* current_take = GetMediaItemTake(current_item, 0);
    AudioAccessor* audio_accessor = CreateTakeAudioAccessor(current_take);
    double start_time = GetAudioAccessorStartTime(audio_accessor);
    double end_time = GetAudioAccessorEndTime(audio_accessor);
    int length = end_time - start_time;
    int sample_rate = 48000;
    int number_of_channels = 2;
    int sample_buffer_size = (length * sample_rate * number_of_channels)+1;
    double* sample_buffer = new double[sample_buffer_size];
    int samples_created = GetAudioAccessorSamples(audio_accessor, sample_rate, number_of_channels, 0.0, length * sample_rate, sample_buffer); // 0.0 - ???
    
    
    // Verify that samples are indeed created.
    if (samples_created == 1){
        // ShowConsoleMsg("Ну вот тебе буфер, карочи: ");
        // ShowConsoleMsg(StringizeSampleBuffer(sample_buffer, sample_buffer_size).c_str());
    } else if (samples_created == 0){
        ShowConsoleMsg("А где звук ёпта\n");
        return;
    } else if (samples_created == -1){
        ShowConsoleMsg("пиздец\n"); return;
    } else ShowConsoleMsg("уберпиздец\n"); return;

    
    // Prepare an AudioData object and move every sample into it.
    KeyFinder::AudioData a;
    a.setFrameRate(sample_rate);
    a.setChannels(number_of_channels);
    a.addToSampleCount(sample_buffer_size);

    for (int i = 0; i < sample_buffer_size; ++i) {
        a.setSample(i, sample_buffer[i]);
    }


    // Since samples are now set in keyfinder's AudioData object, we are destroying an AudioAccessor and sample_buffer
    DestroyAudioAccessor(audio_accessor);
    delete[] sample_buffer;


    // Start analyzing the samples.
    KeyFinder::KeyFinder k;
    KeyFinder::key_t key =  k.keyOfAudio(a);


    // Print out what key it found. Basically a copy-pasta from libkeyfinder example.
    // TODO: Make something more elaborated.
    ShowConsoleMsg("\nAnd your key is: ");
    switch(key) {
        case KeyFinder::A_MAJOR:
            ShowConsoleMsg("A major\n");
            break;
        case KeyFinder::A_MINOR:
            ShowConsoleMsg("A minor\n");
            break;
        case KeyFinder::B_FLAT_MAJOR:
            ShowConsoleMsg("B flat major\n");
            break;
        case KeyFinder::B_FLAT_MINOR:
            ShowConsoleMsg("B flat minor\n");
            break;
        case KeyFinder::B_MAJOR:
            ShowConsoleMsg("B major\n");
            break;
        case KeyFinder::B_MINOR:
            ShowConsoleMsg("B minor\n");
            break;
        case KeyFinder::C_MAJOR:
            ShowConsoleMsg("C major\n");
            break;
        case KeyFinder::C_MINOR:
            ShowConsoleMsg("C minor\n");
            break;
        case KeyFinder::D_FLAT_MAJOR:
            ShowConsoleMsg("D flat major\n");
            break;
        case KeyFinder::D_FLAT_MINOR:
            ShowConsoleMsg("D flat major\n");
            break;
        case KeyFinder::D_MAJOR:
            ShowConsoleMsg("D major\n");
            break;
        case KeyFinder::D_MINOR:
            ShowConsoleMsg("D minor\n");
            break;
        case KeyFinder::E_FLAT_MAJOR:
            ShowConsoleMsg("E flat major\n");
            break;
        case KeyFinder::E_FLAT_MINOR:
            ShowConsoleMsg("E flat minor\n");
            break;
        case KeyFinder::E_MAJOR:
            ShowConsoleMsg("E major\n");
            break;
        case KeyFinder::E_MINOR:
            ShowConsoleMsg("E minor\n");
            break;
        case KeyFinder::F_MAJOR:
            ShowConsoleMsg("G major\n");
            break;
        case KeyFinder::F_MINOR:
            ShowConsoleMsg("G minor\n");
            break;
        case KeyFinder::G_FLAT_MAJOR:
            ShowConsoleMsg("G flat major\n");
            break;
        case KeyFinder::G_FLAT_MINOR:
            ShowConsoleMsg("G flat minor\n");
            break;
        case KeyFinder::G_MAJOR:
            ShowConsoleMsg("G major\n");
            break;
        case KeyFinder::G_MINOR:
            ShowConsoleMsg("G minor\n");
            break;
        case KeyFinder::A_FLAT_MAJOR:
            ShowConsoleMsg("A flat major\n");
            break;
        case KeyFinder::A_FLAT_MINOR:
            ShowConsoleMsg("A flat minor\n");
            break;
        case KeyFinder::SILENCE:
            ShowConsoleMsg("Silence\n");
            break;
    }

    //ShowConsoleMsg("Hello blyat\n");



    
}

// c++11 trailing return type syntax
// REAPER calls this to check my plugin toggle state
auto ToggleActionCallback(int command) -> int
{
    if (command != command_id)
    {
        // not quite our command_id
        return -1;
    }
    if (toggle_action_state) // if toggle_action_state == true
        return 1;
    return 0;
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

    // depending on RUN_ON_TIMER #definition,
    // register my plugins main function to timer
    if (RUN_ON_TIMER) // RUN_ON_TIMER is true or false
    {
        // flip state on/off
        toggle_action_state = !toggle_action_state;

        if (toggle_action_state) // if toggle_action_state == true
        {
            // "reaper.defer(main)"
            plugin_register("timer", (void*)MainFunctionOfMyPlugin);
        }
        else
        {
            // "reaper.atexit(shutdown)"
            plugin_register("-timer", (void*)MainFunctionOfMyPlugin);
            // shutdown stuff
        }
    }
    else
    {
        // else call main function once
        MainFunctionOfMyPlugin();
    }

    return true;
}

// definition string for example API function
auto reascript_api_function_example_defstring =
    "int" // return type
    "\0"  // delimiter ('separator')
    // input parameter types
    "int,bool,double,const char*,int,const int*,double*,char*,int"
    "\0"
    // input parameter names
    "whole_number,boolean_value,decimal_number,string_of_text,"
    "string_of_text_sz,input_parameterInOptional,"
    "return_valueOutOptional,"
    "return_stringOutOptional,return_stringOutsz"
    "\0"
    "help text for myfunction\n"
    "If optional input parameter is provided, produces optional return "
    "value.\n"
    "If boolean is true, copies input string to optional output string.\n";

// example api function
int ReaScriptAPIFunctionExample(
    int whole_number,
    bool boolean_value,
    double decimal_number,
    const char* string_of_text,
    int string_of_text_sz,
    const int* input_parameterInOptional,
    double* return_valueOutOptional,
    char* return_stringOutOptional,
    int return_string_sz
)
{
    // if optional integer is provided
    if (input_parameterInOptional != nullptr)
    {
        // assign value to deferenced output pointer
        *return_valueOutOptional =
            // by making this awesome calculation
            (*input_parameterInOptional + whole_number + decimal_number);
    }

    // lets conditionally produce optional output string
    if (boolean_value)
    {
        // copy string_of_text to return_stringOutOptional
        // *_sz is length/size of zero terminated string (C-style char array)
        memcpy(return_stringOutOptional, string_of_text, min(return_string_sz, string_of_text_sz) * sizeof(char));
    }
    return whole_number * whole_number;
}

auto defstring_GetVersion =
    "void" // return type
    "\0"   // delimiter ('separator')
    // input parameter types
    "int*,int*,int*,int*,char*,int"
    "\0"
    // input parameter names
    "majorOut,minorOut,patchOut,tweakOut,commitOut,commitOut_sz"
    "\0"
    "returns version numbers of my plugin\n";

void GetVersion(int* majorOut, int* minorOut, int* patchOut, int* tweakOut, char* commitOut, int commitOut_sz)
{
    *majorOut = PROJECT_VERSION_MAJOR;
    *minorOut = PROJECT_VERSION_MINOR;
    *patchOut = PROJECT_VERSION_PATCH;
    *tweakOut = PROJECT_VERSION_TWEAK;
    const char* commit = STRINGIZE(PROJECT_VERSION_COMMIT);
    std::copy(commit, commit + min(commitOut_sz - 1, (int)strlen(commit)), commitOut);
    commitOut[min(commitOut_sz - 1, (int)strlen(commit))] = '\0'; // Ensure null termination
}

// when my plugin gets loaded
// function to register my plugins 'stuff' with REAPER
void Register()
{
    // register action name and get command_id
    command_id = plugin_register("custom_action", &action);

    // register action on/off state and callback function
    if (RUN_ON_TIMER)
        plugin_register("toggleaction", (void*)ToggleActionCallback);

    // register run action/command
    plugin_register("hookcommand2", (void*)OnAction);

    // register the API function example
    // function, definition string and function 'signature'
    plugin_register("API_" STRINGIZE(API_ID)"_ReaScriptAPIFunctionExample", (void*)ReaScriptAPIFunctionExample);
    plugin_register(
        "APIdef_" STRINGIZE(API_ID)"_ReaScriptAPIFunctionExample", (void*)reascript_api_function_example_defstring
    );
    plugin_register("APIvararg_" STRINGIZE(API_ID)"_ReaScriptAPIFunctionExample", (void*)&InvokeReaScriptAPI<&ReaScriptAPIFunctionExample>);

    plugin_register("API_" STRINGIZE(API_ID)"_GetVersion", (void*)GetVersion);
    plugin_register("APIdef_" STRINGIZE(API_ID)"_GetVersion", (void*)defstring_GetVersion);
    plugin_register("APIvararg_" STRINGIZE(API_ID)"_GetVersion", (void*)&InvokeReaScriptAPI<&GetVersion>);
}

// shutdown, time to exit
// modern C++11 syntax
auto Unregister() -> void
{
    plugin_register("-custom_action", &action);
    plugin_register("-toggleaction", (void*)ToggleActionCallback);
    plugin_register("-hookcommand2", (void*)OnAction);
}

} // namespace PROJECT_NAME
