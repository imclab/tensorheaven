// ///////////////////////////////////////////////////////////////////////////
// test_typle.cpp by Victor Dods, created 2014/01/13
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#include "test_typle.hpp"

#include "tenh/meta/typle.hpp"
#include "tenh/meta/typle_utility.hpp"

// this is included last because it redefines the `assert` macro,
// which would be bad for the above includes.
#include "lvd_testsystem.hpp"

using namespace Lvd;
using namespace std;
using namespace TestSystem;

namespace Test {
namespace Typle {

template <typename Typle_>
void test_length (Context const &context)
{
    Uint32 expected_length = context.DataAs<Uint32>();
    assert_eq(Typle_::length(), expected_length);
}

template <typename Typle_, Uint32 EXPECTED_LENGTH>
void add_particular_tests_for_typle (Directory &parent)
{
    Directory &dir = parent.GetSubDirectory(Tenh::type_string_of<Typle_>());
    LVD_ADD_NAMED_TEST_CASE_FUNCTION(dir, "length", test_length<Typle_>, new Context::Data<Uint32>(EXPECTED_LENGTH), RESULT_NO_ERROR);
}

template <typename T_> struct IsFloatingPointType_f { static bool const V = false; };
template <> struct IsFloatingPointType_f<float> { static bool const V = true; };
template <> struct IsFloatingPointType_f<double> { static bool const V = true; };
template <> struct IsFloatingPointType_f<long double> { static bool const V = true; };

MAKE_1_ARY_VALUE_EVALUATOR(IsFloatingPointType, bool);

void test_OnEach (Context const &context)
{
    typedef Tenh::Value_t<bool,true> True;
    typedef Tenh::Value_t<bool,false> False;
    static_assert(Tenh::TypesAreEqual_f<Tenh::Hippo::OnEach_f<Tenh::Typle_t<>,IsFloatingPointType_e>::T,
                                        Tenh::Typle_t<>>::V, "error");
    static_assert(Tenh::TypesAreEqual_f<Tenh::Hippo::OnEach_f<Tenh::Typle_t<int>,IsFloatingPointType_e>::T,
                                        Tenh::Typle_t<False>>::V, "error");
    static_assert(Tenh::TypesAreEqual_f<Tenh::Hippo::OnEach_f<Tenh::Typle_t<float>,IsFloatingPointType_e>::T,
                                        Tenh::Typle_t<True>>::V, "error");
    static_assert(Tenh::TypesAreEqual_f<Tenh::Hippo::OnEach_f<Tenh::Typle_t<float,int>,IsFloatingPointType_e>::T,
                                        Tenh::Typle_t<True,False>>::V, "error");

    // there is currently no runtime test code here, but that could change
}

void test_NegationOfPredicate (Context const &context)
{
    typedef Tenh::Value_t<bool,true> True;
    typedef Tenh::Value_t<bool,false> False;
    typedef Tenh::Hippo::NegationOfPredicate_e<IsFloatingPointType_e> IsntFloatingPointType_e;
    static_assert(Tenh::TypesAreEqual_f<Tenh::Hippo::OnEach_f<Tenh::Typle_t<>,IsntFloatingPointType_e>::T,
                                        Tenh::Typle_t<>>::V, "error");
    static_assert(Tenh::TypesAreEqual_f<Tenh::Hippo::OnEach_f<Tenh::Typle_t<int>,IsntFloatingPointType_e>::T,
                                        Tenh::Typle_t<True>>::V, "error");
    static_assert(Tenh::TypesAreEqual_f<Tenh::Hippo::OnEach_f<Tenh::Typle_t<float>,IsntFloatingPointType_e>::T,
                                        Tenh::Typle_t<False>>::V, "error");
    static_assert(Tenh::TypesAreEqual_f<Tenh::Hippo::OnEach_f<Tenh::Typle_t<float,int>,IsntFloatingPointType_e>::T,
                                        Tenh::Typle_t<False,True>>::V, "error");

    // there is currently no runtime test code here, but that could change
}

void test_And (Context const &context)
{
    typedef Tenh::Value_t<bool,true> True;
    typedef Tenh::Value_t<bool,false> False;
    static_assert(Tenh::Hippo::And_f<Tenh::Typle_t<>>::V, "error");
    static_assert(Tenh::Hippo::And_f<Tenh::Typle_t<True>>::V, "error");
    static_assert(!Tenh::Hippo::And_f<Tenh::Typle_t<False>>::V, "error");
    static_assert(Tenh::Hippo::And_f<Tenh::Typle_t<True,True>>::V, "error");
    static_assert(!Tenh::Hippo::And_f<Tenh::Typle_t<False,True>>::V, "error");
    static_assert(!Tenh::Hippo::And_f<Tenh::Typle_t<False,False>>::V, "error");

    // there is currently no runtime test code here, but that could change
}

void test_Or (Context const &context)
{
    typedef Tenh::Value_t<bool,true> True;
    typedef Tenh::Value_t<bool,false> False;
    static_assert(!Tenh::Hippo::Or_f<Tenh::Typle_t<>>::V, "error");
    static_assert(Tenh::Hippo::Or_f<Tenh::Typle_t<True>>::V, "error");
    static_assert(!Tenh::Hippo::Or_f<Tenh::Typle_t<False>>::V, "error");
    static_assert(Tenh::Hippo::Or_f<Tenh::Typle_t<True,True>>::V, "error");
    static_assert(Tenh::Hippo::Or_f<Tenh::Typle_t<False,True>>::V, "error");
    static_assert(!Tenh::Hippo::Or_f<Tenh::Typle_t<False,False>>::V, "error");

    // there is currently no runtime test code here, but that could change
}

void test_EachTypeSatisfies (Context const &context)
{
    static_assert(Tenh::Hippo::EachTypeSatisfies_f<Tenh::Typle_t<>,IsFloatingPointType_e>::V, "error");
    static_assert(Tenh::Hippo::EachTypeSatisfies_f<Tenh::Typle_t<float>,IsFloatingPointType_e>::V, "error");
    static_assert(!Tenh::Hippo::EachTypeSatisfies_f<Tenh::Typle_t<int>,IsFloatingPointType_e>::V, "error");
    static_assert(Tenh::Hippo::EachTypeSatisfies_f<Tenh::Typle_t<float,double>,IsFloatingPointType_e>::V, "error");
    static_assert(!Tenh::Hippo::EachTypeSatisfies_f<Tenh::Typle_t<int,double>,IsFloatingPointType_e>::V, "error");

    // there is currently no runtime test code here, but that could change
}

void test_Sum (Context const &context)
{
    typedef Tenh::Value_t<int,0> Zero;
    typedef Tenh::Value_t<int,1> One;
    typedef Tenh::Value_t<int,3> Three;
    static_assert(Tenh::Hippo::Sum_f<Tenh::Typle_t<>,int>::V == 0, "error");
    static_assert(Tenh::Hippo::Sum_f<Tenh::Typle_t<Zero>,int>::V == 0, "error");
    static_assert(Tenh::Hippo::Sum_f<Tenh::Typle_t<Zero,One>,int>::V == 1, "error");
    static_assert(Tenh::Hippo::Sum_f<Tenh::Typle_t<One,One>,int>::V == 2, "error");
    static_assert(Tenh::Hippo::Sum_f<Tenh::Typle_t<One,One,Three>,int>::V == 5, "error");

    // there is currently no runtime test code here, but that could change
}

void test_Product (Context const &context)
{
    typedef Tenh::Value_t<int,0> Zero;
    typedef Tenh::Value_t<int,1> One;
    typedef Tenh::Value_t<int,3> Three;
    static_assert(Tenh::Hippo::Product_f<Tenh::Typle_t<>,int>::V == 1, "error");
    static_assert(Tenh::Hippo::Product_f<Tenh::Typle_t<Zero>,int>::V == 0, "error");
    static_assert(Tenh::Hippo::Product_f<Tenh::Typle_t<Zero,One>,int>::V == 0, "error");
    static_assert(Tenh::Hippo::Product_f<Tenh::Typle_t<One,One>,int>::V == 1, "error");
    static_assert(Tenh::Hippo::Product_f<Tenh::Typle_t<One,One,Three>,int>::V == 3, "error");
    static_assert(Tenh::Hippo::Product_f<Tenh::Typle_t<Three,Three>,int>::V == 9, "error");
    static_assert(Tenh::Hippo::Product_f<Tenh::Typle_t<Three,Three,Three>,int>::V == 27, "error");

    // there is currently no runtime test code here, but that could change
}

void test_Min (Context const &context)
{
    typedef Tenh::Value_t<int,0> Zero;
    typedef Tenh::Value_t<int,1> One;
    typedef Tenh::Value_t<int,3> Three;
    static_assert(Tenh::Hippo::Min_f<Tenh::Typle_t<>,int>::V == Tenh::NumericBound_t<int>::MAX, "error");
    static_assert(Tenh::Hippo::Min_f<Tenh::Typle_t<Zero,One>,int>::V == 0, "error");
    static_assert(Tenh::Hippo::Min_f<Tenh::Typle_t<One,One>,int>::V == 1, "error");
    static_assert(Tenh::Hippo::Min_f<Tenh::Typle_t<One,One,Three>,int>::V == 1, "error");
    static_assert(Tenh::Hippo::Min_f<Tenh::Typle_t<Three,Three>,int>::V == 3, "error");

    // there is currently no runtime test code here, but that could change
}

void test_Max (Context const &context)
{
    typedef Tenh::Value_t<int,0> Zero;
    typedef Tenh::Value_t<int,1> One;
    typedef Tenh::Value_t<int,3> Three;
    static_assert(Tenh::Hippo::Max_f<Tenh::Typle_t<>,int>::V == Tenh::NumericBound_t<int>::MIN, "error");
    static_assert(Tenh::Hippo::Max_f<Tenh::Typle_t<Zero,One>,int>::V == 1, "error");
    static_assert(Tenh::Hippo::Max_f<Tenh::Typle_t<One,One>,int>::V == 1, "error");
    static_assert(Tenh::Hippo::Max_f<Tenh::Typle_t<One,One,Three>,int>::V == 3, "error");
    static_assert(Tenh::Hippo::Max_f<Tenh::Typle_t<Three,Three>,int>::V == 3, "error");

    // there is currently no runtime test code here, but that could change
}

void test_Element (Context const &context)
{
    typedef Tenh::Typle_t<int,float,bool,char> T;
    // uncommenting the following line should cause a static assert about Typle_t<> having no elements
    //Tenh::Hippo::Element_f<Tenh::Typle_t<>,0>::T x;
    static_assert(Tenh::TypesAreEqual_f<Tenh::Hippo::Element_f<T,0>::T,int>::V, "error");
    static_assert(Tenh::TypesAreEqual_f<Tenh::Hippo::Element_f<T,1>::T,float>::V, "error");
    static_assert(Tenh::TypesAreEqual_f<Tenh::Hippo::Element_f<T,2>::T,bool>::V, "error");
    static_assert(Tenh::TypesAreEqual_f<Tenh::Hippo::Element_f<T,3>::T,char>::V, "error");

    // there is currently no runtime test code here, but that could change
}

void test_Contains (Context const &context)
{
    typedef Tenh::Typle_t<int,float,bool,char> T;
    static_assert(!Tenh::Hippo::Contains_f<Tenh::Typle_t<>,int>::V, "error");

    // there is currently no runtime test code here, but that could change
}

// template <typename TypeList, Uint32 INDEX, typename ExpectedLeadingTypeList>
// void test_leading_type_list (Context const &context)
// {
//     assert((Tenh::TypesAreEqual_f<typename Tenh::LeadingTypeList_f<TypeList,INDEX>::T,ExpectedLeadingTypeList>()));
// }

// template <typename TypeList, Uint32 INDEX, typename ExpectedTrailingTypeList>
// void test_trailing_type_list (Context const &context)
// {
//     assert((Tenh::TypesAreEqual_f<typename Tenh::TrailingTypeList_f<TypeList,INDEX>::T,ExpectedTrailingTypeList>()));
// }

// template <typename TypeList, Uint32 INDEX, typename ExpectedLeadingTypeList, typename ExpectedTrailingTypeList>
// void add_leading_and_trailing_type_list_test (Directory &parent)
// {
//     LVD_ADD_NAMED_TEST_CASE_FUNCTION(parent,
//                                      FORMAT("LeadingTypeList_f<" << Tenh::type_string_of<TypeList>() << ',' << INDEX << '>'),
//                                      test_leading_type_list<TypeList,INDEX,ExpectedLeadingTypeList>,
//                                      RESULT_NO_ERROR);
//     LVD_ADD_NAMED_TEST_CASE_FUNCTION(parent,
//                                      FORMAT("TrailingTypeList_f<" << Tenh::type_string_of<TypeList>() << ',' << INDEX << '>'),
//                                      test_trailing_type_list<TypeList,INDEX,ExpectedTrailingTypeList>,
//                                      RESULT_NO_ERROR);
// }

// void add_leading_and_trailing_type_list_tests (Directory &parent)
// {
//     // length 0 type list
//     add_leading_and_trailing_type_list_test<Tenh::EmptyTypeList,
//                                             0,
//                                             Tenh::EmptyTypeList, // expected leading type list
//                                             Tenh::EmptyTypeList  // expected trailing type list
//                                             >(parent);

//     // length 1 type list
//     add_leading_and_trailing_type_list_test<Tenh::TypeList_t<char>,
//                                             0,
//                                             Tenh::EmptyTypeList,   // expected leading type list
//                                             Tenh::TypeList_t<char> // expected trailing type list
//                                             >(parent);
//     add_leading_and_trailing_type_list_test<Tenh::TypeList_t<char>,
//                                             1,
//                                             Tenh::TypeList_t<char>, // expected leading type list
//                                             Tenh::EmptyTypeList     // expected trailing type list
//                                             >(parent);

//     // length 2 type list
//     add_leading_and_trailing_type_list_test<Tenh::TypeList_t<char,Tenh::TypeList_t<int> >,
//                                             0,
//                                             Tenh::EmptyTypeList,                           // expected leading type list
//                                             Tenh::TypeList_t<char,Tenh::TypeList_t<int> >  // expected trailing type list
//                                             >(parent);
//     add_leading_and_trailing_type_list_test<Tenh::TypeList_t<char,Tenh::TypeList_t<int> >,
//                                             1,
//                                             Tenh::TypeList_t<char>,                        // expected leading type list
//                                             Tenh::TypeList_t<int>                          // expected trailing type list
//                                             >(parent);
//     add_leading_and_trailing_type_list_test<Tenh::TypeList_t<char,Tenh::TypeList_t<int> >,
//                                             2,
//                                             Tenh::TypeList_t<char,Tenh::TypeList_t<int> >, // expected leading type list
//                                             Tenh::EmptyTypeList                            // expected trailing type list
//                                             >(parent);

//     // length 3 type list
//     add_leading_and_trailing_type_list_test<Tenh::TypeList_t<char,Tenh::TypeList_t<int,Tenh::TypeList_t<double> > >,
//                                             0,
//                                             Tenh::EmptyTypeList,                                                     // expected leading type list
//                                             Tenh::TypeList_t<char,Tenh::TypeList_t<int,Tenh::TypeList_t<double> > >  // expected trailing type list
//                                             >(parent);
//     add_leading_and_trailing_type_list_test<Tenh::TypeList_t<char,Tenh::TypeList_t<int,Tenh::TypeList_t<double> > >,
//                                             1,
//                                             Tenh::TypeList_t<char>,                                                  // expected leading type list
//                                             Tenh::TypeList_t<int,Tenh::TypeList_t<double> >                          // expected trailing type list
//                                             >(parent);
//     add_leading_and_trailing_type_list_test<Tenh::TypeList_t<char,Tenh::TypeList_t<int,Tenh::TypeList_t<double> > >,
//                                             2,
//                                             Tenh::TypeList_t<char,Tenh::TypeList_t<int> >,                           // expected leading type list
//                                             Tenh::TypeList_t<double>                                                 // expected trailing type list
//                                             >(parent);
//     add_leading_and_trailing_type_list_test<Tenh::TypeList_t<char,Tenh::TypeList_t<int,Tenh::TypeList_t<double> > >,
//                                             3,
//                                             Tenh::TypeList_t<char,Tenh::TypeList_t<int,Tenh::TypeList_t<double> > >, // expected leading type list
//                                             Tenh::EmptyTypeList                                                      // expected trailing type list
//                                             >(parent);
// }

void AddTests (Directory &parent)
{
    Directory &dir = parent.GetSubDirectory("Typle_t");

    add_particular_tests_for_typle<Tenh::Typle_t<>,0>(dir);
    add_particular_tests_for_typle<Tenh::Typle_t<Sint32>,1>(dir);
    add_particular_tests_for_typle<Tenh::Typle_t<Sint32,Sint8>,2>(dir);
    add_particular_tests_for_typle<Tenh::Typle_t<Sint32,Sint8,double>,3>(dir);
    LVD_ADD_TEST_CASE_FUNCTION(dir, test_OnEach, RESULT_NO_ERROR);
    LVD_ADD_TEST_CASE_FUNCTION(dir, test_NegationOfPredicate, RESULT_NO_ERROR);
    LVD_ADD_TEST_CASE_FUNCTION(dir, test_And, RESULT_NO_ERROR);
    LVD_ADD_TEST_CASE_FUNCTION(dir, test_Or, RESULT_NO_ERROR);
    LVD_ADD_TEST_CASE_FUNCTION(dir, test_EachTypeSatisfies, RESULT_NO_ERROR);
    LVD_ADD_TEST_CASE_FUNCTION(dir, test_Sum, RESULT_NO_ERROR);
    LVD_ADD_TEST_CASE_FUNCTION(dir, test_Product, RESULT_NO_ERROR);
    LVD_ADD_TEST_CASE_FUNCTION(dir, test_Min, RESULT_NO_ERROR);
    LVD_ADD_TEST_CASE_FUNCTION(dir, test_Max, RESULT_NO_ERROR);
    LVD_ADD_TEST_CASE_FUNCTION(dir, test_Element, RESULT_NO_ERROR);
    LVD_ADD_TEST_CASE_FUNCTION(dir, test_Contains, RESULT_NO_ERROR);

    // add_leading_and_trailing_type_list_tests(dir);
}

} // end of namespace Typle
} // end of namespace Test
