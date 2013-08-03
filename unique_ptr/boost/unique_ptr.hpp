//
// unique_ptr.hpp
//
// (c) 2013 helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef UNIQUE_PTR_HPP_
#define UNIQUE_PTR_HPP_

#include <boost/config.hpp>

#define BOOST_NO_CXX11_SMART_PTR

#if defined(BOOST_NO_CXX11_SMART_PTR)
#include <boost/utility/enable_if.hpp>
#include <boost/move/move.hpp>
#include <boost/type_traits.hpp>
#else
#include <memory>
#endif

namespace boost
{
#if defined(BOOST_NO_CXX11_SMART_PTR)

#if defined(BOOST_NO_CXX11_NULLPTR)
#define BOOST_NULLPTR NULL
#else
#define BOOST_NULLPTR nullptr
#endif

    // use custom unique_ptr implementation
    template<class T>
    struct default_delete
    {
    private:
        template<typename U, typename EnableIfT>
        default_delete(const default_delete<U>& d, EnableIfT dummy)
        {

        }

    public:
        default_delete(void)
        {
        }

        template<class U>
        default_delete(const default_delete<U>& d) :
                default_delete<U>(d,
                        ::boost::enable_if_c<
                                ::boost::is_convertible<U*, T*>::value>::type)
        {
        }

        void operator()(T* ptr) const
        {
            delete ptr;
        }
    };

    template<class T>
    struct default_delete<T[]>
    {
    private:
        template<typename U, typename EnableIfT>
        default_delete(const default_delete<U>& d, EnableIfT dummy)
        {

        }
    public:
        default_delete(void)
        {
        }

        template<class U>
        default_delete(const default_delete<U>& d) :
                default_delete<U>(d,
                        ::boost::enable_if_c<
                                ::boost::is_convertible<U*, T*>::value>::type)
        {
        }

        default_delete(const default_delete<T>& d);

        void operator()(T* ptr) const
        {
            delete[] ptr;
        }
    };

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
            static yes& test(typename C::pointer*);

            template<typename >
            static no& test(int);
        public:
            static const bool value = sizeof(test<
                    typename ::boost::remove_reference<Deleter>::type>(0))
                    == sizeof(yes);
        };

        template<typename T, typename Deleter, bool use_ptr>
        struct pointer_type_switch;

        template<typename T, typename Deleter>
        struct pointer_type_switch<T, Deleter, true>
        {
            typedef const typename Deleter::pointer type;
        };

        template<typename T, typename Deleter>
        struct pointer_type_switch<T, Deleter, false>
        {
            typedef T* type;
        };
    }

    template<class T, class Deleter = ::boost::default_delete<T> >
    class unique_ptr
    {

    public:
        typedef typename ::boost::detail::pointer_type_switch<T, Deleter,
                ::boost::detail::has_pointer_type<Deleter>::value>::type pointer;

        typedef T element_type;
        typedef Deleter deleter_type;

        unique_ptr(void) :
                ptr(BOOST_NULLPTR), del()
        {

        }

#if !defined(BOOST_NO_CXX11_NULLPTR)
        unique_ptr(std::nullptr_t) :
                ::boost::unique_ptr<T, Deleter>()
        {

        }
#endif

        explicit unique_ptr(pointer ptr) :
                ptr(ptr), del()
        {

        }

        unique_ptr(pointer ptr,
                typename ::boost::conditional<
                        ::boost::is_reference<deleter_type>::value,
                        deleter_type, const deleter_type&>::type d1) :
                ptr(ptr), del(d1)
        {
        }

        unique_ptr(pointer ptr,
                BOOST_RV_REF(typename ::boost::remove_reference<deleter_type>::type)d2) :
        ptr(ptr), del(::boost::move(d2))
        {
        }

        ~unique_ptr(void)
        {
            del(ptr);
        }

    private:
        pointer ptr;
        deleter_type del;
    };

    template<class T, class Deleter>
    class unique_ptr<T[], Deleter>
    {

    public:
        typedef typename ::boost::detail::pointer_type_switch<T, Deleter,
                ::boost::detail::has_pointer_type<Deleter>::value>::type pointer;

        typedef T element_type;
        typedef Deleter deleter_type;

        unique_ptr(void) :
                ptr(BOOST_NULLPTR), del()
        {

        }

#if !defined(BOOST_NO_CXX11_NULLPTR)
        unique_ptr(std::nullptr_t) :
                ::boost::unique_ptr<T, Deleter>()
        {

        }
#endif

        explicit unique_ptr(pointer ptr) :
                ptr(ptr), del()
        {

        }

        unique_ptr(pointer ptr,
                typename ::boost::conditional<
                        ::boost::is_reference<deleter_type>::value,
                        deleter_type, const deleter_type&>::type d1) :
                ptr(ptr), del(d1)
        {
        }

        unique_ptr(pointer ptr,
                BOOST_RV_REF(typename ::boost::remove_reference<deleter_type>::type)d2) :
            ptr(ptr), del(::boost::move(d2))
            {
            }

            ~unique_ptr(void)
            {
                del(ptr);
            }

        private:
            pointer ptr;
            deleter_type del;
        };
#undef BOOST_NULLPTR
#else
// use standard library features
                using std::default_delete;
                using std::unique_ptr;
#endif
}
#endif // UNIQUE_PTR_HPP_
