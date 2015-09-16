#include "stdafx.h"
#include <tcode/io/ip/tcp/pipeline/acceptor.hpp>
#include <tcode/io/ip/tcp/pipeline/pipeline.hpp>
#include <tcode/io/ip/tcp/pipeline/channel.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    pipeline_acceptor::pipeline_acceptor( io::engine& e )
        : _acceptor( e ) , _fd( e )
    {
    }

    pipeline_acceptor::~pipeline_acceptor( void )
    {
    }

	bool pipeline_acceptor::listen( const tcode::io::ip::address& bind_addr ){
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
        _fd = std::move( tcp::socket( engine() )); 
        _acceptor.accept( _fd , [this]( const std::error_code& ec 
                , const tcode::io::ip::address& addr )
            {
                handle_accept( ec , addr );
            });
    }

	void pipeline_acceptor::handle_accept( const std::error_code& ec
            , const tcode::io::ip::address& addr )
    {
        if ( ec ) {
            on_error( ec );
        } else {
            if ( on_condition( addr )){
                tcode::io::ip::tcp::channel* channel 
                    = new tcode::io::ip::tcp::channel( std::move( _fd ));
                if ( build( channel->pipeline() ) ) {
                    channel->fire_on_open(addr );
                } else {
                    delete channel;
                    on_error( tcode::error_pipeline_build_fail );
                }
            } else {
                _fd.close(); 
            }
        }
        do_accept();
    }

    bool pipeline_acceptor::on_condition( const tcode::io::ip::address& addr ){
        return true; 
    }

	void pipeline_acceptor::on_error( const std::error_code& ec ){
    }

}}}}
