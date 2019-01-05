//          Copyright John W. Wilkinson 2007 - 2011
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.05

#include "reader_test.h"
#include "utils_test.h"
#include <json_spirit/reader.h>
#include <json_spirit/value.h>
#include <json_spirit/writer.h>

#include <limits.h>
#include <sstream>
#include <boost/assign/list_of.hpp>
#include <boost/timer.hpp>
#include <boost/lexical_cast.hpp>

using namespace json_spirit;
using namespace std;
using namespace boost;
using namespace boost::assign;

namespace
{
    template< class String_type, class ValueType >
    void test_read( const String_type& s, ValueType& value )
    {
        // performs both types of read and checks they produce the same value

        read( s, value );

        ValueType value_2;

        read_or_throw( s, value_2 );

        ASSERT_EQ( value, value_2 );
    }

    template< class Config_type >
    struct Test_runner
    {
        typedef typename Config_type::String_type String_type;
        typedef typename Config_type::Object_type Object_type;
        typedef typename Config_type::Array_type Array_type;
        typedef typename Config_type::ValueType ValueType;
        typedef typename Config_type::Pair_type Pair_type;
        typedef typename String_type::value_type Char_type;
        typedef typename String_type::const_iterator Iter_type;
        typedef std::basic_istringstream< Char_type > Istringstream_type;
        typedef std::basic_istream< Char_type > Istream_type;

        String_type to_str( const char* c_str )
        {
            return ::to_str< String_type >( c_str );
        }

        Test_runner()
        {
        }

        void check_eq( const Object_type& obj_1, const Object_type& obj_2 )
        {
            const typename Object_type::size_type size( obj_1.size() );

            ASSERT_EQ( size, obj_2.size() );

            typename Object_type::const_iterator i1 = obj_1.begin();
            typename Object_type::const_iterator i2 = obj_2.begin();

            for( ; i1 != obj_1.end(); ++i1, ++i2 )
            {
                ASSERT_EQ( *i1, *i2 );
            }
        }

        void add_value( Object_type& obj, const char* c_name, const ValueType& value )
        {
            Config_type::add( obj, to_str( c_name ), value );
        }

        void add_c_str( Object_type& obj, const char* c_name, const char* c_value )
        {
            LOG_TEST(add_value( obj, c_name, to_str( c_value ) ));
        }

        void test_syntax( const char* c_str, bool expected_success = true )
        {
            const String_type str = to_str( c_str );

            ValueType value;

            const bool ok = read( str, value );

            ASSERT_EQ( ok, expected_success  );

            try
            {
                read_or_throw( str, value );

                assert( expected_success );
            }
            catch( ... )
            {
                assert( !expected_success );
            }
        }

        template< typename Int >
        void test_syntax( Int min_int, Int max_int )
        {
            ostringstream os;

            os << "[" << min_int << "," << max_int << "]";

            test_syntax( os.str().c_str() );
        }

        void test_syntax( unsigned long long min_ullong, unsigned long long max_ullong )
        {
            ostringstream os;

            os << "[" << min_ullong << "," << max_ullong << "]";

            test_syntax( os.str().c_str() );
        }

        void test_syntax()
        {
            LOG_TEST(test_syntax( "{}" ));
            LOG_TEST(test_syntax( "{ }" ));
            LOG_TEST(test_syntax( "{ } " ));
            LOG_TEST(test_syntax( "{ }  " ));
            LOG_TEST(test_syntax( "{\"\":\"\"}" ));
            LOG_TEST(test_syntax( "{\"test\":\"123\"}" ));
            LOG_TEST(test_syntax( "{\"test\" : \"123\"}" ));
            LOG_TEST(test_syntax( "{\"testing testing testing\":\"123\"}" ));
            LOG_TEST(test_syntax( "{\"\":\"abc\"}" ));
            LOG_TEST(test_syntax( "{\"abc\":\"\"}" ));
            LOG_TEST(test_syntax( "{\"\":\"\"}" ));
            LOG_TEST(test_syntax( "{\"test\":true}" ));
            LOG_TEST(test_syntax( "{\"test\":false}" ));
            LOG_TEST(test_syntax( "{\"test\":null}" ));
            LOG_TEST(test_syntax( "{\"test1\":\"123\",\"test2\":\"456\"}" ));
            LOG_TEST(test_syntax( "{\"test1\":\"123\",\"test2\":\"456\",\"test3\":\"789\"}" ));
            LOG_TEST(test_syntax( "{\"test1\":{\"test2\":\"123\",\"test3\":\"456\"}}" ));
            LOG_TEST(test_syntax( "{\"test1\":{\"test2\":{\"test3\":\"456\"}}}" ));
            LOG_TEST(test_syntax( "{\"test1\":[\"a\",\"bb\",\"cc\"]}" ));
            LOG_TEST(test_syntax( "{\"test1\":[true,false,null]}" ));
            LOG_TEST(test_syntax( "{\"test1\":[true,\"abc\",{\"a\":\"b\"},{\"d\":false},null]}" ));
            LOG_TEST(test_syntax( "{\"test1\":[1,2,-3]}" ));
            LOG_TEST(test_syntax( "{\"test1\":[1.1,2e4,-1.234e-34]}" ));
            LOG_TEST(test_syntax( "{\n"
                          "\t\"test1\":\n"
                          "\t\t{\n"
                          "\t\t\t\"test2\":\"123\",\n"
                          "\t\t\t\"test3\":\"456\"\n"
                          "\t\t}\n"
                          "}\n" ));
            LOG_TEST(test_syntax( "[]" ));
            LOG_TEST(test_syntax( "[ ]" ));
            LOG_TEST(test_syntax( "[1,2,3]" ));
            LOG_TEST(test_syntax( "[ 1, -2, 3]" ));
            LOG_TEST(test_syntax( "[ 1.2, -2e6, -3e-6 ]" ));
            LOG_TEST(test_syntax( "[ 1.2, \"str\", -3e-6, { \"field\" : \"data\" } ]" ));

            LOG_TEST(test_syntax( INT_MIN, INT_MAX ));
            LOG_TEST(test_syntax( LLONG_MIN, LLONG_MAX ));
            LOG_TEST(test_syntax( 0, ULLONG_MAX ));
            LOG_TEST(test_syntax( "[1 2 3]", false ));
        }

        ValueType read_cstr( const char* c_str )
        {
            ValueType value;

            LOG_TEST(test_read( to_str( c_str ), value ));

            return value;
        }

        void read_cstr( const char* c_str, ValueType& value )
        {
            LOG_TEST(test_read( to_str( c_str ), value ));
        }

        void check_reading( const char* c_str, bool same_rewritten = true)
        {
            ValueType value;

            String_type in_s( to_str( c_str ) );

            LOG_TEST(test_read( in_s, value ));

            // We need to exempt some because the reencoding is different
            // (e.g. because the input has more precision than necessary to
            // store a double value, so the reencoded version is shorter)
            if (same_rewritten) {
                const String_type result = write_formatted( value );
                ASSERT_EQ( in_s, result );
            }
        }

        template< typename Int >
        void check_reading( Int min_int, Int max_int )
        {
            ostringstream os;

            os << "[\n"
                   "    " << min_int << ",\n"
                   "    " << max_int << "\n"
                   "]";

            LOG_TEST(check_reading( os.str().c_str() ));
        }

        void check_reading( unsigned long long min_ullong, unsigned long long max_ullong )
        {
            ostringstream os;

            os << "[\n"
                   "    " << min_ullong << ",\n"
                   "    " << max_ullong << "\n"
                   "]";

            LOG_TEST(check_reading( os.str().c_str() ));
        }

        void test_reading()
        {
            LOG_TEST(check_reading( "{\n}" ));

            Object_type obj;
            ValueType value;

            read_cstr( "{\n"
                       "    \"name 1\" : \"value 1\"\n"
                       "}", value );

            add_c_str( obj, "name 1", "value 1" );

            check_eq( value.getObject(), obj );

            read_cstr( "{\"name 1\":\"value 1\",\"name 2\":\"value 2\"}", value );

            add_c_str( obj, "name 2", "value 2" );

            check_eq( value.getObject(), obj );

            read_cstr( "{\n"
                       "    \"name 1\" : \"value 1\",\n"
                       "    \"name 2\" : \"value 2\",\n"
                       "    \"name 3\" : \"value 3\"\n"
                       "}", value );

            add_c_str( obj, "name 3", "value 3" );

            check_eq( value.getObject(), obj );

            check_reading( "{\n"
                            "    \"\" : \"value\",\n"
                            "    \"name\" : \"\"\n"
                            "}" );

            check_reading( "{\n"
                            "    \"name 1\" : \"value 1\",\n"
                            "    \"name 2\" : {\n"
                            "        \"name 3\" : \"value 3\",\n"
                            "        \"name_4\" : \"value_4\"\n"
                            "    }\n"
                            "}" );

            check_reading( "{\n"
                            "    \"name 1\" : \"value 1\",\n"
                            "    \"name 2\" : {\n"
                            "        \"name 3\" : \"value 3\",\n"
                            "        \"name_4\" : \"value_4\",\n"
                            "        \"name_5\" : {\n"
                            "            \"name_6\" : \"value_6\",\n"
                            "            \"name_7\" : \"value_7\"\n"
                            "        }\n"
                            "    }\n"
                            "}" );

            check_reading( "{\n"
                            "    \"name 1\" : \"value 1\",\n"
                            "    \"name 2\" : {\n"
                            "        \"name 3\" : \"value 3\",\n"
                            "        \"name_4\" : {\n"
                            "            \"name_5\" : \"value_5\",\n"
                            "            \"name_6\" : \"value_6\"\n"
                            "        },\n"
                            "        \"name_7\" : \"value_7\"\n"
                            "    }\n"
                            "}" );

            check_reading( "{\n"
                            "    \"name 1\" : \"value 1\",\n"
                            "    \"name 2\" : {\n"
                            "        \"name 3\" : \"value 3\",\n"
                            "        \"name_4\" : {\n"
                            "            \"name_5\" : \"value_5\",\n"
                            "            \"name_6\" : \"value_6\"\n"
                            "        },\n"
                            "        \"name_7\" : \"value_7\"\n"
                            "    },\n"
                            "    \"name_8\" : \"value_8\",\n"
                            "    \"name_9\" : {\n"
                            "        \"name_10\" : \"value_10\"\n"
                            "    }\n"
                            "}" );

            check_reading( "{\n"
                            "    \"name 1\" : {\n"
                            "        \"name 2\" : {\n"
                            "            \"name 3\" : {\n"
                            "                \"name_4\" : {\n"
                            "                    \"name_5\" : \"value\"\n"
                            "                }\n"
                            "            }\n"
                            "        }\n"
                            "    }\n"
                            "}" );

            check_reading( "{\n"
                            "    \"name 1\" : \"value 1\",\n"
                            "    \"name 2\" : true,\n"
                            "    \"name 3\" : false,\n"
                            "    \"name_4\" : \"value_4\",\n"
                            "    \"name_5\" : true\n"
                            "}" );

            check_reading( "{\n"
                            "    \"name 1\" : \"value 1\",\n"
                            "    \"name 2\" : null,\n"
                            "    \"name 3\" : \"value 3\",\n"
                            "    \"name_4\" : null\n"
                            "}" );

            check_reading( "{\n"
                            "    \"name 1\" : \"value 1\",\n"
                            "    \"name 2\" : 123,\n"
                            "    \"name 3\" : \"value 3\",\n"
                            "    \"name_4\" : -567\n"
                            "}" );

            check_reading( "[\n]" );

            check_reading( "[\n"
                           "    1\n"
                           "]" );

            check_reading( "[\n"
                           "    1,\n"
                           "    1.2000000000000000,\n"
                           "    \"john]\",\n"
                           "    true,\n"
                           "    false,\n"
                           "    null\n"
                           "]",
                false // double precision changes
            );

            check_reading( "[\n"
                           "    1,\n"
                           "    [\n"
                           "        2,\n"
                           "        3\n"
                           "    ]\n"
                           "]" );

            check_reading( "[\n"
                           "    1,\n"
                           "    [\n"
                           "        2,\n"
                           "        3\n"
                           "    ],\n"
                           "    [\n"
                           "        4,\n"
                           "        [\n"
                           "            5,\n"
                           "            6,\n"
                           "            7\n"
                           "        ]\n"
                           "    ]\n"
                           "]" );

            check_reading( "[\n"
                           "    {\n"
                           "        \"name\" : \"value\"\n"
                           "    }\n"
                           "]" );

            check_reading( "{\n"
                           "    \"name\" : [\n"
                           "        1\n"
                           "    ]\n"
                           "}" );

            check_reading( "[\n"
                           "    {\n"
                           "        \"name 1\" : \"value\",\n"
                           "        \"name 2\" : [\n"
                           "            1,\n"
                           "            2,\n"
                           "            3\n"
                           "        ]\n"
                           "    }\n"
                           "]" );

            check_reading( "{\n"
                           "    \"name 1\" : [\n"
                           "        1,\n"
                           "        {\n"
                           "            \"name 2\" : \"value 2\"\n"
                           "        }\n"
                           "    ]\n"
                           "}" );

            check_reading( "[\n"
                           "    {\n"
                           "        \"name 1\" : \"value 1\",\n"
                           "        \"name 2\" : [\n"
                           "            1,\n"
                           "            2,\n"
                           "            {\n"
                           "                \"name 3\" : \"value 3\"\n"
                           "            }\n"
                           "        ]\n"
                           "    }\n"
                           "]" );

            check_reading( "{\n"
                           "    \"name 1\" : [\n"
                           "        1,\n"
                           "        {\n"
                           "            \"name 2\" : [\n"
                           "                1,\n"
                           "                2,\n"
                           "                3\n"
                           "            ]\n"
                           "        }\n"
                           "    ]\n"
                           "}" );

            check_reading( INT_MIN, INT_MAX );
            check_reading( LLONG_MIN, LLONG_MAX );
            check_reading( 0, ULLONG_MAX );
        }

        void test_reading_reals()
        {
            ValueType value;

            const String_type in_s = to_str( "[1.200000000000000,1.234567890123456e+125,-1.234000000000000e-123,"
                                             " 1.000000000000000e-123,1234567890.123456,123]" );

            basic_istringstream< Char_type > is( in_s );

            const bool ok = read( is, value );

            ASSERT_EQ( ok, true );
            ASSERT_EQ( value.type(), ValueType::ARRAY_TYPE );

            const Array_type arr = value.getArray();

            ASSERT_EQ( arr.size(), (unsigned int)6 );
            ASSERT_FLOAT_EQ( arr[0].getReal(), 1.200000000000000, 1e-16 );
            ASSERT_FLOAT_EQ( arr[1].getReal(), 1.234567890123456e+125, 1e+110 );
            ASSERT_FLOAT_EQ( arr[2].getReal(), -1.234000000000000e-123, 1e+108 );
            ASSERT_FLOAT_EQ( arr[3].getReal(), 1.000000000000000e-123, 1e+108 );
            ASSERT_FLOAT_EQ( arr[4].getReal(), 1234567890.123456, 1e-7 );
            ASSERT_FLOAT_EQ( arr[5].getReal(), 123.0, 1e-13 );
        }

        void test_from_stream( const char* json_str, bool expected_success,
                               const ParseError& expected_error )
        {
            ValueType value;

            String_type in_s( to_str( json_str ) );

            basic_istringstream< Char_type > is( in_s );

            const bool ok = read( is, value );

            ASSERT_EQ( ok, expected_success );

            if( ok )
            {
                ASSERT_EQ( in_s, write( value ) );
            }

            try
            {
                basic_istringstream< Char_type > is( in_s );

                read_or_throw( is, value );

                ASSERT_EQ( expected_success, true );

                ASSERT_EQ( in_s, write( value ) );
            }
            catch( const ParseError error )
            {
                ASSERT_EQ( error, expected_error );
            }
        }

        void test_from_stream()
        {
            test_from_stream( "[1,2]", true, ParseError() );
            test_from_stream( "\n\n foo", false, ParseError( 3, 2,"not a value"  ) );
        }

        void test_escape_chars( const char* json_str, const char* c_str )
        {
            ValueType value;

            string s( string( "{\"" ) + json_str + "\" : \"" + json_str + "\"} " );

            read_cstr( s.c_str(), value );

            const Pair_type& pair( *value.getObject().begin() );

            ASSERT_EQ( Config_type::get_name ( pair ), to_str( c_str ) );
            ASSERT_EQ( Config_type::get_value( pair ), to_str( c_str ) );
        }

        void test_escape_chars()
        {
            LOG_TEST(test_escape_chars( "\\t", "\t"));
            LOG_TEST(test_escape_chars( "a\\t", "a\t" ));
            LOG_TEST(test_escape_chars( "\\tb", "\tb" ));
            LOG_TEST(test_escape_chars( "a\\tb", "a\tb" ));
            LOG_TEST(test_escape_chars( "a\\tb", "a\tb" ));
            LOG_TEST(test_escape_chars( "a123\\tb", "a123\tb" ));
            LOG_TEST(test_escape_chars( "\\t\\n\\\\", "\t\n\\" ));
            LOG_TEST(test_escape_chars( "\\/\\r\\b\\f\\\"", "/\r\b\f\"" ));
            LOG_TEST(test_escape_chars( "\\h\\j\\k", "" )); // invalid esc chars
            LOG_TEST(test_escape_chars( "\\x61\\x62\\x63", "abc" ));
            LOG_TEST(test_escape_chars( "a\\x62c", "abc" ));
            LOG_TEST(test_escape_chars( "\\x01\\x02\\x7F", "\x01\x02\x7F" )); // NB x7F is the greatest char spirit will parse
            LOG_TEST(test_escape_chars( "\\u0061\\u0062\\u0063", "abc" ));
        }

        void check_is_null( const char* c_str  )
        {
            ASSERT_EQ( read_cstr( c_str ).type(), ValueType::NULL_TYPE );
        }

        template< typename T >
        void check_value( const char* c_str, const T& expected_value )
        {
            const ValueType v( read_cstr( c_str ) );

            ASSERT_EQ( v.template getValue< T >(), expected_value );
        }

        void test_values()
        {
            LOG_TEST(check_value( "1",        1 ));
            LOG_TEST(check_value( "1.5",      1.5 ));
            LOG_TEST(check_value( "\"Test\"", to_str( "Test" ) ));
            LOG_TEST(check_value( "true",     true ));
            LOG_TEST(check_value( "false",    false ));
            LOG_TEST(check_is_null( "null" ));
        }

        void check_read_fails( const char* c_str, int line, int column, const string& reason )
        {
            ValueType value;

            try
            {
                read_cstr( c_str, value );

                assert( false );
            }
            catch( const ParseError posn )
            {
                ASSERT_EQ( posn, ParseError( line, column, reason ) );
            }
        }

        void test_error_cases()
        {
            LOG_TEST(check_read_fails( "",                       1, 1,  "not a value" ));
            LOG_TEST(check_read_fails( "foo",                    1, 1,  "not a value" ));
            LOG_TEST(check_read_fails( " foo",                   1, 2,  "not a value" ));
            LOG_TEST(check_read_fails( "  foo",                  1, 3,  "not a value" ));
            LOG_TEST(check_read_fails( "\n\n foo",               3, 2,  "not a value" ));
            LOG_TEST(check_read_fails( "!!!",                    1, 1,  "not a value" ));
            LOG_TEST(check_read_fails( "\"bar",                  1, 1,  "not a value" ));
            LOG_TEST(check_read_fails( "bar\"",                  1, 1,  "not a value" ));
            LOG_TEST(check_read_fails( "[1}",                    1, 3,  "not an array" ));
            LOG_TEST(check_read_fails( "[1,2?",                  1, 5,  "not an array" ));
            LOG_TEST(check_read_fails( "[1,2}",                  1, 5,  "not an array" ));
            LOG_TEST(check_read_fails( "[1;2]",                  1, 3,  "not an array" ));
            LOG_TEST(check_read_fails( "[1,\n2,\n3,]",           3, 2,  "not an array" ));
            LOG_TEST(check_read_fails( "{\"name\":\"value\"]",   1, 16, "not an object" ));
            LOG_TEST(check_read_fails( "{\"name\",\"value\"}",   1, 8,  "no colon in pair" ));
            LOG_TEST(check_read_fails( "{name:\"value\"}",       1, 2,  "not an object" ));
            LOG_TEST(check_read_fails( "{\n1:\"value\"}",        2, 1,  "not an object" ));
            LOG_TEST(check_read_fails( "{\n  name\":\"value\"}", 2, 3,  "not an object" ));
            LOG_TEST(check_read_fails( "{\"name\":foo}",         1, 9,  "not a value" ));
            LOG_TEST(check_read_fails( "{\"name\":value\"}",     1, 9,  "not a value" ));
        }

        typedef vector< int > Ints;

        bool test_read_range( Iter_type& first, Iter_type last, ValueType& value )
        {
            Iter_type first_ = first;

            const bool ok = read( first, last, value );

            try
            {
                ValueType value_;

                read_or_throw( first_, last, value_ );

                ASSERT_EQ( ok, true );
                ASSERT_EQ( value, value_ );
            }
            catch( ... )
            {
                ASSERT_EQ( ok, false );
            }

            return ok;
        }

        void check_value_sequence( Iter_type first, Iter_type last, const Ints& expected_values, bool all_input_consumed )
        {
            ValueType value;

            for( Ints::size_type i = 0; i < expected_values.size(); ++i )
            {
                const bool ok = test_read_range( first, last, value );

                ASSERT_EQ( ok, true );

                const bool is_last( i == expected_values.size() - 1 );

                ASSERT_EQ( first == last, is_last ? all_input_consumed : false );
            }

            const bool ok = test_read_range( first, last, value );

            ASSERT_EQ( ok, false );
        }

        void check_value_sequence( Istream_type& is, const Ints& expected_values, bool all_input_consumed )
        {
            ValueType value;

            for( Ints::size_type i = 0; i < expected_values.size(); ++i )
            {
                read_or_throw( is, value );

                ASSERT_EQ( value.getInt(), expected_values[i] );

                const bool is_last( i == expected_values.size() - 1 );

                ASSERT_EQ( is.eof(), is_last ? all_input_consumed : false );
            }

            try
            {
                read_or_throw( is, value );

                assert( false );
            }
            catch( ... )
            {
            }

            ASSERT_EQ( is.eof(), true );
        }

        void check_value_sequence( const char* c_str, const Ints& expected_values, bool all_input_consumed )
        {
            const String_type s( to_str( c_str ) );

            check_value_sequence( s.begin(), s.end(), expected_values, all_input_consumed );

            Istringstream_type is( s );

            check_value_sequence( is, expected_values, all_input_consumed );
        }

        void check_array( const ValueType& value, typename Array_type::size_type expected_size )
        {
            ASSERT_EQ( value.type(), ValueType::ARRAY_TYPE );

            const Array_type& arr = value.getArray();

            ASSERT_EQ( arr.size(), expected_size );

            for( typename Array_type::size_type i = 0; i < expected_size; ++i )
            {
                const ValueType& val = arr[i];

                ASSERT_EQ( val.type(), ValueType::INT_TYPE );
                ASSERT_EQ( val.getInt(), int( i + 1 ) );
            }
        }

        void check_reading_array( Iter_type& begin, Iter_type end, typename Array_type::size_type expected_size )
        {
            ValueType value;

            test_read_range( begin, end, value );

            LOG_TEST(check_array( value, expected_size ));
        }

        void check_reading_array( Istream_type& is, typename Array_type::size_type expected_size )
        {
            ValueType value;

            read( is, value );

            LOG_TEST(check_array( value, expected_size ));
        }

        void check_reading_arrays( const char* arrays_str )
        {
            const String_type str( to_str( arrays_str ) );

            Iter_type       begin = str.begin();
            const Iter_type end   = str.end();

            LOG_TEST(check_reading_array( begin, end, 0 ));
            LOG_TEST(check_reading_array( begin, end, 1 ));
            LOG_TEST(check_reading_array( begin, end, 2 ));
            LOG_TEST(check_reading_array( begin, end, 3 ));

            Istringstream_type is( str );

            LOG_TEST(check_reading_array( is, 0 ));
            LOG_TEST(check_reading_array( is, 1 ));
            LOG_TEST(check_reading_array( is, 2 ));
            LOG_TEST(check_reading_array( is, 3 ));
        }

        void test_sequence_of_values()
        {
            LOG_TEST(check_value_sequence( "",   Ints(), false ));
            LOG_TEST(check_value_sequence( " ",  Ints(), false ));
            LOG_TEST(check_value_sequence( "  ", Ints(), false ));
            LOG_TEST(check_value_sequence( "     10 ",      list_of( 10 ), false ));
            LOG_TEST(check_value_sequence( "     10 11 ",   list_of( 10 )( 11 ), false ));
            LOG_TEST(check_value_sequence( "     10 11 12", list_of( 10 )( 11 )( 12 ), true));
            LOG_TEST(check_value_sequence( "10 11 12",      list_of( 10 )( 11 )( 12 ), true));

            //
            // TODO: fails on Travis mac builds, but not reproducible locally
            //LOG_TEST(check_reading_arrays( "[] [ 1 ] [ 1, 2 ] [ 1, 2, 3 ]" ));
 //         LOG_TEST(check_reading_arrays( "[][1][1,2][1,2,3]" ));  // fails due to multi_pass iterator bug,
                                                          // use stream_reader class instead
        }

        void test_uint64( const char* value_str, int expected_int, int64_t expected_int64, uint64_t expected_uint64 )
        {
            const ValueType v( read_cstr( value_str ) );

            ASSERT_EQ( v.getInt(),    expected_int );
            ASSERT_EQ( v.getInt64(),  expected_int64 );
            ASSERT_EQ( v.getUInt64(), expected_uint64 );
        }

        void test_uint64()
        {
            LOG_TEST(test_uint64( "0", 0, 0, 0 ));
            LOG_TEST(test_uint64( "1", 1, 1, 1 ));
            LOG_TEST(test_uint64( "-1", -1, -1, ULLONG_MAX ));
            LOG_TEST(test_uint64( "18446744073709551615", -1, -1, ULLONG_MAX ));
        }

        void test_types()
        {
            ValueType value;

            read( to_str( "[ \"foo\", true, false, 1, 12.3, null ]" ), value );

            ASSERT_EQ( value.type(), ValueType::ARRAY_TYPE );

            const Array_type& a = value.getArray();

            ASSERT_EQ( a[0].getString(), to_str( "foo" ) );
            ASSERT_EQ( a[1].getBool(), true );
            ASSERT_EQ( a[2].getBool(), false );
            ASSERT_EQ( a[3].getInt(), 1 );
            ASSERT_EQ( a[3].getInt64(), 1 );
            ASSERT_EQ( a[3].getUInt64(), 1u );
            ASSERT_EQ( a[3].getReal(), 1.0 );
            ASSERT_EQ( a[4].getReal(), 12.3 );
            ASSERT_EQ( a[5].isNull(), true );
        }

        void run_tests()
        {
            RUN_TEST(test_syntax());
            RUN_TEST(test_reading());
            RUN_TEST(test_reading_reals());
            RUN_TEST(test_from_stream());
            RUN_TEST(test_escape_chars());
            RUN_TEST(test_values());
            RUN_TEST(test_error_cases());
            RUN_TEST(test_sequence_of_values());
            RUN_TEST(test_uint64());
            RUN_TEST(test_types());
        }
    };

#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
    void test_wide_esc_u()
    {
        wValue value;

        LOG_TEST(test_read( L"[\"\\uABCD\"]", value ));

        const wstring s( value.getArray()[0].getString() );

        ASSERT_EQ( s.length(), static_cast< wstring::size_type >( 1u ) );
        ASSERT_EQ( s[0], 0xABCD );
    }
#endif

#ifdef JSON_SPIRIT_VALUE_ENABLED
    void test_extended_ascii( const string& s )
    {
        Value value;

        LOG_TEST(test_read( "[\"" + s + "\"]", value ));

        ASSERT_EQ( value.getArray()[0].getString(), "äöüß" );
    }

    void test_extended_ascii()
    {
        LOG_TEST(test_extended_ascii( "\\u00E4\\u00F6\\u00FC\\u00DF" ));
        LOG_TEST(test_extended_ascii( "äöüß" ));
    }
#endif
}

//#include <fstream>

void json_spirit::test_reader()
{
#ifdef JSON_SPIRIT_VALUE_ENABLED
    printf("reader_test: Config\n");
    Test_runner< Config >().run_tests();
    RUN_TEST(test_extended_ascii());
#endif
#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
    printf("reader_test: Config\n");
    Test_runner< wConfig >().run_tests();
    RUN_TEST(test_wide_esc_u());
#endif


#ifndef _DEBUG
    //ifstream ifs( "test.txt" );

    //string s;

    //getline( ifs, s );

    //timer t;

    //for( int i = 0; i < 2000; ++i )
    //{
    //    Value value;

    //    read( s, value );
    //}

    //cout << t.elapsed() << endl;

//    const string so = write( value );

    //Object obj;

    //for( int i = 0; i < 100000; ++i )
    //{
    //    obj.push_back( Pair( "\x01test\x7F", lexical_cast< string >( i ) ) );
    //}

    //const string s = write( obj );

    //Value value;

    //timer t;

    //read( s, value );

    //cout << t.elapsed() << endl;

    //cout << "obj size " << value.get_obj().size();
#endif
}
