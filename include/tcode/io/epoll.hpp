#ifndef __tcode_io_epoll_h__
#define __tcode_io_epoll_h__

#include <tcode/io/pipe.hpp>
#include <tcode/io/buffer.hpp>
#include <tcode/function.hpp>
#include <tcode/time/timespan.hpp>
#include <tcode/operation.hpp>
#include <tcode/threading/spinlock.hpp>

namespace tcode { namespace io {
namespace ip {

    namespace tcp {
        class operation_connect_base;
        class operation_write_base;
        class operation_read_base;
        class operation_accept_base;
    }
   
    namespace udp {
    }
}
    class engine;
    /*!
     * @class epoll
     * @brief
     */
    class epoll {
    public:
        /*!
         * @class descriptor
         * @brief 
         */
        struct _descriptor;
        typedef _descriptor* descriptor;

        //! ctor
        epoll( engine& e );
        //! dtor
        ~epoll( void );

        int run( const tcode::timespan& ts );

        void wake_up( void );

        bool bind( const descriptor& desc );
        void unbind( descriptor& desc );

        void execute( tcode::operation* op ); 

        void connect( descriptor& desc 
                , ip::tcp::operation_connect_base* op );
        void write( descriptor desc 
                , ip::tcp::operation_write_base* op );
        void read( descriptor desc
                , ip::tcp::operation_read_base* op );
        void accept( descriptor desc
                , ip::tcp::operation_accept_base* op );
    private:
        void op_add( tcode::operation* op );
        void op_run( tcode::operation* op );
    private:
        engine& _engine;
        int _handle;
        tcode::io::pipe _wake_up;
        tcode::threading::spinlock _lock;
        tcode::slist::queue< tcode::operation > _op_queue; 
    public:
        int writev( descriptor desc , tcode::io::buffer* buf , int cnt 
                , std::error_code& ec );
        int readv( descriptor desc , tcode::io::buffer* buf , int cnt 
                , std::error_code& ec ); 
    };

    /*
    class epoll {
    public:
        epoll( void );
        ~epoll( void );

        bool bind( int fd , int ev , tcode::io::event_handler* handler );
        void unbind( int fd );
        int run( const tcode::timespan& ts );
        void wake_up( void );
        void wake_up_handler( int ev );
    private:
        int _handle;
        io::pipe _pipe;
        tcode::function< void (int)> _pipe_handler;
    };
    */
    
    
    
}}

#endif
