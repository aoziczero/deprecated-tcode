#ifndef __tcode_io_ip_tcp_pipeline_connector_h__
#define __tcode_io_ip_tcp_pipeline_connector_h__

#include <tcode/io/engine.hpp>
#include <tcode/io/ip/tcp/socket.hpp>
#include <tcode/io/ip/tcp/pipeline/pipeline_builder.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    class pipeline_connector 
        : public pipeline_builder
    {
    public:
        pipeline_connector( io::engine& e );
        virtual ~pipeline_connector( void );

        void connect( const tcode::io::ip::address& addr
		    , const tcode::timespan& wait_time );

        void connect_sequence( const std::vector< tcode::io::ip::address >& addr 
	    	, const tcode::timespan& per_wait_time );
	
        virtual void on_error( const std::error_code& ec
                , const tcode::io::ip::address& addr ) = 0;
        virtual bool do_continue( const tcode::io::ip::address& addr ) = 0;
        virtual io::engine& engine( void );

        tcode::timespan timeout( void );
        void timeout( const tcode::timespan& timeout );

        void do_connect(const std::vector< tcode::io::ip::address >& addrs );
        void do_connect();
        void handle_connect( const std::error_code& ec  );

        void on_timer( const std::error_code& ec );
    private:
        io::engine& _engine;
        tcode::io::ip::tcp::socket _fd;
        std::shared_ptr< pipeline_builder > _builder;
        std::vector< tcode::io::ip::address > _address;
        int _current_address_index;
        bool _connect_in_progress;
        tcode::io::timer _timer;
        tcode::timespan _timeout;
        tcode::timestamp _connect_time;
    };

}}}}

#endif
