////#ifdef _MSVC #include "stdafx.h" #endif
#define _WINSOCKAPI_

#include "Application.h"

IMPLEMENT_MIIAPP(Application)

int main(int argc, char *argv[ ])
{
	return wxEntry(argc,argv); 

	_CrtDumpMemoryLeaks();
}