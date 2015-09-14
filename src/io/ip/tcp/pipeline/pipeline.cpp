#include "stdafx.h"
#include <tcode/io/ip/tcp/pipeline/pipeline.hpp>
#include <tcode/io/ip/tcp/pipeline/filter.hpp>
#include <tcode/io/ip/tcp/pipeline/channel.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

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
	return channel()->engine().in_run_loop() && (_flag & FLAG_IN_PIPELINE);
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

void pipeline::fire_filter_on_read( tcode::byte_buffer& buf ){
	in_pipeline_holder ipl(_flag);
	if ( _inbound )
		_inbound->filter_on_read( buf );
}

void pipeline::fire_filter_on_write( int written , bool flush ){
	in_pipeline_holder ipl(_flag);
	if ( _inbound )
		_inbound->filter_on_write( written , flush);
}

void pipeline::fire_filter_on_error( const std::error_code& ec ) {
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
        channel()->add_ref();
	    channel()->engine().execute( [pfilter,addr,this] {
			in_pipeline_holder ipl(_flag);
			pfilter->filter_on_open( addr );
            channel()->release();
		});
	}	
}

void pipeline::fire_filter_on_close( filter* pfilter ){
	if ( !pfilter )
		return;
	if ( in_pipeline() ){
		pfilter->filter_on_close();
	} else {
        channel()->add_ref();
	    channel()->engine().execute( [pfilter,this] {
			in_pipeline_holder ipl(_flag);
			pfilter->filter_on_close();
            channel()->release();
		});
	}	
}

void pipeline::fire_filter_on_read( filter* pfilter , tcode::byte_buffer& buf ){
	if ( !pfilter )
		return;

	if ( in_pipeline() ){
		pfilter->filter_on_read(buf);
	} else {
        channel()->add_ref();
	    channel()->engine().execute([pfilter,buf,this] {
			in_pipeline_holder ipl(_flag);
			tcode::byte_buffer nb( buf );
			pfilter->filter_on_read(nb);
            channel()->release();
		});
	}	
}

void pipeline::fire_filter_on_write( filter* pfilter , int written , bool flush ){
	if ( !pfilter )
		return;
	if ( in_pipeline() ){
		pfilter->filter_on_write(written,flush);
	} else {
        channel()->add_ref();
		channel()->engine().execute( [ pfilter,written,flush,this ] {
			in_pipeline_holder ipl(_flag);
			pfilter->filter_on_write(written, flush);
            channel()->release();
		});
	}	
}

void pipeline::fire_filter_on_error( filter* pfilter , const std::error_code& ec ){
	if ( !pfilter )
		return;
	if ( in_pipeline() ){
		pfilter->filter_on_error(ec);
	} else {
        channel()->add_ref();
		channel()->engine().execute([pfilter,ec,this]{
			in_pipeline_holder ipl(_flag);
			pfilter->filter_on_error(ec);
            channel()->release();
		});
	}	
}

void pipeline::fire_filter_do_write( filter* pfilter , tcode::byte_buffer& buf ){
	if ( pfilter ){
		if ( in_pipeline() ){
			pfilter->filter_do_write(buf);
		} else {
            channel()->add_ref();
			channel()->engine().execute([pfilter,buf,this]{
				in_pipeline_holder ipl(_flag);
				tcode::byte_buffer nb( buf );
				pfilter->filter_do_write(nb);
                channel()->release();
			});
		}	
	} else {
		if ( in_pipeline() ){
			channel()->do_write(buf);
		} else {
            channel()->add_ref();
			channel()->engine().execute( [buf,this] {
				in_pipeline_holder ipl(_flag);	
				channel()->do_write(buf);
                channel()->release();
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
	if ( base == nullptr || pfilter == nullptr ) 
		return *this;
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
	if ( base == nullptr || pfilter == nullptr ) 
		return *this;
	pfilter->pipeline(this);
	pfilter->inbound( base );
	pfilter->outbound(base->outbound());

	base->outbound(pfilter);
	
	if ( pfilter->outbound() ) {
		pfilter->outbound()->inbound(pfilter);
	}
	return *this;
}

pipeline& pipeline::remove( filter* pfilter ){
	if ( pfilter == nullptr ) 
		return *this;
	filter* in = pfilter->inbound();
	filter* out = pfilter->outbound();
	
	if ( in ) 
		in->outbound(out);
	
	if ( out ) 
		out->inbound(in);
	
	if ( pfilter == _inbound ) 
		_inbound = out;

	return *this;
}

void pipeline::channel( tcode::io::ip::tcp::channel* chan ){
	_channel = chan;
}

tcode::io::ip::tcp::channel* pipeline::channel( void ){
	return _channel;
}


}}}}
