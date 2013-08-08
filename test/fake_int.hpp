//
// fake_int.hpp
//
// Fake integer pointer deleter
//
// (c) 2013 Andrew Ho
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef FAKE_INT_HPP_
#define FAKE_INT_HPP_
#include <boost/move/move.hpp>

namespace boost
{
    template<typename T>
    class fake_int
    {
    BOOST_COPYABLE_AND_MOVABLE (fake_int)
    public:
        fake_int(void)
        {
        }

        fake_int(const fake_int&)
        {
        }

        fake_int( BOOST_RV_REF(fake_int))
        {
        }

        fake_int& operator =(const fake_int&)
        {
            return *this;
        }

        fake_int& operator =(BOOST_RV_REF(fake_int))
        {
            return *this;
        }

    typedef double* pointer;

        void operator()(pointer val) const
        {
            delete val;
        }
    };

    template<typename T>
    class fake_int<T[]>
    {
    BOOST_COPYABLE_AND_MOVABLE (fake_int)
    public:
        fake_int(void)
        {
        }

        fake_int(const fake_int&)
        {
        }

        fake_int( BOOST_RV_REF(fake_int))
        {
        }

        fake_int& operator =(const fake_int&)
        {
            return *this;
        }

        fake_int& operator =(BOOST_RV_REF(fake_int))
        {
            return *this;
        }

    typedef double* pointer;

        void operator()(pointer val) const
        {
            delete[] val;
        }
    };
}

#endif // FAKE_INT_HPP_
