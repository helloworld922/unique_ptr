//
// base_test.hpp
//
// tests for boost::unique_ptr<T>, where T is not an array
//
// (c) 2013 Andrew Ho
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BASE_TEST_HPP_
#define BASE_TEST_HPP_

#include "stream_closer.hpp"
#include "fake_int.hpp"
#define BOOST_NO_CXX11_SMART_PTR
#include <boost/unique_ptr.hpp>

namespace boost
{
    namespace uptr
    {
        namespace test
        {
            namespace base
            {
                /**
                 * Tests here should compile successfully
                 */
                void valid_compile_test(void);

#if defined(BOOST_UPTR_INVALID_TESTS)
                /**
                 * Tests here should all fail to compile
                 */
                void invalid_compile_test(void);
#endif
            }
        }
    }
}

#endif // BASE_TEST_HPP_
