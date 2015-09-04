#ifndef __tcode_io_completion_port_h__
#define __tcode_io_completion_port_h__

#include <tcode/io/buffer.hpp>
#include <tcode/time/timespan.hpp>
#include <tcode/operation.hpp>
#include <tcode/threading/spinlock.hpp>

namespace tcode { namespace io {
namespace ip {

    class address;

    namespace tcp {
        class operation_connect_base;
        class operation_write_base;
        class operation_read_base;
        class operation_accept_base;
    }
   
    namespace udp {
        class operation_read_base;
        class operation_write_base;
    }
}

	class engine;
    /*!
     * @class completion_port
     * @brief
     */
    class completion_port {
    public:
        /*!
         * @class descriptor
         * @brief 
         */
        struct _descriptor;
        typedef _descriptor* descriptor;

        //! ctor
        completion_port( engine& e );
        //! dtor
        ~completion_port( void );

        //! run complete events
        int run( const tcode::timespan& ts );

        //! wakeup loop
        void wake_up( void );

        //!  
        bool bind( const descriptor& desc );
        //!  
        void unbind( descriptor& desc );
        //!
        void execute( tcode::operation* op ); 
    public:
        //!
        void connect( descriptor& desc 
                , ip::tcp::operation_connect_base* op );
        //!
        void write( descriptor desc 
                , ip::tcp::operation_write_base* op );
        //!
        void read( descriptor desc
                , ip::tcp::operation_read_base* op );
        //!
        bool listen( descriptor& desc 
                , const ip::address& addr );
        //!
        void accept( descriptor listen
                , ip::tcp::operation_accept_base* op );
    public:
        //!
        bool bind( descriptor& desc
                , const ip::address& addr );
        //!
        void write( descriptor& desc 
                , ip::udp::operation_write_base* op );
        //!
        void read( descriptor desc
                , ip::udp::operation_read_base* op );

    public:
        //! post operation write
        int writev( descriptor desc , tcode::io::buffer* buf , int cnt 
                , std::error_code& ec );
        //! post operation read
        int readv( descriptor desc , tcode::io::buffer* buf , int cnt 
                , std::error_code& ec ); 
        //! post operation accept
        int accept( descriptor listen 
                , descriptor& accepted 
                , ip::address& addr
                , std::error_code& ec );
        int read( descriptor desc 
                , tcode::io::buffer& buf 
                , tcode::io::ip::address& addr 
                , std::error_code& ec );
        int write( descriptor desc 
                , tcode::io::buffer& buf 
                , tcode::io::ip::address& addr 
                , std::error_code& ec);
    private:
        void op_add( tcode::operation* op );
        void op_run( tcode::operation* op );
    private:
        engine& _engine;
        HANDLE _handle;
        tcode::threading::spinlock _lock;
        tcode::slist::queue< tcode::operation > _op_queue; 
    };

}}

#endif