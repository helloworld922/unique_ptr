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
                    int val;

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
                    // should be able to construct unique_ptr from nullptr
                    // limitation of C++03, there's no nullptr or nullptr_t
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
                        boost::unique_ptr<int, fake_int<int> > uptr1(new double(3.5));
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
                            boost::unique_ptr<int, boost::default_delete<int> > ptr1(
                                    new int, boost::move(del));
                            boost::unique_ptr<int,
                                    const boost::default_delete<int> > ptr2(
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
                        boost::unique_ptr<cclass, boost::default_delete<bclass> > ptr1;

                        boost::unique_ptr<bclass, boost::default_delete<bclass> > ptr2(boost::move(ptr1));
                    }
                    {
                        boost::default_delete<bclass> del1;
                        {
                            // 20.7.1.2.1-19 If D is not a reference, E must be implicitly convertible to D
                            boost::unique_ptr<cclass, boost::default_delete<cclass> > ptr1;
                            boost::unique_ptr<bclass, boost::default_delete<bclass> > ptr2(boost::move(ptr1));
                            // 20.7.1.2.1-19 If D is not a reference, E& must be implicitly convertible to D
                            boost::unique_ptr<cclass, boost::default_delete<bclass>& > ptr3(new cclass, del1);
                            boost::unique_ptr<bclass, boost::default_delete<bclass> > ptr4(boost::move(ptr3));
                            // 20.7.1.2.1-19 if D is a reference type, E == D
                            boost::unique_ptr<bclass, boost::default_delete<bclass>& > ptr5(boost::move(ptr3));
                        }
                    }
                    // 20.7.1.2.1 unique_ptr<T,D> should be move-constructable from std::auto_ptr<U>. U* must be implicitly convertible to T*
                    // and D must be default_delete<T>
                    {
                        // limitation of C++03 move emulation: std::auto_ptr is not marked movable by Boost.Move
                        // this will fail to compile
                        // work-around: user must manually release from the aptr
                        // uptr(aptr.release());
                        //std::auto_ptr<int> aptr;
                        //boost::unique_ptr<int> uptr(boost::move(aptr));
                    }
                    // 20.7.1.2.2 destructor ~unique_ptr. Already tested above
                    // 20.7.1.2.3 assignment operators
                    {
                        // move assign from unique_ptr<T,D> to unique_ptr<T,D>
                        // 20.7.1.2.3-1 D is not a reference type, D must be MoveAssignable
                        {
                            boost::unique_ptr<int> ptr1;
                            boost::unique_ptr<int> ptr2;
                            ptr1 = boost::move(ptr2);
                        }
                        // should be assignable to nullptr
                        // C++03 limitation, there's no nullptr/nullptr_t type

                        // move assign from unique_ptr<T, D&> to unique_ptr<T, D&>
                        // 20.7.1.2.3-1 D is a reference type, remove_reference<D>::type should be CopyAssignable
                        {
                            boost::default_delete<int> del;
                            {
                                boost::unique_ptr<int, boost::default_delete<int>& > ptr1(new int, del);
                                boost::unique_ptr<int, boost::default_delete<int>& > ptr2(new int, del);
                                ptr2 =boost::move(ptr1);
                            }
                        }
                        // move assign from unique_ptr<U, E> to unique_ptr<T, D>
                        // 20.7.1.2.3-5 unique_ptr<U,E>::pointer must be implicitly convertible to pointer
                        //      and U is not an array type
                        // calls reset(u.release())
                        // assign del = std::forward<D>(u.del)
                        {
                            boost::default_delete<bclass> del_b;
                            boost::default_delete<cclass> del_c;
                            {
                                boost::unique_ptr<bclass> ptr1;
                                boost::unique_ptr<cclass> ptr2;
                                boost::unique_ptr<cclass, boost::default_delete<cclass>& > ptr3(NULL, del_c);
                                ptr1 = boost::move(ptr2);
                                ptr1 = boost::move(ptr3);
                                ptr3 = boost::move(ptr2);
                            }
                        }
                    }
                    // observers
                    {
                        const boost::unique_ptr<bclass> ptr1(new bclass);
                        boost::unique_ptr<bclass> ptr2(new bclass);
                        // operator*
                        bclass tmp = *ptr1;
                        // operator->
                        int x = ptr1->val;
                        x += 1;
                        // get()
                        bclass* g = ptr1.get();
                        x = g->val;
                        // get_deleter() const
                        ptr1.get_deleter();
                        ptr2.get_deleter();
                        // operator bool checks
                        // limitation: C++03 uses a safe-bool idiom so it doesn't behave exactly like a bool
                        if(!ptr1)
                        {
                        }
                        if(!ptr2)
                        {
                        }
                    }
                    // modifiers
                    {
                        // swap
                        {
                            boost::unique_ptr<int> ptr1(new int);
                            boost::unique_ptr<int> ptr2(new int);
                            ptr1.swap(ptr2);
                            //using std::swap;
                            std::swap(ptr1, ptr2);
                        }
                        // reset
                        {
                            boost::unique_ptr<int> ptr1(new int);
                            // limitation of C++03: there's no nullptr/nullptr_t
                            ptr1.reset(NULL);
                            ptr1.reset(new int);
                        }
                    }
                }

#if defined(BOOST_UPTR_INVALID_TESTS)
                /**
                 * Tests here should all fail to compile
                 */
                void invalid_compile_test(void)
                {
//#if false
                    // 20.7.1.2 D shall not be an rvalue reference
                    boost::default_delete<int> del;
                    {
                        boost::unique_ptr<int, BOOST_RV_REF(boost::default_delete<int>) > ptr1(new int, boost::move(del));
                    }
                    // cannot implicitly construct unique_ptr from type pointer
                    {
                        boost::unique_ptr<int> ptr1 = new int;
                    }
                    // 20.7.1.2.1 if D is a reference or pointer type, the user must supply a deleter
                    {
                        boost::unique_ptr< int, boost::default_delete<int>& > ptr1;
                        boost::unique_ptr< int, boost::default_delete<int>& > ptr2(new int);
                        boost::unique_ptr<int, void(*)(int*)> ptr3;
                        boost::unique_ptr<int, void(*)(int*)> ptr4(new int);
                    }
                    // 20.7.1.2.1 If D is a reference type, should not allow unique_ptr(pointer, D&&)
                    {
                        boost::default_delete<int> del;
                        {
                            boost::unique_ptr<int, boost::default_delete<int>& > ptr1(new int, boost::default_delete<int>());
                            boost::unique_ptr<int, boost::default_delete<int>& > ptr2(new int, boost::move(del));
                        }
                    }
                    // 20.7.1.2.1 cannot construct a base unique_ptr from array unique_ptr
                    {
                        boost::unique_ptr<int[]> ptr1;
                        boost::unique_ptr<int> ptr2(boost::move(ptr1));
                    }
                    // 20.7.1.2.1 If D is a reference, move constructing from unique_ptr<U, E>, must have E == D
                    {
                        boost::default_delete<cclass> del;
                        {
                            boost::unique_ptr<cclass, boost::default_delete<cclass> > ptr1(new cclass, del);
                            boost::unique_ptr<cclass, boost::default_delete<cclass>& > ptr2(new cclass, del);
                            boost::unique_ptr<cclass, boost::default_delete<bclass>& > ptr3(boost::move(ptr1));
                            boost::unique_ptr<cclass, boost::default_delete<bclass>& > ptr4(boost::move(ptr2));
                        }
                    }
                    // 20.7.1.2.1 If D is not a reference, move construction from unique_ptr<U, E> must have E convertible to D
                    {
                        boost::default_delete<bclass> del;
                        {
                            boost::unique_ptr<cclass, boost::default_delete<bclass>& > ptr1(new cclass, del);
                            boost::unique_ptr<cclass, boost::default_delete<bclass> > ptr2(new cclass);
                            boost::unique_ptr<cclass, boost::default_delete<cclass> > ptr3(boost::move(ptr1));
                            boost::unique_ptr<cclass, boost::default_delete<cclass> > ptr4(boost::move(ptr2));
                        }
                    }
                    // move assign from unique_ptr<U, E> to unique_ptr<T, D>
                    // 20.7.1.2.3-5 unique_ptr<U,E>::pointer must be implicitly convertible to pointer
                    //      and U is not an array type
                    {
                        boost::unique_ptr<cclass, boost::default_delete<bclass> > ptr1;
                        boost::unique_ptr<bclass> ptr2;
                        boost::unique_ptr<bclass[]> ptr3;
                        ptr1 = boost::move(ptr2);
                        ptr2 = boost::move(ptr3);
                    }
//#endif // temporary: hides some expected compile errors
                }
#endif
            }
        }
    }
}

