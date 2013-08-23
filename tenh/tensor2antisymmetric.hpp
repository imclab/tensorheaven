// ///////////////////////////////////////////////////////////////////////////
// tenh/tensor2antisymmetric.hpp by Victor Dods, created 2013/03/20
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_TENSOR2ANTISYMMETRIC_HPP_
#define TENH_TENSOR2ANTISYMMETRIC_HPP_

#include <cmath>
#include <ostream>

#include "tenh/core.hpp"
#include "tenh/euclideanembedding.hpp"
#include "tenh/innerproduct.hpp"
#include "tenh/interface/tensor.hpp"
#include "tenh/static_scalar_constants.hpp"
#include "tenh/meta/typetuple.hpp"
#include "tenh/vector.hpp"

namespace Tenh {

template <typename Factor1, typename Factor2 = Factor1>
struct BasisOfTensor2Antisymmetric_t
{
    static std::string type_as_string () { return "BasisOfTensor2Antisymmetric_t<" + TypeStringOf_t<Factor1>::eval() + ',' + TypeStringOf_t<Factor2>::eval() + '>'; }
};

// antisymmetric 2-tensor (its transpose is equal to its negative)
template <typename Factor1_, typename Factor2_ = Factor1_, typename Basis_ = BasisOfTensor2Antisymmetric_t<Factor1_,Factor2_>, typename Derived_ = NullType>
struct Tensor2Antisymmetric_t
    :
    public Tensor_i<typename DerivedType_t<Derived_,Tensor2Antisymmetric_t<Factor1_,Factor2_,Basis_,Derived_> >::T,
                    TypeList_t<Factor1_,TypeList_t<Factor2_> >,
                    ((Factor1_::DIM-1)*Factor1_::DIM)/2,
                    Basis_>,
    private Array_t<typename Factor1_::Scalar,
                    ((Factor1_::DIM-1)*Factor1_::DIM)/2,
                    typename DerivedType_t<Derived_,Tensor2Antisymmetric_t<Factor1_,Factor2_,Basis_,Derived_> >::T>
    // privately inherited because the use of Array_t is an implementation detail
{
    enum { STATIC_ASSERT_IN_ENUM((Lvd::Meta::TypesAreEqual<Factor1_,Factor2_>::v), FACTOR1_AND_FACTOR2_MUST_BE_IDENTICAL) };

    typedef Tensor_i<typename DerivedType_t<Derived_,Tensor2Antisymmetric_t<Factor1_,Factor2_,Basis_,Derived_> >::T,
                     TypeList_t<Factor1_,TypeList_t<Factor2_> >,
                     ((Factor1_::DIM-1)*Factor1_::DIM)/2,
                     Basis_> Parent_Tensor_i;
    typedef Array_t<typename Factor1_::Scalar,
                    ((Factor1_::DIM-1)*Factor1_::DIM)/2,
                    typename DerivedType_t<Derived_,Tensor2Antisymmetric_t<Factor1_,Factor2_,Basis_,Derived_> >::T> Parent_Array_t;
    typedef typename Parent_Tensor_i::Derived Derived;
    typedef typename Parent_Tensor_i::Scalar Scalar;
    static Uint32 const DIM = Parent_Tensor_i::DIM;
    typedef typename Parent_Tensor_i::Basis Basis;
    typedef Tensor2Antisymmetric_t<typename Factor1_::WithStandardEuclideanBasis,
                                   typename Factor2_::WithStandardEuclideanBasis,
                                   StandardEuclideanBasis,
                                   Derived_> WithStandardEuclideanBasis; // TEMP KLUDGE -- recursively convert to StandardEuclideanBasis
    typedef typename Parent_Tensor_i::Index Index;
    typedef typename Parent_Tensor_i::FactorTypeList FactorTypeList;
    typedef typename Parent_Tensor_i::FactorIndexTypeList FactorIndexTypeList;
    typedef typename Parent_Tensor_i::MultiIndex MultiIndex;
    using Parent_Tensor_i::ORDER;
    typedef Factor1_ Factor;
    typedef Factor1_ Factor1;
    typedef Factor2_ Factor2;
    static Uint32 const STRICTLY_LOWER_TRIANGULAR_COMPONENT_COUNT = ((Factor::DIM-1)*Factor::DIM)/2;
    static bool const IS_TENSOR2ANTISYMMETRIC_T = true;

    Tensor2Antisymmetric_t (WithoutInitialization const &w) : Parent_Array_t(w) { }
    Tensor2Antisymmetric_t (Scalar fill_with) : Parent_Array_t(fill_with) { }

    template <typename BundleIndexTypeList, typename BundledIndex>
    static MultiIndex_t<BundleIndexTypeList> bundle_index_map (BundledIndex const &b)
    {
        typedef typename BundleIndexTypeList::template El_t<0>::T Index1;
        typedef typename BundleIndexTypeList::template El_t<1>::T Index2;
        // this is just to check that there is a valid conversion to the requested MultiIndex_t type.
        // it doesn't actually produce any side-effects, and should be optimized out.
        {
            STATIC_ASSERT((BundleIndexTypeList::LENGTH == 2), CAN_ONLY_BUNDLE_TWO_INDICES);
            Index1 i1;
            Index2 i2;
            typename Factor1::Index f1(i1);
            typename Factor2::Index f2(i2);
            // check that the parameter BundleIndex type is compatible with Index
            Index i(b);
        }

        Uint32 row;
        Uint32 col;
        contiguous_index_to_rowcol_index(b.value(), row, col);
        return MultiIndex_t<BundleIndexTypeList>(Index1(row, DONT_CHECK_RANGE), Index2(col, DONT_CHECK_RANGE));
    }
    // these are what provide indexed expressions -- via expression templates

    // TODO: because Factor1 and Factor2 are identical, it doesn't make sense to
    // have a type coercion to either one unless they are 1-dimensional, in which case
    // it can be a type coercion to the Scalar type.  however, an antisymmetric
    // 2-tensor on a 1-dimensional space is always zero.

    // TODO: because the diagonal is always zero, there is an easy type coercion to Tensor2Diagonal_t

    using Parent_Array_t::operator[];
    using Parent_Array_t::allocation_size_in_bytes;
    using Parent_Array_t::pointer_to_allocation;

    // using two indices in a Tensor2Antisymmetric_t is breaking apart the Index type and using it
    // as a general tensor -- this is where the fancy indexing scheme happens.
    // Index1 could be Factor1::Index or Factor1::MultiIndex (checked by its use in the other functions)
    // Index2 could be Factor2::Index or Factor2::MultiIndex (checked by its use in the other functions)
    template <typename Index1, typename Index2>
    Scalar operator [] (MultiIndex_t<TypeList_t<Index1,TypeList_t<Index2> > > const &m) const
    {
        return component(m.template el<0>(), m.template el<1>());
    }

    // these are what provide indexed expressions -- via expression templates
    using Parent_Tensor_i::operator();

    // access 2-tensor components
    // Index1 could be Factor1::Index or Factor1::MultiIndex (checked by its use in the other functions)
    // Index2 could be Factor2::Index or Factor2::MultiIndex (checked by its use in the other functions)
    template <typename Index1, typename Index2>
    Scalar component (Index1 const &i1, Index2 const &i2) const
    {
        assert(i1.is_not_at_end() && i2.is_not_at_end() && "you used Index_t(x, DONT_RANGE_CHECK) inappropriately");

        if (Factor1::component_is_immutable_zero(i1) || Factor2::component_is_immutable_zero(i2))
            return Scalar(0);

        MultiIndex m(Factor1::vector_index_of(i1), Factor2::vector_index_of(i2));
        if (component_is_immutable_zero(m))
            return Scalar(0);

        return Factor1::scalar_factor_for_component(i1) *
               Factor2::scalar_factor_for_component(i2) *
               scalar_factor_for_component(m) *
               operator[](vector_index_of(m));
    }
    // write 2-tensor components -- will throw if a component doesn't correspond to a memory location
    // Index1 could be Factor1::Index or Factor1::MultiIndex (checked by its use in the other functions)
    // Index2 could be Factor2::Index or Factor2::MultiIndex (checked by its use in the other functions)
    template <typename Index1, typename Index2>
    void set_component (Index1 const &i1, Index2 const &i2, Scalar s)
    {
        assert(i1.is_not_at_end() && i2.is_not_at_end() && "you used Index_t(x, DONT_RANGE_CHECK) inappropriately");

        if (Factor1::component_is_immutable_zero(i1) || Factor2::component_is_immutable_zero(i2))
            throw std::invalid_argument("this tensor component is not writable");

        MultiIndex m(Factor1::vector_index_of(i1), Factor2::vector_index_of(i2));
        if (component_is_immutable_zero(m))
            throw std::invalid_argument("this tensor component is not writable");

        // write to the component, but divide through by the total scale factor for the component.
        operator[](vector_index_of(m))
            = s / (Factor1::scalar_factor_for_component(i1) * Factor2::scalar_factor_for_component(i2) * scalar_factor_for_component(m));
    }
    using Parent_Tensor_i::component_is_immutable_zero;
    using Parent_Tensor_i::scalar_factor_for_component;
    using Parent_Tensor_i::vector_index_of;
    // the diagonal is not stored in memory -- all components are zero.
    static bool component_is_immutable_zero (MultiIndex const &m)
    {
        return m.template el<0>() == m.template el<1>(); // off diagonal elements correspond to memory locations
    }
    // the diagonal is zero, the upper triangular components have a scalar factor of -1, and 1 otherwise.
    static Scalar scalar_factor_for_component (MultiIndex const &m)
    {
        if (m.template el<0>() == m.template el<1>()) // diagonal component
            return Scalar(0); // not actually used, but there for completeness
        else if (m.template el<0>() < m.template el<1>()) // upper triangular component
            return Scalar(-1);
        else // lower triangular component
            return Scalar(1);
    }
    // this should return iff !component_is_immutable_zero(m) and otherwise throw
    static Index vector_index_of (MultiIndex const &m)
    {
        return Index(rowcol_index_to_contiguous_index(m.template el<0>().value(), m.template el<1>().value()), DONT_CHECK_RANGE);
    }

    static std::string type_as_string ()
    {
        std::string basis_string;
        if (!Lvd::Meta::TypesAreEqual<Basis,BasisOfTensor2Antisymmetric_t<Factor1,Factor2> >())
            basis_string = ',' + TypeStringOf_t<Basis>::eval();

        std::string derived_string;
        if (!Lvd::Meta::TypesAreEqual<Derived_,NullType>())
            derived_string = ',' + TypeStringOf_t<Derived>::eval();

        return "Tensor2Antisymmetric_t<" + TypeStringOf_t<Factor>::eval() + basis_string + derived_string + '>';
    }

private:

    // functions between the indexing schemes -- multi-index is (row,col) with row > col and vector index is contiguous.
    static Uint32 rowcol_index_to_contiguous_index (Uint32 row, Uint32 col)
    {
        // strict lower triangle elements are stored contiguously in row-major order:
        // [ . . . ... ]
        // [ 0 . . ... ]
        // [ 1 2 . ... ]
        // [ 3 4 5 ... ]
        // ...
        // the index of the first element in row R is the (R-1)th triangular number, so the mapping
        // (row,col) to contiguous index i is i := r*(r-1)/2 + c.
        if (row == col)
            throw std::invalid_argument("there is no contiguous index corresponding to a diagonal component index");
        if (row < col)
            std::swap(row, col);
        return row*(row-1)/2 + col;
    }
    static void contiguous_index_to_rowcol_index (Uint32 i, Uint32 &row, Uint32 &col)
    {
        // given i, row = floor((1 + sqrt(1+8*i))/2) and col = i - r*(r-1)/2.
        // TODO: implement as lookup table
        row = Uint32(std::floor(0.5f + std::sqrt(0.25f + 2.0f*i)));
        col = i - row*(row-1)/2;
    }

    friend struct InnerProduct_t<Tensor2Antisymmetric_t,Basis>;

    // this has no definition, and is designed to generate a compiler error if used (use the one accepting WithoutInitialization instead).
    Tensor2Antisymmetric_t ();
};

// template specialization for the inner product in this particular coordinatization of Tensor2Antisymmetric_t
// (specified by BasisOfTensor2Antisymmetric_t<Factor1,Factor2>).
template <typename Factor1, typename Factor2, typename Derived>
struct InnerProduct_t<Tensor2Antisymmetric_t<Factor1,Factor2,BasisOfTensor2Antisymmetric_t<Factor1,Factor2>,Derived>,BasisOfTensor2Antisymmetric_t<Factor1,Factor2> >
{
    typedef Tensor2Antisymmetric_t<Factor1,Factor2,BasisOfTensor2Antisymmetric_t<Factor1,Factor2>,Derived> Tensor2Antisymmetric;
    typedef typename Tensor2Antisymmetric::Scalar Scalar;
    typedef typename Tensor2Antisymmetric::Index Index;

    static Scalar component (Index const &i)
    {
        Uint32 row;
        Uint32 col;
        Tensor2Antisymmetric::contiguous_index_to_rowcol_index(i.value(), row, col);
        return Scalar(2) *
               InnerProduct_t<Factor1,typename Factor1::Basis>::component(typename Factor1::Index(row, DONT_CHECK_RANGE)) *
               InnerProduct_t<Factor2,typename Factor2::Basis>::component(typename Factor2::Index(col, DONT_CHECK_RANGE));
    }
};

template <typename TensorFactor1_, typename TensorFactor2_, typename TensorDerived>
struct EuclideanEmbedding_t<Tensor2Antisymmetric_t<TensorFactor1_,
                                                   TensorFactor2_,
                                                   BasisOfTensor2Antisymmetric_t<TensorFactor1_,TensorFactor2_>,
                                                   TensorDerived> >
    :
    public EuclideanEmbedding_Parent_Tensor_i<Tensor2Antisymmetric_t<TensorFactor1_,
                                                                     TensorFactor2_,
                                                                     BasisOfTensor2Antisymmetric_t<TensorFactor1_,TensorFactor2_>,
                                                                     TensorDerived> >::T
{
    typedef typename EuclideanEmbedding_Parent_Tensor_i<Tensor2Antisymmetric_t<TensorFactor1_,
                                                                               TensorFactor2_,
                                                                               BasisOfTensor2Antisymmetric_t<TensorFactor1_,TensorFactor2_>,
                                                                               TensorDerived> >::T Parent_Tensor_i;
    typedef typename Parent_Tensor_i::Derived Derived;
    typedef typename Parent_Tensor_i::Scalar Scalar;
    using Parent_Tensor_i::DIM;
    typedef typename Parent_Tensor_i::Basis Basis;
    typedef typename Parent_Tensor_i::Index Index;
    typedef typename Parent_Tensor_i::FactorTypeList FactorTypeList;
    typedef typename Parent_Tensor_i::FactorIndexTypeList FactorIndexTypeList;
    typedef typename Parent_Tensor_i::MultiIndex MultiIndex;
    using Parent_Tensor_i::ORDER;
    typedef TensorFactor1_ TensorFactor1;
    typedef TensorFactor2_ TensorFactor2;
    typedef Tensor2Antisymmetric_t<TensorFactor1,
                                   TensorFactor2,
                                   BasisOfTensor2Antisymmetric_t<TensorFactor1,TensorFactor2>,
                                   TensorDerived> Tensor2Antisymmetric;

    Scalar operator [] (MultiIndex const &m) const
    {
        EuclideanEmbedding_t<TensorFactor1> e1;
        EuclideanEmbedding_t<TensorFactor2> e2;
        TypedIndex_t<typename TensorFactor1::WithStandardEuclideanBasis,'i'> i;
        TypedIndex_t<TensorFactor1,'j'> j;
        TypedIndex_t<typename TensorFactor2::WithStandardEuclideanBasis,'k'> k;
        TypedIndex_t<TensorFactor2,'l'> l;
        TypedIndex_t<typename Tensor2Antisymmetric::WithStandardEuclideanBasis,'p'> p;
        TypedIndex_t<Tensor2Antisymmetric,'q'> q;
        // TODO: the InnerProduct_t is providing a factor of 2, hence why we're providing
        // a factor of 1/sqrt(2) -- figure out how to use the default InnerProduct_t instead
        return (Static<Scalar>::SQRT_TWO/Scalar(2)*(e1(i|j)*e2(k|l)).bundle(i|k,p).bundle(j|l,q))[m];
    }

    // NOTE: these may be unnecessary/undesired, because this type does not represent a vector space
//     using Parent_Tensor_i::component_is_immutable_zero;
//     using Parent_Tensor_i::scalar_factor_for_component;
//     using Parent_Tensor_i::vector_index_of;
    static std::string type_as_string ()
    {
        return "EuclideanEmbedding_t<" + TypeStringOf_t<Tensor2Antisymmetric>::eval() + '>';
    }
};

template <typename TensorFactor1_, typename TensorFactor2_, typename TensorDerived>
struct EuclideanEmbeddingInverse_t<Tensor2Antisymmetric_t<TensorFactor1_,
                                                          TensorFactor2_,
                                                          BasisOfTensor2Antisymmetric_t<TensorFactor1_,TensorFactor2_>,
                                                          TensorDerived> >
    :
    public EuclideanEmbeddingInverse_Parent_Tensor_i<Tensor2Antisymmetric_t<TensorFactor1_,
                                                                            TensorFactor2_,
                                                                            BasisOfTensor2Antisymmetric_t<TensorFactor1_,TensorFactor2_>,
                                                                            TensorDerived> >::T
{
    typedef typename EuclideanEmbeddingInverse_Parent_Tensor_i<Tensor2Antisymmetric_t<TensorFactor1_,
                                                                                      TensorFactor2_,
                                                                                      BasisOfTensor2Antisymmetric_t<TensorFactor1_,TensorFactor2_>,
                                                                                      TensorDerived> >::T Parent_Tensor_i;
    typedef typename Parent_Tensor_i::Derived Derived;
    typedef typename Parent_Tensor_i::Scalar Scalar;
    using Parent_Tensor_i::DIM;
    typedef typename Parent_Tensor_i::Basis Basis;
    typedef typename Parent_Tensor_i::Index Index;
    typedef typename Parent_Tensor_i::FactorTypeList FactorTypeList;
    typedef typename Parent_Tensor_i::FactorIndexTypeList FactorIndexTypeList;
    typedef typename Parent_Tensor_i::MultiIndex MultiIndex;
    using Parent_Tensor_i::ORDER;
    typedef TensorFactor1_ TensorFactor1;
    typedef TensorFactor2_ TensorFactor2;
    typedef Tensor2Antisymmetric_t<TensorFactor1,
                                   TensorFactor2,
                                   BasisOfTensor2Antisymmetric_t<TensorFactor1,TensorFactor2>,
                                   TensorDerived> Tensor2Antisymmetric;

    Scalar operator [] (MultiIndex const &m) const
    {
        EuclideanEmbeddingInverse_t<TensorFactor1> e1_inv;
        EuclideanEmbeddingInverse_t<TensorFactor2> e2_inv;
        TypedIndex_t<TensorFactor1,'i'> i;
        TypedIndex_t<typename TensorFactor1::WithStandardEuclideanBasis,'j'> j;
        TypedIndex_t<TensorFactor2,'k'> k;
        TypedIndex_t<typename TensorFactor2::WithStandardEuclideanBasis,'l'> l;
        TypedIndex_t<Tensor2Antisymmetric,'p'> p;
        TypedIndex_t<typename Tensor2Antisymmetric::WithStandardEuclideanBasis,'q'> q;
        // TODO: figure out real scalar factor
        return (Static<Scalar>::SQRT_TWO/Scalar(2)*(e1_inv(i|j)*e2_inv(k|l)).bundle(i|k,p).bundle(j|l,q))[m];
    }

    // NOTE: these may be unnecessary/undesired, because this type does not represent a vector space
//     using Parent_Tensor_i::component_is_immutable_zero;
//     using Parent_Tensor_i::scalar_factor_for_component;
//     using Parent_Tensor_i::vector_index_of;
    static std::string type_as_string ()
    {
        return "EuclideanEmbeddingInverse_t<" + TypeStringOf_t<Tensor2Antisymmetric>::eval() + '>';
    }
};

} // end of namespace Tenh

#endif // TENH_TENSOR2ANTISYMMETRIC_HPP_
