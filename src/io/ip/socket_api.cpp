#include "stdafx.h"
#include <tcode/io/ip/socket_api.hpp>

namespace tcode { namespace io { namespace ip {


    tcp_handle::tcp_handle( void )
    {
        handle( invalid_socket );
    }

    tcp_handle::~tcp_handle( void )
    {
        handle( invalid_socket );
    }

    bool tcp_handle::open( int addressfamily ){
#if defined( TCODE_WIN32 )
        handle(WSASocket( addressfamily ,SOCK_STREAM , IPPROTO_TCP , 
                    nullptr , 0 , WSA_FLAG_OVERLAPPED ));
#else
        handle(::socket( addressfamily ,SOCK_STREAM , IPPROTO_TCP));
#endif
        return good();
    }

    void tcp_handle::close( void ) {
        if ( good() ) {
            socket_type h = handle( invalid_socket );
            if ( h != invalid_socket ) {
#if defined( TCODE_WIN32 )
                closesocket( h );
#else
                ::close( h );   
#endif
            }
        }
    }

    bool tcp_handle::shutdown( const int v ) {
        return ::shutdown( handle() , v ) != -1;
    }

    bool tcp_handle::good( void ) {
        return handle() != invalid_socket;
    }

    socket_type tcp_handle::handle( void ) {
        return _handle;
    }

    socket_type tcp_handle::handle( socket_type fd ){
        socket_type old = _handle;
        _handle = fd;
        return old;
    }

    udp_handle::udp_handle( void )
    {
        handle( invalid_socket );
    }

    udp_handle::~udp_handle( void )
    {
        handle( invalid_socket );
    }

    bool udp_handle::open( int addressfamily ){
#ifdef TCODE_TARGET_WINDOWS
        handle(WSASocket( addressfamily ,SOCK_DGRAM , IPPROTO_UDP , 
                    nullptr , 0 , WSA_FLAG_OVERLAPPED ));
#else
        handle(::socket( addressfamily ,SOCK_DGRAM , IPPROTO_UDP));
#endif
        return good();
    }

    void udp_handle::close( void ) {
        if ( good() ) {
            socket_type h = handle( invalid_socket );
#ifdef TCODE_TARGET_WINDOWS
            closesocket( h );
#else
            ::close( h );   
#endif
        }
    }

    bool udp_handle::good( void ) {
        return handle() != invalid_socket;
    }

    socket_type udp_handle::handle( void ) {
        return _handle;
    }

    socket_type udp_handle::handle( socket_type fd ){
        socket_type old = _handle;
        _handle = fd;
        return old;
    }



}}}
