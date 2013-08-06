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
                class bclass
                {
                public:
                    virtual ~bclass()
                    {}
                };

                class cclass : public bclass
                {
                public:
                    virtual ~cclass()
                    {}
                };

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
                    // 20.7.1.2.1-1 unique_ptr default constructor
                    {
                        const boost::unique_ptr<int> uptr1;
                        const boost::unique_ptr<std::fstream, stream_closer> uptr2;
                    }
                    // 20.7.1.2 const deleter
                    {
                        boost::default_delete<int> del;
                        {
                            const boost::unique_ptr<int,
                                    const boost::default_delete<int> > ptr1;
                            const boost::unique_ptr<int,
                                    const boost::default_delete<int> > ptr2(
                                    new int, boost::move(del));
                            const boost::unique_ptr<int,
                                    const boost::default_delete<int> > ptr3(
                                    new int, del);
                        }
                    }
                    // 20.7.1.2.1-5 can explicitly construct unique_ptr from pointer type
                    {
                        const boost::unique_ptr<int> uptr(new int(5));
                        //std::ifstream stream("test.txt");
                        //const boost::unique_ptr<std::ifstream, stream_closer> uptr2(&stream);
                    }
                    // 20.7.1.2-3 if D::pointer is a type, should use D::pointer instead of T* as pointer type
                    {
                        boost::unique_ptr<int, fake_int> uptr1(new double(3.5));
                    }
                    // 20.7.1.2-1 D can be a function pointer. Must supply a function pointer
                    {
                        boost::unique_ptr<int, void (*)(int*)> uptr1(new int(5),
                                &function_deleter<int>);
                    }
                    // 20.7.1.2-1 reference deleter
                    {
                        boost::default_delete<int> del;
                        // del should be deleted last
                        {
                            boost::unique_ptr<int, boost::default_delete<int>&> ptr1(
                                    new int, del);
                            boost::unique_ptr<int,
                                    const boost::default_delete<int>&> ptr2(
                                    new int, del);
                        }
                    }
                    // 20.7.1.2.1-9 move deleter
                    {
                        boost::default_delete<int> del;
                        // del should be deleted last
                        {
                            boost::unique_ptr<int, boost::default_delete<int>&> ptr1(
                                    new int, boost::move(del));
                            boost::unique_ptr<int,
                                    const boost::default_delete<int>&> ptr2(
                                    new int, boost::move(del));
                            boost::unique_ptr<int, boost::default_delete<int> > ptr3(
                                    new int, boost::move(del));
                        }
                    }
                    // 20.7.1.2.1 move one unique_ptr<T,D> to a unique_ptr<T,D>
                    {
                        boost::unique_ptr<int> ptr1(new int);
                        boost::unique_ptr<int> ptr2 = boost::move(ptr1);
                        boost::unique_ptr<int> ptr3(boost::move(ptr2));
                        boost::default_delete<int> del;
                        {
                            boost::unique_ptr<int, boost::default_delete<int>& > ptr4(new int, del);
                            boost::unique_ptr<int, boost::default_delete<int>& > ptr5(boost::move(ptr4));
                        }
                    }
                    // 20.7.1.2.1 move a unique_ptr<U,E> to a unique_ptr<T,D>
                    {
                        // 20.7.1.2.1-19 should only participate if unique_ptr<U, E>::pointer is implicitly convertible to pointer
                        boost::unique_ptr<cclass> ptr1;

                        boost::unique_ptr<bclass> ptr2(boost::move(ptr1));
                    }
                }

#if defined(BOOST_UPTR_INVALID_TESTS)
                /**
                 * Tests here should all fail to compile
                 */
                void invalid_compile_test(void)
                {
                    // 20.7.1.2 D shall not be an rvalue reference
                    // cannot implicitly construct unique_ptr from type pointer
                    // 20.7.1.2.1 if D is a reference or pointer type, the user must supply a deleter
                    // 20.7.1.2.1 If D is a reference type, should not allow unique_ptr(pointer, D&&)
                    // 20.7.1.2.1 cannot construct a base unique_ptr from array unique_ptr
                    // 20.7.1.2.1 If D is a reference
                }
#endif
            }
        }
    }
}

