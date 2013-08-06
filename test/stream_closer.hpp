//
// stream_closer.hpp
//
// (c) 2013 Andrew Ho
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef STREAM_CLOSER_HPP_
#define STREAM_CLOSER_HPP_
#include <fstream>
#include <boost/move/move.hpp>

namespace boost
{
    class stream_closer
    {
        BOOST_COPYABLE_AND_MOVABLE (stream_closer)
    public:
        stream_closer(void)
        {
        }

        stream_closer(const stream_closer&)
        {
        }

        stream_closer( BOOST_RV_REF(stream_closer))
        {
        }

        stream_closer& operator =(const stream_closer&)
        {
            return *this;
        }

        stream_closer& operator =(BOOST_RV_REF(stream_closer))
        {
            return *this;
        }

        //typedef std::fstream* pointer;

        template<typename T>
        void operator()(T stream) const
        {
            stream->close();
        }
    };
}

#endif // STREAM_CLOSER_HPP_
