#ifndef __tcode_transport_udp_bootstrap_h__
#define __tcode_transport_udp_bootstrap_h__

#include <io/ip/address.hpp>
#include <transport/udp/channel.hpp>
#include <transport/udp/filter.hpp>
#include <transport/udp/pipeline.hpp>
#include <transport/udp/pipeline_builder.hpp>

namespace tcode { namespace transport { namespace udp {

class bootstrap{
public:
	static bool open_server( const tcode::io::ip::address& addr
			, tcode::transport::udp::pipeline_builder_ptr& builder );
	static bool open_client( int af 
			, tcode::transport::udp::pipeline_builder_ptr& builder );
private:
};

}}}

#endif