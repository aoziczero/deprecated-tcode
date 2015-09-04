#ifndef __tcode_io_ip_tcp_operation_write__
#define __tcode_io_ip_tcp_operation_write__

#include <tcode/error.hpp>
#include <tcode/io/operation.hpp>
#include <tcode/io/buffer.hpp>
#include <tcode/io/ip/address.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    /*!
     * @class operation_write_base
     * @brief
     */
    class operation_write_base 
        : public tcode::io::operation
    {
    public:
        operation_write_base( tcode::operation::execute_handler fn
                , tcode::io::buffer* buffer 
                , int buffercnt );
                
        ~operation_write_base( void );

        bool post_write_impl( io::multiplexer* impl
                , io::descriptor desc );

        void buffers( tcode::io::buffer* buf , int cnt );
        int write_size(void);

		tcode::io::buffer* buffers( void );
		int	buffers_count( void );

        static bool post_write( io::operation* op_base 
            , io::multiplexer* impl 
            , io::descriptor desc ) ;
    private:
        tcode::io::buffer* _buffer; 
        int _buffer_count;
        int _write;
    };

    template < typename Handler >
    class operation_write 
        : public operation_write_base 
    {
    public:
        operation_write( const tcode::io::buffer& buf
                , const Handler& handler )
            : operation_write_base( &operation_write::complete 
                    , &_buffer , 1 )
            , _handler( handler )
            , _buffer( buf )
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
            op->~operation_write(); 
            tcode::operation::free( op );
            h( ec , write );
        }
    private:
        tcode::io::buffer _buffer;
        Handler _handler;
    };

    template < typename Handler >
    class operation_writev 
        : public operation_write_base 
    {
    public:
        operation_writev( const std::vector<tcode::io::buffer>& buf
                , const Handler& handler )
            : operation_write_base( &operation_writev::complete , nullptr , 0 )
            , _handler( handler )
            , _buffer( buf )
        {
            buffers( &_buffer[0] , (int)_buffer.size());
        }

        ~operation_writev( void ){
        }

        static void complete( tcode::operation* op_base ) {
            operation_writev* op(
                    static_cast< operation_writev* >(op_base));
            Handler h( std::move( op->_handler ));
            std::error_code ec = op->error();
            int write = op->write_size(); 
            op->~operation_writev(); 
            tcode::operation::free( op );
            h( ec , write );
        }
    private:
        Handler _handler;
        std::vector< tcode::io::buffer > _buffer;
    };

}}}}

#endif
