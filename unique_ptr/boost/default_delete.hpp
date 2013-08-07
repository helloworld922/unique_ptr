//
// default_delete.hpp
//
// Replicates or uses std::default_delete as defined in the C++11 standard.
//
// (c) 2013 Andrew Ho
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_DEFAULT_DELETE_HPP
#define BOOST_DEFAULT_DELETE_HPP

#include <boost/config.hpp>
#include <boost/move/move.hpp>

#if defined(BOOST_NO_CXX11_SMART_PTR)
//#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#else
#include <memory>
#endif

namespace boost
{
#if defined(BOOST_NO_CXX11_SMART_PTR)
    // use custom unique_ptr implementation
    template<class T>
    struct default_delete
    {
    private:
        // Used for testing if templated copy constructor can participate in overload resolution
        struct nat
        {
        };

    BOOST_COPYABLE_AND_MOVABLE(default_delete)

    public:
        default_delete(void)
        {
        }

        default_delete(const default_delete& d)
        {
        }

        default_delete(BOOST_RV_REF(default_delete)d)
        {}

        /**
         * Technically allows an extra char
         */
        template<class U>
        default_delete(const default_delete<U>& d, typename enable_if_c<
                is_convertible<U*, T*>::value, nat>::type = nat())
        {
            //test_exists<>();
        }

        default_delete& operator=(const default_delete& d)
        {
            return *this;
        }

        default_delete& operator=(BOOST_RV_REF(default_delete) d)
        {
            return *this;
        }

        /**
         * Equivalent to: delete ptr;
         */
        void operator()(T* ptr) const
        {
            delete ptr;
        }
    };

    // array specialization
    template<class T>
    struct default_delete<T[]>
    {
    private:
        // Used for testing if templated copy constructor can participate in overload resolution
        struct nat
        {};

    BOOST_COPYABLE_AND_MOVABLE(default_delete)

    public:
        default_delete(void)
        {
        }

        default_delete(const default_delete& d)
        {
        }

        default_delete(BOOST_RV_REF(default_delete) d)
        {}

    // TODO: err... should this constructor even exist?
//        template<class U>
//        default_delete(const default_delete<U>& d,
//                typename ::boost::enable_if_c<
//                        ::boost::is_convertible<U[], T[]>::value, nat>::type =
//                        nat())
//        {
//        }


#if defined(BOOST_NO_CXX11_DELETED_FUNCTIONS)
        private:
            // prevents calling delete[] U;
            template<class U> void operator()(U*) const;
        public:
#else
            template<class U> void operator()(U*) const = delete;
#endif

            /**
             * Equivalent to delete[] ptr;
             */
            void operator()(T* ptr) const
            {
                delete[] ptr;
            }

            default_delete& operator=(const default_delete& d)
            {
                return *this;
            }

            default_delete& operator=(BOOST_RV_REF(default_delete) d)
            {
                return *this;
            }
        };
#else
        using std::default_delete;
#endif // BOOST_NO_CXX11_SMART_PTR
}

#endif // BOOST_DEFAULT_DELETE_HPP
