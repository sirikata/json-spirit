#ifndef JSON_SPIRIT_TEST_UTILS
#define JSON_SPIRIT_TEST_UTILS

//          Copyright John W. Wilkinson 2007 - 2011
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.05

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cassert>
#include <cmath>
#include <cstdio>

#define RUN_TEST(name) \
  printf("Running %s\n", #name); \
  name

// these functions allow you to inspect the values that caused a test to fail

#define ASSERT_EQ(a,b) \
  printf(" assert %s==%s (%s:%d)\n", #a, #b, __FILE__, __LINE__); \
  assert_eq(a, b)

template< class T1, class T2 >
void assert_eq( const T1& t1, const T2& t2 )
{
    if( t1 == t2 ) return;

    assert( false );
}

#define ASSERT_NEQ(a,b) \
  printf(" assert %s!=%s (%s:%d)\n", #a, #b, __FILE__, __LINE__); \
  assert_neq(a, b)

template< class T1, class T2 >
void assert_neq( const T1& t1, const T2& t2 )
{
    if( !(t1 == t2) ) return;

    assert( false );
}

#define ASSERT_FLOAT_EQ(a,b,err) \
  printf(" assert float %s==%s (err=%s) (%s:%d)\n", #a, #b, #err, __FILE__, __LINE__); \
  assert_eq(a, b, err)

void assert_eq( const double d1, const double d2, const double abs_error );

#endif
