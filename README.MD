# libttdat

## A WIP ttgames dat file extractor/patcher library written in c++ with portability in mind.

### Current State

This is in it's early developmental stages and I don't have a lot of time to devote, but I intend for this to be a fully functional dat file extractor (and eventually patcher) for ttgames LEGO DAT files.

This is being written with inspiration from the ttgames quickbms script located at the quickbms homepage: 
https://aluigi.altervista.org/quickbms.htm

After initial implementation, this library should provide similar functionality with improved performance. Eventually I wish to include further functionality, such as repacking, adjusting offsets, and potentially even creating dat files from scratch. Currently this library does essentially nothing but the first few steps of identifying offsets. More functionality should appear in the coming weeks.

This is being written in C++23 on a linux machine with portability in mind. This will be ported to at least Windows and macOS once initial unpacking functionality is completed.

### Building for Linux
 
#### Requirements

 1. Modern g++ or clang++ and llvm
 2. Linux System
 3. Developement packages for your system (make etc...)

#### Building

Run the following in a terminal once you have met the requriements above:

    git clone https://github.com/xmusjackson/libttdat.git
    cd libttdat
    make

There is no install target in the makefile yet as this doesn't really do much.

### Testing

Not much testing to do yet. The test program currently outputs some information about the dat file given as an argument. To test anyway:

After building this project:

    $ LD_LIBRARY_PATH=build/lib:$LD_LIBRARY_PATH build/bin/ttdat_test /home/user/directory/GAME.DAT

Replace the filepath with an actual path to a GAME.DAT.

### Documentation

There is none - yet. I will get to this a little after intial unpacking is implemented.

### Thanks

The developer of Quickbms (and the author of the ttgames script)
http://www.aluigi.altervista.org/quickbms.htm
