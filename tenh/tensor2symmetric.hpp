// ///////////////////////////////////////////////////////////////////////////
// tenh/tensor2symmetric.hpp by Victor Dods, created 2013/03/20
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_TENSOR2SYMMETRIC_HPP_
#define TENH_TENSOR2SYMMETRIC_HPP_

#ifdef __clang_version__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtautological-compare"
#endif // __clang_version__

#include <cmath> // TEMP until rowcol/contiguous index map is implemented as a lookup table
#include <ostream>

#include "tenh/core.hpp"
#include "tenh/euclideanembedding.hpp"
#include "tenh/innerproduct.hpp"
#include "tenh/interface/tensor.hpp"
#include "tenh/meta/typetuple.hpp"
#include "tenh/vector.hpp"

namespace Tenh {

template <typename Factor1, typename Factor2 = Factor1>
struct BasisOfTensor2Symmetric_t
{
    static std::string type_as_string () { return "BasisOfTensor2Symmetric_t<" + TypeStringOf_t<Factor1>::eval() + ',' + TypeStringOf_t<Factor2>::eval() + '>'; }
};

// symmetric 2-tensor (it is equal to its transpose)
template <typename Factor1_, typename Factor2_ = Factor1_, typename Basis_ = BasisOfTensor2Symmetric_t<Factor1_,Factor2_>, typename Derived_ = NullType>
struct Tensor2Symmetric_t
    :
    public Tensor_i<typename DerivedType_t<Derived_,Tensor2Symmetric_t<Factor1_,Factor2_,Basis_,Derived_> >::T,
                    TypeList_t<Factor1_,TypeList_t<Factor2_> >,
                    ((Factor1_::DIM+1)*Factor1_::DIM)/2,
                    Basis_>,
    private Array_t<typename Factor1_::Scalar,
                    ((Factor1_::DIM+1)*Factor1_::DIM)/2,
                    typename DerivedType_t<Derived_,Tensor2Symmetric_t<Factor1_,Factor2_,Basis_,Derived_> >::T>
{
    enum { STATIC_ASSERT_IN_ENUM((Lvd::Meta::TypesAreEqual<Factor1_,Factor2_>::v), FACTOR1_AND_FACTOR2_MUST_BE_IDENTICAL) };

    typedef Tensor_i<typename DerivedType_t<Derived_,Tensor2Symmetric_t<Factor1_,Factor2_,Basis_,Derived_> >::T,
                     TypeList_t<Factor1_,TypeList_t<Factor2_> >,
                     ((Factor1_::DIM+1)*Factor1_::DIM)/2,
                     Basis_> Parent_Tensor_i;
    typedef Array_t<typename Factor1_::Scalar,
                    ((Factor1_::DIM+1)*Factor1_::DIM)/2,
                    typename DerivedType_t<Derived_,Tensor2Symmetric_t<Factor1_,Factor2_,Basis_,Derived_> >::T> Parent_Array_t;
    typedef typename Parent_Tensor_i::Derived Derived;
    typedef typename Parent_Tensor_i::Scalar Scalar;
    static Uint32 const DIM = Parent_Tensor_i::DIM;
    typedef typename Parent_Tensor_i::Basis Basis;
    typedef Tensor2Symmetric_t<typename Factor1_::WithStandardEuclideanBasis,
                               typename Factor2_::WithStandardEuclideanBasis,
                               StandardEuclideanBasis,
                               Derived_> WithStandardEuclideanBasis; // TEMP KLUDGE -- recursively convert to StandardEuclideanBasis
    typedef typename Parent_Tensor_i::Index Index;
    typedef typename Parent_Tensor_i::FactorTypeList FactorTypeList;
    typedef typename Parent_Tensor_i::FactorIndexTypeList FactorIndexTypeList;
    typedef typename Parent_Tensor_i::MultiIndex MultiIndex;
    using Parent_Tensor_i::DEGREE;
    typedef Factor1_ Factor;
    typedef Factor1_ Factor1;
    typedef Factor2_ Factor2;
    static Uint32 const DIAGONAL_COMPONENT_COUNT = Factor::DIM;
    static Uint32 const STRICTLY_LOWER_TRIANGULAR_COMPONENT_COUNT = ((Factor::DIM-1)*Factor::DIM)/2;
    static bool const IS_TENSOR2SYMMETRIC_T = true;

    Tensor2Symmetric_t (WithoutInitialization const &w) : Parent_Array_t(w) { }
    Tensor2Symmetric_t (Scalar fill_with) : Parent_Array_t(fill_with) { }

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

    // TODO: because Factor1 and Factor2 are identical, it doesn't make sense to
    // have a type coercion to either one unless they are 1-dimensional, in which case
    // it can be a type coercion to the Scalar type.

    // TODO: because the diagonal is indexed contiguously last, there is an easy type coercion (projection) to Tensor2Diagonal_t
    // TODO: because the strict lower triangle is indexed contiguously first, there is an easy type coercion (projection) to lower triangular

    using Parent_Array_t::operator[];
    using Parent_Array_t::data_size_in_bytes;
    using Parent_Array_t::data_pointer;

    // using two indices in a Tensor2Symmetric_t is breaking apart the Index type and using it
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
        return Factor1::scalar_factor_for_component(i1) *
               Factor2::scalar_factor_for_component(i2) *
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
        // write to the component, but divide through by the total scale factor for the component.
        operator[](vector_index_of(m))
            = s / (Factor1::scalar_factor_for_component(i1) * Factor2::scalar_factor_for_component(i2));
    }
    using Parent_Tensor_i::component_is_immutable_zero;
    using Parent_Tensor_i::scalar_factor_for_component;
    using Parent_Tensor_i::vector_index_of;
    // all components correspond exactly to a memory location
    static bool component_is_immutable_zero (MultiIndex const &m) { return false; }
    // all components have a scale factor of 1
    static Scalar scalar_factor_for_component (MultiIndex const &m) { return Scalar(1); }
    // this should return iff !component_is_immutable_zero(m) and otherwise throw
    static Index vector_index_of (MultiIndex const &m)
    {
        return Index(rowcol_index_to_contiguous_index(m.template el<0>().value(), m.template el<1>().value()), DONT_CHECK_RANGE);
    }

    static std::string type_as_string ()
    {
        std::string basis_string;
        if (!Lvd::Meta::TypesAreEqual<Basis,BasisOfTensor2Symmetric_t<Factor1,Factor2> >())
            basis_string = ',' + TypeStringOf_t<Basis>::eval();

        std::string derived_string;
        if (!Lvd::Meta::TypesAreEqual<Derived_,NullType>())
            derived_string = ',' + TypeStringOf_t<Derived>::eval();

        return "Tensor2Symmetric_t<" + TypeStringOf_t<Factor>::eval() + basis_string + derived_string + '>';
    }

private:

    // functions between the indexing schemes -- multi-index is (row,col) with row >= col and vector index is contiguous.
    static Uint32 rowcol_index_to_contiguous_index (Uint32 row, Uint32 col)
    {
        // strict lower triangle elements are stored contiguously in row-major order:
        // [ d0  .  . ... ]
        // [  0 d1  . ... ]
        // [  1  2 d2 ... ]
        // [  3  4  5 ... ]
        // ...
        // the index of the first element in row R is the (R-1)th triangular number, so the mapping
        // (row,col) to contiguous index i is i := r*(r-1)/2 + c.
        // the diagonal is indexed contiguously after the strictly lower triangular components.
        if (row == col)
            return STRICTLY_LOWER_TRIANGULAR_COMPONENT_COUNT + row;

        if (row < col)
            std::swap(row, col);//throw std::invalid_argument("row must be greater than col");
        // now it is the same as antisymmetric indexing
        return row*(row-1)/2 + col;
    }
    static void contiguous_index_to_rowcol_index (Uint32 i, Uint32 &row, Uint32 &col)
    {
        // TODO: implement as lookup table
        if (i >= STRICTLY_LOWER_TRIANGULAR_COMPONENT_COUNT)
        {
            row = i - STRICTLY_LOWER_TRIANGULAR_COMPONENT_COUNT;
            col = row;
        }
        else // same as antisymmetric indexing
        {
            // given i, row = floor((1 + sqrt(1+8*i))/2) and col = i - r*(r-1)/2.
            row = Uint32(std::floor(0.5f + std::sqrt(0.25f + 2.0f*i)));
            col = i - row*(row-1)/2;
        }
    }

    friend struct InnerProduct_t<Tensor2Symmetric_t,Basis>;

    // this has no definition, and is designed to generate a compiler error if used (use the one accepting WithoutInitialization instead).
    Tensor2Symmetric_t ();
};

// template specialization for the inner product in this particular coordinatization of Tensor2Symmetric_t
// (specified by BasisOfTensor2Diagonal_t<Factor1,Factor2>).
template <typename Factor1, typename Factor2, typename Derived>
struct InnerProduct_t<Tensor2Symmetric_t<Factor1,Factor2,BasisOfTensor2Symmetric_t<Factor1,Factor2>,Derived>,BasisOfTensor2Symmetric_t<Factor1,Factor2> >
{
    typedef Tensor2Symmetric_t<Factor1,Factor2,BasisOfTensor2Symmetric_t<Factor1,Factor2>,Derived> Tensor2Symmetric;
    typedef typename Tensor2Symmetric::Scalar Scalar;
    typedef typename Tensor2Symmetric::Index Index;

    static Scalar component (Index const &i)
    {
        Uint32 row;
        Uint32 col;
        Tensor2Symmetric::contiguous_index_to_rowcol_index(i.value(), row, col);
        return (i.value() < Tensor2Symmetric::STRICTLY_LOWER_TRIANGULAR_COMPONENT_COUNT ? Scalar(2) : Scalar(1)) *
               InnerProduct_t<Factor1,typename Factor1::Basis>::component(typename Factor1::Index(row, DONT_CHECK_RANGE)) *
               InnerProduct_t<Factor2,typename Factor2::Basis>::component(typename Factor2::Index(col, DONT_CHECK_RANGE));
    }
};

template <typename TensorFactor1_, typename TensorFactor2_, typename TensorDerived>
struct EuclideanEmbedding_t<Tensor2Symmetric_t<TensorFactor1_,
                                               TensorFactor2_,
                                               BasisOfTensor2Symmetric_t<TensorFactor1_,TensorFactor2_>,
                                               TensorDerived> >
    :
    public EuclideanEmbedding_Parent_Tensor_i<Tensor2Symmetric_t<TensorFactor1_,
                                                                 TensorFactor2_,
                                                                 BasisOfTensor2Symmetric_t<TensorFactor1_,TensorFactor2_>,
                                                                 TensorDerived> >::T
{
    typedef typename EuclideanEmbedding_Parent_Tensor_i<Tensor2Symmetric_t<TensorFactor1_,
                                                                           TensorFactor2_,
                                                                           BasisOfTensor2Symmetric_t<TensorFactor1_,TensorFactor2_>,
                                                                           TensorDerived> >::T Parent_Tensor_i;
    typedef typename Parent_Tensor_i::Derived Derived;
    typedef typename Parent_Tensor_i::Scalar Scalar;
    using Parent_Tensor_i::DIM;
    typedef typename Parent_Tensor_i::Basis Basis;
    typedef typename Parent_Tensor_i::Index Index;
    typedef typename Parent_Tensor_i::FactorTypeList FactorTypeList;
    typedef typename Parent_Tensor_i::FactorIndexTypeList FactorIndexTypeList;
    typedef typename Parent_Tensor_i::MultiIndex MultiIndex;
    using Parent_Tensor_i::DEGREE;
    typedef TensorFactor1_ TensorFactor1;
    typedef TensorFactor2_ TensorFactor2;
    typedef Tensor2Symmetric_t<TensorFactor1,
                               TensorFactor2,
                               BasisOfTensor2Symmetric_t<TensorFactor1,TensorFactor2>,
                               TensorDerived> Tensor2Symmetric;

    Scalar operator [] (MultiIndex const &m) const
    {
        EuclideanEmbedding_t<TensorFactor1> e1;
        EuclideanEmbedding_t<TensorFactor2> e2;
        TypedIndex_t<typename TensorFactor1::WithStandardEuclideanBasis,'i'> i;
        TypedIndex_t<TensorFactor1,'j'> j;
        TypedIndex_t<typename TensorFactor2::WithStandardEuclideanBasis,'k'> k;
        TypedIndex_t<TensorFactor2,'l'> l;
        TypedIndex_t<typename Tensor2Symmetric::WithStandardEuclideanBasis,'p'> p;
        TypedIndex_t<Tensor2Symmetric,'q'> q;
        // the InnerProduct_t is providing a factor of 2, hence why we're providing
        // a factor of 1/sqrt(2) -- figure out how to use the default InnerProduct_t instead
        Uint32 col = m.template el<1>().value();
        if (col < Tensor2Symmetric::STRICTLY_LOWER_TRIANGULAR_COMPONENT_COUNT)
        {
            return (Static<Scalar>::SQRT_TWO/Scalar(2)*(e1(i|j)*e2(k|l)).bundle(i|k,p).bundle(j|l,q))[m];
        }
        else
        {
            return ((e1(i|j)*e2(k|l)).bundle(i|k,p).bundle(j|l,q))[m];
        }
    }

    // NOTE: these may be unnecessary/undesired, because this type does not represent a vector space
//     using Parent_Tensor_i::component_is_immutable_zero;
//     using Parent_Tensor_i::scalar_factor_for_component;
//     using Parent_Tensor_i::vector_index_of;
    static std::string type_as_string ()
    {
        return "EuclideanEmbedding_t<" + TypeStringOf_t<Tensor2Symmetric>::eval() + '>';
    }
};

template <typename TensorFactor1_, typename TensorFactor2_, typename TensorDerived>
struct EuclideanEmbeddingInverse_t<Tensor2Symmetric_t<TensorFactor1_,
                                                      TensorFactor2_,
                                                      BasisOfTensor2Symmetric_t<TensorFactor1_,TensorFactor2_>,
                                                      TensorDerived> >
    :
    public EuclideanEmbeddingInverse_Parent_Tensor_i<Tensor2Symmetric_t<TensorFactor1_,
                                                                        TensorFactor2_,
                                                                        BasisOfTensor2Symmetric_t<TensorFactor1_,TensorFactor2_>,
                                                                        TensorDerived> >::T
{
    typedef typename EuclideanEmbeddingInverse_Parent_Tensor_i<Tensor2Symmetric_t<TensorFactor1_,
                                                                                  TensorFactor2_,
                                                                                  BasisOfTensor2Symmetric_t<TensorFactor1_,TensorFactor2_>,
                                                                                  TensorDerived> >::T Parent_Tensor_i;
    typedef typename Parent_Tensor_i::Derived Derived;
    typedef typename Parent_Tensor_i::Scalar Scalar;
    using Parent_Tensor_i::DIM;
    typedef typename Parent_Tensor_i::Basis Basis;
    typedef typename Parent_Tensor_i::Index Index;
    typedef typename Parent_Tensor_i::FactorTypeList FactorTypeList;
    typedef typename Parent_Tensor_i::FactorIndexTypeList FactorIndexTypeList;
    typedef typename Parent_Tensor_i::MultiIndex MultiIndex;
    using Parent_Tensor_i::DEGREE;
    typedef TensorFactor1_ TensorFactor1;
    typedef TensorFactor2_ TensorFactor2;
    typedef Tensor2Symmetric_t<TensorFactor1,
                               TensorFactor2,
                               BasisOfTensor2Symmetric_t<TensorFactor1,TensorFactor2>,
                               TensorDerived> Tensor2Symmetric;

    Scalar operator [] (MultiIndex const &m) const
    {
        EuclideanEmbeddingInverse_t<TensorFactor1> e1_inv;
        EuclideanEmbeddingInverse_t<TensorFactor2> e2_inv;
        TypedIndex_t<TensorFactor1,'i'> i;
        TypedIndex_t<typename TensorFactor1::WithStandardEuclideanBasis,'j'> j;
        TypedIndex_t<TensorFactor2,'k'> k;
        TypedIndex_t<typename TensorFactor2::WithStandardEuclideanBasis,'l'> l;
        TypedIndex_t<Tensor2Symmetric,'p'> p;
        TypedIndex_t<typename Tensor2Symmetric::WithStandardEuclideanBasis,'q'> q;
        // TODO: figure out the real scalar factor
        Uint32 row = m.template el<0>().value();
        if (row < Tensor2Symmetric::STRICTLY_LOWER_TRIANGULAR_COMPONENT_COUNT)
        {
            return (Static<Scalar>::SQRT_TWO/Scalar(2)*(e1_inv(i|j)*e2_inv(k|l)).bundle(i|k,p).bundle(j|l,q))[m];
        }
        else
        {
            return ((e1_inv(i|j)*e2_inv(k|l)).bundle(i|k,p).bundle(j|l,q))[m];
        }
    }

    // NOTE: these may be unnecessary/undesired, because this type does not represent a vector space
//     using Parent_Tensor_i::component_is_immutable_zero;
//     using Parent_Tensor_i::scalar_factor_for_component;
//     using Parent_Tensor_i::vector_index_of;
    static std::string type_as_string ()
    {
        return "EuclideanEmbeddingInverse_t<" + TypeStringOf_t<Tensor2Symmetric>::eval() + '>';
    }
};

} // end of namespace Tenh

#ifdef __clang_version__
#pragma GCC diagnostic pop
#endif // __clang_version__

#endif // TENH_TENSOR2SYMMETRIC_HPP_
