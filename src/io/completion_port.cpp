#include "stdafx.h"
#include <tcode/error.hpp>
#include <tcode/io/io.hpp>
#include <tcode/io/completion_port.hpp>
#include <tcode/io/engine.hpp>
#include <tcode/io/operation.hpp>
#include <tcode/io/ip/option.hpp>
#include <tcode/io/ip/address.hpp>
#include <tcode/io/ip/tcp/operation_connect.hpp>
#include <tcode/io/ip/tcp/operation_write.hpp>
#include <tcode/io/ip/tcp/operation_read.hpp>
#include <tcode/io/ip/tcp/operation_accept.hpp>
#include <tcode/io/ip/udp/operation_write.hpp>
#include <tcode/io/ip/udp/operation_read.hpp>

namespace tcode { namespace io {

    struct completion_port::_descriptor{
        SOCKET fd;
        std::atomic<int> refcount;
    };

    completion_port::completion_port( engine& en )
        : _handle( CreateIoCompletionPort( INVALID_HANDLE_VALUE , NULL , 0 , 1 ))
		, _engine( en )
    {
        
    }

    completion_port::~completion_port( void ){
		CloseHandle(_handle);
		_handle = INVALID_HANDLE_VALUE;
    }

    int completion_port::run( const tcode::timespan& ts ){
		LPOVERLAPPED ov;
		DWORD iobytes;
		VOID* key;
		BOOL r = GetQueuedCompletionStatus( _handle
			, &iobytes
			, reinterpret_cast< PULONG_PTR >( &key)
			, &ov
			, (DWORD)ts.total_milliseconds());
		if (ov == nullptr) {
			tcode::slist::queue< tcode::operation > ops;
            do {
                tcode::threading::spinlock::guard guard(_lock);
                if ( _op_queue.empty() )
                    return 0;
                ops = std::move( _op_queue );
            }while(0);
			int run = 0;
            while( !ops.empty() ){
                tcode::operation* op = ops.front();
                ops.pop_front();
                op_run( op );
                ++run;
            }
			return run;
		}
		else {
			tcode::io::operation* op = tcode::container_of( ov , &tcode::io::operation::ov );
			completion_port::descriptor desc = reinterpret_cast<completion_port::descriptor>(key);
			if ( r == FALSE ) 
				op->error() = tcode::last_error();
			op->io_byte() = iobytes;
			if (op->post_proc(this, desc)) {
				(*op)();
			}
			descriptor_release(desc);
			return 1;
		}
    }
    
    void completion_port::wake_up( void ){
        PostQueuedCompletionStatus( _handle , 0 , 0 , nullptr );
    }

    bool completion_port::bind( const descriptor& d ) {
		return CreateIoCompletionPort(
			(HANDLE)d->fd
			, _handle
			, (ULONG_PTR)d
			, 0 ) == _handle;
    }

    void completion_port::unbind( descriptor& d ) {
        do { 
            tcode::threading::spinlock::guard guard(_lock);
            if ( d == nullptr ) 
                return;
            descriptor desc = nullptr;
            std::swap( desc , d );
			tcode::operation* op = 
                tcode::operation::wrap( 
                        [this,desc] {
                            if ( desc->fd != INVALID_SOCKET ) {
								closesocket(desc->fd);
                                desc->fd = INVALID_SOCKET;
                            }
                            descriptor_release( desc );
                        });
            op_add(op);
        }while(0);
        wake_up();
    }

    void completion_port::execute( tcode::operation* op ) {
        do {
            tcode::threading::spinlock::guard guard(_lock);
            op_add(op);
        } while(0);
        wake_up();
    }

    struct connect_ex {
		connect_ex(SOCKET fd) {
			DWORD b = 0; GUID g = WSAID_CONNECTEX;
			WSAIoctl( fd , SIO_GET_EXTENSION_FUNCTION_POINTER
				, &g , sizeof(g) , &fn , sizeof(fn) , &b , nullptr , nullptr);
		}
		LPFN_CONNECTEX fn;
	};

    void completion_port::connect( 
            completion_port::descriptor& desc 
            , ip::tcp::operation_connect_base* op )
    {
		SOCKET fd = WSASocket( op->address().family() , SOCK_STREAM , IPPROTO_TCP , nullptr , 0 , WSA_FLAG_OVERLAPPED );
        if ( fd != INVALID_SOCKET ) {
			tcode::io::ip::option::non_blocking nb;
			nb.set_option( fd );
			connect_ex func(fd);
			if (func.fn != nullptr) {
				tcode::io::ip::address addr( tcode::io::ip::address::any(0));
				if (::bind(fd, addr.sockaddr(), (int)addr.sockaddr_length()) == 0) {
					desc = descriptor_create(fd);
					if (bind(desc)) {
						LPOVERLAPPED ov = &(op->ov);
						memset( ov , 0 , sizeof(*ov));
						DWORD b=0;
						if (func.fn(desc->fd
							, op->address().sockaddr()
							, op->address().sockaddr_length()
							, nullptr , 0 , &b , ov) == TRUE)
							return;

						if (WSAGetLastError() == WSA_IO_PENDING)
							return;

						op->error() = tcode::last_error();
					} else {
						op->error() = tcode::last_error();
					}
					delete desc;
					desc = nullptr;
				} else {
					op->error() = tcode::last_error();
				}
			} else {
				op->error() = tcode::last_error(); 
			}
			closesocket(fd);
		} else {			
			op->error() = tcode::last_error(); 
		}
        return execute( op );
    }

    void completion_port::write( completion_port::descriptor desc 
            , ip::tcp::operation_write_base* op)
    {
        descriptor_add_ref(desc);
		LPOVERLAPPED ov = &(op->ov);
		memset( ov , 0 , sizeof(*ov));
		DWORD flag = 0;
		if ( WSASend( desc->fd
					, op->buffers()
					, op->buffers_count()
					, nullptr 
					, flag 
					, ov
					, nullptr ) == SOCKET_ERROR )
		{
			if ( WSAGetLastError() != WSA_IO_PENDING ){
				op->error() = tcode::last_error();
				descriptor_release(desc);
				execute(op);
			}
		}
    }

    void completion_port::read( descriptor desc
            , ip::tcp::operation_read_base* op ){
        descriptor_add_ref(desc);
		LPOVERLAPPED ov = &(op->ov);
		memset( ov , 0 , sizeof(*ov));
		DWORD flag = 0;
		if ( WSARecv(	desc->fd
					, op->buffers()
					, op->buffers_count()
					, nullptr 
					, &flag 
					, ov
					, nullptr ) == SOCKET_ERROR )
		{
			if ( WSAGetLastError() != WSA_IO_PENDING ){
				op->error() = tcode::last_error();
				descriptor_release(desc);
				execute(op);
			}
		}
    }
    
    bool completion_port::listen( descriptor& desc 
                , const ip::address& addr )
    {
        SOCKET fd = WSASocket( addr.family() , SOCK_STREAM , IPPROTO_TCP , nullptr , 0 , WSA_FLAG_OVERLAPPED );
        if ( fd != INVALID_SOCKET ) {
            tcode::io::ip::option::non_blocking nb;
            nb.set_option(fd);
            tcode::io::ip::option::reuse_address reuse( true );
            reuse.set_option( fd );
            if ((::bind( fd , addr.sockaddr() , addr.sockaddr_length() ) == 0)
                    && (::listen(fd, SOMAXCONN ) == 0 )) {
                desc = descriptor_create(fd);
                if ( bind( desc ))
                    return true;
                delete desc;
                desc = nullptr;
            }
            closesocket(fd);
        }
        return false;
    }

    void completion_port::accept( descriptor desc
			, int family
            , ip::tcp::operation_accept_base* op ){
        descriptor_add_ref(desc);
		op->accepted_fd() = WSASocket( family , SOCK_STREAM , IPPROTO_TCP , nullptr , 0 , WSA_FLAG_OVERLAPPED );
        if ( op->accepted_fd() != INVALID_SOCKET ) {
			tcode::io::ip::option::non_blocking nb;
			nb.set_option( op->accepted_fd() );
			LPOVERLAPPED ov = &(op->ov);
			memset( ov , 0 , sizeof(*ov));
			DWORD flag = 0;
			if ( AcceptEx( desc->fd 
					, op->accepted_fd()
					, op->address_buf()
					, 0
					, sizeof( tcode::io::ip::address)
					, sizeof( tcode::io::ip::address)
					, &flag
					, ov ) == TRUE )
				return;
			if (WSAGetLastError() == WSA_IO_PENDING)
				return;
			closesocket(op->accepted_fd());
			op->accepted_fd() = INVALID_SOCKET;
			op->error() = tcode::last_error();
		}
		descriptor_release(desc);
		execute(op);
    }
    
    bool completion_port::open( descriptor& desc
                , int af , int type , int proto )
    {
        SOCKET fd = socket( af , type , proto );
        if ( fd != -1 ) {
            tcode::io::ip::option::non_blocking nb;
            nb.set_option(fd);
            desc = descriptor_create( fd );
            if ( bind(desc))
                return true;
            delete desc;
            desc = nullptr;
			closesocket(fd);
        }
        return false;
    }
    

    bool completion_port::bind( descriptor& desc
            , const ip::address& addr ) 
    {
        SOCKET fd = WSASocket( addr.family() , SOCK_DGRAM , IPPROTO_UDP  , nullptr , 0 , WSA_FLAG_OVERLAPPED );
        if ( fd != INVALID_SOCKET ) {
            tcode::io::ip::option::non_blocking nb;
            nb.set_option(fd);
            tcode::io::ip::option::reuse_address reuse( true );
            reuse.set_option( fd );
            if ( ::bind( fd , addr.sockaddr() , addr.sockaddr_length() ) == 0 ){
				desc = descriptor_create(fd);
                if ( bind(desc))
                    return true;
                delete desc;
                desc = nullptr;
            }
            closesocket(fd);
        }
        return false;
    }

    void completion_port::write( descriptor& desc 
            , ip::udp::operation_write_base* op )
    {   
        if ( desc == nullptr ) {
			SOCKET fd = WSASocket( op->address().family() , SOCK_DGRAM , IPPROTO_UDP  , nullptr , 0 , WSA_FLAG_OVERLAPPED );
			if ( fd != INVALID_SOCKET ) {
				desc = descriptor_create(fd);
                if ( !bind( desc )){
                    op->error() = tcode::last_error();
                    delete desc;
                    desc = nullptr;
                    closesocket(fd);
                }
            }else{
                op->error() = tcode::last_error();
            }
            if ( desc == nullptr ){
                return execute(op);
            }
        }
        descriptor_add_ref(desc);
		LPOVERLAPPED ov = &(op->ov);
		memset( ov , 0 , sizeof(*ov));
		DWORD flag = 0;
		if ( WSASendTo(	desc->fd
					, &op->buffer()
					, 1
					, nullptr 
					, flag 
					, op->address().sockaddr()
					, op->address().sockaddr_length()
					, ov
					, nullptr ) == SOCKET_ERROR )
		{
			if ( WSAGetLastError() != WSA_IO_PENDING ){
				op->error() = tcode::last_error();
				descriptor_release(desc);
				execute(op);
			}
		}

    }
    
    void completion_port::read( descriptor desc
            , ip::udp::operation_read_base* op )
    {
        if ( desc == nullptr ) {
            op->error() = tcode::error_invalid; 
            return execute(op);
        }
        descriptor_add_ref(desc);
		LPOVERLAPPED ov = &(op->ov);
		memset( ov , 0 , sizeof(*ov));
		DWORD flag = 0;
		if ( WSARecvFrom(	desc->fd
					, &op->buffer()
					, 1
					, nullptr 
					, &flag 
					, op->address().sockaddr()
					, op->address().sockaddr_length_ptr()
					, ov
					, nullptr ) == SOCKET_ERROR )
		{
			if ( WSAGetLastError() != WSA_IO_PENDING ){
				op->error() = tcode::last_error();
				descriptor_release(desc);
				execute(op);
			}
		}
    }


    void completion_port::op_add( tcode::operation* op ){
        _op_queue.push_back( op );
        _engine.add_ref();
    }

    void completion_port::op_run( tcode::operation* op ){
        _engine.release();
        (*op)();
    }

    int completion_port::accept( descriptor listen 
            , descriptor& accepted 
            , ip::address& addr
			, SOCKET& fd
			, char* address_buf
            , std::error_code& ec )
    {
		if ( listen->fd == INVALID_SOCKET ) {
			ec = tcode::error_invalid;
		} else {
			if (!ec) {
				accepted = descriptor_create(fd);
				if (bind(accepted)) {
					struct sockaddr* local ,*remote;
					INT llen , rlen;
					GetAcceptExSockaddrs( address_buf
							, 0 
							, sizeof( tcode::io::ip::address )
							, sizeof( tcode::io::ip::address )
							, &local
							, &llen
							, &remote
							, &rlen );
					memcpy( addr.sockaddr() , remote , rlen );
					*(addr.sockaddr_length_ptr()) = rlen;
					return 0;
				}
				ec = tcode::last_error();
				delete accepted;
                accepted = nullptr;
			}
		}
		if ( fd != INVALID_SOCKET )
			closesocket(fd);
		fd = INVALID_SOCKET;
		return -1;
    }

	//!
	int completion_port::read(descriptor desc
		, tcode::io::buffer& buf
		, tcode::io::ip::address& addr
		, std::error_code& ec)
	{
		if (desc->fd == INVALID_SOCKET ) {
			ec = tcode::error_invalid;
		}
		else {
			int r = 0;
			do {
				r = ::recvfrom(desc->fd, buf.buf(), buf.len(), 0
					, addr.sockaddr(), addr.sockaddr_length_ptr());
			} while ((r == -1) && (errno == EINTR));
			if (r >= 0) return r;
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
				ec = tcode::error_success;
			else
				ec = tcode::last_error();
		}
		return -1;
	}

	//! 
	int completion_port::write(descriptor desc
		, const tcode::io::buffer& buf
		, const tcode::io::ip::address& addr
		, std::error_code& ec)
	{
		if (desc->fd == INVALID_SOCKET) {
			ec = tcode::error_invalid;
		}
		else {
			int r = 0;
			do {
				r = ::sendto(desc->fd, buf.buf(), buf.len(), 0
					, addr.sockaddr(), addr.sockaddr_length());
			} while ((r == -1) && (errno == EINTR));
			if (r >= 0) return r;
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
				ec = tcode::error_success;
			else
				ec = tcode::last_error();
		}
		return -1;
	}

    SOCKET completion_port::native_descriptor( descriptor d ) {
        return d->fd;
    }
            
	descriptor completion_port::descriptor_create(SOCKET fd) {
		descriptor d = new completion_port::_descriptor();
		d->fd = fd;
		d->refcount.store(1);
		_engine.add_ref();
		return d;
	}

	void completion_port::descriptor_add_ref(descriptor d) {
		d->refcount.fetch_add(1);
	}

	void completion_port::descriptor_release(descriptor d) {
		if (d->refcount.fetch_sub(1) != 1) {
			return;
		}
		_engine.release();
		delete d;
	}
}}
