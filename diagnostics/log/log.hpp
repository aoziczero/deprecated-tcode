#ifndef __tcode_diagnostics_log_h__
#define __tcode_diagnostics_log_h__

#include <diagnostics/log/record.hpp>
#include <diagnostics/log/writer.hpp>
#include <diagnostics/log//formatter.hpp>
#include <diagnostics/log/logger.hpp>
#include <diagnostics/log/console_writer.hpp>
#include <diagnostics/log/file_writer.hpp>

#if defined( _WIN32)
#include <crtdefs.h>

#define LOG_T( tag , msg , ... ) do { tcode::diagnostics::log::logger::instance().write(  tcode::diagnostics::log::level::LOG_TRACE, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , __VA_ARGS__ ); } while(0)
#define LOG_D( tag , msg , ... ) do { tcode::diagnostics::log::logger::instance().write(  tcode::diagnostics::log::level::LOG_DEBUG, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , __VA_ARGS__ ); } while(0)
#define LOG_I( tag , msg , ... ) do { tcode::diagnostics::log::logger::instance().write(  tcode::diagnostics::log::level::LOG_INFO , tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , __VA_ARGS__ ); } while(0)
#define LOG_W( tag , msg , ... ) do { tcode::diagnostics::log::logger::instance().write(  tcode::diagnostics::log::level::LOG_WARN , tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , __VA_ARGS__ ); } while(0)
#define LOG_E( tag , msg , ... ) do { tcode::diagnostics::log::logger::instance().write(  tcode::diagnostics::log::level::LOG_ERROR, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , __VA_ARGS__ ); } while(0)
#define LOG_F( tag , msg , ... ) do { tcode::diagnostics::log::logger::instance().write(  tcode::diagnostics::log::level::LOG_FATAL, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , __VA_ARGS__ ); } while(0)

#define LOG_DUMP_T( tag , buffer , sz , msg , ... ) do { tcode::diagnostics::log::logger::instance().write_dump(  tcode::diagnostics::log::level::LOG_TRACE, tag , __FILE__  ,  __FUNCTION__ , __LINE__ ,(uint8_t*)buffer , sz , msg , __VA_ARGS__ ); } while(0)
#define LOG_DUMP_D( tag , buffer , sz , msg , ... ) do { tcode::diagnostics::log::logger::instance().write_dump(  tcode::diagnostics::log::level::LOG_DEBUG, tag , __FILE__  ,  __FUNCTION__ , __LINE__ ,(uint8_t*)buffer , sz , msg , __VA_ARGS__ ); } while(0)
#define LOG_DUMP_I( tag , buffer , sz , msg , ... ) do { tcode::diagnostics::log::logger::instance().write_dump(  tcode::diagnostics::log::level::LOG_INFO , tag , __FILE__  ,  __FUNCTION__ , __LINE__ ,(uint8_t*)buffer , sz , msg , __VA_ARGS__ ); } while(0)
#define LOG_DUMP_W( tag , buffer , sz , msg , ... ) do { tcode::diagnostics::log::logger::instance().write_dump(  tcode::diagnostics::log::level::LOG_WARN , tag , __FILE__  ,  __FUNCTION__ , __LINE__ ,(uint8_t*)buffer , sz , msg , __VA_ARGS__ ); } while(0)
#define LOG_DUMP_E( tag , buffer , sz , msg , ... ) do { tcode::diagnostics::log::logger::instance().write_dump(  tcode::diagnostics::log::level::LOG_ERROR, tag , __FILE__  ,  __FUNCTION__ , __LINE__ ,(uint8_t*)buffer , sz , msg , __VA_ARGS__ ); } while(0)
#define LOG_DUMP_F( tag , bzffer , sz , msg , ... ) do { tcode::diagnostics::log::logger::instance().write_dump(  tcode::diagnostics::log::level::LOG_FATAL, tag , __FILE__  ,  __FUNCTION__ , __LINE__ ,(uint8_t*)buffer , sz , msg , __VA_ARGS__ ); } while(0)

#define LOGGER_T( l , tag , msg , ... ) do { l.write(  tcode::diagnostics::log::level::LOG_TRACE, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , __VA_ARGS__ ); } while(0)
#define LOGGER_D( l , tag , msg , ... ) do { l.write(  tcode::diagnostics::log::level::LOG_DEBUG, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , __VA_ARGS__ ); } while(0)
#define LOGGER_I( l , tag , msg , ... ) do { l.write(  tcode::diagnostics::log::level::LOG_INFO , tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , __VA_ARGS__ ); } while(0)
#define LOGGER_W( l , tag , msg , ... ) do { l.write(  tcode::diagnostics::log::level::LOG_WARN , tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , __VA_ARGS__ ); } while(0)
#define LOGGER_E( l , tag , msg , ... ) do { l.write(  tcode::diagnostics::log::level::LOG_ERROR, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , __VA_ARGS__ ); } while(0)
#define LOGGER_F( l , tag , msg , ... ) do { l.write(  tcode::diagnostics::log::level::LOG_FATAL, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , __VA_ARGS__ ); } while(0)
		
#define LOGGER_DUMP_T( l ,tag , buffer , sz , msg , ... ) do { l.write_dump(  tcode::diagnostics::log::level::LOG_TRACE, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , (uint8_t*)buffer , sz , msg , __VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_D( l ,tag , buffer , sz , msg , ... ) do { l.write_dump(  tcode::diagnostics::log::level::LOG_DEBUG, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , (uint8_t*)buffer , sz , msg , __VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_I( l ,tag , buffer , sz , msg , ... ) do { l.write_dump(  tcode::diagnostics::log::level::LOG_INFO , tag , __FILE__  ,  __FUNCTION__ , __LINE__ , (uint8_t*)buffer , sz , msg , __VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_W( l ,tag , buffer , sz , msg , ... ) do { l.write_dump(  tcode::diagnostics::log::level::LOG_WARN , tag , __FILE__  ,  __FUNCTION__ , __LINE__ , (uint8_t*)buffer , sz , msg , __VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_E( l ,tag , buffer , sz , msg , ... ) do { l.write_dump(  tcode::diagnostics::log::level::LOG_ERROR, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , (uint8_t*)buffer , sz , msg , __VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_F( l ,tag , bzffer , sz , msg , ... ) do { l.write_dump(  tcode::diagnostics::log::level::LOG_FATAL, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , (uint8_t*)buffer , sz , msg , __VA_ARGS__ ); } while(0)

#else

#define LOG_T( tag , msg , ... ) do { tcode::diagnostics::log::logger::instance().write(  tcode::diagnostics::log::level::LOG_TRACE, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , ##__VA_ARGS__ ); } while(0)
#define LOG_D( tag , msg , ... ) do { tcode::diagnostics::log::logger::instance().write(  tcode::diagnostics::log::level::LOG_DEBUG, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , ##__VA_ARGS__ ); } while(0)
#define LOG_I( tag , msg , ... ) do { tcode::diagnostics::log::logger::instance().write(  tcode::diagnostics::log::level::LOG_INFO , tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , ##__VA_ARGS__ ); } while(0)
#define LOG_W( tag , msg , ... ) do { tcode::diagnostics::log::logger::instance().write(  tcode::diagnostics::log::level::LOG_WARN , tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , ##__VA_ARGS__ ); } while(0)
#define LOG_E( tag , msg , ... ) do { tcode::diagnostics::log::logger::instance().write(  tcode::diagnostics::log::level::LOG_ERROR, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , ##__VA_ARGS__ ); } while(0)
#define LOG_F( tag , msg , ... ) do { tcode::diagnostics::log::logger::instance().write(  tcode::diagnostics::log::level::LOG_FATAL, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , ##__VA_ARGS__ ); } while(0)

#define LOG_DUMP_T( tag , buffer , sz , msg , ... ) do { tcode::diagnostics::log::logger::instance().write_dump(  tcode::diagnostics::log::level::LOG_TRACE, tag , __FILE__  ,  __FUNCTION__ , __LINE__ ,(uint8_t*)buffer , sz , msg , ##__VA_ARGS__ ); } while(0)
#define LOG_DUMP_D( tag , buffer , sz , msg , ... ) do { tcode::diagnostics::log::logger::instance().write_dump(  tcode::diagnostics::log::level::LOG_DEBUG, tag , __FILE__  ,  __FUNCTION__ , __LINE__ ,(uint8_t*)buffer , sz , msg , ##__VA_ARGS__ ); } while(0)
#define LOG_DUMP_I( tag , buffer , sz , msg , ... ) do { tcode::diagnostics::log::logger::instance().write_dump(  tcode::diagnostics::log::level::LOG_INFO , tag , __FILE__  ,  __FUNCTION__ , __LINE__ ,(uint8_t*)buffer , sz , msg , ##__VA_ARGS__ ); } while(0)
#define LOG_DUMP_W( tag , buffer , sz , msg , ... ) do { tcode::diagnostics::log::logger::instance().write_dump(  tcode::diagnostics::log::level::LOG_WARN , tag , __FILE__  ,  __FUNCTION__ , __LINE__ ,(uint8_t*)buffer , sz , msg , ##__VA_ARGS__ ); } while(0)
#define LOG_DUMP_E( tag , buffer , sz , msg , ... ) do { tcode::diagnostics::log::logger::instance().write_dump(  tcode::diagnostics::log::level::LOG_ERROR, tag , __FILE__  ,  __FUNCTION__ , __LINE__ ,(uint8_t*)buffer , sz , msg , ##__VA_ARGS__ ); } while(0)
#define LOG_DUMP_F( tag , bzffer , sz , msg , ... ) do { tcode::diagnostics::log::logger::instance().write_dump(  tcode::diagnostics::log::level::LOG_FATAL, tag , __FILE__  ,  __FUNCTION__ , __LINE__ ,(uint8_t*)buffer , sz , msg , ##__VA_ARGS__ ); } while(0)


#define LOGGER_T( l , tag , msg , ... ) do { l.write(  tcode::diagnostics::log::level::LOG_TRACE, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , ##__VA_ARGS__ ); } while(0)
#define LOGGER_D( l , tag , msg , ... ) do { l.write(  tcode::diagnostics::log::level::LOG_DEBUG, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , ##__VA_ARGS__ ); } while(0)
#define LOGGER_I( l , tag , msg , ... ) do { l.write(  tcode::diagnostics::log::level::LOG_INFO , tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , ##__VA_ARGS__ ); } while(0)
#define LOGGER_W( l , tag , msg , ... ) do { l.write(  tcode::diagnostics::log::level::LOG_WARN , tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , ##__VA_ARGS__ ); } while(0)
#define LOGGER_E( l , tag , msg , ... ) do { l.write(  tcode::diagnostics::log::level::LOG_ERROR, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , ##__VA_ARGS__ ); } while(0)
#define LOGGER_F( l , tag , msg , ... ) do { l.write(  tcode::diagnostics::log::level::LOG_FATAL, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , msg , ##__VA_ARGS__ ); } while(0)
		
#define LOGGER_DUMP_T( l ,tag , buffer , sz , msg , ... ) do { l.write_dump(  tcode::diagnostics::log::level::LOG_TRACE, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , (uint8_t*)buffer , sz , msg , ##__VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_D( l ,tag , buffer , sz , msg , ... ) do { l.write_dump(  tcode::diagnostics::log::level::LOG_DEBUG, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , (uint8_t*)buffer , sz , msg , ##__VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_I( l ,tag , buffer , sz , msg , ... ) do { l.write_dump(  tcode::diagnostics::log::level::LOG_INFO , tag , __FILE__  ,  __FUNCTION__ , __LINE__ , (uint8_t*)buffer , sz , msg , ##__VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_W( l ,tag , buffer , sz , msg , ... ) do { l.write_dump(  tcode::diagnostics::log::level::LOG_WARN , tag , __FILE__  ,  __FUNCTION__ , __LINE__ , (uint8_t*)buffer , sz , msg , ##__VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_E( l ,tag , buffer , sz , msg , ... ) do { l.write_dump(  tcode::diagnostics::log::level::LOG_ERROR, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , (uint8_t*)buffer , sz , msg , ##__VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_F( l ,tag , bzffer , sz , msg , ... ) do { l.write_dump(  tcode::diagnostics::log::level::LOG_FATAL, tag , __FILE__  ,  __FUNCTION__ , __LINE__ , (uint8_t*)buffer , sz , msg , ##__VA_ARGS__ ); } while(0)
#endif
#include <vector>

#endif