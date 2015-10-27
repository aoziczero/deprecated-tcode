#ifndef __tcode_diagnostics_log_h__
#define __tcode_diagnostics_log_h__

#include <tcode/log/record.hpp>
#include <tcode/log/writer.hpp>
#include <tcode/log/formatter.hpp>
#include <tcode/log/logger.hpp>
#include <tcode/log/console_writer.hpp>
#include <tcode/log/file_writer.hpp>
//#include <tcode/log/udp_writer.hpp>
#include <tcode/log/serialize_formatter.hpp>

#if defined(TCODE_WIN32)

#include <crtdefs.h>
//! win32 macros

#define LOG_T(tag,msg,... ) do{tcode::log::logger::instance().write(tcode::log::trace,tag,__FILE__,__FUNCTION__,__LINE__,msg,__VA_ARGS__ ); } while(0)
#define LOG_D(tag,msg,... ) do{tcode::log::logger::instance().write(tcode::log::debug,tag,__FILE__,__FUNCTION__,__LINE__,msg,__VA_ARGS__ ); } while(0)
#define LOG_I(tag,msg,... ) do{tcode::log::logger::instance().write(tcode::log::info,tag,__FILE__,__FUNCTION__,__LINE__,msg,__VA_ARGS__ ); } while(0)
#define LOG_W(tag,msg,... ) do{tcode::log::logger::instance().write(tcode::log::warn,tag,__FILE__,__FUNCTION__,__LINE__,msg,__VA_ARGS__ ); } while(0)
#define LOG_E(tag,msg,... ) do{tcode::log::logger::instance().write(tcode::log::error,tag,__FILE__,__FUNCTION__,__LINE__,msg,__VA_ARGS__ ); } while(0)
#define LOG_F(tag,msg,... ) do{tcode::log::logger::instance().write(tcode::log::fatal,tag,__FILE__,__FUNCTION__,__LINE__,msg,__VA_ARGS__ ); } while(0)

#define LOG_DUMP_T(tag,buffer,sz,msg,... ) do{tcode::log::logger::instance().write_dump(tcode::log::trace,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,__VA_ARGS__ ); } while(0)
#define LOG_DUMP_D(tag,buffer,sz,msg,... ) do{tcode::log::logger::instance().write_dump(tcode::log::debug,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,__VA_ARGS__ ); } while(0)
#define LOG_DUMP_I(tag,buffer,sz,msg,... ) do{tcode::log::logger::instance().write_dump(tcode::log::info,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,__VA_ARGS__ ); } while(0)
#define LOG_DUMP_W(tag,buffer,sz,msg,... ) do{tcode::log::logger::instance().write_dump(tcode::log::warn,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,__VA_ARGS__ ); } while(0)
#define LOG_DUMP_E(tag,buffer,sz,msg,... ) do{tcode::log::logger::instance().write_dump(tcode::log::error,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,__VA_ARGS__ ); } while(0)
#define LOG_DUMP_F(tag,bzffer,sz,msg,... ) do{tcode::log::logger::instance().write_dump(tcode::log::fatal,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,__VA_ARGS__ ); } while(0)

#define LOGGER_T(l,tag,msg,... ) do{l.write(tcode::log::trace,tag,__FILE__,__FUNCTION__,__LINE__,msg,__VA_ARGS__ ); } while(0)
#define LOGGER_D(l,tag,msg,... ) do{l.write(tcode::log::debug,tag,__FILE__,__FUNCTION__,__LINE__,msg,__VA_ARGS__ ); } while(0)
#define LOGGER_I(l,tag,msg,... ) do{l.write(tcode::log::info,tag,__FILE__,__FUNCTION__,__LINE__,msg,__VA_ARGS__ ); } while(0)
#define LOGGER_W(l,tag,msg,... ) do{l.write(tcode::log::warn,tag,__FILE__,__FUNCTION__,__LINE__,msg,__VA_ARGS__ ); } while(0)
#define LOGGER_E(l,tag,msg,... ) do{l.write(tcode::log::error,tag,__FILE__,__FUNCTION__,__LINE__,msg,__VA_ARGS__ ); } while(0)
#define LOGGER_F(l,tag,msg,... ) do{l.write(tcode::log::fatal,tag,__FILE__,__FUNCTION__,__LINE__,msg,__VA_ARGS__ ); } while(0)
		
#define LOGGER_DUMP_T(l,tag,buffer,sz,msg,... ) do{l.write_dump(tcode::log::trace,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,__VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_D(l,tag,buffer,sz,msg,... ) do{l.write_dump(tcode::log::debug,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,__VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_I(l,tag,buffer,sz,msg,... ) do{l.write_dump(tcode::log::info,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,__VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_W(l,tag,buffer,sz,msg,... ) do{l.write_dump(tcode::log::warn,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,__VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_E(l,tag,buffer,sz,msg,... ) do{l.write_dump(tcode::log::error,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,__VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_F(l,tag,bzffer,sz,msg,... ) do{l.write_dump(tcode::log::fatal,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,__VA_ARGS__ ); } while(0)

#else
//! other os macros

#define LOG_T(tag,msg,... ) do{tcode::log::logger::instance().write(tcode::log::trace,tag,__FILE__,__FUNCTION__,__LINE__,msg,##__VA_ARGS__ ); } while(0)
#define LOG_D(tag,msg,... ) do{tcode::log::logger::instance().write(tcode::log::debug,tag,__FILE__,__FUNCTION__,__LINE__,msg,##__VA_ARGS__ ); } while(0)
#define LOG_I(tag,msg,... ) do{tcode::log::logger::instance().write(tcode::log::info,tag,__FILE__,__FUNCTION__,__LINE__,msg,##__VA_ARGS__ ); } while(0)
#define LOG_W(tag,msg,... ) do{tcode::log::logger::instance().write(tcode::log::warn,tag,__FILE__,__FUNCTION__,__LINE__,msg,##__VA_ARGS__ ); } while(0)
#define LOG_E(tag,msg,... ) do{tcode::log::logger::instance().write(tcode::log::error,tag,__FILE__,__FUNCTION__,__LINE__,msg,##__VA_ARGS__ ); } while(0)
#define LOG_F(tag,msg,... ) do{tcode::log::logger::instance().write(tcode::log::fatal,tag,__FILE__,__FUNCTION__,__LINE__,msg,##__VA_ARGS__ ); } while(0)

#define LOG_DUMP_T(tag,buffer,sz,msg,... ) do{tcode::log::logger::instance().write_dump(tcode::log::trace,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,##__VA_ARGS__ ); } while(0)
#define LOG_DUMP_D(tag,buffer,sz,msg,... ) do{tcode::log::logger::instance().write_dump(tcode::log::debug,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,##__VA_ARGS__ ); } while(0)
#define LOG_DUMP_I(tag,buffer,sz,msg,... ) do{tcode::log::logger::instance().write_dump(tcode::log::info,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,##__VA_ARGS__ ); } while(0)
#define LOG_DUMP_W(tag,buffer,sz,msg,... ) do{tcode::log::logger::instance().write_dump(tcode::log::warn,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,##__VA_ARGS__ ); } while(0)
#define LOG_DUMP_E(tag,buffer,sz,msg,... ) do{tcode::log::logger::instance().write_dump(tcode::log::error,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,##__VA_ARGS__ ); } while(0)
#define LOG_DUMP_F(tag,bzffer,sz,msg,... ) do{tcode::log::logger::instance().write_dump(tcode::log::fatal,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,##__VA_ARGS__ ); } while(0)


#define LOGGER_T(l,tag,msg,... ) do{l.write(tcode::log::trace,tag,__FILE__,__FUNCTION__,__LINE__,msg,##__VA_ARGS__ ); } while(0)
#define LOGGER_D(l,tag,msg,... ) do{l.write(tcode::log::debug,tag,__FILE__,__FUNCTION__,__LINE__,msg,##__VA_ARGS__ ); } while(0)
#define LOGGER_I(l,tag,msg,... ) do{l.write(tcode::log::info,tag,__FILE__,__FUNCTION__,__LINE__,msg,##__VA_ARGS__ ); } while(0)
#define LOGGER_W(l,tag,msg,... ) do{l.write(tcode::log::warn,tag,__FILE__,__FUNCTION__,__LINE__,msg,##__VA_ARGS__ ); } while(0)
#define LOGGER_E(l,tag,msg,... ) do{l.write(tcode::log::error,tag,__FILE__,__FUNCTION__,__LINE__,msg,##__VA_ARGS__ ); } while(0)
#define LOGGER_F(l,tag,msg,... ) do{l.write(tcode::log::fatal,tag,__FILE__,__FUNCTION__,__LINE__,msg,##__VA_ARGS__ ); } while(0)
		
#define LOGGER_DUMP_T(l,tag,buffer,sz,msg,... ) do{l.write_dump(tcode::log::trace,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,##__VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_D(l,tag,buffer,sz,msg,... ) do{l.write_dump(tcode::log::debug,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,##__VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_I(l,tag,buffer,sz,msg,... ) do{l.write_dump(tcode::log::info,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,##__VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_W(l,tag,buffer,sz,msg,... ) do{l.write_dump(tcode::log::warn,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,##__VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_E(l,tag,buffer,sz,msg,... ) do{l.write_dump(tcode::log::error,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,##__VA_ARGS__ ); } while(0)
#define LOGGER_DUMP_F(l,tag,bzffer,sz,msg,... ) do{l.write_dump(tcode::log::fatal,tag,__FILE__,__FUNCTION__,__LINE__,(uint8_t*)buffer,sz,msg,##__VA_ARGS__ ); } while(0)

#endif

#endif
