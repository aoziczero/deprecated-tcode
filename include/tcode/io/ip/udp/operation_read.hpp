#ifndef __tcode_io_ip_udp_operation_read__
#define __tcode_io_ip_udp_operation_read__

#include <tcode/error.hpp>
#include <tcode/io/operation.hpp>
#include <tcode/io/buffer.hpp>
#include <tcode/io/ip/address.hpp>

namespace tcode { namespace io { namespace ip { namespace udp {

    /*!
     * @class operation_read_base
     * @brief
     */
    class operation_read_base 
        : public tcode::io::operation
    {
    public:
        //! ctor
        operation_read_base( tcode::operation::execute_handler fn
                , const tcode::io::buffer& buffer );
        //! dtor
        ~operation_read_base( void );

        //!
        bool post_read_impl( io::multiplexer* impl
                , io::descriptor desc );

        int read_size(void);

        tcode::io::ip::address& address( void );

		tcode::io::buffer& buffer( void );

        static bool post_read( io::operation* op_base 
            , io::multiplexer* impl 
            , io::descriptor desc ) ;
    private:
        tcode::io::ip::address _address;
        tcode::io::buffer _buffer; 
        int _read;
    };

    /*!
     * @class operation_read
     * @brief 
     */
    template < typename Handler >
    class operation_read 
        : public operation_read_base 
    {
    public:
        operation_read( const tcode::io::buffer& buf
                , const Handler& handler )
            : operation_read_base( &operation_read::complete , buf )
            , _handler( handler )
        {
        }

        ~operation_read( void ){
        }

        static void complete( tcode::operation* op_base ) {
            operation_read* op(
                    static_cast< operation_read* >(op_base));
            Handler h( std::move( op->_handler ));
            std::error_code ec = op->error();
            int read = op->read_size(); 
            tcode::io::ip::address addr( op->address());
            op->~operation_read(); 
            tcode::operation::free( op );
            h( ec , read , addr );
        }
    private:
        Handler _handler;
    };


}}}}

#endif
