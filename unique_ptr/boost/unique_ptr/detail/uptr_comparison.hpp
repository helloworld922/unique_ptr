//
// uptr_comparison.hpp
//
// (c) 2013 Andrew Ho
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_UPTR_COMPARISON_HPP
#define BOOST_UPTR_COMPARISON_HPP

namespace boost
{
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

    // nullptr_t comparison operators
#if defined(BOOST_NO_CXX11_NULLPTR)
    class nullptr_nat;
#endif

    template<class T, class D>
    bool operator ==(const ::boost::unique_ptr<T, D>& ptr, BOOST_NULLPTR_TYPE)
    {
        return ptr.get() == BOOST_NULLPTR;
    }

    template<class T, class D>
    bool operator ==(BOOST_NULLPTR_TYPE, const ::boost::unique_ptr<T, D>& ptr)
    {
        return BOOST_NULLPTR == ptr.get();
    }

    template<class T, class D>
    bool operator !=(const ::boost::unique_ptr<T, D>& ptr, BOOST_NULLPTR_TYPE)
    {
        return ptr.get() != BOOST_NULLPTR;
    }

    template<class T, class D>
    bool operator !=(BOOST_NULLPTR_TYPE, const ::boost::unique_ptr<T, D>& ptr)
    {
        return BOOST_NULLPTR != ptr.get();
    }

    template<class T, class D>
    bool operator <(const ::boost::unique_ptr<T, D>& ptr, BOOST_NULLPTR_TYPE)
    {
        return ptr.get() < BOOST_NULLPTR;
    }

    template<class T, class D>
    bool operator <(BOOST_NULLPTR_TYPE, const ::boost::unique_ptr<T, D>& ptr)
    {
        return BOOST_NULLPTR < ptr.get();
    }

    template<class T, class D>
    bool operator <=(const ::boost::unique_ptr<T, D>& ptr, BOOST_NULLPTR_TYPE)
    {
        return !(BOOST_NULLPTR < ptr.get());
    }

    template<class T, class D>
    bool operator <=(BOOST_NULLPTR_TYPE, const ::boost::unique_ptr<T, D>& ptr)
    {
        return !(ptr.get() < BOOST_NULLPTR);
    }

    template<class T, class D>
    bool operator >(const ::boost::unique_ptr<T, D>& ptr, BOOST_NULLPTR_TYPE)
    {
        return BOOST_NULLPTR < ptr.get();
    }

    template<class T, class D>
    bool operator >(BOOST_NULLPTR_TYPE, const ::boost::unique_ptr<T, D>& ptr)
    {
        return ptr.get() < BOOST_NULLPTR;
    }

    template<class T, class D>
    bool operator >=(const ::boost::unique_ptr<T, D>& ptr, BOOST_NULLPTR_TYPE)
    {
        return !(ptr.get() < BOOST_NULLPTR);
    }

    template<class T, class D>
    bool operator >=(BOOST_NULLPTR_TYPE, const ::boost::unique_ptr<T, D>& ptr)
    {
        return !(BOOST_NULLPTR < ptr.get());
    }
}

#endif // BOOST_UPTR_COMPARISON_HPP
