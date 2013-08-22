// ///////////////////////////////////////////////////////////////////////////
// tenh/expression_templates_utility.hpp by Victor Dods, created 2013/03/20
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_EXPRESSION_TEMPLATES_UTILITY_HPP_
#define TENH_EXPRESSION_TEMPLATES_UTILITY_HPP_

#include "tenh/core.hpp"

#include "tenh/componentindex.hpp"
#include "tenh/conceptual/abstractindex.hpp"
#include "tenh/dimindex.hpp"
// #include "tenh/innerproduct.hpp"
#include "tenh/meta/typelist.hpp"
#include "tenh/multiindex.hpp"

namespace Tenh {

// this file contains template metaprograms which assist in the construction and evaluation of expression templates

template <typename Scalar, typename Space> struct ImplementationOf_t;

// for this to work correctly on DimIndex_t types, the dimensions must be correct (i.e. the
// primal/dual vector space checking must already be done).  TODO: redesign-away this caveat
template <typename IndexTypeList>
struct SummedIndexTypeList_t
{
    typedef typename ElementsHavingMultiplicity_t<IndexTypeList,2>::T T;
};

// for this to work correctly on DimIndex_t types, the dimensions must be correct (i.e. the
// primal/dual vector space checking must already be done).  TODO: redesign-away this caveat
template <typename IndexTypeList>
struct FreeIndexTypeList_t
{
    typedef typename ElementsHavingMultiplicity_t<IndexTypeList,1>::T T;
};

template <typename FreeIndexTypeList>
struct IndexIsFree_t
{
    enum { STATIC_ASSERT_IN_ENUM(IsATypeList_t<FreeIndexTypeList>::V, MUST_BE_TYPELIST) };

    template <typename T>
    struct Eval_t
    {
        enum
        {
            STATIC_ASSERT_IN_ENUM(IsATypeList_t<T>::V, MUST_BE_TYPELIST),
            STATIC_ASSERT_IN_ENUM(T::LENGTH == 2, LENGTH_MUST_BE_EXACTLY_2)
        };
        typedef typename T::HeadType Factor;
        typedef typename T::BodyTypeList::HeadType Index;
        static bool const V = FreeIndexTypeList::template Contains_t<Index>::V;
    };
};

template <typename Unzipped>
struct FreeFactorTypeListHelper_t
{
    typedef typename Unzipped::HeadType T;
};

template <>
struct FreeFactorTypeListHelper_t<EmptyTypeList>
{
    typedef EmptyTypeList T;
};

// for this to work correctly on DimIndex_t types, the dimensions must be correct (i.e. the
// primal/dual vector space checking must already be done).  TODO: redesign-away this caveat
template <typename FactorTypeList, typename IndexTypeList>
struct FreeFactorTypeList_t
{
private:
    typedef typename Zip_t<TypeList_t<FactorTypeList,TypeList_t<IndexTypeList> > >::T FactorAndIndexPairTypeList;
    typedef typename FreeIndexTypeList_t<IndexTypeList>::T FreeIndexTypeList;
    typedef typename ElementsOfTypeListSatisfyingPredicate_t<FactorAndIndexPairTypeList,IndexIsFree_t<FreeIndexTypeList> >::T FreeFactorAndIndexPairTypeList;
    typedef typename Unzip_t<FreeFactorAndIndexPairTypeList>::T Unzipped;
    enum { STATIC_ASSERT_IN_ENUM((Unzipped::LENGTH == 0 || Unzipped::LENGTH == 2), LENGTH_MUST_BE_EXACTLY_0_OR_2) };
public:
    // each pair in FreeFactorAndIndexPairTypeList is a (factor, index) TypeList_t
    typedef typename FreeFactorTypeListHelper_t<Unzipped>::T T;
};

/*
template <typename HeadType>
typename HeadType::Owner::Scalar summation_component_factor (MultiIndex_t<TypeList_t<HeadType> > const &s)
{
    return InnerProduct_t<typename HeadType::Owner,typename HeadType::Owner::Basis>::component(s.head());
}

template <typename HeadType, typename BodyTypeList>
typename HeadType::Owner::Scalar summation_component_factor (MultiIndex_t<TypeList_t<HeadType,BodyTypeList> > const &s)
{
    return InnerProduct_t<typename HeadType::Owner,typename HeadType::Owner::Basis>::component(s.head()) * summation_component_factor(s.body());
}
*/

template <typename DimIndexTypeList>
struct AbstractIndicesOfDimIndexTypeList_t
{
    enum { STATIC_ASSERT_IN_ENUM(EachTypeIsADimIndex_f<DimIndexTypeList>::V, MUST_BE_TYPELIST_OF_DIM_INDEX_TYPES) };
    typedef TypeList_t<AbstractIndex_c<DimIndexTypeList::HeadType::SYMBOL>,
                       typename AbstractIndicesOfDimIndexTypeList_t<typename DimIndexTypeList::BodyTypeList>::T> T;
};

template <>
struct AbstractIndicesOfDimIndexTypeList_t<EmptyTypeList>
{
    typedef EmptyTypeList T;
};

template <typename AbstractIndexTypeList, typename SummedAbstractIndexTypeList, typename AbstractIndex>
struct SummedAbstractIndexPairElementIndices_t
{
    enum
    {
        STATIC_ASSERT_IN_ENUM(IsAbstractIndex_f<AbstractIndex>::V, MUST_BE_ABSTRACT_INDEX),
        STATIC_ASSERT_IN_ENUM((Occurrence_t<AbstractIndexTypeList,AbstractIndex>::COUNT == 2), MUST_OCCUR_EXACTLY_TWICE)
    };
    static Uint32 const FIRST = FirstMatchingIn_t<AbstractIndexTypeList,AbstractIndex>::INDEX;
    static Uint32 const FIRST_MATCHING_IN_REST = FirstMatchingIn_t<typename AbstractIndexTypeList::template TrailingTypeList_t<FIRST+1>::T,AbstractIndex>::INDEX;
    static Uint32 const SECOND = FIRST + 1 + FIRST_MATCHING_IN_REST;
};

template <typename FirstFactor, typename SecondFactor>
struct AssertThatSummationIsNaturalPairing_t
{
    enum { STATIC_ASSERT_IN_ENUM((Lvd::Meta::TypesAreEqual<FirstFactor,typename SecondFactor::Dual>::v), SUMMATION_MUST_BE_NATURAL_PAIRING) };
    static bool const V = true;
};

template <typename FactorTypeList, typename AbstractIndexTypeList, typename SummedAbstractIndexTypeList, typename AbstractIndex>
struct FactorsOfSummation_t
{
    typedef SummedAbstractIndexPairElementIndices_t<AbstractIndexTypeList,SummedAbstractIndexTypeList,AbstractIndex> Pair;
    typedef typename FactorTypeList::template El_t<Pair::FIRST>::T FirstFactor;
    typedef typename FactorTypeList::template El_t<Pair::SECOND>::T SecondFactor;
    enum
    {
        STATIC_ASSERT_IN_ENUM((FactorTypeList::LENGTH == AbstractIndexTypeList::LENGTH), MUST_HAVE_EQUAL_LENGTHS),
        STATIC_ASSERT_IN_ENUM__UNIQUE(HasBasedVectorSpaceStructure_f<FirstFactor>::V, MUST_BE_BASED_VECTOR_SPACE, FIRSTFACTOR),
        STATIC_ASSERT_IN_ENUM__UNIQUE(HasBasedVectorSpaceStructure_f<SecondFactor>::V, MUST_BE_BASED_VECTOR_SPACE, SECONDFACTOR)
    };
};

template <typename FactorTypeList, typename AbstractIndexTypeList, typename SummedAbstractIndexTypeList>
struct AssertThatAllSummationsAreNaturalPairings_t
{
    typedef FactorsOfSummation_t<FactorTypeList,
                                 AbstractIndexTypeList,
                                 SummedAbstractIndexTypeList,
                                 typename SummedAbstractIndexTypeList::HeadType> FactorsOfSummation;
    typedef AssertThatSummationIsNaturalPairing_t<typename FactorsOfSummation::FirstFactor,
                                                  typename FactorsOfSummation::SecondFactor> A1;
    typedef AssertThatAllSummationsAreNaturalPairings_t<FactorTypeList,
                                                        AbstractIndexTypeList,
                                                        typename SummedAbstractIndexTypeList::BodyTypeList> A2;
    static bool const V = A1::V && A2::V;
};

template <typename FactorTypeList, typename AbstractIndexTypeList>
struct AssertThatAllSummationsAreNaturalPairings_t<FactorTypeList,AbstractIndexTypeList,EmptyTypeList>
{
    static bool const V = true; // so this can be used in STATIC_ASSERT_IN_ENUM
};

// TODO: think about how UnarySummation_t and BinarySummation_t could be combined (if it makes sense to do it)

// this is designed to handle trace-type expression templates, such as u(i,i) or v(i,j,i)
// technically SummedDimIndexTypeList is a redundant argument (as it is derivable from TensorDimIndexTypeList),
// but it is necessary so that a template specialization can be made for when it is EmptyTypeList.
template <typename Tensor, typename TensorDimIndexTypeList, typename SummedDimIndexTypeList>
struct UnarySummation_t
{
private:
    typedef typename AbstractIndicesOfDimIndexTypeList_t<TensorDimIndexTypeList>::T AbstractIndexTypeList;
    typedef typename AbstractIndicesOfDimIndexTypeList_t<SummedDimIndexTypeList>::T SummedAbstractIndexTypeList;
public:
    enum
    {
        STATIC_ASSERT_IN_ENUM((AssertThatAllSummationsAreNaturalPairings_t<typename Tensor::FactorTypeList,
                                                                           AbstractIndexTypeList,
                                                                           SummedAbstractIndexTypeList>::V), ALL_SUMMATIONS_MUST_BE_NATURAL_PAIRINGS)
    };

    typedef typename Tensor::Scalar Scalar;
    typedef typename FreeIndexTypeList_t<TensorDimIndexTypeList>::T FreeDimIndexTypeList;
    typedef MultiIndex_t<FreeDimIndexTypeList> MultiIndex;

    static Scalar eval (Tensor const &tensor, MultiIndex const &m)
    {
        typedef typename ConcatenationOfTypeLists_t<FreeDimIndexTypeList,SummedDimIndexTypeList>::T TotalDimIndexTypeList;
        typedef MultiIndex_t<TotalDimIndexTypeList> TotalMultiIndex;
        typedef MultiIndex_t<SummedDimIndexTypeList> SummedMultiIndex;

        // the operands take indices that are a subset of the summed indices and free indices.

        // constructing t with m initializes the first elements which correpond to
        // MultiIndex with the value of m, and initializes the remaining elements to zero.
        TotalMultiIndex t(m);
        Scalar retval(0);
        // get the map which produces the MultiIndex for each tensor from the TotalMultiIndex t
        typedef MultiIndexMap_t<TotalDimIndexTypeList,TensorDimIndexTypeList> TensorIndexMap;
        static typename TensorIndexMap::EvalMapType const tensor_index_map = TensorIndexMap::eval;
        // t = (f,s), which is a concatenation of the free access indices and the summed access indices.
        // s is a reference to the second part, which is what is iterated over in the summation.
        for (SummedMultiIndex &s = t.template trailing_list<FreeDimIndexTypeList::LENGTH>(); s.is_not_at_end(); ++s)
            // TODO: when the dual-vector-space/conceptual refactor is done, this summation_component_factor
            // should go away, since this is a non-natural pairing, and it causes C++ plumbing issues
            // (getting the C++ scalar type from the index, where the index will only be aware of the
            // abstract BasedVectorSpace).
            retval += tensor[tensor_index_map(t)];// * summation_component_factor(s);
        return retval;
    }
};

template <typename Tensor, typename TensorDimIndexTypeList>
struct UnarySummation_t<Tensor,TensorDimIndexTypeList,EmptyTypeList>
{
    // no summations to check the natural pairing for

    typedef typename Tensor::Scalar Scalar;
    typedef typename FreeIndexTypeList_t<TensorDimIndexTypeList>::T FreeDimIndexTypeList;
    typedef MultiIndex_t<FreeDimIndexTypeList> MultiIndex;

    static Scalar eval (Tensor const &tensor, MultiIndex const &m) { return tensor[m]; }
};

template <typename LeftOperand, typename RightOperand, typename FreeDimIndexTypeList, typename SummedDimIndexTypeList>
struct BinarySummation_t
{
private:
    typedef typename ConcatenationOfTypeLists_t<typename LeftOperand::FreeFactorTypeList,
                                                typename RightOperand::FreeFactorTypeList>::T FactorTypeList;
    typedef typename ConcatenationOfTypeLists_t<typename LeftOperand::FreeDimIndexTypeList,
                                                typename RightOperand::FreeDimIndexTypeList>::T DimIndexTypeList;
    typedef typename AbstractIndicesOfDimIndexTypeList_t<DimIndexTypeList>::T AbstractIndexTypeList;
    typedef typename AbstractIndicesOfDimIndexTypeList_t<SummedDimIndexTypeList>::T SummedAbstractIndexTypeList;
public:
    enum
    {
        STATIC_ASSERT_IN_ENUM(LeftOperand::IS_EXPRESSION_TEMPLATE_I, LEFT_OPERAND_IS_EXPRESSION_TEMPLATE),
        STATIC_ASSERT_IN_ENUM(RightOperand::IS_EXPRESSION_TEMPLATE_I, RIGHT_OPERAND_IS_EXPRESSION_TEMPLATE),
        STATIC_ASSERT_IN_ENUM((Lvd::Meta::TypesAreEqual<typename LeftOperand::Scalar,typename RightOperand::Scalar>::v), OPERAND_SCALAR_TYPES_ARE_EQUAL),
        STATIC_ASSERT_IN_ENUM((SummedDimIndexTypeList::LENGTH > 0), LENGTH_MUST_BE_POSITIVE),
        STATIC_ASSERT_IN_ENUM((AssertThatAllSummationsAreNaturalPairings_t<FactorTypeList,
                                                                           AbstractIndexTypeList,
                                                                           SummedAbstractIndexTypeList>::V), ALL_SUMMATIONS_MUST_BE_NATURAL_PAIRINGS)
    };

    typedef typename LeftOperand::Scalar Scalar;
    typedef MultiIndex_t<FreeDimIndexTypeList> MultiIndex;

    static Scalar eval (LeftOperand const &left_operand, RightOperand const &right_operand, MultiIndex const &m)
    {
        typedef typename ConcatenationOfTypeLists_t<FreeDimIndexTypeList,SummedDimIndexTypeList>::T TotalDimIndexTypeList;
        typedef MultiIndex_t<TotalDimIndexTypeList> TotalMultiIndex;
        typedef MultiIndex_t<SummedDimIndexTypeList> SummedMultiIndex;

        // the operands take indices that are a subset of the summed indices and free indices.

        // constructing t with m initializes the first elements which correpond to
        // MultiIndex with the value of m, and initializes the remaining elements to zero.
        TotalMultiIndex t(m);
        Scalar retval(0);
        // get the map which produces the MultiIndex for each operand from the TotalMultiIndex t
        typedef MultiIndexMap_t<TotalDimIndexTypeList,typename LeftOperand::FreeDimIndexTypeList> LeftOperandIndexMap;
        typedef MultiIndexMap_t<TotalDimIndexTypeList,typename RightOperand::FreeDimIndexTypeList> RightOperandIndexMap;
        static typename LeftOperandIndexMap::EvalMapType const left_operand_index_map = LeftOperandIndexMap::eval;
        static typename RightOperandIndexMap::EvalMapType const right_operand_index_map = RightOperandIndexMap::eval;
        // t = (f,s), which is a concatenation of the free access indices and the summed access indices.
        // s is a reference to the second part, which is what is iterated over in the summation.
        for (SummedMultiIndex &s = t.template trailing_list<FreeDimIndexTypeList::LENGTH>(); s.is_not_at_end(); ++s)
            retval += left_operand[left_operand_index_map(t)] *
                      right_operand[right_operand_index_map(t)];// *
                      //summation_component_factor(s);
        return retval;
    }
};

// template specialization handles summation over no indices
template <typename LeftOperand, typename RightOperand, typename FreeDimIndexTypeList>
struct BinarySummation_t<LeftOperand,RightOperand,FreeDimIndexTypeList,EmptyTypeList>
{
    enum
    {
        STATIC_ASSERT_IN_ENUM(LeftOperand::IS_EXPRESSION_TEMPLATE_I, LEFT_OPERAND_IS_EXPRESSION_TEMPLATE),
        STATIC_ASSERT_IN_ENUM(RightOperand::IS_EXPRESSION_TEMPLATE_I, RIGHT_OPERAND_IS_EXPRESSION_TEMPLATE),
        STATIC_ASSERT_IN_ENUM((Lvd::Meta::TypesAreEqual<typename LeftOperand::Scalar,typename RightOperand::Scalar>::v), OPERAND_SCALAR_TYPES_ARE_EQUAL),
        STATIC_ASSERT_IN_ENUM(EachTypeIsADimIndex_f<FreeDimIndexTypeList>::V, MUST_BE_TYPELIST_OF_DIM_INDEX_TYPES)
        // no summation, so no need to check naturality of pairings
    };

    typedef typename LeftOperand::Scalar Scalar;
    typedef MultiIndex_t<FreeDimIndexTypeList> MultiIndex;

    static Scalar eval (LeftOperand const &left_operand, RightOperand const &right_operand, MultiIndex const &m)
    {
        // get the map which produces the MultiIndex for each operand from the free indices m
        typedef MultiIndexMap_t<FreeDimIndexTypeList,typename LeftOperand::FreeDimIndexTypeList> LeftOperandIndexMap;
        typedef MultiIndexMap_t<FreeDimIndexTypeList,typename RightOperand::FreeDimIndexTypeList> RightOperandIndexMap;
        static typename LeftOperandIndexMap::EvalMapType const left_operand_index_map = LeftOperandIndexMap::eval;
        static typename RightOperandIndexMap::EvalMapType const right_operand_index_map = RightOperandIndexMap::eval;
        return left_operand[left_operand_index_map(m)] * right_operand[right_operand_index_map(m)];
    }
};


template <typename LeftOperand, typename RightOperand>
struct FreeDimIndexTypeListOfMultiplication_t
{
private:
    // the free indices are the single-occurrence indices of the concatenated
    // list of free indices from the left and right operands
    typedef typename ConcatenationOfTypeLists_t<typename LeftOperand::FreeDimIndexTypeList,
                                                typename RightOperand::FreeDimIndexTypeList>::T CombinedFreeDimIndexTypeList;
public:
    typedef typename ElementsHavingMultiplicity_t<CombinedFreeDimIndexTypeList,1>::T T;
};

template <typename LeftOperand, typename RightOperand>
struct SummedDimIndexTypeListOfMultiplication_t
{
private:
    // the free indices are the single-occurrence indices of the concatenated
    // list of free indices from the left and right operands
    typedef typename ConcatenationOfTypeLists_t<typename LeftOperand::FreeDimIndexTypeList,
                                                typename RightOperand::FreeDimIndexTypeList>::T CombinedFreeDimIndexTypeList;
public:
    // the summed indices (at this level) are the double-occurrences indices
    // of the concatenated list of free indices from the left and right operands
    typedef typename ElementsHavingMultiplicity_t<CombinedFreeDimIndexTypeList,2>::T T;
};

template <typename LeftOperand, typename RightOperand>
struct UsedDimIndexTypeListOfMultiplication_t
{
private:
    typedef typename SummedDimIndexTypeListOfMultiplication_t<LeftOperand,RightOperand>::T SummedDimIndexTypeList;
public:
    // typelist of used indices which are prohibited from using higher up in the AST
    typedef typename UniqueTypesIn_t<
        typename ConcatenationOfTypeLists_t<
            typename ConcatenationOfTypeLists_t<typename LeftOperand::UsedDimIndexTypeList,
                                                typename RightOperand::UsedDimIndexTypeList>::T,
            SummedDimIndexTypeList>::T>::T T;
};

template <typename LeftOperand, typename RightOperand>
struct FreeFactorTypeListOfMultiplication_t
{
private:
    typedef typename ConcatenationOfTypeLists_t<typename LeftOperand::FreeFactorTypeList,
                                                typename RightOperand::FreeFactorTypeList>::T CombinedFactorTypeList;
    typedef typename ConcatenationOfTypeLists_t<typename LeftOperand::FreeDimIndexTypeList,
                                                typename RightOperand::FreeDimIndexTypeList>::T CombinedDimIndexTypeList;
public:
    typedef typename FreeFactorTypeList_t<CombinedFactorTypeList,CombinedDimIndexTypeList>::T T;
};



template <typename AbstractIndexTypeList, typename FactorTypeList, typename ExtractionAbstractIndexTypeList>
struct ExtractFactorsForAbstractIndices_t
{
private:
    enum
    {
        STATIC_ASSERT_IN_ENUM((Occurrence_t<AbstractIndexTypeList,typename ExtractionAbstractIndexTypeList::HeadType>::COUNT == 1), MUST_OCCUR_EXACTLY_ONCE),
        STATIC_ASSERT_IN_ENUM((IsASubsetOf_t<ExtractionAbstractIndexTypeList,AbstractIndexTypeList>::V), MUST_BE_SUBSET_OF)
    };
    static Uint32 const INDEX = FirstMatchingIn_t<AbstractIndexTypeList,typename ExtractionAbstractIndexTypeList::HeadType>::INDEX;
public:
    typedef TypeList_t<typename FactorTypeList::template El_t<INDEX>::T,
                       typename ExtractFactorsForAbstractIndices_t<AbstractIndexTypeList,
                                                                   FactorTypeList,
                                                                   typename ExtractionAbstractIndexTypeList::BodyTypeList>::T> T;
};

template <typename AbstractIndexTypeList, typename FactorTypeList>
struct ExtractFactorsForAbstractIndices_t<AbstractIndexTypeList,FactorTypeList,EmptyTypeList>
{
    typedef EmptyTypeList T;
};

// TODO: Scalar will go away once the bundle map is moved into the conceptual layer.
template <typename Scalar, typename BundleDimIndexTypeList, typename ResultingFactorType, typename ResultingDimIndexType>
struct BundleIndexMap_t
{
    typedef MultiIndex_t<BundleDimIndexTypeList> (*T) (ResultingDimIndexType const &);
    static T const V;
};

template <typename Scalar, typename BundleDimIndexTypeList, typename ResultingFactorType, typename ResultingDimIndexType>
typename BundleIndexMap_t<Scalar,BundleDimIndexTypeList,ResultingFactorType,ResultingDimIndexType>::T const BundleIndexMap_t<Scalar,BundleDimIndexTypeList,ResultingFactorType,ResultingDimIndexType>::V =
    ImplementationOf_t<Scalar,ResultingFactorType>::template bundle_index_map<BundleDimIndexTypeList,ResultingDimIndexType>;

// not an expression template, but just something that handles the bundled indices
template <typename Operand, typename BundleAbstractIndexTypeList, typename ResultingFactorType, typename ResultingAbstractIndexType>
struct IndexBundle_t
{
    typedef typename AbstractIndicesOfDimIndexTypeList_t<typename Operand::FreeDimIndexTypeList>::T OperandFreeAbstractIndexTypeList;

    enum
    {
        STATIC_ASSERT_IN_ENUM(IsAbstractIndex_f<ResultingAbstractIndexType>::V, MUST_BE_ABSTRACT_INDEX),
        STATIC_ASSERT_IN_ENUM((IsASubsetOf_t<BundleAbstractIndexTypeList,OperandFreeAbstractIndexTypeList>::V), BUNDLE_INDICES_MUST_BE_FREE),
        STATIC_ASSERT_IN_ENUM((!BundleAbstractIndexTypeList::template Contains_t<ResultingAbstractIndexType>::V), BUNDLE_AND_RESULTING_MUST_BE_DISTINCT),
        STATIC_ASSERT_IN_ENUM(Operand::IS_EXPRESSION_TEMPLATE_I, OPERAND_IS_EXPRESSION_TEMPLATE)
        // TODO: check that the factor types specified by BundleAbstractIndexTypeList can actually be
        // bundled into ResultingFactorType
    };

    // if Operand's free indices are i|j|k|l|m|n, the bundle indices are i|k|l, and the resulting
    // index is P, then the transformation is
    //      i|j|k|l|m|n --> j|m|n|i|k|l --> j|m|n|P
    // the first transformation being putting the bundle indices at the back (preserving their order),
    // and the second one transforming them into the bundled index.  the component access goes backward
    // in this diagram (components are accessed via the indices j|m|n|P, the bundle map is applied to
    // P to get i|k|l, and then j|m|n|i|k|l is transformed to i|j|k|l|m|n to get the component from
    // Operand).

    typedef typename DimIndexTypeListOf_t<typename Operand::FreeFactorTypeList,
                                          OperandFreeAbstractIndexTypeList>::T OperandFreeDimIndexTypeList;
    typedef typename ExtractFactorsForAbstractIndices_t<OperandFreeAbstractIndexTypeList,
                                                        typename Operand::FreeFactorTypeList,
                                                        BundleAbstractIndexTypeList>::T BundleFactorTypeList;
    typedef typename DimIndexTypeListOf_t<BundleFactorTypeList,
                                          BundleAbstractIndexTypeList>::T BundleDimIndexTypeList;
    typedef DimIndex_t<ResultingAbstractIndexType::SYMBOL,
                       ResultingFactorType::DIM> ResultingDimIndexType;

    // zip the stuff so that the transformations can act on both the DimIndex_t and factor lists
    typedef typename Zip_t<TypeList_t<OperandFreeDimIndexTypeList,
                           TypeList_t<typename Operand::FreeFactorTypeList> > >::T OperandFreeDimIndexAndFactorTypeList;
    typedef TypeList_t<ResultingDimIndexType,TypeList_t<ResultingFactorType> > ResultingDimIndexAndFactorType;
    typedef typename Zip_t<TypeList_t<BundleDimIndexTypeList,
                           TypeList_t<BundleFactorTypeList> > >::T BundleDimIndexAndFactorTypeList;

    // ResultingDimIndexAndFactorType comes last.  DimIndexAndFactorTypeList may have summed indices
    typedef typename SetSubtraction_t<
        typename ConcatenationOfTypeLists_t<OperandFreeDimIndexAndFactorTypeList,
                                            TypeList_t<ResultingDimIndexAndFactorType> >::T,
        BundleDimIndexAndFactorTypeList>::T DimIndexAndFactorTypeList;
    // this seems like it should be the same as OperandFreeDimIndexAndFactorTypeList, but it has
    // the indices in BundleDimIndexAndFactorTypeList coming last.
    typedef typename ConcatenationOfTypeLists_t<
        typename SetSubtraction_t<OperandFreeDimIndexAndFactorTypeList,BundleDimIndexAndFactorTypeList>::T,
        BundleDimIndexAndFactorTypeList>::T UnpackedDimIndexAndFactorTypeList;

    // unzip the stuff
    typedef typename Unzip_t<DimIndexAndFactorTypeList>::T DimIndexAndFactorTypeList_Unzipped;
    typedef typename Unzip_t<UnpackedDimIndexAndFactorTypeList>::T UnpackedDimIndexAndFactorTypeList_Unzipped;
    typedef typename DimIndexAndFactorTypeList_Unzipped::BodyTypeList::HeadType FactorTypeList;
    typedef typename DimIndexAndFactorTypeList_Unzipped::HeadType DimIndexTypeList;
    typedef typename UnpackedDimIndexAndFactorTypeList_Unzipped::HeadType UnpackedDimIndexTypeList;

    // finally, define the multi-index and bundle index map.
    typedef MultiIndex_t<DimIndexTypeList> MultiIndex;
    typedef typename Operand::Scalar Scalar;
    typedef BundleIndexMap_t<Scalar,
                             BundleDimIndexTypeList,
                             ResultingFactorType,
                             ResultingDimIndexType> BundleIndexMap;

    IndexBundle_t (Operand const &operand) : m_operand(operand) { }

    Scalar operator [] (MultiIndex const &m) const
    {
        // replace the head of m with the separate indices that it bundles.
        // use MultiIndexMap_t to place the indices in the correct order.
        typedef MultiIndexMap_t<UnpackedDimIndexTypeList,typename Operand::FreeDimIndexTypeList> OperandIndexMap;
        static typename OperandIndexMap::EvalMapType const operand_index_map = OperandIndexMap::eval;
        static typename BundleIndexMap::T const bundle_index_map = BundleIndexMap::V;
        // |= is concatenation of MultiIndex_t instances
        return m_operand[operand_index_map(m.template leading_list<MultiIndex::LENGTH-1>() |=
                                           bundle_index_map(m.template el<MultiIndex::LENGTH-1>()))];
    }

    template <typename OtherTensor>
    bool uses_tensor (OtherTensor const &t) const { return m_operand.uses_tensor(t); }

private:

    Operand const &m_operand;
};

// not an expression template, but just something that handles the split indices
template <typename Operand, typename SourceAbstractIndexType, typename SplitAbstractIndexTypeList>
struct IndexSplitter_t
{
    typedef typename AbstractIndicesOfDimIndexTypeList_t<typename Operand::FreeDimIndexTypeList>::T OperandFreeAbstractIndexTypeList;

    enum
    {
        STATIC_ASSERT_IN_ENUM(IsAbstractIndex_f<SourceAbstractIndexType>::V, MUST_BE_ABSTRACT_INDEX),
        STATIC_ASSERT_IN_ENUM((OperandFreeAbstractIndexTypeList::template Contains_t<SourceAbstractIndexType>::V), SOURCE_INDEX_MUST_BE_FREE),
        STATIC_ASSERT_IN_ENUM((!HasNontrivialIntersectionAsSets_t<TypeList_t<SourceAbstractIndexType>,SplitAbstractIndexTypeList>::V), SOURCE_AND_SPLIT_MUST_BE_DISTINCT),
        STATIC_ASSERT_IN_ENUM(Operand::IS_EXPRESSION_TEMPLATE_I, OPERAND_IS_EXPRESSION_TEMPLATE)
    };

    typedef typename Operand::Scalar Scalar;
    // we must replace SourceAbstractIndexType with MultiIndex_t<SplitAbstractIndexTypeList> in the index type list
    static Uint32 const SOURCE_INDEX_TYPE_INDEX = FirstMatchingIn_t<OperandFreeAbstractIndexTypeList,SourceAbstractIndexType>::INDEX;
    typedef typename Operand::FreeFactorTypeList::template El_t<SOURCE_INDEX_TYPE_INDEX>::T SourceFactor;

    enum
    {
        // TODO: assert that SourceFactor can actually be split (can be embedded into a tensor space)
        STATIC_ASSERT_IN_ENUM__UNIQUE((SourceFactor::FactorTypeList::LENGTH == SplitAbstractIndexTypeList::LENGTH), MUST_HAVE_EQUAL_LENGTHS, FREEFACTORTYPELIST)
    };

    typedef typename ConcatenationOfTypeLists_t<
        typename Operand::FreeFactorTypeList::template LeadingTypeList_t<SOURCE_INDEX_TYPE_INDEX>::T,
        typename ConcatenationOfTypeLists_t<
            typename SourceFactor::FactorTypeList,
            typename Operand::FreeFactorTypeList::template TrailingTypeList_t<SOURCE_INDEX_TYPE_INDEX+1>::T
            >::T
        >::T FactorTypeList;
    typedef typename ConcatenationOfTypeLists_t<
        typename Operand::FreeDimIndexTypeList::template LeadingTypeList_t<SOURCE_INDEX_TYPE_INDEX>::T,
        typename ConcatenationOfTypeLists_t<
            SplitAbstractIndexTypeList,
            typename Operand::FreeDimIndexTypeList::template TrailingTypeList_t<SOURCE_INDEX_TYPE_INDEX+1>::T
            >::T
        >::T AbstractIndexTypeList;
    typedef typename DimIndexTypeListOf_t<FactorTypeList,AbstractIndexTypeList>::T DimIndexTypeList;
    typedef typename ConcatenationOfTypeLists_t<typename Operand::UsedDimIndexTypeList,SplitAbstractIndexTypeList>::T UsedDimIndexTypeList;
    typedef MultiIndex_t<DimIndexTypeList> MultiIndex;

    enum
    {
        STATIC_ASSERT_IN_ENUM__UNIQUE((FactorTypeList::LENGTH == DimIndexTypeList::LENGTH), MUST_HAVE_EQUAL_LENGTHS, FACTORTYPELIST)
    };

    IndexSplitter_t (Operand const &operand) : m_operand(operand) { }

    Scalar operator [] (MultiIndex const &m) const
    {
        typedef typename DimIndexTypeListOf_t<typename SourceFactor::FactorTypeList,
                                              SplitAbstractIndexTypeList>::T SourceFactorDimIndexTypeList;
        typedef ComponentIndex_t<SourceFactor::DIM> SourceFactorComponentIndex;
        typedef MultiIndex_t<SourceFactorDimIndexTypeList> SourceFactorMultiIndex;
        typedef ImplementationOf_t<Scalar,SourceFactor> ImplementationOfSourceFactor;

        SourceFactorMultiIndex s(m.template range<SOURCE_INDEX_TYPE_INDEX,SOURCE_INDEX_TYPE_INDEX+SplitAbstractIndexTypeList::LENGTH>());
        if (ImplementationOfSourceFactor::component_is_immutable_zero(s))
            return Scalar(0);

        SourceFactorComponentIndex i(ImplementationOfSourceFactor::vector_index_of(s));
        // this replaces the SplitAbstractIndexTypeList portion with SourceAbstractIndexType
        typename Operand::MultiIndex c_rebundled(m.template leading_list<SOURCE_INDEX_TYPE_INDEX>() |=
                                                 (i >>= m.template trailing_list<SOURCE_INDEX_TYPE_INDEX+SplitAbstractIndexTypeList::LENGTH>()));
        return ImplementationOfSourceFactor::scalar_factor_for_component(s) * m_operand[c_rebundled];
    }

    template <typename OtherTensor>
    bool uses_tensor (OtherTensor const &t) const { return m_operand.uses_tensor(t); }

private:

    Operand const &m_operand;
};

} // end of namespace Tenh

#endif // TENH_EXPRESSION_TEMPLATES_UTILITY_HPP_
