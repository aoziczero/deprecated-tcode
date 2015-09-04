#ifndef __tcode_io_ip_udp_operation_write__
#define __tcode_io_ip_udp_operation_write__

#include <tcode/error.hpp>
#include <tcode/io/operation.hpp>
#include <tcode/io/buffer.hpp>
#include <tcode/io/ip/address.hpp>

namespace tcode { namespace io { namespace ip { namespace udp {

    /*!
     * @class operation_write_base
     * @brief
     */
    class operation_write_base 
        : public tcode::io::operation
    {
    public:
        operation_write_base( tcode::operation::execute_handler fn
                , const tcode::io::buffer& buf 
                , const tcode::io::ip::address& addr );
                
        ~operation_write_base( void );

        bool post_write_impl( io::multiplexer* impl
                , io::descriptor desc );

        int write_size(void);

        tcode::io::ip::address& address( void );

		tcode::io::buffer& buffer( void );

        static bool post_write( io::operation* op_base 
            , io::multiplexer* impl 
            , io::descriptor desc ) ;
    private:
        tcode::io::ip::address _address;
        tcode::io::buffer _buffer; 
        int _write;
    };

    template < typename Handler >
    class operation_write 
        : public operation_write_base 
    {
    public:
        operation_write( const tcode::io::buffer& buf
                , const tcode::io::ip::address& addr 
                , const Handler& handler )
            : operation_write_base( &operation_write::complete 
                    , buf , addr  )
            , _handler( handler )
        {
        }

        ~operation_write( void ){
        }

        static void complete( tcode::operation* op_base ) {
            operation_write* op(
                    static_cast< operation_write* >(op_base));
            Handler h( std::move( op->_handler ));
            std::error_code ec = op->error();
            int write = op->write_size(); 
            tcode::io::ip::address addr(op->address());
            op->~operation_write(); 
            tcode::operation::free( op );
            h( ec , write , addr );
        }
    private:
        Handler _handler;
    };

}}}}

#endif
