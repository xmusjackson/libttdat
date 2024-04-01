<# 
.SYNOPSIS
    Builds the library and a test executable for windows.

.DESCRIPTION 
    A simple build script which uses cmake to generate MSVC project files to build the library.
 
.NOTES 
    Requires VS2022, C++ Desktop Developement package, and CMake (from VS Installer)

.LINK 
    http://github.com/xmusjackson/libttdat.git
#>

Remove-Item -Force -Recurse -Path build
mkdir build
cd build
& 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe' ..
MSBuild.exe ALL_BUILD.vcxproj
cd ..