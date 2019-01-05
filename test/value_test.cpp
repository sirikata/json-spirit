//          Copyright John W. Wilkinson 2007 - 2011
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.05

#include "value_test.h"
#include "utils_test.h"
#include <json_spirit/value.h>
#include <limits.h>
#include <list>
#include <boost/assign/list_of.hpp>
#include <boost/integer_traits.hpp>

using namespace json_spirit;
using namespace std;
using namespace boost;
using namespace boost::assign;

namespace
{
#ifdef JSON_SPIRIT_VALUE_ENABLED

    const int64_t max_int64   = integer_traits< int64_t  >::max();
    const uint64_t max_uint64 = integer_traits< uint64_t >::max();

    void test_obj_value()
    {
        const Object::value_type p1( "name1", "value1" );
        const Object::value_type p3( "name3", "value3" );

        Object obj_1; obj_1.insert( p1 );
        Object obj_2; obj_2.insert( p1 );
        Object obj_3; obj_3.insert( p3 );

        Value v1( obj_1 );
        Value v2( obj_2 );
        Value v3( obj_3 );

        ASSERT_EQ( v1.type(), Value::OBJECT_TYPE );
        ASSERT_EQ ( v1, v2 );
        ASSERT_NEQ( v1, v3 );

        ASSERT_EQ( v1.getObject(), obj_1 );
        ASSERT_EQ( v3.getObject(), obj_3 );
    }

    void test_array_value()
    {
        Array array_1; array_1.push_back( 1 ); array_1.push_back( "2" );
        Array array_2; array_2.push_back( 1 ); array_2.push_back( "2" );
        Array array_3; array_3.push_back( 1 ); array_3.push_back( "X" );

        Value v1( array_1 );
        Value v2( array_2 );
        Value v3( array_3 );

        ASSERT_EQ( v1.type(), Value::ARRAY_TYPE );
        ASSERT_EQ ( v1, v2 );
        ASSERT_NEQ( v1, v3 );

        ASSERT_EQ( v1.getArray(), array_1 );
        ASSERT_EQ( v3.getArray(), array_3 );
    }

    void test_bool_value()
    {
        Value v1( true );
        Value v2( true );
        Value v3( false );

        ASSERT_EQ( v1.type(), Value::BOOL_TYPE );
        ASSERT_EQ ( v1, v2 );
        ASSERT_NEQ( v1, v3 );

        assert( v1.getBool() );
        assert( !v3.getBool() );
    }

    void test_int_value()
    {
        Value v1( 1 );
        Value v2( 1 );
        Value v3( INT_MAX );

        ASSERT_EQ( v1.type(), Value::INT_TYPE );
        ASSERT_EQ ( v1, v2 );
        ASSERT_EQ( v3.type(), Value::INT_TYPE );
        ASSERT_NEQ( v1, v3 );

        unsigned int uint_max = INT_MAX;

        ASSERT_EQ( v1.getInt(),    1 );
        ASSERT_EQ( v1.getInt64(),  1 );
        ASSERT_EQ( v1.getUInt64(), 1u );
        ASSERT_EQ( v3.getInt(),    INT_MAX );
        ASSERT_EQ( v3.getInt64(),  INT_MAX );
        ASSERT_EQ( v3.getUInt64(), uint_max );

        Value v5( max_int64 );

        ASSERT_EQ( v5.getInt64(), max_int64 );
        ASSERT_EQ( v5.getUInt64(), static_cast< uint64_t >( max_int64 ) );

        const uint64_t max_int64_plus_1 = max_int64 + uint64_t( 1 );

        Value v6( max_int64_plus_1 );

        ASSERT_EQ( v6.getUInt64(), max_int64_plus_1 );

        Value v7( max_uint64 );

        ASSERT_EQ( v7.getUInt64(), max_uint64 );

        Value v8( 0 );

        ASSERT_EQ( v8.getInt(),    0 );
        ASSERT_EQ( v8.getInt64(),  0 );
        ASSERT_EQ( v8.getUInt64(), 0u );

        Value v9( -1 );

        ASSERT_EQ( v9.getInt(),   -1 );
        ASSERT_EQ( v9.getInt64(), -1 );
        ASSERT_EQ( v9.getUInt64(), max_uint64 );
    }

    void test_real_value()
    {
        Value v1( 1.0 );
        Value v2( 1.0 );
        Value v3( 2.0 );

        ASSERT_EQ( v1.type(), Value::REAL_TYPE );
        ASSERT_EQ ( v1, v2 );
        ASSERT_NEQ( v1, v3 );

        ASSERT_EQ( v1.getReal(), 1.0 );
        ASSERT_EQ( v3.getReal(), 2.0 );
    }

    void test_null_value()
    {
        Value v1;
        Value v2;

        ASSERT_EQ( v1.type(), Value::NULL_TYPE );
        ASSERT_EQ( v1.isNull(), true );
        ASSERT_EQ( v1, v2 );
        ASSERT_EQ( v1.isNull(), true );
        ASSERT_EQ( Value( 1 ).isNull(), false );
    }

    template< typename T >
    void test_get_value( const T& t )
    {
        ASSERT_EQ( Value( t ).getValue< T >(), t );
    }

    void test_get_value()
    {
        test_get_value( 123 );
        test_get_value( max_int64 );
        test_get_value( 1.23 );
        test_get_value( true );
        test_get_value( false );
        test_get_value( string( "test" ) );

        Array a; a.push_back( 1 ); a.push_back( "2" );

        test_get_value( a );

        Object obj; obj.insert( Object::value_type( "name1", "value1" ) );

        test_get_value( obj );
    }

    void assert_array_eq( const Value& v, const Array& a )
    {
        ASSERT_EQ( v.getArray(), a );
    }

    void assert_obj_eq( const Value& v, const Object& obj )
    {
        ASSERT_EQ( v.getObject(), obj );
    }

    template< typename T >
    void check_copy( const T& t )
    {
        const Value v1( t );
        const Value v2( v1 );
        Value v3;
        v3 = v1;

        ASSERT_EQ( v1, v2 );
        ASSERT_EQ( v1, v3 );

        ASSERT_EQ( v2.getValue< T >(), t );
        ASSERT_EQ( v3.getValue< T >(), t );

        ASSERT_EQ( v1.isUInt64(), v2.isUInt64() );
        ASSERT_EQ( v1.isUInt64(), v3.isUInt64() );
    }

    void check_copying_null()
    {
        const Value v1;
        const Value v2( v1 );
        Value v3;
        v3 = v1;

        ASSERT_EQ( v2.type(), Value::NULL_TYPE );
        ASSERT_EQ( v3.type(), Value::NULL_TYPE );
    }

    void test_copying()
    {
        {
            const Array array_1 = list_of(1)(2);

            Value v1( array_1 );
            const Value v2( v1 );

            assert_array_eq( v1, array_1 );
            assert_array_eq( v2, array_1 );

            v1.getArray()[0] = 3;

            assert_array_eq( v1, list_of(3)(2) );
            assert_array_eq( v2, array_1 );
        }
        {
            const Object obj_1 = map_list_of( "a", 1 )( "b", 2 );

            Value v1( obj_1 );
            Value v2;

            v2 = v1;

            assert_obj_eq( v1, obj_1 );
            assert_obj_eq( v2, obj_1 );
        }
        {
            LOG_TEST(check_copy( 1 ));
            LOG_TEST(check_copy( 2.0 ));
            LOG_TEST(check_copy( max_int64 ));
            LOG_TEST(check_copy( max_uint64 ));
            LOG_TEST(check_copy( string("test") ));
            LOG_TEST(check_copy( true ));
            LOG_TEST(check_copy( false ));
            const Array array_1 = list_of(1)(2);
            LOG_TEST(check_copy( array_1 ));
            const Object obj_1 = map_list_of( "a", 1 )( "b", 2 );
            LOG_TEST(check_copy( obj_1 ));
            LOG_TEST(check_copying_null());
        }
    }

    template< typename ObjectType > void check_pair_typedefs( ObjectType &object )
    {
        typename ObjectType::key_type name = object.begin()->first;
        typename ObjectType::value_type::second_type value = object.begin()->second;
    }

    void check_pair_typedefs()
    {
        Object o;
        check_pair_typedefs( o );

#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
        wObject wo;
        check_pair_typedefs( wo );
#endif
    }

    void test_obj_map_implemention()
    {
#ifdef JSON_SPIRIT_VALUE_ENABLED
        Object obj;

        obj[ "name 1" ] = 1;
        obj[ "name 2" ] = "two";

        ASSERT_EQ( obj.size(), 2u );

        ASSERT_EQ( obj.find( "name 1" )->second.getInt(), 1 );
        ASSERT_EQ( obj.find( "name 2" )->second.getString(), "two" );
#endif
    }

    template< typename Int >
    void check_an_int_is_a_real( Int i, bool expected_result )
    {
        ASSERT_EQ( Value( i ).isUInt64(), expected_result );
    }

    void test_is_uint64()
    {
        LOG_TEST(check_an_int_is_a_real( 1,                            false ));
        LOG_TEST(check_an_int_is_a_real( static_cast< int64_t  >( 1 ), false ));
        LOG_TEST(check_an_int_is_a_real( static_cast< uint64_t >( 1 ), true ));
    }

    template< typename Int >
    void check_an_int_is_a_real( Int i, double expected_result )
    {
        ASSERT_EQ( Value( i ).getReal(), expected_result );
    }

    void test_an_int_is_a_real()
    {
        LOG_TEST(check_an_int_is_a_real( -1, -1.0 ));
        LOG_TEST(check_an_int_is_a_real(  0,  0.0 ));
        LOG_TEST(check_an_int_is_a_real(  1,  1.0 ));
        LOG_TEST(check_an_int_is_a_real( max_int64,  9223372036854775800.0 ));
        LOG_TEST(check_an_int_is_a_real( max_uint64, 18446744073709552000.0 ));
    }

    template< typename T >
    void check_wrong_type_exceptions( const Value::Type vtype )
    {
        Value v;

        ASSERT_EQ( v.type(), Value::NULL_TYPE );

        try
        {
            v.getValue< T >();

            assert( false );
        }
        catch( const runtime_error& e )
        {
            ostringstream os;

            os << "value type is " << (int)Value::NULL_TYPE << " not " << (int)vtype;

            ASSERT_EQ( e.what(), os.str() );
        }
    }

    void test_wrong_type_exceptions()
    {
        LOG_TEST(check_wrong_type_exceptions< Object >( Value::OBJECT_TYPE ));
        LOG_TEST(check_wrong_type_exceptions< Array >( Value::ARRAY_TYPE ));
        LOG_TEST(check_wrong_type_exceptions< string >( Value::STRING_TYPE ));
        LOG_TEST(check_wrong_type_exceptions< bool >( Value::BOOL_TYPE ));
        LOG_TEST(check_wrong_type_exceptions< boost::int64_t >( Value::INT_TYPE ));
        LOG_TEST(check_wrong_type_exceptions< int >( Value::INT_TYPE ));
        LOG_TEST(check_wrong_type_exceptions< double >( Value::REAL_TYPE ));
    }

void test_path_contains() {
    // Construct a hierarchy of objects for tests
    const Object bar = map_list_of( "a", 1 )( "b", 2 );
    const Object foo = map_list_of( "bar", Value(bar) )( "c", 3 );
    const Object obj1 = map_list_of( "foo", Value(foo))( "d", 4 );
    const Value v1(obj1);

    // Top level get of a value
    ASSERT_EQ(v1.contains("d"), true);
    ASSERT_EQ(v1.contains("not there"), false);

    // Two level
    ASSERT_EQ(v1.contains("foo.c"), true);
    ASSERT_EQ(v1.contains("foo.not there"), false);

    // Three level
    ASSERT_EQ(v1.contains("foo.bar.a"), true);
    ASSERT_EQ(v1.contains("foo.bar.not there"), false);
}

void test_path_get() {
    // Construct a hierarchy of objects for tests
    const Object bar = map_list_of( "a", 1 )( "b", 2 );
    const Object foo = map_list_of( "bar", Value(bar) )( "c", 3 );
    const Object obj1 = map_list_of( "foo", Value(foo))( "d", 4 );
    const Value v1(obj1);

    // Top level get of a value
    const Value& dval = v1.get("d");
    ASSERT_EQ(dval.type(), Value::INT_TYPE);
    ASSERT_EQ(dval.getInt(), 4);

    // Two level
    const Value& cval = v1.get("foo.c");
    ASSERT_EQ(cval.type(), Value::INT_TYPE);
    ASSERT_EQ(cval.getInt(), 3);

    // Three level
    const Value& aval = v1.get("foo.bar.a");
    ASSERT_EQ(aval.type(), Value::INT_TYPE);
    ASSERT_EQ(aval.getInt(), 1);

    // Mutable access
    Value v2(obj1);
    Value& subval1 = v2.get("foo.bar");
    ASSERT_EQ(subval1.type(), Value::OBJECT_TYPE);
    subval1.insert("x", "val");
    ASSERT_EQ(subval1.getObject()["x"].getString(), "val");
}

void check_get_path_error_exception(const Value& v, const std::string& path, const Value::PathError& expected_error) {
    try {
        v.get(path);
    }
    catch(const Value::PathError& e) {
        ASSERT_EQ(expected_error, e);
        return;
    }
    assert(false);
}

void test_path_get_exceptions() {
    // Construct a hierarchy of objects for tests
    const Object bar = map_list_of( "a", 1 )( "b", 2 );
    const Object foo = map_list_of( "bar", Value(bar) )( "c", 3 );
    const Object obj1 = map_list_of( "foo", Value(foo))( "d", 4 );
    const Value v1(obj1);

    // Non-object
    const Value vint(2);
    LOG_TEST(check_get_path_error_exception(vint, "x", Value::PathError("x", "<root>")));

    // Non-existent path component
    LOG_TEST(check_get_path_error_exception(v1, "shazaam", Value::PathError("shazaam", "shazaam")));

    // Empty subpath
    LOG_TEST(check_get_path_error_exception(v1, "foo..a", Value::PathError("foo..a", "")));

    // Non-object in path
    LOG_TEST(check_get_path_error_exception(v1, "foo.d.c", Value::PathError("foo.d.c", "d")));
}

void test_path_get_helpers() {
    // Test the helpers that extract values directly by path. Provide
    // one of each type
    const Object obj = map_list_of("a", 2)("b", 3);
    const Array arr = list_of(2)(3);
    const Object foo = map_list_of
        ("string", Value("a string"))
        ("object", obj)
        ("array", arr)
        ("bool", true)
        ("int", 42)
        ("real", 52.3)
        ("realint", 12)
        ;
    Value v(foo);

    ASSERT_EQ(v.getString("string"), "a string");
    ASSERT_EQ(v.getObject("object"), obj);
    ASSERT_EQ(v.getArray("array"), arr);
    ASSERT_EQ(v.getBool("bool"), true);
    ASSERT_EQ(v.getInt("int"), 42);
    ASSERT_EQ(v.getInt64("int"), (boost::int64_t)42);
    ASSERT_EQ(v.getUInt64("int"), (boost::uint64_t)42);
    ASSERT_EQ(v.getReal("realint"), 12.f);
}

// Test that get helpers *don't* get defaults when the values are available
void test_path_get_helpers_not_defaults() {
    // Test the helpers that extract values directly by path. Provide
    // one of each type
    const Object obj = map_list_of("a", 2)("b", 3);
    const Array arr = list_of(2)(3);
    const Object foo = map_list_of
        ("string", Value("a string"))
        ("object", obj)
        ("array", arr)
        ("bool", true)
        ("int", 42)
        ("real", 52.3)
        ("realint", 12)
        ;
    Value v(foo);

    const Object bad_obj = map_list_of("x", 4)("y", 5);
    const Array bad_arr = list_of(7)(8);

    ASSERT_EQ(v.getString("string", "not a string"), "a string");
    ASSERT_EQ(v.getObject("object", bad_obj), obj);
    ASSERT_EQ(v.getArray("array", bad_arr), arr);
    ASSERT_EQ(v.getBool("bool", false), true);
    ASSERT_EQ(v.getInt("int", 0), 42);
    ASSERT_EQ(v.getInt64("int", (boost::int64_t)0), (boost::int64_t)42);
    ASSERT_EQ(v.getUInt64("int", (boost::uint64_t)0), (boost::uint64_t)42);
    ASSERT_EQ(v.getReal("realint", 0.f), 12.f);
}

// Test defaults are retrieved when fields are not available
void test_path_get_helpers_defaults() {
    // Test the helpers that extract values directly by path. Provide
    // one of each type
    const Object foo = map_list_of("x", 2);
    Value v(foo);

    const Object default_obj = map_list_of("x", 4)("y", 5);
    const Array default_arr = list_of(7)(8);

    ASSERT_EQ(v.getString("string", "not a string"), "not a string");
    ASSERT_EQ(v.getObject("object", default_obj), default_obj);
    ASSERT_EQ(v.getArray("array", default_arr), default_arr);
    ASSERT_EQ(v.getBool("bool", false), false);
    ASSERT_EQ(v.getInt("int", 0), 0);
    ASSERT_EQ(v.getInt64("int", (boost::int64_t)0), (boost::int64_t)0);
    ASSERT_EQ(v.getUInt64("int", (boost::uint64_t)0), (boost::uint64_t)0);
    ASSERT_EQ(v.getReal("realint", 0.f), 0.f);
}

void test_path_insert() {
    Object n;
    Value v1(n);
    ASSERT_EQ( v1.type(), Value::OBJECT_TYPE);

    bool inserted;

    // Single element path, integer
    inserted = v1.insert("a", 7);
    ASSERT_EQ(inserted, true);
    assert( v1.getObject().find("a") != v1.getObject().end());
    ASSERT_EQ( v1.getObject()["a"].type(), Value::INT_TYPE );
    ASSERT_EQ( v1.getObject()["a"].getInt(), 7);

    // Two element path, string
    inserted = v1.insert("b.cd", "XXX");
    ASSERT_EQ(inserted, true);
    assert( v1.getObject().find("b") != v1.getObject().end());
    ASSERT_EQ( v1.getObject()["b"].type(), Value::OBJECT_TYPE );
    assert( v1.getObject()["b"].getObject().find("cd") != v1.getObject()["b"].getObject().end());
    ASSERT_EQ( v1.getObject()["b"].getObject()["cd"].type(), Value::STRING_TYPE);
    ASSERT_EQ( v1.getObject()["b"].getObject()["cd"].getString(), "XXX");

    // Two elements, preserve previous elements
    inserted = v1.insert("b.e", "YYY");
    ASSERT_EQ(inserted, true);
    ASSERT_EQ( v1.getObject()["b"].getObject()["cd"].getString(), "XXX");
    assert( v1.getObject()["b"].getObject().find("e") != v1.getObject()["b"].getObject().end());
    ASSERT_EQ( v1.getObject()["b"].getObject()["e"].type(), Value::STRING_TYPE);
    ASSERT_EQ( v1.getObject()["b"].getObject()["e"].getString(), "YYY");

    // Fail to insert over existing element
    inserted = v1.insert("b.e", "ZZZ");
    ASSERT_EQ(inserted, false);
    ASSERT_EQ( v1.getObject()["b"].getObject()["e"].getString(), "YYY");
}

void test_path_put() {
    // Assume test_path_insert() passes. We only need to check the changes in
    // behavior: overwriting data.

    Object n;
    Value v1(n);
    ASSERT_EQ( v1.type(), Value::OBJECT_TYPE);

    // Prep some test data
    v1.put("b.e", "YYY");
    assert( v1.getObject()["b"].getObject().find("e") != v1.getObject()["b"].getObject().end());
    ASSERT_EQ( v1.getObject()["b"].getObject()["e"].type(), Value::STRING_TYPE);
    ASSERT_EQ( v1.getObject()["b"].getObject()["e"].getString(), "YYY");

    // Overwrite test
    v1.put("b.e", 2);
    assert( v1.getObject()["b"].getObject().find("e") != v1.getObject()["b"].getObject().end());
    ASSERT_EQ( v1.getObject()["b"].getObject()["e"].type(), Value::INT_TYPE);
    ASSERT_EQ( v1.getObject()["b"].getObject()["e"].getInt(), 2);
}

template<typename T>
void check_insert_path_error_exception(Value& v, const std::string& path, const T& val, const Value::PathError& expected_error) {
    try {
        v.insert(path, val);
    }
    catch(const Value::PathError& e) {
        ASSERT_EQ(expected_error, e);
        return;
    }
    assert(false);
}

void test_path_insert_error() {
    // Non-object values
    Value v1(2);
    LOG_TEST(check_insert_path_error_exception(v1, "foo", 2, Value::PathError("foo", "<root>")));

    Object n;
    Value v2(n);

    // Empty subpath
    LOG_TEST(check_insert_path_error_exception(v2, "foo..bar", 2, Value::PathError("foo..bar", "")));

    // baz isn't an object
    v2.insert("foo.baz", 2);
    LOG_TEST(check_insert_path_error_exception(v2, "foo.baz.bar", 2, Value::PathError("foo.baz.bar", "baz")));

}

#endif

    template< class Config_type >
    class Container_constructor_runner
    {
    public:

        Container_constructor_runner()
        {
            vector< double > vd = list_of( 1.2 )( 1.3 );  LOG_TEST(test_container_constructor( vd ));
            {
                vector< int >    vi = list_of( 1 );           LOG_TEST(test_container_constructor( vi ));
            }
            {
                vector< int >    vi = list_of( 1 )( 2 );      LOG_TEST(test_container_constructor( vi ));
            }
            {
                vector< int >    vi = list_of( 1 )( 2 )( 3 ); LOG_TEST(test_container_constructor( vi ));
            }
            list< double >   ld = list_of( 1.2 )( 1.3 );  LOG_TEST(test_container_constructor( ld ));
            {
                list< int >      li = list_of( 1 );           LOG_TEST(test_container_constructor( li ));
            }
            {
                list< int >      li = list_of( 1 )( 2 );      LOG_TEST(test_container_constructor( li ));
            }
            {
                list< int >      li = list_of( 1 )( 2 )( 3 ); LOG_TEST(test_container_constructor( li ));
            }
        }

    private:

        typedef typename Config_type::Array_type Array_type;
        typedef typename Config_type::ValueType ValueType;

        template< class Cont >
        void test_container_constructor( const Cont& cont )
        {
            typedef typename Cont::value_type Cont_value_type;
            const ValueType val( cont.begin(), cont.end() );
            const Array_type& arr = val.getArray();
            Cont result;
            for( unsigned int i = 0; i < arr.size(); ++i )
            {
                result.push_back( arr[i].template getValue< Cont_value_type>() );
            }
            ASSERT_EQ( result, cont );
        }
    };

    void test_container_constructor()
    {
#ifdef JSON_SPIRIT_VALUE_ENABLED
       Container_constructor_runner< Config >();
#endif
#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
        Container_constructor_runner< wConfig >();
#endif
    }

    template< class Config_type >
    class Variant_constructor_runner
    {
    public:

        Variant_constructor_runner()
        {
            LOG_TEMPLATE_TEST();
            test_variant_constructor< variant< int, double > >( 1.23 );
            LOG_TEMPLATE_TEST();
            test_variant_constructor< variant< int, double > >( 123 );
            LOG_TEMPLATE_TEST();
            test_variant_constructor< variant< int, double, String_type > >( to_str< String_type >( "foo" ) );
            LOG_TEMPLATE_TEST();
            test_variant_constructor< variant< int, double, String_type, bool > >( true );
            LOG_TEMPLATE_TEST();
            test_variant_constructor< variant< int, double, String_type, bool, boost::int64_t > >( boost::int64_t( 123 ) );
            LOG_TEMPLATE_TEST();
            test_variant_constructor< variant< int, double, String_type, bool, boost::uint64_t > >( boost::uint64_t( 123 ) );

            {
                variant< int, Null > variant = Null();
                const ValueType val( variant );
                assert( val.isNull() );
            }

            vector< double > vd = list_of( 1.2 )( 1.3 );   LOG_TEST(test_variant_array_constructor< double > ( vd ));
            {
                vector< int >    vi = list_of( 1 );            LOG_TEST(test_variant_array_constructor< int >( vi ));
            }
            {
                vector< int >    vi = list_of( 1 )( 2 );       LOG_TEST(test_variant_array_constructor< int >( vi ));
            }
            {
                vector< int >    vi = list_of( 1 )( 2 )( 3 );  LOG_TEST(test_variant_array_constructor< int >( vi ));
            }
            list< double >   ld = list_of( 1.2 )( 1.3 );   LOG_TEST(test_variant_array_constructor< double >( ld ));
            {
                list< int >      li = list_of( 1 );            LOG_TEST(test_variant_array_constructor< int >( li ));
            }
            {
                list< int >      li = list_of( 1 )( 2 );       LOG_TEST(test_variant_array_constructor< int >( li ));
            }
            {
                list< int >      li = list_of( 1 )( 2 )( 3 );  LOG_TEST(test_variant_array_constructor< int >( li ));
            }
        }

    private:

        typedef typename Config_type::String_type String_type;
        typedef typename Config_type::Array_type Array_type;
        typedef typename Config_type::ValueType ValueType;

        template< class Variant_t, typename T >
        void test_variant_constructor( const T& t )
        {
            const Variant_t variant( t );
            const ValueType val( variant );
            ASSERT_EQ( val.template getValue< T >(), t );
        }

        template< typename T, typename A, template< typename, typename > class Cont >
        void test_variant_array_constructor( const Cont< T, A >& cont )
        {
            const variant< int, Cont< T, A > > variant = cont;
            const ValueType val( variant );
            const Array_type& arr = val.getArray();
            Cont< T, A > result;
            for( unsigned int i = 0; i < arr.size(); ++i )
            {
                result.push_back( arr[i].template getValue< T >() );
            }
            ASSERT_EQ( result, cont );
        }
    };

    void test_variant_constructor()
    {
#ifdef JSON_SPIRIT_VALUE_ENABLED
        Variant_constructor_runner< Config >();
#endif
#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
       Variant_constructor_runner< wConfig >();
#endif
    }
}

void json_spirit::test_value()
{
#ifdef JSON_SPIRIT_VALUE_ENABLED
    Object obj;
    Value value_str ( "value" );
    Value value_obj ( obj );
    Value value_bool( true );

    Value value_str_2 ( string( "value" ) );
    Value value_obj_2 ( obj );
    Value value_bool_2( false );

    const char* str( "value" );
    Value value_str_2b ( str );

    ASSERT_EQ( value_str, value_str );
    ASSERT_EQ( value_str, value_str_2 );
    ASSERT_EQ( value_str, value_str_2b );
    ASSERT_EQ( value_obj, value_obj );
    ASSERT_EQ( value_obj, value_obj_2 );
    ASSERT_NEQ( value_str, value_obj );
    ASSERT_NEQ( value_str, value_bool );

    Object obj_2;
    obj_2.insert( Object::value_type( "name", value_str ) );
    Value value_str_3( "xxxxx" );
    Value value_obj_3( obj_2 );

    ASSERT_NEQ( value_str, value_str_3 );
    ASSERT_NEQ( value_obj, value_obj_3 );

    RUN_TEST(test_obj_value());
    RUN_TEST(test_array_value());
    RUN_TEST(test_bool_value());
    RUN_TEST(test_int_value());
    RUN_TEST(test_real_value());
    RUN_TEST(test_null_value());
    RUN_TEST(test_get_value());
    RUN_TEST(test_copying());
    RUN_TEST(test_obj_map_implemention());
    RUN_TEST(test_is_uint64());
    RUN_TEST(test_an_int_is_a_real());
    RUN_TEST(test_wrong_type_exceptions());
    RUN_TEST(test_path_contains());
    RUN_TEST(test_path_get());
    RUN_TEST(test_path_get_exceptions());
    RUN_TEST(test_path_get_helpers());
    RUN_TEST(test_path_insert());
    RUN_TEST(test_path_put());
    RUN_TEST(test_path_insert_error());
#endif
    RUN_TEST(test_container_constructor());
    RUN_TEST(test_variant_constructor());
}
