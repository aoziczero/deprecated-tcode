#include "stdafx.h"
#include <tcode/error.hpp>
#include <tcode/io/io.hpp>
#include <tcode/io/epoll.hpp>
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
#include <sys/epoll.h>
#include <unistd.h>

namespace tcode { namespace io {

    namespace {
        const int epoll_max_event = 256;
    }

    struct epoll::_descriptor{
        int fd;
        tcode::slist::queue< tcode::operation > op_queue[tcode::io::ev_max];
        std::atomic<int> refcount;
    };

    epoll::epoll( engine& en )
        : _handle( epoll_create(epoll_max_event))
        , _engine( en )
    {
        struct epoll_event e;
        e.events = EPOLLIN ;
        e.data.ptr = nullptr;
        epoll_ctl( _handle , EPOLL_CTL_ADD , _wake_up.rd_pipe() , &e );
    }

    epoll::~epoll( void ){
        ::close( _handle );
    }

    int epoll::run( const tcode::timespan& ts ){
        struct epoll_event events[epoll_max_event];
        int nofd  = epoll_wait( _handle 
            , events
            , epoll_max_event
            , static_cast<int>(ts.total_milliseconds()));
        if ( nofd <= 0  ) {
            return 0; 
        }
        int run = nofd;
        bool execute_op = false;
        for ( int i = 0 ; i < nofd; ++i ){
            epoll::descriptor desc =
                static_cast< epoll::descriptor >( events[i].data.ptr );
            if ( desc ) {
				descriptor_dispatch(desc, events[i].events);
            } else {
                char pipe_r[256];
                ::read( _wake_up.rd_pipe() , pipe_r , 256 );
                execute_op = true;
                --run;
            }
        }
        if ( execute_op ){
            tcode::slist::queue< tcode::operation > ops;
            do {
                tcode::threading::spinlock::guard guard(_lock);
                ops = std::move(_op_queue );
            }while(0);
            while( !ops.empty() ){
                tcode::operation* op = ops.front();
                ops.pop_front();
                op_run( op );
                ++run;
            }
        }
        return run;
    }
    
    void epoll::wake_up( void ){
        char ch = 0;
        ::write( _wake_up.wr_pipe() , &ch , 1 );
    }

    bool epoll::bind( const descriptor& d ) {
        struct epoll_event e;
        e.events = EPOLLIN | EPOLLOUT | EPOLLET;
        e.data.ptr = d;
        return epoll_ctl( _handle , EPOLL_CTL_ADD , d->fd , &e ) == 0;
    }

    void epoll::unbind( descriptor& d ) {
        do { 
            tcode::threading::spinlock::guard guard(_lock);
            if ( d == nullptr ){
                return;
            }
            descriptor desc = d;
            d = nullptr;
            op_add( tcode::operation::wrap( 
                    [this,desc] {
                        if ( desc->fd != -1 ) {
                            epoll_ctl( _handle , EPOLL_CTL_DEL , desc->fd , nullptr );
                            ::close( desc->fd );
                            desc->fd = -1;
                        }
						descriptor_release(desc);
                    })
            );
        }while(0);
        wake_up();
    }

    void epoll::execute( tcode::operation* op ) {
        do {
            tcode::threading::spinlock::guard guard(_lock);
            op_add(op);
        } while(0);
        wake_up();
    }

    void epoll::connect( 
            epoll::descriptor& desc 
            , ip::tcp::operation_connect_base* op )
    {
        int fd = socket( op->address().family() , SOCK_STREAM , IPPROTO_TCP );
        if ( fd == -1 ) {
            op->error() = tcode::last_error(); 
        } else {
            tcode::io::ip::option::non_blocking nb;
            nb.set_option( fd );
            int r;
            do {
                r = ::connect( fd , op->address().sockaddr() , op->address().sockaddr_length());
            }while((r == -1) && (errno == EINTR));
            if ( (r == 0) || (errno == EINPROGRESS )){
                desc = descriptor_create( fd );
                desc->op_queue[tcode::io::ev_connect].push_back( op );
                if ( !bind( desc ) ){
					::close(fd);
					op->error() = tcode::last_error();
					descriptor_release(desc);
					desc = nullptr;
				}
				return;
            }
            ::close(fd);
            op->error() = tcode::last_error();
        }
        execute(op);
    }

    void epoll::write( epoll::descriptor desc 
            , ip::tcp::operation_write_base* op)
    {
        descriptor_add_ref(desc);
        execute( tcode::operation::wrap(
            [this,desc,op]{
				descriptor_dispatch_write(desc, op);
            }));
    }

    void epoll::read( descriptor desc
            , ip::tcp::operation_read_base* op ){
        descriptor_add_ref(desc);
        execute( tcode::operation::wrap(
            [this,desc,op]{
				descriptor_dispatch_read(desc, op);
            }));

    }
    
    bool epoll::listen( descriptor& desc 
                , const ip::address& addr )
    {
        int fd = socket( addr.family() , SOCK_STREAM , IPPROTO_TCP );
        if ( fd != -1 ){
            tcode::io::ip::option::non_blocking nb;
            nb.set_option(fd);
            tcode::io::ip::option::reuse_address reuse( true );
            reuse.set_option( fd );
            if ((::bind( fd , addr.sockaddr() , addr.sockaddr_length() ) == 0)
                    && (::listen(fd, SOMAXCONN ) == 0 )) {
				desc = descriptor_create(fd);
                if ( bind( desc ))
                    return true;
				descriptor_release(desc);
                desc = nullptr;
            }
            ::close(fd);
        }
        return false;
    }

    void epoll::accept( descriptor desc
            , int
            , ip::tcp::operation_accept_base* op ){
        if ( desc == nullptr ){
            //todo
            return;
        }

        descriptor_add_ref(desc);
        execute( tcode::operation::wrap(
            [this,desc,op]{
				descriptor_dispatch_read(desc, op);
            }));
    }
    
    bool epoll::open( descriptor& desc
                , int af , int type , int proto )
    {
        int fd = socket( af , type , proto );
        if ( fd != -1 ) {
            tcode::io::ip::option::non_blocking nb;
            nb.set_option(fd);
			desc = descriptor_create(fd);
            if ( bind(desc))
                return true;
			descriptor_release(desc);
			desc = nullptr;
            ::close(fd);
        }
        return false;
    }
        
    
    bool epoll::bind( descriptor& desc
            , const ip::address& addr ) 
    {
        int fd = socket( addr.family() , SOCK_DGRAM , IPPROTO_UDP );
        if ( fd != -1 ) {
            tcode::io::ip::option::non_blocking nb;
            nb.set_option(fd);
            tcode::io::ip::option::reuse_address reuse( true );
            reuse.set_option( fd );
            if ( ::bind( fd , addr.sockaddr() , addr.sockaddr_length() ) == 0 ){
                desc = descriptor_create(fd);
                if ( bind(desc))
                    return true;
				descriptor_release(desc);
				desc = nullptr;
            }
            ::close(fd);
        }
        return false;
    }

    void epoll::write( descriptor& desc 
            , ip::udp::operation_write_base* op )
    {   
        if ( desc == nullptr ) {
            int fd = socket( op->address().family() , SOCK_DGRAM , IPPROTO_UDP);
            if ( fd != -1 ){
                desc = descriptor_create(fd);
                if ( !bind( desc )){
                    op->error() = tcode::last_error();
					descriptor_release(desc);
					desc = nullptr;
                    ::close(fd);
                }
            }else{
                op->error() = tcode::last_error();
            }
            if ( desc == nullptr ){
                return execute(op);
            }
        }
        
        descriptor_add_ref(desc);
        execute( tcode::operation::wrap(
            [this,desc,op]{
				descriptor_dispatch_write(desc, op);
            }));
    }
    
    void epoll::read( descriptor desc
            , ip::udp::operation_read_base* op )
    {
        if ( desc == nullptr ) {
            op->error() = tcode::error_invalid; 
            return execute(op);
        }
        descriptor_add_ref(desc);
        execute( tcode::operation::wrap(
            [this,desc,op]{
				descriptor_dispatch_read(desc, op);
            }));
    }


    void epoll::op_add( tcode::operation* op ){
        _op_queue.push_back( op );
        _engine.add_ref();
    }

    void epoll::op_run( tcode::operation* op ){
        _engine.release();
        (*op)();
    }

    int epoll::writev( descriptor desc 
            , tcode::io::buffer* buf , int cnt
            , std::error_code& ec )
    {
        if ( desc->fd == -1 ) {
            ec = tcode::error_invalid;
        } else {
            int r = 0;
            do {
                r = ::writev( desc->fd , buf , cnt );
            }while((r == -1) && (errno == EINTR));
            if ( r >= 0 ) return r;
            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                ec = tcode::error_success;
            else
                ec = tcode::last_error();
        }
        return -1;
    }

    int epoll::readv( descriptor desc 
            , tcode::io::buffer* buf , int cnt
            , std::error_code& ec )
    {
        if ( desc->fd == -1 ) {
            ec = tcode::error_invalid;
        } else {
            int r = 0;
            do {
                r = ::readv( desc->fd , buf , cnt );
            }while((r == -1) && (errno == EINTR));
            if ( r > 0 ) return r;
            if ( r == 0 ){
                ec = tcode::error_disconnected;
            } else {
                if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                    ec = tcode::error_success;
                else
                    ec = tcode::last_error();
            }
        }
        return -1;
    }

    int epoll::accept( descriptor listen 
            , descriptor& accepted 
            , ip::address& addr
            , std::error_code& ec )
    {
        if ( listen->fd == -1 ) {
            ec = tcode::error_invalid;
        } else {
            int fd = -1;
            do {
                fd = ::accept( listen->fd
                        , addr.sockaddr() 
                        , addr.sockaddr_length_ptr());
            }while( ( fd == -1 ) && ( errno == EINTR ));
            if ( fd != -1 ) {
                tcode::io::ip::option::non_blocking nb;
                nb.set_option( fd );

                accepted = descriptor_create(fd);
                bind( accepted );
                return 0;
            }
            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                ec = tcode::error_success; 
            else
                ec = tcode::last_error();
        }
        return -1;
    }

    int epoll::read( descriptor desc 
            , tcode::io::buffer& buf 
            , tcode::io::ip::address& addr 
            , std::error_code& ec )
    {
        if ( desc->fd == -1 ) {
            ec = tcode::error_invalid;
        } else {
            int r = 0;
            do {
                r = ::recvfrom( desc->fd , buf.buf() , buf.len(),0
                        , addr.sockaddr() , addr.sockaddr_length_ptr());
            }while((r == -1) && (errno == EINTR));
            if ( r >= 0 ) return r;
            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                ec = tcode::error_success; 
            else
                ec = tcode::last_error();
        }
        return -1;
    }

    int epoll::write( descriptor desc 
            , const tcode::io::buffer& buf 
            , const tcode::io::ip::address& addr 
            , std::error_code& ec )
    {
        if ( desc->fd == -1 ) {
            ec = tcode::error_invalid;
        } else {
            int r = 0;
            do {
                r = ::sendto( desc->fd , buf.buf() , buf.len() , 0
                        , addr.sockaddr() , addr.sockaddr_length());
            }while((r == -1) && (errno == EINTR));
            if ( r >= 0 ) return r;
            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                ec = tcode::error_success; 
            else
                ec = tcode::last_error();
        }
        return -1;
    }
    
    int epoll::native_descriptor( descriptor desc ) {
        return desc->fd;
    }

	descriptor epoll::descriptor_create(int fd) {
		descriptor d = new epoll::_descriptor();
		d->fd = fd;
		d->refcount.store(1);
		_engine.add_ref();
		return d;
	}

	void epoll::descriptor_add_ref(descriptor d) {
		d->refcount.fetch_add(1);
	}

	void epoll::descriptor_release(descriptor d) {
		if (d->refcount.fetch_sub(1) != 1) {
			return;
		}
		_engine.release();

		tcode::slist::queue< tcode::operation > ops;
		for (int i = 0; i < tcode::io::ev_max; ++i) {
			while (!d->op_queue[i].empty()) {
				io::operation* op = d->op_queue[i].front< io::operation >();
				d->op_queue[i].pop_front();
				if ( !op->error())
					op->error() = tcode::error_aborted;
				ops.push_back(op);
			}
		}

		if (!ops.empty()) {
			do {
				tcode::threading::spinlock::guard guard(_lock);
				while (!ops.empty()) {
					tcode::operation* op = ops.front();
					ops.pop_front();
					op_add(op);
				}
			} while (0);
			wake_up();
		}
		delete d;
	}

	void epoll::descriptor_dispatch_read(descriptor d, tcode::operation* op) {
		d->op_queue[tcode::io::ev_read ].push_back(op);
		descriptor_dispatch(d, EPOLLIN);
		descriptor_release(d);
	}

	void epoll::descriptor_dispatch_write(descriptor d, tcode::operation* op) {
		d->op_queue[tcode::io::ev_write].push_back(op);
		descriptor_dispatch(d, EPOLLOUT);
		descriptor_release(d);
	}

	void epoll::descriptor_dispatch(descriptor d, int events) {
		static const int epollev[] = {
			EPOLLIN , EPOLLOUT , EPOLLPRI
		};
		for (int i = 0; i < tcode::io::ev_max; ++i) {
			if (events & epollev[i]) {
				while (!d->op_queue[i].empty()) {
					io::operation* op = d->op_queue[i].front<io::operation>();
					if (op->post_proc(this, d) {
						d->op_queue[i].pop_front();
						(*op)();
					}
					else {
						break;
					}
				}
			}
		}
	}
}}
