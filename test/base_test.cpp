//
// base_test.cpp
//
// (c) 2013 Andrew Ho
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_UPTR_INVALID_TESTS

#include "base_test.hpp"
#include <fstream>

namespace boost
{
    namespace uptr
    {
        namespace test
        {
            namespace base
            {
                template<typename T>
                void function_deleter(T* val)
                {
                    delete val;
                }

                /**
                 * Tests here should compile successfully
                 */
                void valid_compile_test(void)
                {
                    // 20.7.1.2.1 unique_ptr default constructor
                    {
                        const boost::unique_ptr<int> uptr1;
                        const boost::unique_ptr<std::fstream, stream_closer> uptr2;
                    }
                    // 20.7.1.2.1 can explicitly construct unique_ptr from pointer type
                    {
                        const boost::unique_ptr<int> uptr(new int(5));
                        //std::ifstream stream("test.txt");
                        //const boost::unique_ptr<std::ifstream, stream_closer> uptr2(&stream);
                    }
                    // 20.7.1.2 if D::pointer is a type, should use D::pointer instead of T* as pointer type
                    {
                        boost::unique_ptr<int, fake_int> uptr1(new double(3.5));
                    }
                    // 2.7.1.2.1 D can be a function pointer. Must supply a function pointer
                    {
                        boost::unique_ptr<int, void(*)(int*) > uptr1(new int(5), &function_deleter<int>);
                    }
                    // reference deleter
                    {
                        boost::default_delete<int> del;
                        // del should be deleted last
                        {
                            boost::unique_ptr<int, boost::default_delete<int>& > ptr1(new int, del);
                        }
                    }
                }

#if defined(BOOST_UPTR_INVALID_TESTS)
                /**
                 * Tests here should all fail to compile
                 */
                void invalid_compile_test(void)
                {

                }
#endif
            }
        }
    }
}

