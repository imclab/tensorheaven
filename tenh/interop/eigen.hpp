// ///////////////////////////////////////////////////////////////////////////
// tenh/interop/eigen.hpp by Victor Dods, created 2013/03/31
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_INTEROP_EIGEN_HPP_
#define TENH_INTEROP_EIGEN_HPP_

#include "tenh/core.hpp"
#include "tenh/tensor2.hpp"

#include "Eigen/Core"

namespace Tenh {

template <typename Factor1, typename Factor2, typename Basis, typename Derived>
Eigen::Map<Eigen::Matrix<typename Factor1::Scalar,Factor1::DIM,Factor2::DIM,Eigen::RowMajor> const>
    EigenMap_of_Tensor2 (Tensor2_t<Factor1,Factor2,Basis,Derived> const &t)
{
    return Eigen::Map<Eigen::Matrix<typename Factor1::Scalar,Factor1::DIM,Factor2::DIM,Eigen::RowMajor> const>
        (t.data_pointer());
}

template <typename Factor1, typename Factor2, typename Basis, typename Derived>
Eigen::Map<Eigen::Matrix<typename Factor1::Scalar,Factor1::DIM,Factor2::DIM,Eigen::RowMajor> >
    EigenMap_of_Tensor2 (Tensor2_t<Factor1,Factor2,Basis,Derived> &t)
{
    return Eigen::Map<Eigen::Matrix<typename Factor1::Scalar,Factor1::DIM,Factor2::DIM,Eigen::RowMajor> >
        (t.data_pointer());
}

template <typename Tensor2Type>
Eigen::Matrix<typename Tensor2Type::Scalar,Tensor2Type::Factor1::DIM,Tensor2Type::Factor2::DIM,Eigen::RowMajor>
    euclideanly_embedded_EigenMatrix_of (Tensor2Type const &t)
{
    Tensor2_t<typename Tensor2Type::WithStandardEuclideanBasis::Factor1,
              typename Tensor2Type::WithStandardEuclideanBasis::Factor2> temp(Static<WithoutInitialization>::SINGLETON);
    TypedIndex_t<typename Tensor2Type::WithStandardEuclideanBasis,'p'> p;
    TypedIndex_t<Tensor2Type,'q'> q;
    EuclideanEmbedding_t<Tensor2Type> e;
    TypedIndex_t<typename Tensor2Type::WithStandardEuclideanBasis::Factor1,'i'> i;
    TypedIndex_t<typename Tensor2Type::WithStandardEuclideanBasis::Factor2,'j'> j;
    temp(i|j).no_alias() = (e(p|q)*t(q)).split(p,i|j); // Euclideanly embed and "blow up" into temp
    return EigenMap_of_Tensor2(temp); // NOTE: this converts into Eigen::Matrix upon return
}

} // end of namespace Tenh

#endif // TENH_INTEROP_EIGEN_HPP_
