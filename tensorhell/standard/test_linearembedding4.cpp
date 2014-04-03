// ///////////////////////////////////////////////////////////////////////////
// test_linearembedding4.cpp by Victor Dods, created 2013/12/31
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#include "test_linearembedding.hpp"

using namespace Lvd;
using namespace std;
using namespace TestSystem;

namespace Test {
namespace LinearEmbedding {

void AddTests4 (Directory &parent)
{
    Directory &dir = parent.GetSubDirectory("linearembedding");

    typedef float Scalar;

    typedef Tenh::BasedVectorSpace_c<Tenh::VectorSpace_c<Tenh::RealField,1,Tenh::Generic>,Tenh::Basis_c<Tenh::Generic>> B1;
    typedef Tenh::BasedVectorSpace_c<Tenh::VectorSpace_c<Tenh::RealField,2,Tenh::Generic>,Tenh::Basis_c<Tenh::Generic>> B2;
    typedef Tenh::BasedVectorSpace_c<Tenh::VectorSpace_c<Tenh::RealField,3,Tenh::Generic>,Tenh::Basis_c<Tenh::Generic>> B3;
    typedef Tenh::BasedVectorSpace_c<Tenh::VectorSpace_c<Tenh::RealField,4,Tenh::Generic>,Tenh::Basis_c<Tenh::Generic>> B4;
    typedef Tenh::BasedVectorSpace_c<Tenh::VectorSpace_c<Tenh::RealField,5,Tenh::Generic>,Tenh::Basis_c<Tenh::Generic>> B5;

    add_checks_for_two_spaces<B4,B1,double>(dir);
    add_checks_for_two_spaces<B4,B2,double>(dir);
    add_checks_for_two_spaces<B4,B3,double>(dir);
    add_checks_for_two_spaces<B4,B5,double>(dir);
}

} // end of namespace LinearEmbedding
} // end of namespace Test
