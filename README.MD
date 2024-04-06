# libttdat

## A WIP ttgames dat file extractor/patcher library written in c++.

### Current State

This is in it's early developmental stages and I don't have a lot of time to devote, but I intend for this to be a fully functional dat file extractor (and eventually patcher) for ttgames LEGO DAT files.

This is being written with inspiration from the ttgames quickbms script located at the quickbms homepage: 
https://aluigi.altervista.org/quickbms.htm

After initial implementation, this library should provide similar functionality with improved performance. Eventually I wish to include further functionality, such as repacking, adjusting offsets, and potentially even creating dat files from scratch. Currently this library does essentially nothing but the first few steps of identifying offsets. More functionality should appear in the coming weeks.

This is being written in C++20 on a linux machine with portability in mind. This will be ported to macOS once initial unpacking functionality is completed.

### Building

#### Building for Linux
 
##### Requirements

 1. Modern g++ or clang++ and llvm
 2. Linux System
 3. Development packages for your system, including make and cmake

##### Building

Run the following in a terminal once you have met the requirements above:

    git clone https://github.com/xmusjackson/libttdat.git
    cd libttdat
    ./build-linux.sh

There is no install target in the CMakeLists yet as this doesn't really do much.

#### Building for Windows
 
##### Requirements

 1. Visual Studio 2022
 2. Windows System
 3. C++ Desktop Development package and CMake from Visual Studio Installer

##### Building

Run the following in an MSVC Developer Powershell once you have met the requirements above:

    git clone https://github.com/xmusjackson/libttdat.git
    cd libttdat
    .\build-windows.ps1

There is no install target in the CMakeLists yet as this doesn't really do much.


### Testing

Not much testing to do yet. The library gathers offsets and other important data from the DAT and builds a file list. The test program outputs the information to stdout. 
To test anyway:

After building this project:

Windows:

    .\build\Debug\libttdat_test.exe C:\path\to\GAME.DAT

Linux:

    build/libttdat_test /path/to/GAME.DAT

* Replace the filepath with an actual path to a GAME.DAT.

Example output: 

    $ LD_LIBRARY_PATH=build/lib build/libttdat_test /home/christian/steamdir/steamapps/common/The\ LEGO\ Movie\ -\ Videogame/GAME.DAT 
      Info Offset: 12
      New Info Offset: 0
      Info Size: 751320
      File Count: 12065
      Info Type: -5
      New Format?: False
      Has HDR?: True
      Is Compressed?: False
      File Name Count: 14280
      File Names Offset: 364416
      File Names Size: 338636
      CRCs Offset: 703056
      Name Info Offset: 193056


### Documentation

There is none - yet. I will get to this a little after intial unpacking is implemented.

### Thanks

The developer of Quickbms (and the author of the ttgames script)
http://www.aluigi.altervista.org/quickbms.htm
