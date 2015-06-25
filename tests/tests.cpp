// tests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#if defined ( TCODE_TARGET_WINDOWS )
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "gtestd-mtdd")
#pragma comment(lib, "tcode.common")


int _tmain(int argc, _TCHAR* argv[])
#else
int main( int argc , char* argv[])
#endif
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
	return 0;
}

