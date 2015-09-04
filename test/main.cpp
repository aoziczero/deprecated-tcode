#include "stdafx.h"
#include <tcode/tcode.hpp>

#if defined( TCODE_WIN32 )

#pragma comment( lib , "ws2_32.lib")
#pragma comment( lib , "mswsock.lib")
#pragma comment( lib , "tcode.lib")
#pragma comment( lib , "gtestd-mtdd" )

#endif

int main( int argc , char* argv[])
{
#if defined( TCODE_WIN32 )
	WSAData ws;
	WSAStartup( MAKEWORD( 2,2 ) , &ws );
#endif
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
