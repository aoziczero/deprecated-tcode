#include "stdafx.h"
#include <tcode/io/ip/tcp/pipeline/connector.hpp>
#include <tcode/io/ip/tcp/pipeline/channel.hpp>

#include <tcode/time/timestamp.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    pipeline_connector::pipeline_connector( io::engine& e )
        : _engine( e )
          , _current_address_index(0)
          , _timeout( tcode::timespan::minutes(1))
          , _connect_in_progress(false)
          , _fd(engine())
          , _timer(engine())
    {

    }

    pipeline_connector::~pipeline_connector( void ){

    }

    void pipeline_connector::connect( const tcode::io::ip::address& addr
            , const tcode::timespan& wait_time  )
    {
        std::vector< tcode::io::ip::address > addrs;
        addrs.push_back(addr);
        connect_sequence( addrs , wait_time );
    }

    void pipeline_connector::connect_sequence( const std::vector< tcode::io::ip::address >& addr 
            , const tcode::timespan& per_wait_time )
    {
        _timeout = per_wait_time;
        do_connect( addr );
    }

    io::engine& pipeline_connector::engine( void ) {
        return _engine;
    }

    tcode::timespan pipeline_connector::timeout( void ){
        return _timeout;
    }

    void pipeline_connector::timeout( const tcode::timespan& timeout ){
        _timeout = timeout;
    }

    void pipeline_connector::do_connect(const std::vector< tcode::io::ip::address >& addrs ){	
        _address = addrs;
        _current_address_index = 0;
        _timer.due_time( _timeout )
            .repeat( _timeout )
            .callback( [this]( const std::error_code& ec ) {
                    on_timer(ec);
                    });
        do_connect();
        _timer.fire();
    }

    void pipeline_connector::do_connect(){
        tcode::io::ip::address& addr = _address[_current_address_index];	
        _connect_in_progress = true;
        _connect_time = tcode::timestamp::now();
        _fd.connect( addr , [this]( const std::error_code& ec 
                    , const tcode::io::ip::address& addr )
                {
                handle_connect(ec);          
                });
    }

    void pipeline_connector::handle_connect( const std::error_code& ec  ){
        _connect_in_progress = false;
        std::error_code er = ec;
        if ( !er ) {
            tcode::io::ip::tcp::channel* channel 
                = new tcode::io::ip::tcp::channel( std::move( _fd ));
            if ( build( channel->pipeline() ) ) {
                channel->fire_on_open( _address[_current_address_index]);
            } else {
                delete channel;
            }          
            er = tcode::error_pipeline_build_fail;
        }
        _fd.close();
        on_error(ec , _address[_current_address_index] );
        _current_address_index = ( _current_address_index + 1 ) % _address.size();
        if (do_continue(_address[_current_address_index])){		
            do_connect();
        } else {
            _timer.cancel();
        }
    }

    void pipeline_connector::on_timer( const std::error_code& ec ){
        if( ec ) return;
        if ( _connect_in_progress ) {
            if ( tcode::timestamp::now() > _connect_time + _timeout ){
                _fd.close();
            }
        } 
    }

}}}}

