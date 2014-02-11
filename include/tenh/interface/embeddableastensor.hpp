// ///////////////////////////////////////////////////////////////////////////
// tenh/interface/embeddableastensor.hpp by Victor Dods, created 2013/08/13
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_INTERFACE_EMBEDDABLEASTENSOR_HPP_
#define TENH_INTERFACE_EMBEDDABLEASTENSOR_HPP_

#include "tenh/core.hpp"

#include <ostream>

#include "tenh/componentindex.hpp"
#include "tenh/conceptual/embeddableintensorproduct.hpp"
#include "tenh/conceptual/tensorproduct.hpp"
#include "tenh/expression_templates.hpp"
#include "tenh/interface/vector.hpp"
#include "tenh/meta/typle_utility.hpp"

namespace Tenh {

template <typename FactorTyple_>
struct FactorComponentIndexTyple_f
{
    typedef typename Hippo::HeadBodyTyple_f<ComponentIndex_t<DimensionOf_f<typename Hippo::Head_f<FactorTyple_>::T>::V>,
                                            typename FactorComponentIndexTyple_f<typename Hippo::BodyTyple_f<FactorTyple_>::T>::T>::T T;
};

template <typename HeadType_>
struct FactorComponentIndexTyple_f<Typle_t<HeadType_>>
{
    typedef Typle_t<ComponentIndex_t<DimensionOf_f<HeadType_>::V>> T;
};

template <>
struct FactorComponentIndexTyple_f<Typle_t<>>
{
    typedef Typle_t<> T;
};

// ///////////////////////////////////////////////////////////////////////////
// metafunctions and functions for using an EmbeddableAsTensor_i as
// a multilinear form.
// ///////////////////////////////////////////////////////////////////////////

template <typename ParameterTyple_, typename AbstractIndexTyple_>
struct IndexedParameterListReturnType_f
{
private:
    typedef typename Hippo::Head_f<ParameterTyple_>::T HeadParameter;
    typedef typename Hippo::BodyTyple_f<ParameterTyple_>::T BodyParameterTyple;
    typedef typename Hippo::Head_f<AbstractIndexTyple_>::T HeadAbstractIndex;
    typedef typename Hippo::BodyTyple_f<AbstractIndexTyple_>::T BodyAbstractIndexTyple;
    static AbstractIndexSymbol const HEAD_SYMBOL = SymbolOf_f<HeadAbstractIndex>::V;
    typedef typename HeadParameter::template IndexedExpressionConstType_f<HEAD_SYMBOL>::T LeftOperand;
    typedef typename IndexedParameterListReturnType_f<BodyParameterTyple,BodyAbstractIndexTyple>::T RightOperand;
public:
    typedef ExpressionTemplate_Multiplication_t<LeftOperand,RightOperand> T;
};

template <typename HeadParameter, typename HeadAbstractIndex>
struct IndexedParameterListReturnType_f<Typle_t<HeadParameter>,Typle_t<HeadAbstractIndex>>
{
private:
    static AbstractIndexSymbol const HEAD_SYMBOL = SymbolOf_f<HeadAbstractIndex>::V;
public:
    typedef typename HeadParameter::template IndexedExpressionConstType_f<HEAD_SYMBOL>::T T;
};

template <typename ParameterTyple_, typename AbstractIndexTyple_>
typename IndexedParameterListReturnType_f<ParameterTyple_,AbstractIndexTyple_>::T
    indexed_parameter_list (List_t<ParameterTyple_> const &p, AbstractIndexTyple_ const &)
{
    typedef typename Hippo::Head_f<AbstractIndexTyple_>::T HeadAbstractIndex;
    typedef typename Hippo::BodyTyple_f<AbstractIndexTyple_>::T BodyAbstractIndexTyple;
    return p.head()(HeadAbstractIndex())*indexed_parameter_list(p.body(), BodyAbstractIndexTyple());
}

template <typename HeadParameter, typename HeadAbstractIndex>
typename IndexedParameterListReturnType_f<Typle_t<HeadParameter>,Typle_t<HeadAbstractIndex>>::T
    indexed_parameter_list (List_t<Typle_t<HeadParameter>> const &p, Typle_t<HeadAbstractIndex> const &)
{
    return p.head()(HeadAbstractIndex());
}

// ///////////////////////////////////////////////////////////////////////////
// EmbeddableAsTensor_i
// ///////////////////////////////////////////////////////////////////////////

// compile-time interface for a symmetric tensor type -- e.g. exterior/symmetric product.
// EmbeddableAsTensor_ should be a EmbeddableAsTensor_c type.
// TODO: technically, this should be LinearlyEmbeddableAsTensor_i
template <typename Derived_,
          typename Scalar_,
          typename EmbeddableInTensorProductOfBasedVectorSpaces_,
          ComponentQualifier COMPONENT_QUALIFIER_>
struct EmbeddableAsTensor_i
    :
    public Vector_i<Derived_,Scalar_,EmbeddableInTensorProductOfBasedVectorSpaces_,COMPONENT_QUALIFIER_>
{
    enum
    {
        STATIC_ASSERT_IN_ENUM((!TypesAreEqual_f<Derived_,NullType>::V), DERIVED_MUST_NOT_BE_NULL_TYPE),
        STATIC_ASSERT_IN_ENUM(IS_EMBEDDABLE_IN_TENSOR_PRODUCT_OF_BASED_VECTOR_SPACES_UNIQUELY(EmbeddableInTensorProductOfBasedVectorSpaces_),
                              MUST_BE_EMBEDDABLE_IN_TENSOR_PRODUCT_OF_BASED_VECTOR_SPACES),
    };

    typedef Vector_i<Derived_,Scalar_,EmbeddableInTensorProductOfBasedVectorSpaces_,COMPONENT_QUALIFIER_> Parent_Vector_i;
    typedef typename Parent_Vector_i::Derived Derived;
    typedef typename Parent_Vector_i::Scalar Scalar;
    typedef typename Parent_Vector_i::BasedVectorSpace BasedVectorSpace;
    using Parent_Vector_i::DIM; // does this make any sense?  e.g. simple tensors aren't a linear space
    typedef typename Parent_Vector_i::ComponentIndex ComponentIndex;
    using Parent_Vector_i::COMPONENT_QUALIFIER;
    typedef typename Parent_Vector_i::ComponentAccessConstReturnType ComponentAccessConstReturnType;
    typedef typename Parent_Vector_i::ComponentAccessNonConstReturnType ComponentAccessNonConstReturnType;
    typedef typename Parent_Vector_i::QualifiedComponent QualifiedComponent;

    typedef EmbeddableInTensorProductOfBasedVectorSpaces_ EmbeddableInTensorProductOfBasedVectorSpaces;
    typedef typename AS_EMBEDDABLE_IN_TENSOR_PRODUCT_OF_BASED_VECTOR_SPACES(EmbeddableInTensorProductOfBasedVectorSpaces)::TensorProductOfBasedVectorSpaces TensorProductOfBasedVectorSpaces;
    typedef typename FactorTypleOf_f<TensorProductOfBasedVectorSpaces>::T FactorTyple;
    typedef MultiIndex_t<typename FactorComponentIndexTyple_f<FactorTyple>::T> MultiIndex;
    // this is not the "fully expanded" order, but the number of [what you could think of
    // as "parenthesized"] factors that formed this tensor product type.
    static Uint32 const ORDER = OrderOf_f<TensorProductOfBasedVectorSpaces>::V;
    // static bool const IS_EMBEDDABLE_AS_TENSOR_I = true; // TODO: deprecate this in favor of IsEmbeddableAsTensor_i<...>

    static Uint32 order () { return ORDER; }

    using Parent_Vector_i::dim;
    using Parent_Vector_i::as_derived;
    using Parent_Vector_i::operator[];
    // you have to split before multi-index accessing

    using Parent_Vector_i::operator();

    // can't directly multi-index this object -- use split instead.
    // this provides the "split" operation without needing an intermediate temporary index,
    // since this object will be frequently split.
    // TODO: could the C++11 infer the return type?  this return type is annoying
    template <typename... AbstractIndexTypes_>
    typename Parent_Vector_i::template IndexedExpressionConstType_f<666>::T
                            ::template SplitReturnType_f<AbstractIndex_c<666>,
                                                         AbstractIndexTypes_...>::T
        split (Typle_t<AbstractIndexTypes_...> const &abstract_multiindex) const
    {
        typedef Typle_t<AbstractIndexTypes_...> AbstractIndexTyple;
        // make sure that the index type list actually contains AbstractIndex_c types
        STATIC_ASSERT((Hippo::EachTypeSatisfies_f<AbstractIndexTyple, IsAbstractIndex_e>::V), MUST_BE_TYPELIST_OF_ABSTRACT_INDEX_TYPES);
        AbstractIndex_c<666> dummy_index;
        return Parent_Vector_i::operator()(dummy_index).split(dummy_index, abstract_multiindex);
    }

    // this is for using this object as a multilinear (bilinear) form.
    // e.g. if X is this object, and u and v are vectors dual to the factor
    // types of TensorProductOfBasedVectorSpaces, then
    //   X(u, v)
    // is equivalent to
    //   X.split(i*j)*u(i)*v(j)
    // though is implemented using coembed, which should be close to optimally efficient.
    // this is a special case for when ORDER == 2, i.e. it is a bilinear form.
    template <typename Derived0_,
              typename BasedVectorSpace0_,
              ComponentQualifier COMPONENT_QUALIFIER0_,
              typename Derived1_,
              typename BasedVectorSpace1_,
              ComponentQualifier COMPONENT_QUALIFIER1_>
    Scalar_ operator () (Vector_i<Derived0_,Scalar_,BasedVectorSpace0_,COMPONENT_QUALIFIER0_> const &v0,
                         Vector_i<Derived1_,Scalar_,BasedVectorSpace1_,COMPONENT_QUALIFIER1_> const &v1) const
    {
        STATIC_ASSERT(ORDER == 2, ORDER_MUST_BE_EXACTLY_2);
        TensorProductOfBasedVectorSpaces upstairs;
        EmbeddableInTensorProductOfBasedVectorSpaces_ downstairs;
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'p'> p;
        AbstractIndex_c<'q'> q;
        return operator()(p) * (v0(i)*v1(j)).bundle(i*j,dual(upstairs),q).coembed(q,dual(downstairs),p);
    }
    // this is for using this object as a multilinear form.
    // e.g. if X is this object, and v_1, ..., v_k are vectors dual to the factor
    // types of TensorProductOfBasedVectorSpaces, then
    //   X(tuple(v_1, ..., v_k))
    // is equivalent to
    //   X.split(i_1*...*i_k)*v_1(i_1)*...*v_k(i_k)
    // though is implemented using coembed, which should be close to optimally efficient.
    template <typename ParameterTyple_>
    Scalar_ operator () (List_t<ParameterTyple_> const &l) const
    {
        STATIC_ASSERT(Hippo::Length_f<ParameterTyple_>::V == ORDER, ARGUMENT_LENGTH_MUST_EQUAL_ORDER);
        typedef typename AbstractIndexRangeTyple_f<Hippo::Length_f<ParameterTyple_>::V,667>::T AbstractIndexTyple;
        TensorProductOfBasedVectorSpaces upstairs;
        EmbeddableInTensorProductOfBasedVectorSpaces_ downstairs;
        AbstractIndex_c<'p'> p;
        AbstractIndex_c<'q'> q;
        AbstractIndexTyple a;
        return operator()(p) * indexed_parameter_list(l,a).bundle(a,dual(upstairs),q).coembed(q,dual(downstairs),p);
    }

    static bool component_is_procedural_zero (MultiIndex const &m) { return as_derived().component_is_procedural_zero(m); }
    static Scalar scalar_factor_for_component (MultiIndex const &m) { return as_derived().scalar_factor_for_component(m); }
    static ComponentIndex vector_index_of (MultiIndex const &m) { return as_derived().vector_index_of(m); }

    static std::string type_as_string ()
    {
        return "EmbeddableAsTensor_i<" + type_string_of<Derived>() + ','
                                       + type_string_of<Scalar>() + ','
                                       + type_string_of<EmbeddableInTensorProductOfBasedVectorSpaces>() + ','
                                       + component_qualifier_as_string(COMPONENT_QUALIFIER_) + '>';
    }
};

template <typename Derived_, typename Scalar_, typename EmbeddableInTensorProductOfBasedVectorSpaces_, ComponentQualifier COMPONENT_QUALIFIER_>
std::ostream &operator << (std::ostream &out, EmbeddableAsTensor_i<Derived_,Scalar_,EmbeddableInTensorProductOfBasedVectorSpaces_,COMPONENT_QUALIFIER_> const &e)
{
    return out << *static_cast<Vector_i<Derived_,Scalar_,EmbeddableInTensorProductOfBasedVectorSpaces_,COMPONENT_QUALIFIER_> const *>(&e);
}

} // end of namespace Tenh

#endif // TENH_INTERFACE_EMBEDDABLEASTENSOR_HPP_
