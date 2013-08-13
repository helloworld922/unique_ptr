//
// unique_ptr.hpp
//
// Special thanks to Howard Hinnant's unique_ptr emulation (http://home.roadrunner.com/~hinnant/unique_ptr03.html)
//
// There are some implementation details based off of his implementation.
//
// (c) 2013 Andrew Ho
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_UNIQUE_PTR_HPP
#define BOOST_UNIQUE_PTR_HPP

//#include <boost/config.hpp>

#include <boost/default_delete.hpp>

#if defined(BOOST_NO_CXX11_SMART_PTR)
//#include <boost/move/move.hpp>
#include <boost/static_assert.hpp>
//#include <functional>
//#include <boost/type_traits.hpp>
#endif

//#include <memory>

namespace boost
{
#if defined(BOOST_NO_CXX11_SMART_PTR)

#if defined(BOOST_NO_CXX11_NULLPTR)
#define BOOST_NULLPTR NULL
#define BOOST_NULLPTR_TYPE nullptr_nat*
    class nullptr_nat;
#else
#define BOOST_NULLPTR nullptr
#define BOOST_NULLPTR_TYPE std::nullptr_t
#endif

//#define BOOST_COMMA ,

    namespace uptr_detail
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

        // currently boost::forward doesn't handle lvalue refs right

        //////////////////////////////////////////////////////////////////////////////
        //
        //                            forward()
        //
        //////////////////////////////////////////////////////////////////////////////
#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        // Is a boost::rv, forward as boost::rv
        template <class T>
        inline typename ::boost::move_detail::enable_if_c
            < enable_move_utility_emulation<T>::value && !::boost::move_detail::is_rv<T>::value, BOOST_RV_REF(T) >::type
        forward(BOOST_RV_REF(T) x) BOOST_NOEXCEPT
        {
           return x;
        }

        // if T is not a lvalue ref, not an rv value, T can be movable
        // forward as boost::rv
        template <class T>
        inline typename enable_if_c
            < !is_lvalue_reference<T>::value && enable_move_utility_emulation<T>::value &&
            !::boost::move_detail::is_rv<T>::value && has_move_emulation_enabled<T>::value, BOOST_RV_REF(T)>::type
        forward(T& x) BOOST_NOEXCEPT
        {
           return boost::move(x);
        }

        // if T is not a lvalue ref, not an rv value, T isn't movable
        // forward as T&
        template <class T>
        inline typename enable_if_c
           < !is_lvalue_reference<T>::value && enable_move_utility_emulation<T>::value &&
           !::boost::move_detail::is_rv<T>::value && !has_move_emulation_enabled<T>::value, T&>::type
           forward(T& x) BOOST_NOEXCEPT
        {
           return x;
        }

        // if T is not an rv, is a lvalue ref
        // forward as T&
        template <class T>
        inline typename ::boost::move_detail::enable_if_c
           < !::boost::move_detail::is_rv<T>::value && ::boost::move_detail::is_lvalue_reference<T>::value, T>::type
               forward(T x) BOOST_NOEXCEPT
        {
           return x;
        }
#else
        using std::forward;
#endif
    }
}

#include <boost/unique_ptr/detail/uptr_base.hpp>
#include <boost/unique_ptr/detail/uptr_arrays.hpp>
#include <boost/unique_ptr/detail/uptr_comparison.hpp>

#undef BOOST_NULLPTR
#undef BOOST_NULLPTR_TYPE
#undef BOOST_COMMA
#else
// use standard library features
using std::unique_ptr;
}
#endif

#endif // BOOST_UNIQUE_PTR_HPP
