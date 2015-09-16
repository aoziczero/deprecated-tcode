#include "stdafx.h"
#include <tcode/io/ip/tcp/pipeline/acceptor.hpp>
#include <tcode/io/ip/tcp/pipeline/pipeline.hpp>
#include <tcode/io/ip/tcp/pipeline/channel.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    pipeline_acceptor::pipeline_acceptor( io::engine& e )
        : _acceptor( e ) 
        , _fd( e )
    {
    }

	bool pipeline_acceptor::listen( const tcode::io::ip::address& bind_addr );
        return _acceptor.listen( bind_addr ); 
    }
    
    bool pipeline_acceptor::listen( int port ){
        return _acceptor.listen( tcode::io::ip::address(
                    tcode::io::ip::address::any( port )));
    }

    void pipeline_acceptor::close( void ){
        _acceptor.close();
    }

    void pipeline_acceptor::do_accept( void ){
        if ( _descriptor == nullptr )
            return;
        _fd = std::move( tcp::socket( engine() )); 
        _acceptor.acceptor( _fd , [this]( const std::error_code& ec 
                , const tcode::io::ip::address& addr )
            {
                handle_accept( ec , addr );
            });
    }

	void pipeline_acceptor::handle_accept( const std::error_code& ec
            , tcode::io::ip::address& addr );
    {
        if ( on_condition( addr )){
            /*
                tcode::transport::tcp::channel* channel 
                        = new tcode::transport::tcp::channel( 
                                _handler->channel_loop() ,tcp_handle.handle());
                if ( _handler->build( channel->pipeline() ) ) {
                    channel->fire_on_open( handler->address_ptr()[1] );
                } else {
                    tcp_handle.close();
                    delete channel;
                }
                */
        } else {
            _fd.close(); 
        }
        do_accept();
    }

}}}}
