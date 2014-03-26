// ///////////////////////////////////////////////////////////////////////////
// test_homogeneouspolynomials1.cpp by Ted Nitz, created 2013/04/02
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#include "test_homogeneouspolynomials.hpp"

namespace Test {
namespace HomogeneousPolynomials {

void AddTests1 (Directory &parent)
{
    Directory &dir = parent.GetSubDirectory("HomogeneousPolynomial");

    // the reason all the different combinations are enumerated by hand is
    // so that this source file can be easily broken into several source
    // files, reducing the compilation resources required and allowing
    // parallel compilation.

    // // add_particular_tests_for_scalar<Tenh::Sint8>(dir);
    // // add_particular_tests_for_scalar<Tenh::Sint32>(dir);
    // // add_particular_tests_for_scalar<Tenh::Sint64>(dir);

    // TODO: re-add tests for integral types once integral types are working

    add_particular_tests<double,1,1>(dir);
    add_particular_tests<double,1,2>(dir);
    add_particular_tests<double,1,3>(dir);
    add_particular_tests<double,2,1>(dir);
    add_particular_tests<double,2,2>(dir);
    add_particular_tests<double,2,3>(dir);
    add_particular_tests<double,3,1>(dir);
    add_particular_tests<double,3,2>(dir);
    add_particular_tests<double,3,3>(dir);
}

} // end of namespace Vector
} // end of namespace Test
