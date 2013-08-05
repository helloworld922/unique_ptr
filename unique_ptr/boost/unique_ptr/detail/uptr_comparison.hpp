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

#if !defined(BOOST_NO_CXX11_NULLPTR)
// TODO: comparison operators for nullptr_t
    template<class T, class D>
    bool operator ==(const ::boost::unique_ptr<T, D>& ptr, std::nullptr_t)
    {
        return ptr.get() == nullptr;
    }

    template<class T, class D>
    bool operator ==(std::nullptr_t, const ::boost::unique_ptr<T, D>& ptr)
    {
        return nullptr == ptr.get();
    }

    template<class T, class D>
    bool operator !=(const ::boost::unique_ptr<T, D>& ptr, std::nullptr_t)
    {
        return ptr.get() != nullptr;
    }

    template<class T, class D>
    bool operator !=(std::nullptr_t, const ::boost::unique_ptr<T, D>& ptr)
    {
        return nullptr != ptr.get();
    }

    template<class T, class D>
    bool operator <(const ::boost::unique_ptr<T, D>& ptr, std::nullptr_t)
    {
        return ptr.get() < nullptr;
    }

    template<class T, class D>
    bool operator <(std::nullptr_t, const ::boost::unique_ptr<T, D>& ptr)
    {
        return nullptr < ptr.get();
    }

    template<class T, class D>
    bool operator <=(const ::boost::unique_ptr<T, D>& ptr, std::nullptr_t)
    {
        return !(nullptr < ptr.get());
    }

    template<class T, class D>
    bool operator <=(std::nullptr_t, const ::boost::unique_ptr<T, D>& ptr)
    {
        return !(ptr.get() < nullptr);
    }

    template<class T, class D>
    bool operator >(const ::boost::unique_ptr<T, D>& ptr, std::nullptr_t)
    {
        return nullptr < ptr.get();
    }

    template<class T, class D>
    bool operator >(std::nullptr_t, const ::boost::unique_ptr<T, D>& ptr)
    {
        return ptr.get() < nullptr;
    }

    template<class T, class D>
    bool operator >=(const ::boost::unique_ptr<T, D>& ptr, std::nullptr_t)
    {
        return !(ptr.get() < nullptr);
    }

    template<class T, class D>
    bool operator >=(std::nullptr_t, const ::boost::unique_ptr<T, D>& ptr)
    {
        return !(nullptr < ptr.get());
    }
#endif
}

#endif // BOOST_UPTR_COMPARISON_HPP
