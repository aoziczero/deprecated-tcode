#ifndef __tcode_io_engine_h__
#define __tcode_io_engine_h__

#include <tcode/threading/spinlock.hpp>
#include <tcode/function.hpp>

namespace tcode { namespace io { 
    /*!
     *  @class  engine
     *  @brief  
     */
    class engine {
    public:
        engine( void );
        ~engine( void );

        typedef tcode::function<void () > operation;

        void post( const operation& op );

        void run( void );
    private:
        class op_queue {
        public:
            op_queue( void );
            ~op_queue( void );
            void post( const operation& op );
            void drain( void );
        private:
            tcode::threading::spinlock _lock;
            std::vector< operation > _ops;
            std::vector< operation > _swap;
        };
        op_queue _op_queue;
    };

}}

#endif
