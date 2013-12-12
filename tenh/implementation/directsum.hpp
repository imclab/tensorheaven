// ///////////////////////////////////////////////////////////////////////////
// tenh/implementation/directsum.hpp by Ted Nitz, created 2013/10/24
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_IMPLEMENTATION_DIRECT_SUM_HPP_
#define TENH_IMPLEMENTATION_DIRECT_SUM_HPP_

#include "tenh/core.hpp"

#include "tenh/conceptual/directsum.hpp"
#include "tenh/implementation/implementationof.hpp"
#include "tenh/interface/vector.hpp"
#include "tenh/meta/tuple.hpp"

namespace Tenh {

template <typename SummandTypeList_, Uint32 N>
struct OffsetForComponent_f
{
private:
    enum { STATIC_ASSERT_IN_ENUM((SummandTypeList_::LENGTH > N), ATTEMPTED_ACCESS_PAST_LIST_END) };
public:
    static const Uint32 V = DimensionOf_f<typename SummandTypeList_::HeadType>::V + OffsetForComponent_f<typename SummandTypeList_::BodyTypeList, N-1>::V;
};

template <typename SummandTypeList_>
struct OffsetForComponent_f<SummandTypeList_,0>
{
    static const Uint32 V = 0;
};

inline Uint32 component_for_offset (EmptyTypeList const &, Uint32 offset)
{
    assert(false && "this should never actually be called");
    return 0;
}

template <typename SummandTypeList_>
Uint32 component_for_offset (SummandTypeList_ const &, Uint32 offset)
{
    STATIC_ASSERT((Length_f<SummandTypeList_>::V > 0), LENGTH_MUST_BE_POSITIVE);
    typedef typename Head_f<SummandTypeList_>::T HeadSummand;
    typedef typename Body_f<SummandTypeList_>::T BodySummand;
    if (offset < DimensionOf_f<HeadSummand>::V)
        return 0;
    else
        return component_for_offset(BodySummand(), offset - DimensionOf_f<HeadSummand>::V);
}

template <typename SummandTypeList_, typename Scalar_, typename UseArrayType_, typename Derived_>
struct ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTypeList_>,Scalar_,UseArrayType_,Derived_>
    :
    public ImplementationOf_t<typename UniqueBasedVectorSpaceStructureOf_f<DirectSumOfBasedVectorSpaces_c<SummandTypeList_> >::T,Scalar_,UseArrayType_, typename DerivedType_f<Derived_, ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTypeList_>,Scalar_,UseArrayType_,Derived_> >::T>
{
    typedef ImplementationOf_t<typename UniqueBasedVectorSpaceStructureOf_f<DirectSumOfBasedVectorSpaces_c<SummandTypeList_> >::T,Scalar_,UseArrayType_, typename DerivedType_f<Derived_, ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTypeList_>,Scalar_,UseArrayType_,Derived_> >::T> Parent_Implementation;
    typedef typename Parent_Implementation::Parent_Vector_i Parent_Vector_i;
    typedef typename Parent_Implementation::Parent_Array_i Parent_Array_i;

    typedef DirectSumOfBasedVectorSpaces_c<SummandTypeList_> Concept;
    typedef UseArrayType_ UseArrayType;
    typedef typename Parent_Implementation::Derived Derived;
    typedef typename Parent_Implementation::Scalar Scalar;
    typedef typename Parent_Implementation::BasedVectorSpace BasedVectorSpace;
    using Parent_Implementation::DIM;
    typedef typename Parent_Implementation::ComponentIndex ComponentIndex;
    typedef typename Parent_Implementation::MultiIndex MultiIndex;

    using Parent_Array_i::COMPONENTS_ARE_IMMUTABLE;
    typedef typename Parent_Implementation::ComponentAccessConstReturnType ComponentAccessConstReturnType;
    typedef typename Parent_Implementation::ComponentAccessNonConstReturnType ComponentAccessNonConstReturnType;

    typedef typename DualOf_f<ImplementationOf_t>::T Dual; // relies on the template specialization below

    typedef ImplementationOf_t<Concept,
                               Scalar_,
                               UseImmutableArray_t<typename ComponentGenerator_Constant_f<Scalar_,DIM,0>::T>,
                               Derived_> Zero;
    static Zero const ZERO;

    template <Uint32 INDEX_>
    struct BasisVector_f
    {
    private:
        enum { STATIC_ASSERT_IN_ENUM((INDEX_ < DIM), INDEX_OUT_OF_RANGE) };
        BasisVector_f () { }
    public:
        typedef ImplementationOf_t<Concept,
                                   Scalar_,
                                   UseImmutableArray_t<typename ComponentGenerator_Characteristic_f<Scalar_,DIM,INDEX_>::T>,
                                   Derived_> T;
        static T const V;
    };

    explicit ImplementationOf_t (WithoutInitialization const &w) : Parent_Implementation(w) { }

    // only use these if UseMemberArray is specified

    // probably only useful for zero element (because this is basis-dependent)
    ImplementationOf_t (FillWith_t<Scalar> const &fill_with)
        :
        Parent_Implementation(fill_with)
    {
        STATIC_ASSERT_TYPES_ARE_EQUAL(UseArrayType_,UseMemberArray);
    }
    // this is the tuple-based constructor
    template <typename HeadType_, typename BodyTypeList_>
    ImplementationOf_t (List_t<TypeList_t<HeadType_,BodyTypeList_> > const &x)
        :
        Parent_Implementation(x.as_member_array())
    {
        STATIC_ASSERT_TYPES_ARE_EQUAL(UseArrayType_,UseMemberArray);
    }

    // only use these if UsePreallocatedArray is specified

    explicit ImplementationOf_t (Scalar *pointer_to_allocation, bool check_pointer = CHECK_POINTER)
        :
        Parent_Implementation(pointer_to_allocation, check_pointer)
    {
        STATIC_ASSERT_TYPES_ARE_EQUAL(UseArrayType_,UsePreallocatedArray);
    }
    ImplementationOf_t (FillWith_t<Scalar> const &fill_with,
                        Scalar *pointer_to_allocation, bool check_pointer = CHECK_POINTER)
        :
        Parent_Implementation(fill_with, pointer_to_allocation, check_pointer)
    {
        STATIC_ASSERT_TYPES_ARE_EQUAL(UseArrayType_,UsePreallocatedArray);
    }
    // this is the tuple-based constructor
    template <typename HeadType_, typename BodyTypeList_>
    ImplementationOf_t (List_t<TypeList_t<HeadType_,BodyTypeList_> > const &x,
                        Scalar *pointer_to_allocation, bool check_pointer = CHECK_POINTER)
        :
        Parent_Implementation(x, pointer_to_allocation, check_pointer)
    {
        STATIC_ASSERT_TYPES_ARE_EQUAL(UseArrayType_,UsePreallocatedArray);
    }

    // only use this if UseImmutableArray_t<...> is specified or if the vector space is 0-dimensional
    ImplementationOf_t ()
        :
        Parent_Implementation(WithoutInitialization()) // sort of meaningless constructor
    {
        STATIC_ASSERT(IsUseImmutableArray_f<UseArrayType_>::V || DIM == 0, MUST_BE_USE_IMMUTABLE_ARRAY_OR_BE_ZERO_DIMENSIONAL);
    }

    using Parent_Implementation::as_derived;
    using Parent_Implementation::operator[];
    using Parent_Implementation::allocation_size_in_bytes;
    using Parent_Implementation::pointer_to_allocation;

    template <Uint32 N>
    struct ElementType_f
    {
    private:
        enum { STATIC_ASSERT_IN_ENUM((SummandTypeList_::LENGTH > N), ATTEMPTED_ACCESS_PAST_LIST_END) };
    public:
        typedef ImplementationOf_t<typename Element_f<SummandTypeList_, N>::T,
                                   Scalar_,
                                   typename If_f<IsUseImmutableArray_f<UseArrayType_>::V,
                                                 UseArrayType_,
                                                 UsePreallocatedArray>::T >
                T;
    };

    template <Uint32 N>
    typename ElementType_f<N>::T el ()
    {
        STATIC_ASSERT((SummandTypeList_::LENGTH > N), ATTEMPTED_ACCESS_PAST_LIST_END);
        return typename ElementType_f<N>::T(pointer_to_allocation() + OffsetForComponent_f<SummandTypeList_,N>::V);
    }

    template <Uint32 N>
    typename ElementType_f<N>::T const el () const
    {
        STATIC_ASSERT((SummandTypeList_::LENGTH > N), ATTEMPTED_ACCESS_PAST_LIST_END);
        return typename ElementType_f<N>::T(pointer_to_allocation() + OffsetForComponent_f<SummandTypeList_,N>::V);
    }

    typename ElementType_f<0>::T el (Uint32 n)
    {
        STATIC_ASSERT(TypeListIsUniform_t<SummandTypeList_>::V, TYPELIST_MUST_BE_UNIFORM);
        return typename ElementType_f<0>::T(pointer_to_allocation() + DimensionOf_f<typename SummandTypeList_::HeadType>::V * n);
    }

    typename ElementType_f<0>::T const el (Uint32 n) const
    {
        STATIC_ASSERT(TypeListIsUniform_t<SummandTypeList_>::V, TYPELIST_MUST_BE_UNIFORM);
        return typename ElementType_f<0>::T(pointer_to_allocation() + DimensionOf_f<typename SummandTypeList_::HeadType>::V * n);
    }

    // These versions of el<...> are intended to allow use like el<n>(i) rather than the more clunky el<n>()(i) to get an indexed expression.
    // Unfortunately the indexed expression type stores a reference to the ImplementationOf_t internally, and in this code that object is
    // a temporary, and so the behavior is undefined.
    // template <Uint32 N, AbstractIndexSymbol SYMBOL_>
    // typename ElementType_f<N>::T::template IndexedExpressionNonConstType_f<SYMBOL_>::T el(AbstractIndex_c<SYMBOL_> const & i)
    // {
    //     STATIC_ASSERT((SummandTypeList_::LENGTH > N), ATTEMPTED_ACCESS_PAST_LIST_END);
    //     return typename ElementType_f<N>::T(pointer_to_allocation() + OffsetForComponent_f<SummandTypeList_,N>::V)(i);
    // }
    //
    // template <Uint32 N, AbstractIndexSymbol SYMBOL_>
    // typename ElementType_f<N>::T::template IndexedExpressionConstType_f<SYMBOL_>::T el(AbstractIndex_c<SYMBOL_> const & i) const
    // {
    //     STATIC_ASSERT((SummandTypeList_::LENGTH > N), ATTEMPTED_ACCESS_PAST_LIST_END);
    //     return typename ElementType_f<N>::T(pointer_to_allocation() + OffsetForComponent_f<SummandTypeList_,N>::V)(i);
    // }

    static std::string type_as_string ()
    {
        return "ImplementationOf_t<" + type_string_of<DirectSumOfBasedVectorSpaces_c<SummandTypeList_> >() + ','
                                     + type_string_of<Scalar>() + ','
                                     + type_string_of<UseArrayType_>() + '>';
    }
};

template <typename SummandTypeList_, typename Scalar_, typename UseArrayType_, typename Derived_>
typename ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTypeList_>,Scalar_,UseArrayType_,Derived_>::Zero const ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTypeList_>,Scalar_,UseArrayType_,Derived_>::ZERO;

template <typename SummandTypeList_, typename Scalar_, typename UseArrayType_, typename Derived_>
template <Uint32 INDEX_>
typename ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTypeList_>,Scalar_,UseArrayType_,Derived_>::template BasisVector_f<INDEX_>::T const ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTypeList_>,Scalar_,UseArrayType_,Derived_>::BasisVector_f<INDEX_>::V;

template <typename SummandTypeList_, typename Scalar_, typename UseArrayType_, typename Derived_>
struct DualOf_f<ImplementationOf_t<DirectSumOfBasedVectorSpaces_c<SummandTypeList_>,Scalar_,UseArrayType_,Derived_> >
{
    typedef ImplementationOf_t<typename DualOf_f<DirectSumOfBasedVectorSpaces_c<SummandTypeList_> >::T,Scalar_,typename DualOf_f<UseArrayType_>::T, typename DualOf_f<Derived_>::T> T;
};

// ///////////////////////////////////////////////////////////////////////////
// direct sum of immutable 2-tensors (essentially gives a block-diag matrix)
// ///////////////////////////////////////////////////////////////////////////

template <typename Immutable2TensorImplementationTypeList_>
struct ConceptualTypeOfDirectSumOfImmutable2Tensors_f
{
private:
    typedef typename ConceptOfEachTypeIn_f<Immutable2TensorImplementationTypeList_>::T ConceptTypeList;
    enum
    {
        STATIC_ASSERT_IN_ENUM((EachTypeSatisfies_f<ConceptTypeList,IsTensorProductOfBasedVectorSpaces_p>::V), MUST_BE_TYPELIST_OF_DIAGONAL_2_TENSORS),
        STATIC_ASSERT_IN_ENUM(EachTypeIsA2TensorProductOfBasedVectorSpaces_f<ConceptTypeList>::V, MUST_BE_TYPELIST_OF_2_TENSORS)
    };
    typedef typename FactorNOfEachTypeIn_f<0,ConceptTypeList>::T Factor0TypeList;
    typedef typename FactorNOfEachTypeIn_f<1,ConceptTypeList>::T Factor1TypeList;
    typedef DirectSumOfBasedVectorSpaces_c<Factor0TypeList> Factor0DirectSum;
    typedef DirectSumOfBasedVectorSpaces_c<Factor1TypeList> Factor1DirectSum;
    ConceptualTypeOfDirectSumOfImmutable2Tensors_f();
public:
    typedef TensorProductOfBasedVectorSpaces_c<TypeList_t<Factor0DirectSum,TypeList_t<Factor1DirectSum> > > T;
};

namespace ComponentGeneratorEvaluator {

template <typename Immutable2TensorImplementationTypeList_,
          typename ConceptualTypeOfDirectSum_,
          typename Scalar_>
struct DirectSumOf2TensorsHelper_t
{
    typedef ComponentIndex_t<DimensionOf_f<ConceptualTypeOfDirectSum_>::V> ComponentIndex;
    typedef typename ConceptualTypeOfDirectSumOfImmutable2Tensors_f<Immutable2TensorImplementationTypeList_>::T ConceptualTypeOfDirectSum;
    enum { STATIC_ASSERT_IN_ENUM((TypesAreEqual_f<ConceptualTypeOfDirectSum_,ConceptualTypeOfDirectSum>::V), TYPES_MUST_BE_EQUAL) };
    typedef typename FactorTypeListOf_f<ConceptualTypeOfDirectSum_>::T FactorTypeList;
    typedef typename FactorTypeList::HeadType Factor0;
    typedef typename FactorTypeList::BodyTypeList::HeadType Factor1;
    typedef typename ImplementationOf_t<ConceptualTypeOfDirectSum_,Scalar_,UseMemberArray>::MultiIndex MultiIndex;
    enum { STATIC_ASSERT_IN_ENUM((MultiIndex::LENGTH == 2), LENGTH_MUST_BE_EXACTLY_2) };
    typedef typename Head_f<Immutable2TensorImplementationTypeList_>::T HeadImplementation;
    typedef typename FactorTypeListOf_f<typename HeadImplementation::Concept>::T HeadFactorTypeList;
    typedef typename Element_f<HeadFactorTypeList,0>::T HeadFactor0;
    typedef typename Element_f<HeadFactorTypeList,1>::T HeadFactor1;

    static Scalar_ evaluate (ComponentIndex_t<DimensionOf_f<ConceptualTypeOfDirectSum_>::V> const &i)
    {
        // this breaks up the component index into the corresponding multiindex.
        MultiIndex m(i);
        bool first_block_for_row = m.template el<0>().value() < DimensionOf_f<HeadFactor0>::V;
        bool first_block_for_col = m.template el<1>().value() < DimensionOf_f<HeadFactor1>::V;

        if (first_block_for_row != first_block_for_col) // off block-diagonal
        {
            return Scalar_(0);
        }
        else if (first_block_for_row && first_block_for_col) // on block-diagonal, upper-left block
        {
            HeadImplementation h;
            typedef typename HeadImplementation::MultiIndex M;
            M head_m(m.template el<0>().value(), m.template el<1>().value(), DONT_CHECK_RANGE);
            return h[head_m];
        }
        else // body block
        {
            typedef typename Body_f<Immutable2TensorImplementationTypeList_>::T Immutable2TensorImplementationBodyTypeList;
            typedef typename ConceptualTypeOfDirectSumOfImmutable2Tensors_f<Immutable2TensorImplementationBodyTypeList>::T ConceptualTypeOfDirectSumBody;
            typedef DirectSumOf2TensorsHelper_t<Immutable2TensorImplementationBodyTypeList,
                                                ConceptualTypeOfDirectSumBody,
                                                Scalar_> DirectSumOf2TensorsHelper;
            typedef typename DirectSumOf2TensorsHelper::MultiIndex BodyMultiIndex;
            BodyMultiIndex body_m(m.template el<0>().value() - DimensionOf_f<HeadFactor0>::V,
                                  m.template el<1>().value() - DimensionOf_f<HeadFactor1>::V,
                                  DONT_CHECK_RANGE);
            return DirectSumOf2TensorsHelper::evaluate(body_m.as_component_index());
        }
    }
};

template <typename HeadImmutable2TensorImplementation_,
          typename ConceptualTypeOfDirectSum_,
          typename Scalar_>
struct DirectSumOf2TensorsHelper_t<TypeList_t<HeadImmutable2TensorImplementation_>,ConceptualTypeOfDirectSum_,Scalar_>
{
    typedef typename HeadImmutable2TensorImplementation_::MultiIndex MultiIndex;

    static Scalar_ evaluate (ComponentIndex_t<DimensionOf_f<ConceptualTypeOfDirectSum_>::V> const &i)
    {
        HeadImmutable2TensorImplementation_ h;
        return h[i];
    }
};

template <typename Immutable2TensorImplementationTypeList_,
          typename ConceptualTypeOfDirectSum_,
          typename Scalar_>
Scalar_ direct_sum_of_2tensors (ComponentIndex_t<DimensionOf_f<ConceptualTypeOfDirectSum_>::V> const &i)
{
    return DirectSumOf2TensorsHelper_t<Immutable2TensorImplementationTypeList_,
                                       ConceptualTypeOfDirectSum_,
                                       Scalar_>::evaluate(i);
}

} // end of namespace ComponentGeneratorEvaluator

template <typename Immutable2TensorImplementationTypeList_>
struct DirectSumOfImmutable2Tensors_f
{
private:
    typedef typename ConceptOfEachTypeIn_f<Immutable2TensorImplementationTypeList_>::T ConceptTypeList;
    typedef typename ScalarOfEachTypeIn_f<Immutable2TensorImplementationTypeList_>::T ScalarTypeList;
    typedef typename ConceptualTypeOfDirectSumOfImmutable2Tensors_f<Immutable2TensorImplementationTypeList_>::T ConceptualTypeOfDirectSum;

    enum
    {
        STATIC_ASSERT_IN_ENUM(TypeListIsUniform_t<ScalarTypeList>::V, ALL_FACTOR_TYPE_SCALARS_ARE_EQUAL),
        STATIC_ASSERT_IN_ENUM((EachTypeUsesImmutableArray_f<Immutable2TensorImplementationTypeList_>::V), MUST_BE_TYPELIST_OF_IMMUTABLE_IMPLEMENTATIONS)
    };

    typedef typename ScalarTypeList::HeadType Scalar;
    typedef ComponentGenerator_t<Scalar,
                                 DimensionOf_f<ConceptualTypeOfDirectSum>::V,
                                 ComponentGeneratorEvaluator::direct_sum_of_2tensors<Immutable2TensorImplementationTypeList_,
                                                                                     ConceptualTypeOfDirectSum,
                                                                                     Scalar>,
                                 DirectSum_c<Immutable2TensorImplementationTypeList_> > ComponentGenerator;
private:
    DirectSumOfImmutable2Tensors_f();
public:
    typedef ImplementationOf_t<ConceptualTypeOfDirectSum,Scalar,UseImmutableArray_t<ComponentGenerator> > T;
};

} // end of namespace Tenh

#endif // TENH_IMPLEMENTATION_DIRECT_SUM_HPP_
