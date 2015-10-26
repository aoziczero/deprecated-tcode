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

        //! bind iocp
        bool bind( const descriptor& desc );
        //! unbind
        void unbind( descriptor& desc );
        //! execute op on run loop context
        void execute( tcode::operation* op ); 
    public:
        //! tcp connect
        void connect( descriptor& desc 
                , ip::tcp::operation_connect_base* op );
        //! tcp write
        void write( descriptor desc 
                , ip::tcp::operation_write_base* op );
        //! tcp read 
        void read( descriptor desc
                , ip::tcp::operation_read_base* op );
        //! acceptor listen
        bool listen( descriptor& desc 
                , const ip::address& addr );
        //! do accept
        void accept( descriptor listen
				, int family
                , ip::tcp::operation_accept_base* op );
    public:
        //!
		bool open(descriptor& desc
			, int af, int type, int proto);
    
        //! udp bind
        bool bind( descriptor& desc
                , const ip::address& addr );
        //! udp write
        void write( descriptor& desc 
                , ip::udp::operation_write_base* op );
        //! udp read
        void read( descriptor desc
                , ip::udp::operation_read_base* op );
    public:       
        //! post operation accept
        int accept( descriptor listen 
                , descriptor& accepted 
                , ip::address& addr
				, SOCKET& fd
				, char* address_buf
                , std::error_code& ec );

		//!
		int read(descriptor desc
			, tcode::io::buffer& buf
			, tcode::io::ip::address& addr
			, std::error_code& ec);

		//! 
		int write(descriptor desc
			, const tcode::io::buffer& buf
			, const tcode::io::ip::address& addr
			, std::error_code& ec);
        
        SOCKET native_descriptor( descriptor desc );
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
