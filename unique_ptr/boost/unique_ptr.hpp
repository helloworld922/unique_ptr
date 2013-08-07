//
// unique_ptr.hpp
//
// (c) 2013 Andrew Ho
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_UNIQUE_PTR_HPP
#define BOOST_UNIQUE_PTR_HPP

#include <boost/config.hpp>

#include <boost/default_delete.hpp>

#if defined(BOOST_NO_CXX11_SMART_PTR)
//#include <boost/move/move.hpp>
#include <boost/static_assert.hpp>
#include <functional>
//#include <boost/type_traits.hpp>
#endif

#include <memory>

namespace boost
{
#if defined(BOOST_NO_CXX11_SMART_PTR)

#if defined(BOOST_NO_CXX11_NULLPTR)
#define BOOST_NULLPTR NULL
#else
#define BOOST_NULLPTR nullptr
#endif

#define BOOST_COMMA ,

    namespace detail
    {
        // http://stackoverflow.com/a/3980926/558546
        // thanks to GManNickG for this solution for selectively typedef'ing
        template<typename Deleter>
        class has_pointer_type
        {
            typedef char yes[1];
            typedef char no[2];
            template<typename C>
            static yes& test(typename C::pointer);

            template<typename>
            static no& test(...);
        public:
            static const bool value = sizeof(test<typename ::boost::remove_reference<Deleter>::type>(0)) == sizeof(yes);
        };

        template<typename T, typename Deleter, bool use_ptr>
        struct pointer_type_switch;

        template<typename T, typename Deleter>
        struct pointer_type_switch<T, Deleter, true>
        {
            typedef typename Deleter::pointer type;
        };

        template<typename T, typename Deleter>
        struct pointer_type_switch<T, Deleter, false>
        {
            typedef T* type;
        };
    }
}
#include <boost/unique_ptr/detail/uptr_base.hpp>
#include <boost/unique_ptr/detail/uptr_arrays.hpp>
#include <boost/unique_ptr/detail/uptr_comparison.hpp>

#undef BOOST_NULLPTR
#undef BOOST_COMMA
#else
// use standard library features
using std::unique_ptr;
}
#endif

#endif // BOOST_UNIQUE_PTR_HPP
