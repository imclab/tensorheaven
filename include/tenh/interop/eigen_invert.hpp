// ///////////////////////////////////////////////////////////////////////////
// tenh/interop/eigen_invert.hpp by Victor Dods, created 2013/12/13
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_INTEROP_EIGEN_INVERT_HPP_
#define TENH_INTEROP_EIGEN_INVERT_HPP_

#include "tenh/core.hpp"

#include "tenh/conceptual/symmetricpower.hpp"
#include "tenh/conceptual/tensorproduct.hpp"
#include "tenh/interop/eigen.hpp"
#include "tenh/implementation/tensor.hpp"

namespace Tenh {

// returns true iff the tensor was actually considered to be invertible (by Eigen::MatrixBase::computeInverseWithCheck)
template <typename Derived0_,
          typename Derived1_,
          typename Scalar_,
          typename Factor0_,
          typename Factor1_,
          ComponentQualifier COMPONENT_QUALIFIER_>
bool invert_2tensor (Tensor_i<Derived0_,
                              Scalar_,
                              TensorProductOfBasedVectorSpaces_c<Typle_t<Factor0_,Factor1_>>,
                              COMPONENT_QUALIFIER_> const &t,
                     Tensor_i<Derived1_,
                              Scalar_,
                              TensorProductOfBasedVectorSpaces_c<Typle_t<typename DualOf_f<Factor1_>::T,typename DualOf_f<Factor0_>::T>>,
                              ComponentQualifier::NONCONST_MEMORY> &t_inverse)
{
    STATIC_ASSERT(DimensionOf_f<Factor0_>::V == DimensionOf_f<Factor1_>::V, FACTOR_DIMENSIONS_MUST_BE_EQUAL);
    // create Eigen Maps for each of the parameters -- this way no copying is necessary;
    // the t tensor's components are read directly by Eigen, and t_inverse's components
    // are directly written to by Eigen.
    typedef Eigen::Matrix<Scalar_,
                          DimensionOf_f<Factor0_>::V,
                          DimensionOf_f<Factor1_>::V,
                          Eigen::RowMajor> EigenMatrixType;
    typename EigenMapOf2Tensor_const_f<Factor0_,Factor1_,Scalar_>::T eigen_map_of_t(EigenMap_of_2tensor(t));
    typename EigenMapOf2Tensor_nonconst_f<typename DualOf_f<Factor1_>::T,
                                          typename DualOf_f<Factor0_>::T,
                                          Scalar_>::T eigen_map_of_t_inverse(EigenMap_of_2tensor(t_inverse));
    Eigen::FullPivLU<EigenMatrixType> lu(eigen_map_of_t);

    bool is_invertible = lu.isInvertible();
    if (is_invertible)
        eigen_map_of_t_inverse = lu.inverse();
    return is_invertible;
}

// returns true iff the tensor was actually considered to be invertible (by Eigen::MatrixBase::computeInverseWithCheck)
template <typename Derived0_,
          typename Derived1_,
          typename Scalar_,
          typename Factor_,
          ComponentQualifier COMPONENT_QUALIFIER_>
bool invert_2tensor (EmbeddableAsTensor_i<Derived0_,
                                          Scalar_,
                                          SymmetricPowerOfBasedVectorSpace_c<2,Factor_>,
                                          COMPONENT_QUALIFIER_> const &s,
                     EmbeddableAsTensor_i<Derived1_,
                                          Scalar_,
                                          SymmetricPowerOfBasedVectorSpace_c<2,typename DualOf_f<Factor_>::T>,
                                          ComponentQualifier::NONCONST_MEMORY> &s_inverse)
{
    // create Eigen Maps for each of the parameters -- this way no copying is necessary;
    // the t tensor's components are read directly by Eigen, and t_inverse's components
    // are directly written to by Eigen.
    typedef ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<Typle_t<Factor_,Factor_>>,
                               Scalar_> TwoTensorType;
    typedef ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<Typle_t<typename DualOf_f<Factor_>::T,typename DualOf_f<Factor_>::T>>,
                               Scalar_> TwoTensorDualType;
    typedef SymmetricPowerOfBasedVectorSpace_c<2,typename DualOf_f<Factor_>::T> SymDualType;

    AbstractIndex_c<'a'> a;
    AbstractIndex_c<'i'> i;
    AbstractIndex_c<'j'> j;

    TwoTensorType t(Static<WithoutInitialization>::SINGLETON);
    TwoTensorDualType t_inverse(Static<WithoutInitialization>::SINGLETON);
    t(i*j) = s.split(i*j);

    bool is_invertible = invert_2tensor(t, t_inverse);
    if (is_invertible)
        s_inverse(a) = t_inverse(i*j).bundle(i*j, SymDualType(), a);
    return is_invertible;
}

} // end of namespace Tenh

#endif // TENH_INTEROP_EIGEN_INVERT_HPP_
