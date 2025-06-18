# ReaKeyFinder
A Reaper extension for key and bpm extraction of selected media item take

At this moment you can easily install this extension only on Linux because of libkeyfinder, so if you want a safe installation on Windows, you need to wait for some fixes for the Win build, sorry

If you don't care about this, follow the instructions for installation below.

## en
### Prerequisites
`Linux: CMake, Make, GCC or Clang, libfftw3`

`Windows: Windows SDK, MSVC, install fftw3 and libkeyfinder via vcpkg`

`*Reaper: ReaImgui via ReaPack for ImGui interface`
### installation
#### Linux:
Install dependencies and .so file in release and put this file in directory:

    ~/.config/REAPER/UserPlugin

#### Windows:
**Warning: Unsafe installation!!!!**

You need to download .zip archive from the Releases page and put keyfinder.dll and fftw3.dll in the Windows folder (or register them via regsvr, no instructions provided).

Next, extract UserPlugins folder into:
        
    %APPDATA%\REAPER

### How to use

Select any item that you want to analyze and then follow to `Actions->Show Action List->Search bar->Reakeyfinder` and click on the **Run** button.

### ImGui interface
If you want to use ImGui interface you need to install ReaImGui via ReaPack and follow `Actions->New Action->Show Action List->New Action` and select ReaKeyfinder.lua

Then you can use it via ReaKeyFinder script as before.
## ru
### Преустановка
`Linux: CMake, Make, GCC или Clang, libfftw3`

`Windows: Windows SDK, MSVC, install fftw3 и libkeyfinder через vcpkg`

`*Reaper: ReaImgui с помощю SWS для imgui интерфейса`
### Установка
#### Linux:
Установите зависимости и собранный файл в релизе и перекиньте его в директорию:

    ~/.config/REAPER/UserPlugin

#### Windows:
**Предупреждение: Небезопасная установка!!!!**

Вам нужно установить .zip архив из релиза, далее засунуть keyfinder.dll и fftw3.dll в папку "Windows"

И после этого вы можете перекинуть папку UserPlugins в
        
    AppData\Local\REAPER

### Как им пользоваться

Выделите любой айтем, который вы хотите проанализировать, затем перейдите в `Actions->Show Action List->Search bar->Reakeyfinder` и нажмите на кнопку **Run** 

### ImGui интерфейс
Если вы хотите использовать ImGui интерфейс, вам нужно установить ReaImGui с помощью ReaPack and перейдите в `Actions->New Action->Show Action List->New Action` и выделите ReaKeyfinder.lua

Затем вы сможете использовать его через ReaKeyFinder скрипт как и в прошлые разы

## Known issues
Framerate drops when moving an ImGui window
