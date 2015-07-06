#ifndef __tcode_diagnostics_log_logger_h__
#define __tcode_diagnostics_log_logger_h__

#include <threading/spin_lock.hpp>
#include <diagnostics/log/record.hpp>
#include <diagnostics/log/writer.hpp>
#include <vector>

namespace tcode { namespace diagnostics { namespace log {

class logger {
public:
	logger( void );
	~logger ( void );
	
	log::level log_level( void );

	bool enabled( log::level lv );
	void enable( log::level lv );
	
	void write( 
		log::level lv 
		, const char* tag
		, const char* file 
		, const char* function 		
		, const int line 
		, const char* msg
		, ... );


	void write_dump( 
		log::level lv 
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
	tcode::threading::spin_lock _lock;
	std::vector< writer_ptr > _writers;
	log::level _level;
public:
	static logger& instance( void );
};

}}}

#endif