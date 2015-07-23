#include "stdafx.h"
#include "pipeline.hpp"
#include "filter.hpp"
#include "channel.hpp"
#include <transport/event_loop.hpp>

namespace tcode { namespace transport { namespace tcp {

static const int FLAG_IN_PIPELINE = 0x01;

class in_pipeline_holder{
	int& flag;
public:
	in_pipeline_holder( int& f ) 
		: flag(f)
	{
		flag |= FLAG_IN_PIPELINE;
	}
	~in_pipeline_holder(void){
		flag ^= FLAG_IN_PIPELINE;
	}
};

pipeline::pipeline( void )
	: _flag(0)
	, _inbound( nullptr )
	, _channel(nullptr)
{
}

pipeline::pipeline( const pipeline& pl )
	: _flag( pl._flag ) , _inbound(pl._inbound) 
{
	
}

pipeline::~pipeline( void )
{

}
	
bool pipeline::in_pipeline( void ){
	return channel()->loop().in_event_loop() && (_flag & FLAG_IN_PIPELINE);
}

void pipeline::fire_filter_on_end_reference( void ){
	in_pipeline_holder ipl(_flag);
	filter* pinbound_filter = _inbound;
	while( pinbound_filter ) {
		filter* pfilter = pinbound_filter;
		pinbound_filter = pfilter->inbound();
		pfilter->filter_on_end_reference();
	}
}

void pipeline::fire_filter_on_open( const tcode::io::ip::address& addr ){
	in_pipeline_holder ipl(_flag);
	if ( _inbound )
		_inbound->filter_on_open( addr );
}

void pipeline::fire_filter_on_read( tcode::buffer::byte_buffer& buf ){
	in_pipeline_holder ipl(_flag);
	if ( _inbound )
		_inbound->filter_on_read( buf );
}

void pipeline::fire_filter_on_write( int written , bool flush ){
	in_pipeline_holder ipl(_flag);
	if ( _inbound )
		_inbound->filter_on_write( written , flush);
}

void pipeline::fire_filter_on_error( const tcode::diagnostics::error_code& ec ) {
	in_pipeline_holder ipl(_flag);
	if ( _inbound ) 
		_inbound->filter_on_error( ec );
}

void pipeline::fire_filter_on_close( void ) {
	in_pipeline_holder ipl(_flag);
	if ( _inbound ) 
		_inbound->filter_on_close();
}

void pipeline::fire_filter_on_open(  filter* pfilter , const tcode::io::ip::address& addr ){
	if( !pfilter )
		return;
	if ( in_pipeline() ){
		pfilter->filter_on_open( addr );
	} else {
		tcode::rc_ptr< tcp::channel > pchan( channel());
		channel()->loop().execute( [ pfilter , addr , pchan , this ] {
			in_pipeline_holder ipl(_flag);
			pfilter->filter_on_open( addr );
		});
	}	
}

void pipeline::fire_filter_on_close( filter* pfilter ){
	if ( !pfilter )
		return;
	if ( in_pipeline() ){
		pfilter->filter_on_close();
	} else {
		tcode::rc_ptr< tcp::channel > pchan( channel());
		channel()->loop().execute( [ pfilter , pchan , this ] {
			in_pipeline_holder ipl(_flag);
			pfilter->filter_on_close();
		});
	}	
}

void pipeline::fire_filter_on_read( filter* pfilter , tcode::buffer::byte_buffer& buf ){
	if ( !pfilter )
		return;

	if ( in_pipeline() ){
		pfilter->filter_on_read(buf);
	} else {
		tcode::rc_ptr< tcp::channel > pchan( channel());
		channel()->loop().execute( [ pfilter , buf , pchan , this ] {
			in_pipeline_holder ipl(_flag);
			tcode::buffer::byte_buffer nb( buf );
			pfilter->filter_on_read(nb);
		});
	}	
}

void pipeline::fire_filter_on_write( filter* pfilter , int written , bool flush ){
	if ( !pfilter )
		return;
	if ( in_pipeline() ){
		pfilter->filter_on_write(written,flush);
	} else {
		tcode::rc_ptr< tcp::channel > pchan( channel());
		channel()->loop().execute( [ pfilter,written,flush,pchan,this ] {
			in_pipeline_holder ipl(_flag);
			pfilter->filter_on_write(written, flush);
		});
	}	
}

void pipeline::fire_filter_on_error( filter* pfilter , const std::error_code& ec ){
	if ( !pfilter )
		return;
	if ( in_pipeline() ){
		pfilter->filter_on_error(ec);
	} else {
		tcode::rc_ptr< tcp::channel > pchan( channel());
		channel()->loop().execute( [ pfilter , ec ,  pchan , this ] {
			in_pipeline_holder ipl(_flag);
			pfilter->filter_on_error(ec);
		});
	}	
}

void pipeline::fire_filter_do_write( filter* pfilter , tcode::buffer::byte_buffer& buf ){
	if ( pfilter ){
		if ( in_pipeline() ){
			pfilter->filter_do_write(buf);
		} else {
			tcode::rc_ptr< tcp::channel > pchan( channel());
			channel()->loop().execute( [ pfilter , buf , pchan , this ] {
				in_pipeline_holder ipl(_flag);
				tcode::buffer::byte_buffer nb( buf );
				pfilter->filter_do_write(nb);
			});
		}	
	} else {
		if ( in_pipeline() ){
			channel()->do_write(buf);
		} else {
			tcode::rc_ptr< tcp::channel > pchan( channel());
			channel()->loop().execute( [  buf , pchan , this ] {
				in_pipeline_holder ipl(_flag);	
				pchan->do_write(buf);
			});
		}	
	}
}

pipeline& pipeline::add( filter* pfilter ){
	pfilter->pipeline(this);
	if( _inbound == nullptr )
		_inbound = pfilter;
	else {
		filter* last = _inbound;

		while( last->inbound() != nullptr )
			last = last->inbound();
	
		last->inbound( pfilter );
		pfilter->inbound( nullptr );
		pfilter->outbound(last);
	}
	return *this;
}

pipeline& pipeline::add_inbound( filter* base , filter* pfilter ){
	pfilter->pipeline(this);
	pfilter->inbound( base->inbound() );
	pfilter->outbound(base);

	base->inbound(pfilter);
	
	if ( pfilter->inbound() ) {
		pfilter->inbound()->outbound(pfilter);
	}
	return *this;
}

pipeline& pipeline::add_outbound( filter* base , filter* pfilter ){
	pfilter->pipeline(this);
	pfilter->inbound( base );
	pfilter->outbound(base->outbound());

	base->outbound(pfilter);
	
	if ( pfilter->outbound() ) {
		pfilter->outbound()->inbound(pfilter);
	}
	return *this;
}

void pipeline::channel( tcp::channel* chan ){
	_channel = chan;
}

tcp::channel* pipeline::channel( void ){
	return _channel;
}


}}}