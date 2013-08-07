//
// uptr_base.hpp
//
// This file is only included if unique_ptr.hpp can't find std::unique_ptr
//
// (c) 2013 Andrew Ho
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_UPTR_BASE_HPP
#define BOOST_UPTR_BASE_HPP

namespace boost
{
    template<class T, class D = default_delete<T> >
    class unique_ptr
    {
        BOOST_MOVABLE_BUT_NOT_COPYABLE (unique_ptr)
        // private typedefs

    public:
        typedef T element_type;
        typedef D deleter_type;
        typedef typename ::boost::detail::pointer_type_switch<T, deleter_type,
                ::boost::detail::has_pointer_type<D>::value>::type pointer;

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    private:
        typedef typename remove_reference<D>::type& deleter_lref;

    public:
        // No reference collapse rules in C++03, manually add it
        deleter_lref get_deleter(
                void)
        {
            return del;
        }

        // No reference collapse rules in C++03, manually add it
        const deleter_lref get_deleter(
                void) const
        {
            return del;
        }
#else
        D& get_deleter(void)
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

        void reset(pointer p = pointer())
        {
            pointer old_ptr = boost::move(ptr);
            ptr = p;
            if (old_ptr != BOOST_NULLPTR)
            {
                del(old_ptr);
            }
        }

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    private:
        // TODO: probably should have special overloads if U == T is not a ref
        // E == D, E is not a ref
        // safe to move-swap
        template<typename E>
        void swap_impl(unique_ptr<T, E>& other)
        {
            // should really use std::swap
            // swap managed objects
            std::swap(boost::move(ptr), boost::move(other.ptr));
            //pointer tmp = ptr;
            //ptr = other.ptr;
            //other.ptr = tmp;
            // swap deleter
            std::swap(boost::move(del), boost::move(other.del));
            //deleter_type d = ::boost::move(del);
            //del = boost::move(other.del);
            //other.del = ::boost::move(d);
        }
        // E == D
        // E is a ref
        // swap by copy
        template<typename E>
        void swap_impl(unique_ptr<T, E&>& other)
        {
            // should really use std::swap
            // swap managed objects
            std::swap(boost::move(ptr), boost::move(other.ptr));
            //pointer tmp = ptr;
            //ptr = other.ptr;
            //other.ptr = tmp;
            // swap deleter
            std::swap(ptr, other.ptr);
            //deleter_type d = del;
            //del = other.del;
            //other.del = d;
        }
    public:
        void swap(unique_ptr& other)
        {
            if(this != &other)
            {
                swap_impl(other);
            }
        }

#else
        void swap(unique_ptr& other)
        {
            if (this != &other)
            {
                // swap managed objects
                std::swap(std::forward<pointer>(ptr), std::forward<pointer>(other.ptr));
                // swap deleter
                std::swap(std::forward<D>(del), std::forward<D>(other.del));
            }
        }
#endif

        typename add_lvalue_reference<T>::type operator*(void) const
        {
            return *ptr;
        }

        pointer operator->(void) const
        {
            return ptr;
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
            // if D is a reference or pointer type this is ill-formed
            BOOST_STATIC_ASSERT_MSG(
                !(is_reference<D>::value || is_pointer<D>::value),
                "Cannot default initialize deleter if it is a pointer or reference type.");
        }

#if !defined(BOOST_NO_CXX11_NULLPTR)
        unique_ptr(std::nullptr_t) :
            boost::unique_ptr<T, D>()
        {
            // if D is a reference or pointer type this is ill-formed
            BOOST_STATIC_ASSERT_MSG(
                !(is_reference<D>::value || is_pointer<D>::value),
                "Cannot default initialize deleter if it is a pointer or reference type.");
        }
#endif

        explicit unique_ptr(pointer ptr) :
            ptr(ptr), del()
        {
            // if D is a reference or pointer type this is ill-formed
            BOOST_STATIC_ASSERT_MSG(
                !(is_reference<D>::value || is_pointer<D>::value),
                "Cannot default initialize deleter if it is a pointer or reference type.");
        }

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        // no reference collapse rules, need to manually remove reference
        unique_ptr(pointer ptr,
            typename conditional< is_reference<D>::value, D,
            const typename remove_reference<D>::type& >::type d1) :
        ptr(ptr), del(d1)
        {
        }
#else
        unique_ptr(pointer ptr,
            typename conditional< is_reference<D>::value, D, const D&>::type d1) :
            ptr(ptr), del(d1)
        {
        }


#endif
        unique_ptr(pointer ptr,
                BOOST_RV_REF(typename remove_reference<D>::type) d2) :
        ptr(ptr), del(boost::move(d2))
        {
            BOOST_STATIC_ASSERT_MSG( !is_reference<D>::value, "cannot instantiate D& with rvalue deleter" );
        }


#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        // TODO: why is this overload even needed?
//        unique_ptr(BOOST_RV_REF(unique_ptr) u) : ptr(u.release()), del(::boost::move(u.del))
//        {
//        }

//#if defined(BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS)
    private:
        struct nat
        {};
    public:
        // only participates in overload resolution if:
        // unique_ptr<U, E>::pointer can be implicitly casted to pointer
        // U is not an array type
        // D is a reference and E == D
        // err... can never happen because this requires E is not reference
//        template<typename U, typename E>
//        unique_ptr(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E>) u, typename boost::enable_if_c<
//                boost::is_convertible<pointer, typename boost::unique_ptr<U, E>::pointer>::value
//                && !boost::is_array<U>::value && boost::is_reference<D>::value
//                && boost::is_same<D, E>::value, nat>::type = nat()) : ptr(u.release()), del(boost::move(u.del))
//        {
//        }

        // only participates in overload resolution if:
        // unique_ptr<U, E>::pointer can be implicitly casted to pointer
        // U is not an array type
        // D is not a reference and E is implicitly convertible to D
        template<typename U, typename E>
        unique_ptr(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E>) u, typename enable_if_c<
            is_convertible<typename boost::unique_ptr<U, E>::pointer, pointer>::value
            && !is_array<U>::value && !is_reference<D>::value
            && is_convertible<E, D>::value, nat>::type = nat()) : ptr(u.release()), del(boost::move(u.del))
        {
        }

        // only participates in overload resolution if:
        // unique_ptr<U, E&>::pointer can be implicitly casted to pointer
        // U is not an array type
        // D is a reference and E& == D
        template<typename U, typename E>
        unique_ptr(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E&>) u, typename enable_if_c<
            is_convertible<typename boost::unique_ptr<U, E&>::pointer, pointer>::value
            && !is_array<U>::value && is_reference<D>::value
            && is_same<D, E&>::value, nat>::type = nat()) : ptr(u.release()), del(u.del)
        {
        }

        // only participates in overload resolution if:
        // unique_ptr<U, E&>::pointer can be implicitly casted to pointer
        // U is not an array type
        // D is not a reference and E& is implicitly convertible to D
        template<typename U, typename E>
        unique_ptr(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E&>) u, typename enable_if_c<
            is_convertible<typename boost::unique_ptr<U, E&>::pointer, pointer>::value
            && !is_array<U>::value && !is_reference<D>::value
            && is_convertible<E&, D>::value, nat>::type = nat()) : ptr(u.release()), del(u.del)
        {
        }
//#else
//        // TODO: specialization if function template default args support?
//#endif // BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#else
        unique_ptr(unique_ptr&& u) :
            ptr(std::move(u.release())), del(std::forward < deleter_type > (u.del))
        {
        }

        // TODO: need to make sure this only participates in overload resolution if:
        // U is not an array type
        // if D is a reference type, then E == D. Otherwise, E must be implicitly convertible to D
        template<typename U, typename E>
        unique_ptr(unique_ptr<U, E>&& u, typename conditional< is_reference<D>::value) :
                ptr(std::move(u.release())), del(std::forward < E > (u.del))
        {
        }

        // TODO: std::auto_ptr is not marked as movable by move emulation. How should this be handled?
        // TODO: should only participate in overload resolution if U* is implicitly convertible to T*
        //      and D = default_delete<T>
        template<typename U>
        unique_ptr(BOOST_RV_REF(std::auto_ptr<U>) u) :
        ptr(u.release()), del()
        {
        }
#endif // BOOST_NO_CXX11_RVALUE_REFERENCES
        ~unique_ptr(void)
        {
            if (ptr != BOOST_NULLPTR)
            {
                del(ptr);
            }
        }

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        // if D is a reference type, copy assign
        // otherwise, move-assign
        unique_ptr& operator=(BOOST_RV_REF(unique_ptr) r)
        {
            if(this != &r)
            {
                reset(r.release());
                // TODO: better way to forward?
                if(is_reference<D>::value)
                {
                    // copy assign
                    del = r.del;
                }
                else
                {
                    // move assign
                    del = boost::move(r.del);
                }
            }
            return *this;
        }

        // only participates in overload resolution iff:
        // U is not an array type
        // unique_ptr<U, E>::pointer is implicitly convertible to pointer
        template<class U, class E>
        typename enable_if_c<
            !is_array<U>::value &&
            is_convertible< typename unique_ptr<U, E>::pointer, pointer >::value, unique_ptr&>::type
        operator=(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E>) r)
        {
            reset(r.release());
            del = boost::move(r.del);
            return *this;
        }

        // Only participates in overload resolution iff:
        // U is not an array type
        // unique_ptr<U, E&>::pointer is implicitly convertible to pointer
        template<class U, class E>
        typename enable_if_c<
            !is_array<U>::value &&
            is_convertible< typename unique_ptr<U, E&>::pointer, pointer >::value, unique_ptr&>::type
        operator=(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E&>) r)
        {
            reset(r.release());
            del = r.del;
            return *this;
        }

#else
        unique_ptr& operator=(unique_ptr&& r)
        {
            if (this != &r)
            {
                reset(r.release());
                // forward deleter
                del = std::forward < deleter_type > (r.del);
            }
            return *this;
        }

        // TODO: need to make sure this only participates in overload resolution if:
        // U is not an array type
        // if D is a reference type, then E == D. Otherwise, E must be implicitly convertible to D
        template<class U, class E>
        unique_ptr& operator=(unique_ptr<U, E> && r)
        {
            reset(r.release());
            // forward deleter
            del = std::forward < E > (r.del);
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

        template<typename U, typename E>
        friend class unique_ptr;
    };

    template<typename T, typename D>
    class unique_ptr< T, BOOST_RV_REF(D) >
    {
        BOOST_STATIC_ASSERT_MSG(!is_same<T BOOST_COMMA T>::value, "cannot instantiate a unique_ptr with rvalue ref D");
    };
}

#endif // BOOST_UPTR_BASE_HPP
