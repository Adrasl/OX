#ifndef _COREEXPORT_
#define _COREEXPORT_

#include <stdlib.h>

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
    #  if defined( CORE_EXPORTS )
    #    define COREEXPORT __declspec(dllexport)
    #  else
    #    define COREEXPORT __declspec(dllimport)
    #  endif
#else
    #  define COREEXPORT
#endif


#ifdef _DEBUG
	//#define _CRTDBG_MAP_ALLOC
	//#define _CRTDBG_MAP_ALLOC_NEW
	#include <stdlib.h>
	//#include <crtdbg.h>
#endif

#endif
