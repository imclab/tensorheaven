// ///////////////////////////////////////////////////////////////////////////
// tenh/interop/eigen_svd.hpp by Victor Dods, created 2013/03/31
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_INTEROP_EIGEN_SVD_HPP_
#define TENH_INTEROP_EIGEN_SVD_HPP_

#include "tenh/core.hpp"
#include "tenh/interop/eigen.hpp"
#include "tenh/implementation/diagonal2tensor.hpp"
#include "tenh/implementation/tensor.hpp"

#include "Eigen/Core"
#include "Eigen/SVD"

namespace Tenh {

template <typename T> struct SVDReturnTypesOf_m;

// We must have t(i|l) == u(i|j) * s(j|k) * v(l|k), i.e. t = u * s * v^T. Also u, and v are square matrices.
template <typename Factor1_, typename Factor2_, typename Scalar_, typename UseArrayTypeT_>
struct SVDReturnTypesOf_m<ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<TypeList_t<Factor1_,TypeList_t<Factor2_> > >,Scalar_, UseArrayTypeT_> >
{
    template <typename UseArrayTypeU_ = UseArrayTypeT_>
    struct OfU_f
    {
    private:
        enum { STATIC_ASSERT_IN_ENUM((!IsUseImmutableArray_f<UseArrayTypeU_>::V), MUST_NOT_BE_USE_IMMUTABLE_ARRAY) };
    public:
        typedef ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<TypeList_t<Factor1_,
                                                                      TypeList_t<typename DualOf_f<Factor1_>::T > > >,
                                   Scalar_,
                                   UseArrayTypeU_> T;
    };

    template <typename UseArrayTypeS_ = UseArrayTypeT_>
    struct OfS_f
    {
    private:
        enum { STATIC_ASSERT_IN_ENUM((!IsUseImmutableArray_f<UseArrayTypeS_>::V), MUST_NOT_BE_USE_IMMUTABLE_ARRAY) };
    public:
        typedef ImplementationOf_t<Diagonal2TensorProductOfBasedVectorSpaces_c<Factor1_,Factor2_>,
                                   Scalar_,
                                   UseArrayTypeS_> T;
    };

    template <typename UseArrayTypeV_ = UseArrayTypeT_>
    struct OfV_f
    {
    private:
        enum { STATIC_ASSERT_IN_ENUM((!IsUseImmutableArray_f<UseArrayTypeV_>::V), MUST_NOT_BE_USE_IMMUTABLE_ARRAY) };
    public:
        typedef ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<TypeList_t<Factor2_,
                                                                      TypeList_t<typename DualOf_f<Factor2_>::T > > >,
                                   Scalar_,
                                   UseArrayTypeV_> T;
    };
};

template <typename T> struct EigenMatrixFor_f;

template <typename Factor1_, typename Factor2_, typename Scalar_, typename UseArrayType_>
struct EigenMatrixFor_f<ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<TypeList_t<Factor1_,TypeList_t<Factor2_> > >,Scalar_, UseArrayType_> >
{
    typedef Eigen::Matrix<Scalar_,DimensionOf_f<Factor1_>::V,DimensionOf_f<Factor2_>::V,Eigen::RowMajor> T;
};

template <typename TType_, typename UseArrayTypeU_ = UseMemberArray, typename UseArrayTypeS_ = UseMemberArray, typename UseArrayTypeV_ = UseMemberArray>
void SVD_of_2tensor (TType_ const &t,
                     typename SVDReturnTypesOf_m<TType_>::template OfU_f<UseArrayTypeU_>::T &u,
                     typename SVDReturnTypesOf_m<TType_>::template OfS_f<UseArrayTypeS_>::T &s,
                     typename SVDReturnTypesOf_m<TType_>::template OfV_f<UseArrayTypeV_>::T &v)
{
    typedef typename EigenMatrixFor_f<TType_>::T EigenMatrix;
    Eigen::JacobiSVD<EigenMatrix> svd(EigenMap_of_2tensor(t).jacobiSvd(Eigen::ComputeFullU|Eigen::ComputeFullV));
    memcpy(u.pointer_to_allocation(), &svd.matrixU()(0,0), u.allocation_size_in_bytes());
    memcpy(s.pointer_to_allocation(), &svd.singularValues()(0,0), s.allocation_size_in_bytes());
    memcpy(v.pointer_to_allocation(), &svd.matrixV()(0,0), v.allocation_size_in_bytes());
}

// SVD with u matrix but no v matrix; the singular values are in s.
template <typename TType_, typename UseArrayTypeU_ = UseMemberArray, typename UseArrayTypeS_ = UseMemberArray>
void SVD_of_2tensor (TType_ const &t,
                     typename SVDReturnTypesOf_m<TType_>::template OfU_f<UseArrayTypeU_>::T &u,
                     typename SVDReturnTypesOf_m<TType_>::template OfS_f<UseArrayTypeS_>::T &s)
{
    typedef typename EigenMatrixFor_f<TType_>::T EigenMatrix;
    Eigen::JacobiSVD<EigenMatrix> svd(EigenMap_of_2tensor(t).jacobiSvd(Eigen::ComputeFullU));
    memcpy(u.pointer_to_allocation(), &svd.matrixU()(0,0), u.allocation_size_in_bytes());
    memcpy(s.pointer_to_allocation(), &svd.singularValues()(0,0), s.allocation_size_in_bytes());
}

// SVD with v matrix but no u matrix; the singular values are in s.
template <typename TType_, typename UseArrayTypeS_ = UseMemberArray, typename UseArrayTypeV_ = UseMemberArray>
void SVD_of_2tensor (TType_ const &t,
                     typename SVDReturnTypesOf_m<TType_>::template OfS_f<UseArrayTypeS_>::T &s,
                     typename SVDReturnTypesOf_m<TType_>::template OfV_f<UseArrayTypeV_>::T &v)
{
    typedef typename EigenMatrixFor_f<TType_>::T EigenMatrix;
    Eigen::JacobiSVD<EigenMatrix> svd(EigenMap_of_2tensor(t).jacobiSvd(Eigen::ComputeFullV));
    memcpy(s.pointer_to_allocation(), &svd.singularValues()(0,0), s.allocation_size_in_bytes());
    memcpy(v.pointer_to_allocation(), &svd.matrixV()(0,0), v.allocation_size_in_bytes());
}

// SVD with no u or v matrix; the singular values are in s.
template <typename TType_, typename UseArrayTypeS_ = UseMemberArray>
void SVD_of_2tensor (TType_ const &t,
                     typename SVDReturnTypesOf_m<TType_>::template OfS_f<UseArrayTypeS_>::T &s)
{
    typedef typename EigenMatrixFor_f<TType_>::T EigenMatrix;
    Eigen::JacobiSVD<EigenMatrix> svd(EigenMap_of_2tensor(t).jacobiSvd());
    memcpy(s.pointer_to_allocation(), &svd.singularValues()(0,0), s.allocation_size_in_bytes());
}

} // end of namespace Tenh

#endif // TENH_INTEROP_EIGEN_SVD_HPP_
