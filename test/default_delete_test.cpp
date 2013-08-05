//
// default_delete_test.cpp
//
// (c) 2013 Andrew Ho
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "default_delete_test.hpp"

namespace boost
{
    namespace unique_ptr
    {
        namespace test
        {
            class incomplete_type;

            class base
            {};

            class child : public base
            {};

            /**
             * Tests here should compile successfully
             */
            void valid_compile_test(void)
            {
                // 20.7.1.1.1 template parameter can be an incomplete type
                {
                    boost::default_delete<incomplete_type> it_del;
                }
                // 20.7.1.1.2 can copy default_delete<U> to default_delete<T> iff U* is implicitly convertible to T*
                {
                    const boost::default_delete<child> c_del;
                    boost::default_delete<child> c2_del;
                    const boost::default_delete<base> b1_del(c_del);
                    boost::default_delete<base> b2_del(b1_del);
                    b2_del = c_del;
                    b2_del = b1_del;
                    b2_del = boost::move(c2_del);
                }
                // 20.7.1.1.2 operator(T* ptr) const
                {
                    int* my_int = new int(3);
                    const boost::default_delete<int> del;
                    del(my_int);
                    child* my_child = new child();
                    const boost::default_delete<base> bdel;
                    bdel(my_child);
                }
                // 20.7.1.1.3 operator(T* ptr) const for arrays
                {
                    int* my_int = new int[2];
                    const boost::default_delete<int[]> del;
                    del(my_int);
                }
                // default_delete should be copyable
                {
                    boost::default_delete<int> del1;
                    boost::default_delete<int> del2(del1);
                    del1 = del2;
                    const boost::default_delete<int[]> del3;
                    boost::default_delete<int[]> del4(del3);
                    del4 = del3;
                }
                // default_delete should be movable
                {
                    const boost::default_delete<child> del1;
                    boost::default_delete<child> del2(boost::move(del1));
                    del2 = boost::move(del1);
                    boost::default_delete<base> del3(boost::move(del1));
                    del3 = boost::move(del1);

                    const boost::default_delete<child[]> del4;
                    boost::default_delete<child[]> del5(boost::move(del4));
                    del5 = boost::move(del4);
                }
            }

#if defined(BOOST_UPTR_INVALID_TESTS)
            void invalid_compile_test(void)
            {
                // 20.7.1.1.2 can only copy default_delete<U> to default_delete<T> iff U* is implicitly convertible to T*
                {
                    const boost::default_delete<base> b_del;
                    const boost::default_delete<child> c_del(b_del);
                    const boost::default_delete<base[]> c_del(b_del);
                }
                // 20.7.1.1.3 can't call operator(U*) if U != T
                {
                    const boost::default_delete<base[]> b_del;
                    child *c = new child[2];
                    b_del(c);
                }
            }
#endif
            class incomplete_type
            {
                int val;
            };
        }
    }
}
