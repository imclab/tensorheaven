// ///////////////////////////////////////////////////////////////////////////
// test_multivariatepolynomials2.cpp by Ted Nitz, created 2013/10/01
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#include "test_multivariatepolynomials.hpp"

namespace Test {
namespace MultivariatePolynomials {

void AddTests2 (Directory &parent)
{
    Directory &dir = parent.GetSubDirectory("MultivariatePolynomial");

    // the reason all the different combinations are enumerated by hand is
    // so that this source file can be easily broken into several source
    // files, reducing the compilation resources required and allowing
    // parallel compilation.

    // // add_particular_tests_for_scalar<Tenh::Sint8>(dir);
    // // add_particular_tests_for_scalar<Tenh::Uint32>(dir);
    // // add_particular_tests_for_scalar<Tenh::Sint64>(dir);

    // TODO: re-add tests for integral types once integral types are working

    add_particular_tests<complex<float>,1,1>(dir);
    add_particular_tests<complex<float>,1,2>(dir);
    add_particular_tests<complex<float>,1,3>(dir);
    add_particular_tests<complex<float>,2,1>(dir);
    add_particular_tests<complex<float>,2,2>(dir);
    add_particular_tests<complex<float>,2,3>(dir);
    add_particular_tests<complex<float>,3,1>(dir);
    add_particular_tests<complex<float>,3,2>(dir);
    add_particular_tests<complex<float>,3,3>(dir);
}

} // end of namespace Vector
} // end of namespace Test
