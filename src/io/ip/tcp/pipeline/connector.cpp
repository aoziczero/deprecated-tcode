#include "stdafx.h"
#include <tcode/io/ip/tcp/pipeline/connector.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

connector::connector(void){

}

connector::~connector(void){

}

void connector::connect( const tcode::io::ip::address& addr
		, const tcode::timespan& wait_time 
		, const std::shared_ptr< connector_handler >& handler )
{
	std::vector< tcode::io::ip::address > addrs;
	addrs.push_back(addr);
	connect_sequence( addrs , wait_time , handler );
}

void connector::connect_sequence( const std::vector< tcode::io::ip::address >& addr 
		, const tcode::timespan& per_wait_time 
		, const std::shared_ptr< connector_handler >& handler )
	
{
    _handler = handler;
	_handler->timeout(per_wait_time);
	_handler->do_connect( addr );
}

}}}}
