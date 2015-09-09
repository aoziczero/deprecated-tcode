#ifndef __tcode_log_logger_h__
#define __tcode_log_logger_h__

#include <tcode/threading/spinlock.hpp>
#include <tcode/log/record.hpp>
#include <tcode/log/writer.hpp>
#include <vector>

namespace tcode { namespace log {

class logger {
public:
	logger( void );
	~logger ( void );
	
    log::type type( void );

	bool enabled( log::type type );
	void enable( log::type type );
	
	void write( 
		log::type t 
		, const char* tag
		, const char* file 
		, const char* function 		
		, const int line 
		, const char* msg
		, ... );


	void write_dump( 
		log::type t 
		, const char* tag
		, const char* file 
		, const char* function 		
		, const int line 
        , uint8_t* buffer 
        , int sz
		, const char* msg
		, ... );
	
	void write( const record& r );
	
	void add_writer( const writer_ptr& ptr );
private:
	tcode::threading::spinlock _lock;
	std::vector< writer_ptr > _writers;
	log::type _type;
public:
	static logger& instance( void );
};

}}

#endif
