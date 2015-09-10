#include "stdafx.h"
#include <tcode/io/ip/tcp/pipeline/channel.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp { namespace pipeline {

    channel::channel( tcp::socket&& s )
        : _socket( std::move(s))
    {
    }

    channel::~channel( void ){
    }

}}}}}
