#ifndef _IGUIEXPORT_
#define _IGUIEXPORT_


#if defined(_MSC_VER)
    #pragma warning( disable : 4244 )
    #pragma warning( disable : 4251 )
    #pragma warning( disable : 4267 )
    #pragma warning( disable : 4275 )
    #pragma warning( disable : 4290 )
    #pragma warning( disable : 4786 )
    #pragma warning( disable : 4305 )
    #pragma warning( disable : 4996 )
#endif


#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
    #  if defined( IGUI_STATIC )
    #    define IGUIEXPORT
    #  elif defined( IGUI_EXPORTS )
    #    define IGUIEXPORT  __declspec(dllexport)
    #  else
    #    define IGUIEXPORT  __declspec(dllimport)
    #  endif
#else
    #  define IGUIEXPORT
#endif

#endif