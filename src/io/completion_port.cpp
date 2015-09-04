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

        _descriptor( completion_port* mux , SOCKET fd  );

        void add_ref( void );
        void release( completion_port* mux );
    };

    completion_port::_descriptor::_descriptor( completion_port* mux , SOCKET fd ) {
        refcount.store(1);
        mux->_engine.active().inc();
        this->fd = fd;
    }

    void completion_port::_descriptor::add_ref( void ) {
        refcount.fetch_add(1);
    }

    void completion_port::_descriptor::release( completion_port* mux ){
        if ( refcount.fetch_sub(1) != 1 ) {
            return; 
        }
        mux->_engine.active().dec();
        delete this;
    }


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
			
		}
        struct completion_port_event events[completion_port_max_event];
        int nofd  = completion_port_wait( _handle 
            , events
            , completion_port_max_event
            , static_cast<int>(ts.total_milliseconds()));
        if ( nofd <= 0  ) {
            return 0; 
        }
        int run = nofd;
        bool execute_op = false;
        for ( int i = 0 ; i < nofd; ++i ){
            completion_port::descriptor desc =
                static_cast< completion_port::descriptor >( events[i].data.ptr );
            if ( desc ) {
                desc->complete( this , events[i].events );
            } else {
                execute_op = true;
                --run;
            }
        }
        if ( execute_op ){
            
        }
        return run;
    }
    
    void completion_port::wake_up( void ){
        struct completion_port_event e;
        e.events = completion_portOUT | completion_portONESHOT;
        e.data.ptr = nullptr;
        completion_port_ctl( _handle , completion_port_CTL_MOD , _wake_up.wr_pipe() , &e );
    }

    bool completion_port::bind( const descriptor& d ) {
        struct completion_port_event e;
        e.events = completion_portIN | completion_portOUT | completion_portET;
        e.data.ptr = d;
        return completion_port_ctl( _handle , completion_port_CTL_ADD , d->fd , &e ) == 0;
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
                            if ( desc->fd != -1 ) {
                                completion_port_ctl( _handle , completion_port_CTL_DEL , desc->fd , nullptr );
                                ::close( desc->fd );
                                desc->fd = -1;
                            }
                            desc->release( this );
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

    void completion_port::connect( 
            completion_port::descriptor& desc 
            , ip::tcp::operation_connect_base* op )
    {
        int fd = socket( op->address().family() , SOCK_STREAM , IPPROTO_TCP );
        if ( fd == -1 ) {
            op->error() = tcode::last_error(); 
            execute( op );
            return;
        }
        
        tcode::io::ip::option::non_blocking nb;
        nb.set_option( fd );

        int r;
        do {
            r = ::connect( fd , op->address().sockaddr() , op->address().sockaddr_length());
        }while((r == -1) && (errno == EINTR));
        if ( (r == 0 ) || (errno == EINPROGRESS )){
            desc = new completion_port::_descriptor(this , fd );
            desc->op_queue[tcode::io::ev_connect].push_back( op );
            bind( desc );
        } else {
            ::close(fd);
            op->error() = tcode::last_error();
            execute(op);
        }
    }

    void completion_port::write( completion_port::descriptor desc 
            , ip::tcp::operation_write_base* op)
    {
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_write].push_back(op);
                desc->complete( this , completion_portOUT );
                desc->release(this);
            }));
    }

    void completion_port::read( descriptor desc
            , ip::tcp::operation_read_base* op ){
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_read].push_back(op);
                desc->complete( this , completion_portIN );
                desc->release(this);
            }));

    }
    
    bool completion_port::listen( descriptor& desc 
                , const ip::address& addr )
    {
        int fd = socket( addr.family() , SOCK_STREAM , IPPROTO_TCP );
        if ( fd == -1 )
            return false;
        tcode::io::ip::option::non_blocking nb;
        nb.set_option(fd);
        tcode::io::ip::option::reuse_address reuse( true );
        reuse.set_option( fd );
        if ( ::bind( fd , addr.sockaddr() , addr.sockaddr_length() ) < 0 ){
            ::close(fd);
            return false;
        }
        if ( ::listen(fd, SOMAXCONN ) < 0 ) {
            ::close(fd);
            return false;
        }
        desc = new completion_port::_descriptor(this,fd);
        bind( desc );
        return true;
    }

    void completion_port::accept( descriptor desc
            , ip::tcp::operation_accept_base* op ){
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_accept].push_back(op);
                desc->complete( this , completion_portIN );
                desc->release(this);
            }));
    }
    
    bool completion_port::bind( descriptor& desc
            , const ip::address& addr ) 
    {
        int fd = socket( addr.family() , SOCK_DGRAM , IPPROTO_UDP );
        if ( fd == -1 )
            return false;
        tcode::io::ip::option::non_blocking nb;
        nb.set_option(fd);
        tcode::io::ip::option::reuse_address reuse( true );
        reuse.set_option( fd );
        if ( ::bind( fd , addr.sockaddr() , addr.sockaddr_length() ) < 0 ){
            ::close(fd);
            return false;
        }
        desc = new completion_port::_descriptor( this , fd );
        bind( desc );
        return true;
    }

    void completion_port::write( descriptor& desc 
            , ip::udp::operation_write_base* op )
    {   
        if ( desc == nullptr ) {
            int fd = socket( op->address().family() , SOCK_DGRAM , IPPROTO_UDP);
            if ( fd == -1 ){
                op->error() = tcode::last_error();
                execute( op );
                return;
            }
            desc = new completion_port::_descriptor( this , fd );
            bind( desc );
        }
        
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_write].push_back(op);
                desc->complete( this , completion_portOUT );
                desc->release(this);
            }));
    }
    
    void completion_port::read( descriptor desc
            , ip::udp::operation_read_base* op )
    {
        if ( desc == nullptr ) {
            op->error() = std::error_code( EBADF , std::generic_category());
            execute(op);
            return;
        }
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_read].push_back(op);
                desc->complete( this , completion_portIN);
                desc->release(this);
            }));
    }


    void completion_port::op_add( tcode::operation* op ){
        _op_queue.push_back( op );
        _engine.active().inc();
    }

    void completion_port::op_run( tcode::operation* op ){
        _engine.active().dec();
        (*op)();
    }

    int completion_port::writev( descriptor desc 
            , tcode::io::buffer* buf , int cnt
            , std::error_code& ec )
    {
        if ( desc->fd == -1 ) {
            ec = std::error_code( EBADF , std::generic_category() );
        } else {
            int r = 0;
            do {
                r = ::writev( desc->fd , buf , cnt );
            }while((r == -1) && (errno == EINTR));
            if ( r >= 0 ) return r;
            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                ec = std::error_code();
            else
                ec = std::error_code( errno , std::generic_category());
        }
        return -1;
    }

    int completion_port::readv( descriptor desc 
            , tcode::io::buffer* buf , int cnt
            , std::error_code& ec )
    {
        if ( desc->fd == -1 ) {
            ec = std::error_code( EBADF , std::generic_category() );
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
                    ec = std::error_code();
                else
                    ec = std::error_code( errno , std::generic_category());
            }
        }
        return -1;
    }

    int completion_port::accept( descriptor listen 
            , descriptor& accepted 
            , ip::address& addr
            , std::error_code& ec )
    {
        if ( listen->fd == -1 ) {
            ec = std::error_code( EBADF , std::generic_category() );
        } else {
            int fd = -1;
            do {
                fd = ::accept( listen->fd
                        , addr.sockaddr() 
                        , addr.sockaddr_length_ptr());
            }while( ( fd == -1 ) && ( errno == EINTR ));
            if ( fd != -1 ) {
                accepted = new completion_port::_descriptor( this , fd );
                bind( accepted );
                return 0;
            }
            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                ec = std::error_code(); 
            else
                ec = std::error_code( errno , std::generic_category());
        }
        return -1;
    }

    int completion_port::read( descriptor desc 
            , tcode::io::buffer& buf 
            , tcode::io::ip::address& addr 
            , std::error_code& ec )
    {
        if ( desc->fd == -1 ) {
            ec = std::error_code( EBADF , std::generic_category() );
        } else {
            int r = 0;
            do {
                r = ::recvfrom( desc->fd , buf.buf() , buf.len(),0
                        , addr.sockaddr() , addr.sockaddr_length_ptr());
            }while((r == -1) && (errno == EINTR));
            if ( r >= 0 ) return r;
            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                ec = std::error_code(); 
            else
                ec = std::error_code( errno , std::generic_category());
        }
        return -1;
    }

    int completion_port::write( descriptor desc 
            , tcode::io::buffer& buf 
            , tcode::io::ip::address& addr 
            , std::error_code& ec )
    {
        if ( desc->fd == -1 ) {
            ec = std::error_code( EBADF , std::generic_category() );
        } else {
            int r = 0;
            do {
                r = ::sendto( desc->fd , buf.buf() , buf.len() , 0
                        , addr.sockaddr() , addr.sockaddr_length());
            }while((r == -1) && (errno == EINTR));
            if ( r >= 0 ) return r;
            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                ec = std::error_code(); 
            else
                ec = std::error_code( errno , std::generic_category());
        }
        return -1;

        return 0;
    }
}}
