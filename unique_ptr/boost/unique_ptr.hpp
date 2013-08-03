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

#if defined(BOOST_NO_CXX11_SMART_PTR)
#include <boost/default_delete.hpp>
//#include <boost/utility/enable_if.hpp>
#include <boost/move/move.hpp>
#include <functional>
//#include <boost/preprocessor/comma.hpp>
//#include <boost/type_traits.hpp>
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
    BOOST_MOVABLE_BUT_NOT_COPYABLE(unique_ptr)
    public:
        typedef typename ::boost::detail::pointer_type_switch<T, Deleter,
                ::boost::detail::has_pointer_type<Deleter>::value>::type pointer;

        typedef T element_type;
        typedef Deleter deleter_type;

        Deleter& get_deleter(void)
        {
            return del;
        }

        const Deleter& get_deleter(void) const
        {
            return del;
        }

        pointer release(void)
        {
            pointer tmp = ptr;
            ptr = BOOST_NULLPTR;
            return tmp;
        }

        pointer get(void) const
        {
            return ptr;
        }

        void reset(pointer p = pointer())
        {
            pointer old_ptr = ::boost::move(ptr);
            ptr = p;
            if (old_ptr != BOOST_NULLPTR)
            {
                get_deleter()(old_ptr);
            }
        }

        void swap(unique_ptr& other)
        {
            if (this != &other)
            {
                // swap managed objects
                pointer tmp = ptr;
                ptr = other.ptr;
                other.ptr = tmp;
                // swap deleter
                deleter_type d = del;
                del = ::boost::move(other.del);
                other.del = ::boost::move(d);
            }
        }

        typename ::boost::add_lvalue_reference<T>::type operator*(void) const
        {
            return *get();
        }

        pointer operator->(void) const
        {
            return get();
        }

#if defined(BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)
        operator bool(void) const
        {
            return get() != BOOST_NULLPTR;
        }
#else
        explicit operator bool(void) const
        {
            return get() != BOOST_NULLPTR;
        }
#endif

        unique_ptr(void) :
                ptr(), del()
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

        unique_ptr(BOOST_RV_REF(unique_ptr) u) : ptr(::boost::move(u.ptr)), del(::boost::move(u.del))
        {
            u.ptr = pointer();
        }

        template<typename U, typename E>
        unique_ptr(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E>) u) : ptr(::boost::move(u.ptr)), del(::boost::move(u.del))
        {
            u.ptr = U::pointer();
        }

        ~unique_ptr(void)
        {
            if(get() != BOOST_NULLPTR)
            {
                get_deleter()(get());
            }
        }

    private:
        pointer ptr;
        deleter_type del;
    };

    template<class T, class Deleter>
    class unique_ptr<T[], Deleter>
    {
    BOOST_MOVABLE_BUT_NOT_COPYABLE(unique_ptr)
    public:
        typedef typename ::boost::detail::pointer_type_switch<T, Deleter,
                ::boost::detail::has_pointer_type<Deleter>::value>::type pointer;

        typedef T element_type;
        typedef Deleter deleter_type;

        Deleter& get_deleter(void)
        {
            return del;
        }

        const Deleter& get_deleter(void) const
        {
            return del;
        }

        pointer release(void)
        {
            pointer tmp = ptr;
            ptr = BOOST_NULLPTR;
            return tmp;
        }

        pointer get(void) const
        {
            return ptr;
        }

        void reset(pointer p = pointer())
        {
            pointer old_ptr = ::boost::move(ptr);
            ptr = p;
            if (old_ptr != BOOST_NULLPTR)
            {
                get_deleter()(old_ptr);
            }
        }

        void swap(unique_ptr& other)
        {
            if (this != &other)
            {
                // swap managed objects
                pointer tmp = ptr;
                ptr = other.ptr;
                other.ptr = tmp;
                // swap deleter
                deleter_type d = del;
                del = ::boost::move(other.del);
                other.del = ::boost::move(d);
            }
        }

        typename ::boost::add_lvalue_reference<T>::type operator*(void) const
        {
            return *get();
        }

        pointer operator->(void) const
        {
            return get();
        }

        T& operator[](size_t i) const
        {
            return get()[i];
        }

#if defined(BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)
        operator bool(void) const
        {
            return get() != BOOST_NULLPTR;
        }
#else
        explicit operator bool(void) const
        {
            return get() != BOOST_NULLPTR;
        }
#endif

        unique_ptr(void) :
                ptr(), del()
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

        unique_ptr(BOOST_RV_REF(unique_ptr) u) : ptr(::boost::move(u.ptr)), del(::boost::move(u.del))
        {
            u.ptr = pointer();
        }

        template<typename U, typename E>
        unique_ptr(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E>) u) : ptr(::boost::move(u.ptr)), del(::boost::move(u.del))
        {
            u.ptr = U::pointer();
        }

        ~unique_ptr(void)
        {
            if(get() != BOOST_NULLPTR)
            {
                get_deleter()(get());
            }
        }

    private:
        pointer ptr;
        deleter_type del;
    };

    template<class T1, class D1, class T2, class D2>
    bool operator==(const ::boost::unique_ptr<T1, D1>& lhs,
            const ::boost::unique_ptr<T2, D2>& rhs)
    {
        return lhs.get() == rhs.get();
    }

    template<class T1, class D1, class T2, class D2>
    bool operator!=(const ::boost::unique_ptr<T1, D1>& lhs,
            const ::boost::unique_ptr<T2, D2>& rhs)
    {
        return lhs.get() != rhs.get();
    }

    template<class T1, class D1, class T2, class D2>
    bool operator<(const ::boost::unique_ptr<T1, D1>& lhs,
            const ::boost::unique_ptr<T2, D2>& rhs)
    {
        return std::less<
                typename ::boost::common_type<
                        typename ::boost::unique_ptr<T1, D1>::pointer,
                        typename ::boost::unique_ptr<T2, D2>::pointer>::type>(
                lhs.get(), rhs.get());
    }

    template<class T1, class D1, class T2, class D2>
    bool operator<=(const ::boost::unique_ptr<T1, D1>& lhs,
            const ::boost::unique_ptr<T2, D2>& rhs)
    {
        return !(rhs < lhs);
    }

    template<class T1, class D1, class T2, class D2>
    bool operator>(const ::boost::unique_ptr<T1, D1>& lhs,
            const ::boost::unique_ptr<T2, D2>& rhs)
    {
        return rhs < lhs;
    }

    template<class T1, class D1, class T2, class D2>
    bool operator>=(const ::boost::unique_ptr<T1, D1>& lhs,
            const ::boost::unique_ptr<T2, D2>& rhs)
    {
        return !(lhs < rhs);
    }

// TODO: how do we compare vs. nullptr_t?

#undef BOOST_NULLPTR
#undef BOOST_COMMA
#else
// use standard library features
using std::unique_ptr;
#endif
}
#endif // UNIQUE_PTR_HPP_
