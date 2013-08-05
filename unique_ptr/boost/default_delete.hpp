//
// default_delete.hpp
//
// Replicates or uses std::default_delete as defined in the C++11 standard.
//
// (c) 2013 Andrew Ho
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_DEFAULT_DELETE_HPP
#define BOOST_DEFAULT_DELETE_HPP

#include <boost/config.hpp>

#if defined(BOOST_NO_CXX11_SMART_PTR)
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#else
#include <memory>
#endif

namespace boost
{
#if defined(BOOST_NO_CXX11_SMART_PTR)
    // use custom unique_ptr implementation
    template<class T>
    struct default_delete
    {
    private:
        // Used for testing if templated copy constructor can participate in overload resolution
        struct nat
        {};

    public:
        default_delete(void)
        {}

        default_delete(const default_delete& d)
        {}

        /**
         * Technically allows an extra char
         */
        template<class U>
        default_delete(const default_delete<U>& d, typename ::boost::enable_if_c<
                ::boost::is_convertible<U*, T*>::value, nat>::type = nat())
        {
            //test_exists<>();
        }

        /**
         * Equivalent to: delete ptr;
         */
        void operator()(T* ptr) const
        {
            delete ptr;
        }
    };

    // array specialization
    template<class T>
    struct default_delete<T[]>
    {
    private:
        // Used for testing if templated copy constructor can participate in overload resolution
        struct nat
        {};

    public:
        default_delete(void)
        {}

        default_delete(const default_delete& d)
        {}

        template<class U>
        default_delete(const default_delete<U>& d, typename ::boost::enable_if_c<
                ::boost::is_convertible<U[], T[]>::value, nat>::type = nat())
        {
        }

        /**
         * Equivalent to delete[] ptr;
         */
        void operator()(T* ptr) const
        {
            delete[] ptr;
        }
    };
#else
    using std::default_delete;
#endif // BOOST_NO_CXX11_SMART_PTR
}

#endif // BOOST_DEFAULT_DELETE_HPP
