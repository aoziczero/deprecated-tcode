#ifndef __tcode_transport_event_timer_h__
#define __tcode_transport_event_timer_h__

#include <common/time_stamp.hpp>
#include <common/rc_ptr.hpp>
#include <common/closure.hpp>
#include <diagnostics/tcode_error_code.hpp>
#include <atomic>

namespace tcode { namespace transport {

class event_loop;
class event_timer : 
	public tcode::ref_counted<event_timer>{
public:	
	typedef tcode::handler<
		 void ( const tcode::diagnostics::error_code& ec, const event_timer& )
	> handler;
	
	~event_timer( void );
	
	DISALLOW_COPY( event_timer );

	tcode::time_stamp expired_at( void ) const;
	void expired_at( const tcode::time_stamp& at );
	
	handler on_expired( void ) const ;	
	void on_expired( const handler& h ) ;
	

	void operator()(void);

	void fire( void );
	void cancel( void );
	
	typedef tcode::rc_ptr< event_timer > pointer_type;

private:
	event_timer( event_loop& l );
private:
	event_loop& _loop;
	tcode::time_stamp _expired_at;
	handler _handler;
	std::atomic< int > _flag;
public:
	static pointer_type create( event_loop& l);
};

}}

#endif