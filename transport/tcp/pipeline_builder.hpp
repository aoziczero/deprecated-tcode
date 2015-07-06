#ifndef __tcode_transport_tcp_pipeline_builder_h__
#define __tcode_transport_tcp_pipeline_builder_h__

#include <system_error>
#include <common/rc_ptr.hpp>
namespace tcode { namespace transport { 

class event_loop;

namespace tcp {

class pipeline;
class pipeline_builder 
	: public tcode::ref_counted< pipeline_builder >
	{
public:
	pipeline_builder( void ){}
	virtual ~pipeline_builder( void ){}
	virtual event_loop& channel_loop( void ) = 0;
	virtual bool build( pipeline& p ){
		return false;
	}
private:
};

typedef tcode::rc_ptr<pipeline_builder> pipeline_builder_ptr;

}}}

#endif
