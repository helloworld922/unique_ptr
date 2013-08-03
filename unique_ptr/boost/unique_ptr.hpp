//
// unique_ptr.hpp
//
// (c) 2013 Andrew Ho
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef UNIQUE_PTR_HPP_
#define UNIQUE_PTR_HPP_

#include <boost/config.hpp>

#if defined(BOOST_NO_CXX11_SMART_PTR)
#include <boost/default_delete.hpp>
#include <boost/move/move.hpp>
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

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        typename ::boost::conditional<boost::is_reference<Deleter>::value,
                Deleter, typename ::boost::remove_reference<Deleter>::type&>::type get_deleter(
                void)
        {
            return del;
        }

        const typename ::boost::conditional<boost::is_reference<Deleter>::value,
                Deleter, typename ::boost::remove_reference<Deleter>::type&>::type get_deleter(
                void) const
        {
            return del;
        }
#else
        deleter_type& get_deleter(
                void)
        {
            return del;
        }

        const deleter_type& get_deleter(
                void) const
        {
            return del;
        }
#endif

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
                del = ::boost::forward<Deleter>(other.del);
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
        // safe bool idiom
    private:
        typedef void (unique_ptr::*bool_type)() const;
        void this_type_does_not_support_comparisons() const
        {
        }
    public:
        operator bool_type(void) const
        {
            return (get() != BOOST_NULLPTR) ?
                    (&unique_ptr::this_type_does_not_support_comparisons) :
                    BOOST_NULLPTR;
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
                        deleter_type,
                        const typename ::boost::remove_reference<deleter_type>::type&>::type d1) :
                ptr(ptr), del(d1)
        {
        }

        unique_ptr(pointer ptr,
        BOOST_RV_REF(typename ::boost::remove_reference<deleter_type>::type)d2) :
        ptr(ptr), del(::boost::move(d2))
        {
        }

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        // TODO: why can't this be move?
        unique_ptr(BOOST_RV_REF(unique_ptr) u) : ptr(u.release()), del(::boost::move(u.del))
        {
        }

        // TODO: really should be forward<E>(u.get_deleter()), how can we emulate this in C++03?
        template<typename U, typename E>
        unique_ptr(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E>) u) : ptr(u.release()), del(boost::move(u.del))
        {
        }

        // TODO: really should be forward<E>(u.get_deleter()), how can we emulate this in C++03?
        template<typename U, typename E>
        unique_ptr(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E&>) u) : ptr(u.release()), del(u.del)
        {
        }

        // TODO: really should be forward<E>(u.get_deleter()), how can we emulate this in C++03?
        template<typename U, typename E>
        unique_ptr(BOOST_RV_REF(unique_ptr<U BOOST_COMMA const E&>) u) : ptr(u.release()), del(u.del)
        {
        }
#else
        unique_ptr(unique_ptr&& u) : ptr(::boost::move(u.release())), del(std::forward<Deleter>(u.del))
        {
        }

        template<typename U, typename E>
        unique_ptr(unique_ptr<U, E>&& u) : ptr(::boost::move(u.release())), del(std::forward<E>(u.del))
        {
        }
#endif

        template<typename U>
        unique_ptr(BOOST_RV_REF(std::auto_ptr<U>) u) : ptr(::boost::move(u.release())), del()
        {
        }

        ~unique_ptr(void)
        {
            if(get() != BOOST_NULLPTR)
            {
                get_deleter()(get());
            }
        }

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        unique_ptr& operator=(BOOST_RV_REF(unique_ptr) r)
        {
            if(this != &r)
            {
                reset(r.release());
                // TODO: why can't this be move?
                del = ::boost::forward<Deleter>(r.get_deleter());
            }
            return *this;
        }

        template<class U, class E>
        unique_ptr& operator=(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E>) r)
        {
            if(this != &r)
            {
                reset(r.release());
                // TODO: really should be forward(u.get_deleter()), how can we emulate this in C++03?
                del = boost::forward<E>(r.get_deleter());
            }
            return *this;
        }
#else
        unique_ptr& operator=(unique_ptr&& r)
        {
            if(this != &r)
            {
                reset(r.release());
                // forward deleter
                del = std::forward<deleter_type>(r.get_deleter());
            }
            return *this;
        }

        template<class U, class E>
        unique_ptr& operator=(unique_ptr<U, E>&& r)
        {
            if(this != &r)
            {
                reset(r.release());
                // forward deleter
                del = std::forward<E>(r.get_deleter());
            }
            return *this;
        }
#endif

#if !defined(BOOST_NO_CXX11_NULLPTR)
        unique_ptr& operator=(std::nullptr_t)
        {
            reset();
            return *this;
        }
#endif

        //private:
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
        // safe bool idiom
    private:
        typedef void (unique_ptr::*bool_type)() const;
        void this_type_does_not_support_comparisons() const
        {
        }
    public:
        operator bool_type(void) const
        {
            return (get() != BOOST_NULLPTR) ?
                    (&unique_ptr::this_type_does_not_support_comparisons) :
                    BOOST_NULLPTR;
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

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        unique_ptr(BOOST_RV_REF(unique_ptr) u) : ptr(u.release()), del(::boost::forward<Deleter>(u.get_deleter()))
        {
        }

        // TODO: really should be forward<E>(u.get_deleter()), how can we emulate this in C++03?
        template<typename U, typename E>
        unique_ptr(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E>) u) : ptr(u.release()), del(boost::forward<E>(u.get_deleter()))
        {
        }
#else
        unique_ptr(unique_ptr&& u) : ptr(::boost::move(u.release())), del(std::forward<Deleter>(u.get_deleter()))
        {
        }

        template<typename U, typename E>
        unique_ptr(unique_ptr<U, E>&& u) : ptr(::boost::move(u.release())), del(std::forward<E>(u.get_deleter()))
        {
        }
#endif

        ~unique_ptr(void)
        {
            if(get() != BOOST_NULLPTR)
            {
                get_deleter()(get());
            }
        }

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        unique_ptr& operator=(BOOST_RV_REF(unique_ptr) r)
        {
            if(this != &r)
            {
                reset(r.release());
                // TODO: why can't this be move?
                del = ::boost::forward<Deleter>(r.get_deleter());
            }
            return *this;
        }

        template<class U, class E>
        unique_ptr& operator=(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E>) r)
        {
            if(this != &r)
            {
                reset(r.release());
                // TODO: really should be forward(u.get_deleter()), how can we emulate this in C++03?
                del = boost::forward<E>(r.get_deleter());
            }
            return *this;
        }
#else
        unique_ptr& operator=(unique_ptr&& r)
        {
            if(this != &r)
            {
                reset(r.release());
                // forward deleter
                del = std::forward<deleter_type>(r.get_deleter());
            }
            return *this;
        }

        template<class U, class E>
        unique_ptr& operator=(unique_ptr<U, E>&& r)
        {
            if(this != &r)
            {
                reset(r.release());
                // forward deleter
                del = std::forward<E>(r.get_deleter());
            }
            return *this;
        }
#endif

#if !defined(BOOST_NO_CXX11_NULLPTR)
        unique_ptr& operator=(std::nullptr_t)
        {
            reset();
            return *this;
        }
#endif

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
        return lhs.get() < rhs.get();
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

#if !defined(BOOST_NO_CXX11_NULLPTR)
// TODO: comparison operators for nullptr_t
#endif

#undef BOOST_NULLPTR
#undef BOOST_COMMA
#else
// use standard library features
using std::unique_ptr;
#endif
}
#endif // UNIQUE_PTR_HPP_
