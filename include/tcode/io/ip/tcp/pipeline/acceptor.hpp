#ifndef __tcode_transport_tcp_channel_acceptor_h__
#define __tcode_transport_tcp_channel_acceptor_h__

#include <tcode/io/ip/tcp/acceptor.hpp>
#include <tcode/io/ip/tcp/pipeline/pipeline_builder.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

class pipeline_acceptor
    : public pipeline_builder
{
public:
	pipeline_acceptor( io::engine& e );
	virtual ~pipeline_acceptor(void);		

	bool listen( const tcode::io::ip::address& bind_addr );
    bool listen( int port );

	void close( void );

	void do_accept(void);
	void handle_accept( const std::error_code& ec , tcode::io::ip::address& addr );
    
    virtual bool on_condition( const tcode::io::ip::address& addr );
	virtual void on_error( const tcode::diagnostics::error_code& ec );
private:
    tcp::acceptor _acceptor;
    tcp::socket _fd;
};

}}}}

#endif
