#ifndef WINDOWS_DLL_EXPORTS
    #define WINDOWS_DLL_EXPORTS
    #ifdef WIN32
        #ifdef LIBTTDAT_EXPORTS
            #define LIBTTDAT_API __declspec(dllexport)
        #else
            #define LIBTTDAT_API __declspec(dllimport)
        #endif
    #else 
        #define LIBTTDAT_API
    #endif
#endif