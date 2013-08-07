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
    template<class T, class D = ::boost::default_delete<T> >
    class unique_ptr
    {
        BOOST_MOVABLE_BUT_NOT_COPYABLE (unique_ptr)
        // private typedefs

    public:
        typedef T element_type;
        typedef D deleter_type;
        typedef typename ::boost::detail::pointer_type_switch<T, deleter_type,
                ::boost::detail::has_pointer_type<deleter_type>::value>::type pointer;

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    private:
        typedef typename ::boost::conditional< ::boost::is_reference<deleter_type>::value,
        deleter_type, typename ::boost::add_reference<deleter_type>::type>::type deleter_lref;

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

        void reset(pointer p = pointer())
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
        void swap_impl(unique_ptr<T, E>& other)
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
        void swap_impl(unique_ptr<T, E&>& other)
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
                deleter_type d = std::forward < deleter_type > (del);
                del = std::forward < deleter_type > (other.del);
                other.del = std::forward < deleter_type > (d);
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
                    !(::boost::is_reference<D>::value
                            || ::boost::is_pointer<D>::value),
                    "Cannot default initialize deleter if it is a pointer or reference type.");
        }

#if !defined(BOOST_NO_CXX11_NULLPTR)
        unique_ptr(std::nullptr_t) :
                ::boost::unique_ptr<T, D>()
        {
            // if D is a reference or pointer type this is ill-formed
            BOOST_STATIC_ASSERT_MSG(
                    !(::boost::is_reference<D>::value
                            || ::boost::is_pointer<D>::value),
                    "Cannot default initialize deleter if it is a pointer or reference type.");
        }
#endif

        explicit unique_ptr(pointer ptr) :
                ptr(ptr), del()
        {
            // if D is a reference or pointer type this is ill-formed
            BOOST_STATIC_ASSERT_MSG(
                    !(::boost::is_reference<D>::value
                            || ::boost::is_pointer<D>::value),
                    "Cannot default initialize deleter if it is a pointer or reference type.");
        }

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        unique_ptr(pointer ptr,
                typename ::boost::conditional<
                ::boost::is_reference<D>::value,
                D, const typename ::boost::remove_reference<D>::type& >::type d1) :
        ptr(ptr), del(d1)
        {
        }
#else
        unique_ptr(pointer ptr,
                typename ::boost::conditional<
                        ::boost::is_reference<deleter_type>::value,
                        deleter_type, const deleter_type&>::type d1) :
                ptr(ptr), del(d1)
        {
        }


#endif
        unique_ptr(pointer ptr,
                BOOST_RV_REF(typename ::boost::remove_reference<deleter_type>::type) d2) :
        ptr(ptr), del(::boost::move(d2))
        {
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
        unique_ptr(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E>) u, typename boost::enable_if_c<
                boost::is_convertible<typename boost::unique_ptr<U, E>::pointer, pointer>::value
                && !boost::is_array<U>::value && !boost::is_reference<D>::value
                && boost::is_convertible<E, D>::value, nat>::type = nat()) : ptr(u.release()), del(boost::move(u.del))
        {
        }

        // only participates in overload resolution if:
        // unique_ptr<U, E&>::pointer can be implicitly casted to pointer
        // U is not an array type
        // D is a reference and E& == D
        template<typename U, typename E>
        unique_ptr(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E&>) u, typename boost::enable_if_c<
                boost::is_convertible<typename boost::unique_ptr<U, E&>::pointer, pointer>::value
                && !boost::is_array<U>::value && boost::is_reference<D>::value
                && boost::is_same<D, E&>::value, nat>::type = nat()) : ptr(u.release()), del(u.del)
        {
        }

        // only participates in overload resolution if:
        // unique_ptr<U, E&>::pointer can be implicitly casted to pointer
        // U is not an array type
        // D is not a reference and E& is implicitly convertible to D
        template<typename U, typename E>
        unique_ptr(BOOST_RV_REF(unique_ptr<U BOOST_COMMA E&>) u, typename boost::enable_if_c<
                boost::is_convertible<typename boost::unique_ptr<U, E&>::pointer, pointer>::value
                && !boost::is_array<U>::value && !boost::is_reference<D>::value
                && boost::is_convertible<E&, D>::value, nat>::type = nat()) : ptr(u.release()), del(u.del)
        {
        }
//#else
//        // TODO: specialization if function template default args support?
//#endif // BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#else
        unique_ptr(unique_ptr&& u) :
                ptr(::boost::move(u.release())), del(
                        std::forward < deleter_type > (u.del))
        {
        }

        // TODO: need to make sure this only participates in overload resolution if:
        // U is not an array type
        // if D is a reference type, then E == D. Otherwise, E must be implicitly convertible to D
        template<typename U, typename E>
        unique_ptr(unique_ptr<U, E> && u) :
                ptr(::boost::move(u.release())), del(std::forward < E > (u.del))
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
                if(boost::is_reference<D>::value)
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
        unique_ptr& operator=(typename boost::enable_if_c<
                !boost::is_array<U>::value && boost::is_convertible< typename unique_ptr<U, E>::pointer, pointer >::value,
                BOOST_RV_REF(unique_ptr<U BOOST_COMMA E>) >::type r)
        {
            reset(r.release());
            del = boost::move(r.del);
            return *this;
        }

        // Only participates in overload resolution iff:
        // U is not an array type
        // unique_ptr<U, E&>::pointer is implicitly convertible to pointer
        template<class U, class E>
        unique_ptr& operator=(typename boost::enable_if_c<
                !boost::is_array<U>::value && boost::is_convertible< typename unique_ptr<U, E&>::pointer, pointer >::value,
                BOOST_RV_REF(unique_ptr<U BOOST_COMMA E&>) >::type r)
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
}

#endif // BOOST_UPTR_BASE_HPP
