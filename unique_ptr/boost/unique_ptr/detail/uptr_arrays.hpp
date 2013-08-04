//
// uptr_arrays.hpp
//
// (c) 2013 Andrew Ho
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_UPTR_ARRAYS_HPP
#define BOOST_UPTR_ARRAYS_HPP

namespace boost
{
    template<class T, class Deleter>
    class unique_ptr<T[], Deleter>
    {
        BOOST_MOVABLE_BUT_NOT_COPYABLE (unique_ptr)
    public:
        typedef T element_type;
        typedef Deleter deleter_type;
        typedef typename ::boost::detail::pointer_type_switch<T, deleter_type,
                ::boost::detail::has_pointer_type<deleter_type>::value>::type pointer;

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    private:
        typedef typename ::boost::conditional< ::boost::is_reference<deleter_type>::value,
        deleter_type, typename ::boost::remove_reference<deleter_type>::type&>::type deleter_lref;
    public:

        deleter_lref get_deleter(
                void)
        {
            return del;
        }

        const deleter_lref get_deleter(
                void) const
        {
            return del;
        }
#else
        deleter_type& get_deleter(void)
        {
            return del;
        }

        const deleter_type& get_deleter(void) const
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

        void reset(void)
        {
            if (ptr != BOOST_NULLPTR)
            {
                del(ptr);
                ptr = pointer();
            }
        }

        void reset(pointer p)
        {
            pointer old_ptr = ::boost::move(ptr);
            ptr = p;
            if (old_ptr != BOOST_NULLPTR)
            {
                del(old_ptr);
            }
        }

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    private:
        template<typename E>
        void swap_impl(unique_ptr<T[], E>& other)
        {
            if (this != &other)
            {
                // swap managed objects
                pointer tmp = ptr;
                ptr = other.ptr;
                other.ptr = tmp;
                // swap deleter
                deleter_type d = ::boost::move(del);
                del = boost::move(other.del);
                other.del = ::boost::move(d);
            }
        }
        template<typename E>
        void swap_impl(unique_ptr<T[], E&>& other)
        {
            if (this != &other)
            {
                // swap managed objects
                pointer tmp = ptr;
                ptr = other.ptr;
                other.ptr = tmp;
                // swap deleter
                deleter_type d = del;
                del = other.del;
                other.del = d;
            }
        }
    public:
        void swap(unique_ptr& other)
        {
            swap_impl(other);
        }

#else
        void swap(unique_ptr& other)
        {
            if (this != &other)
            {
                // swap managed objects
                pointer tmp = ptr;
                ptr = other.ptr;
                other.ptr = tmp;
                // swap deleter
                deleter_type d = std::forward<deleter_type>(del);
                del = std::forward<deleter_type>(other.del);
                other.del = std::forward<deleter_type>(d);
            }
        }
#endif

        typename ::boost::add_lvalue_reference<T>::type operator*(void) const
        {
            return *ptr;
        }

        pointer operator->(void) const
        {
            return ptr;
        }

        T& operator[](size_t i) const
        {
            return ptr[i];
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
            return (ptr != BOOST_NULLPTR) ?
            (&unique_ptr::this_type_does_not_support_comparisons) :
            BOOST_NULLPTR;
        }
#else
        explicit operator bool(void) const
        {
            return ptr != BOOST_NULLPTR;
        }
#endif

        unique_ptr(void) :
                ptr(), del()
        {
        }

#if !defined(BOOST_NO_CXX11_NULLPTR)
        unique_ptr(std::nullptr_t) :
                ptr(), del()
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
                        deleter_type, const deleter_lref>::type d1) :
                ptr(ptr), del(d1)
        {
        }

        unique_ptr(pointer ptr,
                BOOST_RV_REF(typename ::boost::remove_reference<deleter_type>::type)d2) :
        ptr(ptr), del(::boost::move(d2))
        {
        }

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        // TODO: why is this overload even needed?
        //        unique_ptr(BOOST_RV_REF(unique_ptr) u) : ptr(u.release()), del(::boost::move(u.del))
        //        {
        //        }

        template<typename U, typename E>
        unique_ptr(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E>) u) : ptr(u.release()), del(boost::move(u.del))
        {
        }

        template<typename U, typename E>
        unique_ptr(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E&>) u) : ptr(u.release()), del(u.del)
        {
        }

        template<typename U, typename E>
        unique_ptr(BOOST_RV_REF(unique_ptr<U BOOST_COMMA const E&>) u) : ptr(u.release()), del(u.del)
        {
        }
#else
        unique_ptr(unique_ptr&& u) :
                ptr(::boost::move(u.release())), del(
                        std::forward<deleter_type>(u.del))
        {
        }

        template<typename U, typename E>
        unique_ptr(unique_ptr<U, E> && u) :
                ptr(::boost::move(u.release())), del(std::forward<E>(u.del))
        {
        }
#endif

        template<typename U>
        unique_ptr(BOOST_RV_REF(std::auto_ptr<U>) u) : ptr(::boost::move(u.release())), del()
        {
        }

        ~unique_ptr(void)
        {
            if (ptr != BOOST_NULLPTR)
            {
                del(ptr);
            }
        }

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        unique_ptr& operator=(BOOST_RV_REF(unique_ptr) r)
        {
            if(this != &r)
            {
                reset(r.release());
                // TODO: why can't this be move?
                del = ::boost::move(r.del);
            }
            return *this;
        }

        template<class U, class E>
        unique_ptr& operator=(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E>) r)
        {
            reset(r.release());
            del = boost::move(r.del);
            return *this;
        }

        template<class U, class E>
        unique_ptr& operator=(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E&>) r)
        {
            reset(r.release());
            del = r.del;
            return *this;
        }

        // err... this really shouldn't be legal?
//        template<class U, class E>
//        unique_ptr& operator=(BOOST_RV_REF(unique_ptr<U BOOST_COMMA const E&>) r)
//        {
//            if(this != &r)
//            {
//                reset(r.release());
//                del = r.del;
//            }
//            return *this;
//        }
#else
        unique_ptr& operator=(unique_ptr&& r)
        {
            if (this != &r)
            {
                reset(r.release());
                // forward deleter
                del = std::forward<deleter_type>(r.del);
            }
            return *this;
        }

        template<class U, class E>
        unique_ptr& operator=(unique_ptr<U, E> && r)
        {
            if (this != &r)
            {
                reset(r.release());
                // forward deleter
                del = std::forward<E>(r.del);
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
        template<class U, class E>
        friend class unique_ptr;
    };
}

#endif // BOOST_UPTR_ARRAYS_HPP
