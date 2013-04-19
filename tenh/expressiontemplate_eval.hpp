// ///////////////////////////////////////////////////////////////////////////
// tenh/expressiontemplate_eval.hpp by Victor Dods, created 2013/04/09
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_EXPRESSIONTEMPLATE_EVAL_HPP_
#define TENH_EXPRESSIONTEMPLATE_EVAL_HPP_

#include "tenh/core.hpp"
#include "tenh/index.hpp"
#include "tenh/interface/expressiontemplate.hpp"
#include "tenh/multiindex.hpp"
#include "tenh/tensor2.hpp" // TEMP until general Tensor_t and eval are implemented

namespace Tenh {

// ////////////////////////////////////////////////////////////////////////////
// evaluating an indexed expression for the purpose of avoiding aliasing
// ////////////////////////////////////////////////////////////////////////////

// general declaration for when the general form is implemented
// TODO: once general form is implemented remove FreeIndexTypeList_ parameter,
// since it is really just Operand::FreeIndexTypeList.
template <typename Operand, typename FreeIndexTypeList_>
struct ExpressionTemplate_Eval_t;

// temporary dumbness (template specializations for 1- and 2-tensors), but this
// will go away once the general form is implemented.
template <typename Operand, typename Index1>
struct ExpressionTemplate_Eval_t<Operand,TypeList_t<Index1> >
    :
    public ExpressionTemplate_i<ExpressionTemplate_Eval_t<Operand,TypeList_t<Index1> >,
                                typename Operand::Scalar,
                                TypeList_t<Index1>,
                                EmptyTypeList>
{
    enum
    {
        STATIC_ASSERT_IN_ENUM(Operand::IS_EXPRESSION_TEMPLATE_I, OPERAND_IS_EXPRESSION_TEMPLATE),
        STATIC_ASSERT_IN_ENUM((AreEqualAsSets_t<typename Operand::FreeIndexTypeList,TypeList_t<Index1> >::V), OPERAND_HAS_THE_SAME_FREE_INDICES),
        STATIC_ASSERT_IN_ENUM((Operand::FreeIndexTypeList::LENGTH == TypeList_t<Index1>::LENGTH), FREE_INDICES_HAVE_NO_DUPLICATES)
    };

    typedef ExpressionTemplate_i<ExpressionTemplate_Eval_t<Operand,TypeList_t<Index1> >,
                                 typename Operand::Scalar,
                                 TypeList_t<Index1>,
                                 EmptyTypeList> Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeIndexTypeList FreeIndexTypeList;
    typedef typename Parent::UsedIndexTypeList UsedIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;
    using Parent::IS_EXPRESSION_TEMPLATE_I;

    ExpressionTemplate_Eval_t (Operand const &operand)
        :
        m_operand(operand),
        m_eval_is_cached(false),
        m_cached_tensor(Static<WithoutInitialization>::SINGLETON)
    { }

//     operator Scalar () const // TODO: only use this in the no-free-index one
//     {
//         STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeIndexTypeList);
//         return operator[](MultiIndex());
//     }

    // requires InnerProduct_t to be implemented for all free-indexed types
    // NOTE: will not currently work for complex types
    typename AssociatedFloatingPointType_t<Scalar>::T squared_norm () const
    {
        ensure_tensor_is_cached();
        return m_cached_tensor.squared_norm();
    }
    // requires InnerProduct_t to be implemented for all free-indexed types
    // NOTE: will not currently work for complex types
    typename AssociatedFloatingPointType_t<Scalar>::T norm () const
    {
        this->ensure_tensor_is_cached();
        return m_cached_tensor.norm();
    }

    Scalar const &operator [] (MultiIndex const &m) const
    {
        this->ensure_tensor_is_cached();
        return m_cached_tensor[m];
    }

    // this blocks aliasing, so it doesn't use the tensor in the sense of this method
    template <typename OtherTensor>
    bool uses_tensor (OtherTensor const &t) const { return false; }

private:

    void ensure_tensor_is_cached () const
    {
        if (!m_eval_is_cached)
        {
            m_cached_tensor(Index1()) = m_operand; // this should populate m_cached_tensor2 via expression templates
            m_eval_is_cached = true;
        }
    }

    typedef typename FreeIndexTypeList::HeadType::Owner::Derived Factor1;
    typedef Factor1 Tensor;

    Operand const &m_operand;
    mutable bool m_eval_is_cached;
    mutable Tensor m_cached_tensor;
};

template <typename Operand, typename Index1, typename Index2>
struct ExpressionTemplate_Eval_t<Operand,TypeList_t<Index1,TypeList_t<Index2> > >
    :
    public ExpressionTemplate_i<ExpressionTemplate_Eval_t<Operand,TypeList_t<Index1,TypeList_t<Index2> > >,
                                typename Operand::Scalar,
                                TypeList_t<Index1,TypeList_t<Index2> >,
                                EmptyTypeList>
{
    enum
    {
        STATIC_ASSERT_IN_ENUM(Operand::IS_EXPRESSION_TEMPLATE_I, OPERAND_IS_EXPRESSION_TEMPLATE),
        STATIC_ASSERT_IN_ENUM((AreEqualAsSets_t<typename Operand::FreeIndexTypeList,TypeList_t<Index1,TypeList_t<Index2> > >::V), OPERAND_HAS_THE_SAME_FREE_INDICES),
        STATIC_ASSERT_IN_ENUM((Operand::FreeIndexTypeList::LENGTH == TypeList_t<Index1,TypeList_t<Index2> >::LENGTH), FREE_INDICES_HAVE_NO_DUPLICATES)
    };

    typedef ExpressionTemplate_i<ExpressionTemplate_Eval_t<Operand,TypeList_t<Index1,TypeList_t<Index2> > >,
                                 typename Operand::Scalar,
                                 TypeList_t<Index1,TypeList_t<Index2> >,
                                 EmptyTypeList> Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeIndexTypeList FreeIndexTypeList;
    typedef typename Parent::UsedIndexTypeList UsedIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;
    using Parent::IS_EXPRESSION_TEMPLATE_I;

    ExpressionTemplate_Eval_t (Operand const &operand)
        :
        m_operand(operand),
        m_eval_is_cached(false),
        m_cached_tensor(Static<WithoutInitialization>::SINGLETON)
    { }

//     operator Scalar () const // TODO: only use this in the no-free-index one
//     {
//         STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeIndexTypeList);
//         return operator[](MultiIndex());
//     }

    // requires InnerProduct_t to be implemented for all free-indexed types
    // NOTE: will not currently work for complex types
    typename AssociatedFloatingPointType_t<Scalar>::T squared_norm () const
    {
        this->ensure_tensor_is_cached();
        return m_cached_tensor.squared_norm();
    }
    // requires InnerProduct_t to be implemented for all free-indexed types
    // NOTE: will not currently work for complex types
    typename AssociatedFloatingPointType_t<Scalar>::T norm () const
    {
        this->ensure_tensor_is_cached();
        return m_cached_tensor.norm();
    }

    Scalar const &operator [] (MultiIndex const &m) const
    {
        ensure_tensor_is_cached();
        return m_cached_tensor[m];
    }

    // this blocks aliasing, so it doesn't use the tensor in the sense of this method
    template <typename OtherTensor>
    bool uses_tensor (OtherTensor const &t) const { return false; }

private:

    void ensure_tensor_is_cached () const
    {
        if (!m_eval_is_cached)
        {
            m_cached_tensor(FreeIndexTypeList()) = m_operand; // this should populate m_cached_tensor2 via expression templates
            m_eval_is_cached = true;
        }
    }

    typedef typename FreeIndexTypeList::HeadType::Owner::Derived Factor1;
    typedef typename FreeIndexTypeList::BodyTypeList::HeadType::Owner::Derived Factor2;
    typedef Tensor2_t<Factor1,Factor2> Tensor;

    Operand const &m_operand;
    mutable bool m_eval_is_cached;
    mutable Tensor m_cached_tensor;
};

// definitions of the squared_norm and norm methods of ExpressionTemplate_i had to wait until ExpressionTemplate_Eval_t was defined.
template <typename Derived, typename Scalar, typename FreeIndexTypeList, typename UsedIndexTypeList>
typename AssociatedFloatingPointType_t<Scalar>::T ExpressionTemplate_i<Derived,Scalar,FreeIndexTypeList,UsedIndexTypeList>::squared_norm () const
{
    return eval().squared_norm();
}

template <typename Derived, typename Scalar, typename FreeIndexTypeList, typename UsedIndexTypeList>
typename AssociatedFloatingPointType_t<Scalar>::T ExpressionTemplate_i<Derived,Scalar,FreeIndexTypeList,UsedIndexTypeList>::norm () const
{
    return eval().norm();
}

} // end of namespace Tenh

#endif // TENH_EXPRESSIONTEMPLATE_EVAL_HPP_
