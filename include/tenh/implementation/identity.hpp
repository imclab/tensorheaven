// ///////////////////////////////////////////////////////////////////////////
// tenh/implementation/identity.hpp by Victor Dods, created 2013/12/03
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_IMPLEMENTATION_IDENTITY_HPP_
#define TENH_IMPLEMENTATION_IDENTITY_HPP_

#include "tenh/core.hpp"

#include "tenh/implementation/scalar2tensor.hpp"

namespace Tenh {

// ///////////////////////////////////////////////////////////////////////////
// Identity_f metafunction (should return an ImplementationOf_t type that
// uses ProceduralArray_t)
// ///////////////////////////////////////////////////////////////////////////

template <typename OnBasedVectorSpace_, typename Scalar_>
struct Identity_f
{
private:
    typedef Scalar2TensorProductOfBasedVectorSpaces_c<OnBasedVectorSpace_,
                                                      typename DualOf_f<OnBasedVectorSpace_>::T> Scalar2Tensor;
    typedef typename ComponentGenerator_Constant_f<Scalar_,DimensionOf_f<Scalar2Tensor>::V,1>::T ComponentGenerator;
    Identity_f ();
public:
    typedef ImplementationOf_t<Scalar2Tensor,Scalar_,UseProceduralArray_t<ComponentGenerator>> T;
};

} // end of namespace Tenh

#endif // TENH_IMPLEMENTATION_IDENTITY_HPP_
