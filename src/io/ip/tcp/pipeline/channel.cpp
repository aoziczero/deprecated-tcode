#include "stdafx.h"
#include <tcode/io/ip/tcp/pipeline/channel.hpp>
#include <tcode/io/ip/tcp/pipeline/pipeline.hpp>
#include <tcode/error.hpp>

namespace tcode{ namespace io { namespace ip { namespace tcp{ 

    const int error_flag = 0x10000000;
    const int close_flag = 0x20000000; 

    bool reset_if_val_contains_bit( std::atomic<int>& val , int bit ) {
        int exp = val.load();
        if ((exp & bit) != bit ) 
            return false;
        do {
            if ( val.compare_exchange_strong( exp , exp^bit) ) 
                return true;        
            else 
                if ((exp & bit)!= bit) 
                    return false;
        }while(true);
    }

    bool check_if_val_contains_bit( std::atomic<int>& val , int bit ) {
        int exp = 0; int v = 0;
        val.compare_exchange_strong( exp , v );
        return (exp & bit) == bit;
    }

    channel::channel( tcode::io::ip::tcp::socket&& fd  ) 
        : _fd( std::move(fd))
    {
        _flag.store( error_flag | close_flag );
        _pipeline.channel( this );
    }

    channel::~channel( void ) {
        _fd.close();
    }

    tcode::io::engine& channel::engine( void ){
        return _fd.engine();
    }

    tcp::pipeline& channel::pipeline( void ){
        return _pipeline;
    }

    void channel::close( void ){
        close( tcode::error_aborted );
    }

    void channel::close( const std::error_code& ec ){
        if ( reset_if_val_contains_bit( _flag , close_flag )) {
            engine().execute([this,ec]{ fire_on_close(ec); });
        }
    }

    void channel::add_ref( void ){
        _flag.fetch_add(1);
    }

    int channel::release( void ){
        int val = _flag.fetch_sub( 1 , std::memory_order::memory_order_release ) - 1;
        if ( val == 0 )
            engine().execute([this]{ fire_on_end_reference(); });	
        return val;
    }

    void channel::fire_on_open( const tcode::io::ip::address& addr ){

        add_ref();
        if ( engine().in_run_loop() ){
            _pipeline.fire_filter_on_open(addr);
            read();	
        } else {
            engine().execute([this,addr]{ fire_on_open(addr); });
        }
    }

    void channel::fire_on_end_reference( void ){
        _pipeline.fire_filter_on_end_reference();
        delete this;
    }

    void channel::fire_on_close( const std::error_code& ec  ){
        _fd.close();
        if ( reset_if_val_contains_bit( _flag , error_flag )) 
            _pipeline.fire_filter_on_error( ec );
        _pipeline.fire_filter_on_close();
        release();
    }

    void channel::do_write( tcode::byte_buffer buf ){
        if ( !check_if_val_contains_bit( _flag , close_flag )){
            return;
        }

        if ( engine().in_run_loop() ){
            bool needwrite = _write_buffers.empty();
            _write_buffers.push_back( buf );
            if ( !needwrite ) 
                return;
            write_remains();
        } else {
            add_ref();
            engine().execute([this,buf]{ 
                        do_write( buf ); 
                        release();
                    });
        }
    }

    void channel::read( void ) {
        if ( !check_if_val_contains_bit( _flag , close_flag ))
            return;

        _buffer.reserve( 4096 );
        add_ref();
        _fd.read( tcode::io::buffer(reinterpret_cast<char*>( _buffer.wr_ptr()), _buffer.space())
                , [this] ( const std::error_code& ec , int nbyte ){
                    handle_read( ec , nbyte );
                    release();
                });
        
    }

    void channel::handle_read( const std::error_code& ec 
            , const int nbyte  )
    {
        if ( !check_if_val_contains_bit( _flag , close_flag ))
            return;
        
        if ( ec )
            close(ec);
        else {
            _buffer.wr_ptr( nbyte );
            _pipeline.fire_filter_on_read( _buffer );	
            _buffer.clear(); 
            read();
        }
    }

    void channel::handle_write( const std::error_code& ec 
            , const int nbytes  )
    {	
        if ( !check_if_val_contains_bit( _flag , close_flag ))
            return;
        
        if ( ec )
            close(ec);
        else {
            int bytes = nbytes;
            auto it = _write_buffers.begin();
            while( it != _write_buffers.end() && bytes > 0 ) {
                bytes -= it->rd_ptr( bytes );
                ++it;
            }
            _write_buffers.erase( 
                    std::remove_if( _write_buffers.begin() , _write_buffers.end() , 
                        [] ( tcode::byte_buffer& buf ) -> bool {
                            return buf.length() == 0;
                        }) , _write_buffers.end());
            bool flush = _write_buffers.empty();
            _pipeline.fire_filter_on_write( nbytes , flush );
            if ( !flush ) 
                write_remains();
        }
    }

    void channel::write_remains(void){
        if ( !check_if_val_contains_bit( _flag , close_flag ))
            return;

        add_ref();
        if ( _write_buffers.size() == 1 ) {
            _fd.write( tcode::io::buffer( reinterpret_cast<char*>(_write_buffers[0].rd_ptr())
                        , _write_buffers[0].length()) 
                    , [this]( const std::error_code& ec , int nbyte ) {
                        handle_write( ec , nbyte );
                        release();
                    });
         } else {
            std::vector< tcode::io::buffer > write_bufs( _write_buffers.size());
            for ( std::size_t i = 0 ; i < _write_buffers.size() ; ++i ) {
                write_bufs.push_back( tcode::io::buffer( 
                            reinterpret_cast<char*>(_write_buffers[i].rd_ptr()) ,
                            _write_buffers[i].length()));
            }
            _fd.write( write_bufs 
                    , [this]( const std::error_code& ec , int nbyte ) {
                        handle_write( ec , nbyte );
                        release();
                    }); 
         }
    }

}}}}
