//
// default_delete_test.hpp
//
// (c) 2013 Andrew Ho
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef DEFAULT_DELETE_TEST_HPP_
#define DEFAULT_DELETE_TEST_HPP_

#include <boost/default_delete.hpp>

namespace boost
{
    namespace uptr
    {
        namespace test
        {
            namespace default_delete
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

#endif // DEFAULT_DELETE_TEST_HPP_
