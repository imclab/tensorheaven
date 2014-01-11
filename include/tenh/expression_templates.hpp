// ///////////////////////////////////////////////////////////////////////////
// tenh/expression_templates.hpp by Victor Dods, created 2013/03/13
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_EXPRESSION_TEMPLATES_HPP_
#define TENH_EXPRESSION_TEMPLATES_HPP_

#include "tenh/core.hpp"

#include <stdexcept>

#include "tenh/expression_templates_utility.hpp"
#include "tenh/interface/expressiontemplate.hpp"
#include "tenh/reindex.hpp"

namespace Tenh {

// ////////////////////////////////////////////////////////////////////////////
// expression-template-generation (making ETs from vectors/tensors)
// ////////////////////////////////////////////////////////////////////////////

static bool const FORCE_CONST = true;
static bool const DONT_FORCE_CONST = false;

static bool const CHECK_FOR_ALIASING = true;
static bool const DONT_CHECK_FOR_ALIASING = false;

// this is the "const" version of an indexed tensor expression (it has summed indices, so it doesn't make sense to assign to it)
template <typename Object,
          typename FactorTypeList, // this is necessary because the factor type depends on if the thing is being indexed as a vector or tensor
          typename DimIndexTypeList,
          typename SummedDimIndexTypeList_,
          bool FORCE_CONST_,
          bool CHECK_FOR_ALIASING_,
          typename Derived_ = NullType>
struct ExpressionTemplate_IndexedObject_t
    :
    public ExpressionTemplate_i<typename DerivedType_f<Derived_,
                                                       ExpressionTemplate_IndexedObject_t<Object,
                                                                                          FactorTypeList,
                                                                                          DimIndexTypeList,
                                                                                          SummedDimIndexTypeList_,
                                                                                          FORCE_CONST_,
                                                                                          CHECK_FOR_ALIASING_,
                                                                                          Derived_> >::T,
                                typename Object::Scalar,
                                typename FreeFactorTypeList_t<FactorTypeList,DimIndexTypeList>::T,
                                typename FreeIndexTypeList_t<DimIndexTypeList>::T,
                                SummedDimIndexTypeList_>
{
    enum
    {
        // TODO: assert that Object is NOT an expression template (it could be e.g. an IndexBundle_t though)
        // TODO: assert that FactorTypeList is a TypeList of BasedVectorSpace_c types.
        STATIC_ASSERT_IN_ENUM__UNIQUE((EachTypeSatisfies_f<DimIndexTypeList,IsDimIndex_p>::V), MUST_BE_TYPELIST_OF_DIM_INDEX_TYPES, DIMINDEXTYPELIST),
        STATIC_ASSERT_IN_ENUM__UNIQUE((EachTypeSatisfies_f<SummedDimIndexTypeList_,IsDimIndex_p>::V), MUST_BE_TYPELIST_OF_DIM_INDEX_TYPES, SUMMEDDIMINDEXTYPELIST)
    };

    typedef ExpressionTemplate_i<typename DerivedType_f<Derived_,
                                                        ExpressionTemplate_IndexedObject_t<Object,
                                                                                           FactorTypeList,
                                                                                           DimIndexTypeList,
                                                                                           SummedDimIndexTypeList_,
                                                                                           FORCE_CONST_,
                                                                                           CHECK_FOR_ALIASING_,
                                                                                           Derived_> >::T,
                                 typename Object::Scalar,
                                 typename FreeFactorTypeList_t<FactorTypeList,DimIndexTypeList>::T,
                                 typename FreeIndexTypeList_t<DimIndexTypeList>::T,
                                 SummedDimIndexTypeList_> Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeFactorTypeList FreeFactorTypeList;
    typedef typename Parent::FreeDimIndexTypeList FreeDimIndexTypeList;
    typedef typename Parent::UsedDimIndexTypeList UsedDimIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;
    typedef SummedDimIndexTypeList_ SummedDimIndexTypeList;

    // object must be a "terminal" instance (e.g. an ImplementationOf_t type) which will live
    // at least as long as the relevant indexed expression.
    ExpressionTemplate_IndexedObject_t (Object const &object) : m_object(object) { }

    operator Scalar () const
    {
        STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeDimIndexTypeList);
        return operator[](MultiIndex());
    }

    // read-only, because it doesn't make sense to assign to an expression which is a summation.
    Scalar operator [] (MultiIndex const &m) const
    {
        return UnarySummation_t<Object,DimIndexTypeList,SummedDimIndexTypeList>::eval(m_object, m);
    }

    bool overlaps_memory_range (Uint8 const *ptr, Uint32 range) const
    {
        return m_object.overlaps_memory_range(ptr, range);
    }

    Object const &object () const { return m_object; }

    static std::string type_as_string (bool verbose)
    {
        return "ExpressionTemplate_IndexedObject_t<" + type_string_of<Object>() + ','
                                                     + type_string_of<FactorTypeList>() + ','
                                                     + type_string_of<DimIndexTypeList>() + ','
                                                     + type_string_of<SummedDimIndexTypeList_>() + ','
                                                     + FORMAT(FORCE_CONST_) + ','
                                                     + FORMAT(CHECK_FOR_ALIASING_) + ','
                                                     + type_string_of<Derived_>() + '>';
    }

private:

    void operator = (ExpressionTemplate_IndexedObject_t const &);

    Object const &m_object;
};

template <typename Object_,
          typename FactorTypeList_,
          typename DimIndexTypeList_,
          typename SummedDimIndexTypeList_,
          bool FORCE_CONST_,
          bool CHECK_FOR_ALIASING_,
          typename Derived_>
struct IsExpressionTemplate_f<ExpressionTemplate_IndexedObject_t<Object_,
                                                                 FactorTypeList_,
                                                                 DimIndexTypeList_,
                                                                 SummedDimIndexTypeList_,
                                                                 FORCE_CONST_,
                                                                 CHECK_FOR_ALIASING_,
                                                                 Derived_> >
{
    static bool const V = true;
private:
    IsExpressionTemplate_f();
};

// this is the "non-const" version of an indexed tensor expression (it has no summed indices, so it makes sense to assign to it)
template <typename Object,
          typename FactorTypeList,
          typename DimIndexTypeList,
          bool CHECK_FOR_ALIASING_,
          typename Derived_>
struct ExpressionTemplate_IndexedObject_t<Object,FactorTypeList,DimIndexTypeList,EmptyTypeList,DONT_FORCE_CONST,CHECK_FOR_ALIASING_,Derived_>
    :
    public ExpressionTemplate_i<ExpressionTemplate_IndexedObject_t<Object,FactorTypeList,DimIndexTypeList,EmptyTypeList,DONT_FORCE_CONST,CHECK_FOR_ALIASING_,Derived_>,
                                typename Object::Scalar,
                                typename FreeFactorTypeList_t<FactorTypeList,DimIndexTypeList>::T,
                                typename FreeIndexTypeList_t<DimIndexTypeList>::T,
                                EmptyTypeList>
{
    typedef ExpressionTemplate_i<ExpressionTemplate_IndexedObject_t<Object,FactorTypeList,DimIndexTypeList,EmptyTypeList,DONT_FORCE_CONST,CHECK_FOR_ALIASING_,Derived_>,
                                 typename Object::Scalar,
                                 typename FreeFactorTypeList_t<FactorTypeList,DimIndexTypeList>::T,
                                 typename FreeIndexTypeList_t<DimIndexTypeList>::T,
                                 EmptyTypeList> Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeFactorTypeList FreeFactorTypeList;
    typedef typename Parent::FreeDimIndexTypeList FreeDimIndexTypeList;
    typedef typename Parent::UsedDimIndexTypeList UsedDimIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;

    // object must be a "terminal" instance (e.g. an ImplementationOf_t type) which will live
    // at least as long as the relevant indexed expression.
    ExpressionTemplate_IndexedObject_t (Object &object) : m_object(object) { }

    // call this on the left-hand side (LHS) of an indexed assignment to avoid the run-time
    // aliasing check.  this should only be done when the human can guarantee that there is
    // no memory aliasing in the assignment (where the same memory location is being referenced
    // on both the LHS and RHS of the assignment, therefore causing the non-atomically
    // evaluated result to be implementation-dependent and incorrect).
    ExpressionTemplate_IndexedObject_t<Object,FactorTypeList,DimIndexTypeList,EmptyTypeList,DONT_FORCE_CONST,DONT_CHECK_FOR_ALIASING,Derived_> no_alias ()
    {
        return ExpressionTemplate_IndexedObject_t<Object,FactorTypeList,DimIndexTypeList,EmptyTypeList,DONT_FORCE_CONST,DONT_CHECK_FOR_ALIASING,Derived_>(m_object);
    }

    operator Scalar () const
    {
        STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeDimIndexTypeList);
        return operator[](MultiIndex());
    }

    // read-only, because it doesn't necessarily make sense to assign to an expression
    // template -- the expression may be a product or some such, where each component
    // is not an L-value.
    Scalar operator [] (MultiIndex const &m) const { return m_object[m]; }

    // for some dumb reason, the compiler needed a non-templatized assignment operator for the exact matching type
    void operator = (ExpressionTemplate_IndexedObject_t const &right_operand)
    {
        // if right and left operands' m_tensor references are the same, this is a no-op
        if (&right_operand.m_object == &m_object)
            return;

        // can't do a memcpy because the right operand might be an procedural vector and have no memory
        for (typename Object::ComponentIndex i; i.is_not_at_end(); ++i)
            m_object[i] = right_operand[typename ExpressionTemplate_IndexedObject_t::MultiIndex(i)];
    }

    void operator += (ExpressionTemplate_IndexedObject_t const &right_operand)
    {
        for (typename Object::ComponentIndex i; i.is_not_at_end(); ++i)
            m_object[i] += right_operand[typename ExpressionTemplate_IndexedObject_t::MultiIndex(i)];
    }

    void operator -= (ExpressionTemplate_IndexedObject_t const &right_operand)
    {
        for (typename Object::ComponentIndex i; i.is_not_at_end(); ++i)
            m_object[i] -= right_operand[typename ExpressionTemplate_IndexedObject_t::MultiIndex(i)];
    }

    template <typename RightOperand>
    void operator = (RightOperand const &right_operand)
    {
        enum
        {
            STATIC_ASSERT_IN_ENUM(IsExpressionTemplate_f<RightOperand>::V, RIGHT_OPERAND_IS_EXPRESSION_TEMPLATE),
            STATIC_ASSERT_IN_ENUM((TypesAreEqual_f<Scalar,typename RightOperand::Scalar>::V), OPERAND_SCALAR_TYPES_ARE_EQUAL),
            STATIC_ASSERT_IN_ENUM((AreEqualAsSets_t<FreeDimIndexTypeList,typename RightOperand::FreeDimIndexTypeList>::V),OPERANDS_HAVE_SAME_FREE_INDICES),
            STATIC_ASSERT_IN_ENUM((!ContainsDuplicates_t<FreeDimIndexTypeList>::V), LEFT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES),
            STATIC_ASSERT_IN_ENUM((!ContainsDuplicates_t<typename RightOperand::FreeDimIndexTypeList>::V), RIGHT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES)
        };

        // check for aliasing (where source and destination memory overlap)
        Uint8 const *ptr = reinterpret_cast<Uint8 const *>(m_object.pointer_to_allocation());
        Uint32 range = m_object.allocation_size_in_bytes();
        if (CHECK_FOR_ALIASING_ && right_operand.overlaps_memory_range(ptr, range))
            throw std::invalid_argument("aliased tensor assignment (source and destination memory overlap) -- see eval() and no_alias()");

        typedef MultiIndexMap_t<FreeDimIndexTypeList,typename RightOperand::FreeDimIndexTypeList> RightOperandIndexMap;
        typename RightOperandIndexMap::EvalMapType right_operand_index_map = RightOperandIndexMap::eval;

        // component-wise assignment via the free index type.
        for (MultiIndex m; m.is_not_at_end(); ++m)
            m_object[m] = right_operand[right_operand_index_map(m)];
    }

    template <typename RightOperand>
    void operator += (RightOperand const &right_operand)
    {
        enum
        {
            STATIC_ASSERT_IN_ENUM(IsExpressionTemplate_f<RightOperand>::V, RIGHT_OPERAND_IS_EXPRESSION_TEMPLATE),
            STATIC_ASSERT_IN_ENUM((TypesAreEqual_f<Scalar,typename RightOperand::Scalar>::V), OPERAND_SCALAR_TYPES_ARE_EQUAL),
            STATIC_ASSERT_IN_ENUM((AreEqualAsSets_t<FreeDimIndexTypeList,typename RightOperand::FreeDimIndexTypeList>::V),OPERANDS_HAVE_SAME_FREE_INDICES),
            STATIC_ASSERT_IN_ENUM((!ContainsDuplicates_t<FreeDimIndexTypeList>::V), LEFT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES),
            STATIC_ASSERT_IN_ENUM((!ContainsDuplicates_t<typename RightOperand::FreeDimIndexTypeList>::V), RIGHT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES)
        };

        // check for aliasing (where source and destination memory overlap)
        Uint8 const *ptr = reinterpret_cast<Uint8 const *>(m_object.pointer_to_allocation());
        Uint32 range = m_object.allocation_size_in_bytes();
        if (CHECK_FOR_ALIASING_ && right_operand.overlaps_memory_range(ptr, range))
            throw std::invalid_argument("aliased tensor assignment (source and destination memory overlap) -- see eval() and no_alias()");

        typedef MultiIndexMap_t<FreeDimIndexTypeList,typename RightOperand::FreeDimIndexTypeList> RightOperandIndexMap;
        typename RightOperandIndexMap::EvalMapType right_operand_index_map = RightOperandIndexMap::eval;

        // component-wise assignment via the free index type.
        for (MultiIndex m; m.is_not_at_end(); ++m)
            m_object[m] += right_operand[right_operand_index_map(m)];
    }

    template <typename RightOperand>
    void operator -= (RightOperand const &right_operand)
    {
        enum
        {
            STATIC_ASSERT_IN_ENUM(IsExpressionTemplate_f<RightOperand>::V, RIGHT_OPERAND_IS_EXPRESSION_TEMPLATE),
            STATIC_ASSERT_IN_ENUM((TypesAreEqual_f<Scalar,typename RightOperand::Scalar>::V), OPERAND_SCALAR_TYPES_ARE_EQUAL),
            STATIC_ASSERT_IN_ENUM((AreEqualAsSets_t<FreeDimIndexTypeList,typename RightOperand::FreeDimIndexTypeList>::V),OPERANDS_HAVE_SAME_FREE_INDICES),
            STATIC_ASSERT_IN_ENUM((!ContainsDuplicates_t<FreeDimIndexTypeList>::V), LEFT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES),
            STATIC_ASSERT_IN_ENUM((!ContainsDuplicates_t<typename RightOperand::FreeDimIndexTypeList>::V), RIGHT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES)
        };

        // check for aliasing (where source and destination memory overlap)
        Uint8 const *ptr = reinterpret_cast<Uint8 const *>(m_object.pointer_to_allocation());
        Uint32 range = m_object.allocation_size_in_bytes();
        if (CHECK_FOR_ALIASING_ && right_operand.overlaps_memory_range(ptr, range))
            throw std::invalid_argument("aliased tensor assignment (source and destination memory overlap) -- see eval() and no_alias()");

        typedef MultiIndexMap_t<FreeDimIndexTypeList,typename RightOperand::FreeDimIndexTypeList> RightOperandIndexMap;
        typename RightOperandIndexMap::EvalMapType right_operand_index_map = RightOperandIndexMap::eval;

        // component-wise assignment via the free index type.
        for (MultiIndex m; m.is_not_at_end(); ++m)
            m_object[m] -= right_operand[right_operand_index_map(m)];
    }

    bool overlaps_memory_range (Uint8 const *ptr, Uint32 range) const
    {
        return m_object.overlaps_memory_range(ptr, range);
    }

    Object &object () const { return m_object; }

    static std::string type_as_string (bool verbose)
    {
        return "ExpressionTemplate_IndexedObject_t<" + type_string_of<Object>() + ','
                                                     + type_string_of<FactorTypeList>() + ','
                                                     + type_string_of<DimIndexTypeList>() + ','
                                                     + type_string_of<EmptyTypeList>() + ','
                                                     + FORMAT(DONT_FORCE_CONST) + ','
                                                     + FORMAT(CHECK_FOR_ALIASING_) + ','
                                                     + type_string_of<Derived_>() + '>';
    }

private:

    Object &m_object;
};

// ////////////////////////////////////////////////////////////////////////////
// addition of expression templates
// ////////////////////////////////////////////////////////////////////////////

// NOTE: if this is ever subclassed, then it will be necessary to change the inheritance to pass in the Derived type
template <typename LeftOperand, typename RightOperand, char OPERATOR>
struct ExpressionTemplate_Addition_t
    :
    public ExpressionTemplate_i<ExpressionTemplate_Addition_t<LeftOperand,RightOperand,OPERATOR>,
                                typename LeftOperand::Scalar,
                                typename LeftOperand::FreeFactorTypeList,
                                typename LeftOperand::FreeDimIndexTypeList,
                                EmptyTypeList>
{
    typedef ExpressionTemplate_i<ExpressionTemplate_Addition_t<LeftOperand,RightOperand,OPERATOR>,
                                 typename LeftOperand::Scalar,
                                 typename LeftOperand::FreeFactorTypeList,
                                 typename LeftOperand::FreeDimIndexTypeList,
                                 EmptyTypeList> Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeFactorTypeList FreeFactorTypeList;
    typedef typename Parent::FreeDimIndexTypeList FreeDimIndexTypeList;
    typedef typename Parent::UsedDimIndexTypeList UsedDimIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;

private:

    typedef typename Zip_t<TypeList_t<typename LeftOperand::FreeFactorTypeList,
                           TypeList_t<typename LeftOperand::FreeDimIndexTypeList> > >::T LeftFactorAndIndexTypeList;
    typedef typename Zip_t<TypeList_t<typename RightOperand::FreeFactorTypeList,
                           TypeList_t<typename RightOperand::FreeDimIndexTypeList> > >::T RightFactorAndIndexTypeList;

    enum
    {
        STATIC_ASSERT_IN_ENUM(IsExpressionTemplate_f<LeftOperand>::V, LEFT_OPERAND_IS_EXPRESSION_TEMPLATE),
        STATIC_ASSERT_IN_ENUM(IsExpressionTemplate_f<RightOperand>::V, RIGHT_OPERAND_IS_EXPRESSION_TEMPLATE),
        STATIC_ASSERT_IN_ENUM((TypesAreEqual_f<typename LeftOperand::Scalar,typename RightOperand::Scalar>::V), OPERAND_SCALAR_TYPES_ARE_EQUAL),
        STATIC_ASSERT_IN_ENUM((AreEqualAsSets_t<LeftFactorAndIndexTypeList,RightFactorAndIndexTypeList>::V), OPERANDS_HAVE_SAME_FACTORS),
        STATIC_ASSERT_IN_ENUM((AreEqualAsSets_t<typename LeftOperand::FreeDimIndexTypeList,typename RightOperand::FreeDimIndexTypeList>::V), OPERANDS_HAVE_SAME_FREE_INDICES),
        STATIC_ASSERT_IN_ENUM(!ContainsDuplicates_t<typename LeftOperand::FreeDimIndexTypeList>::V, LEFT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES),
        STATIC_ASSERT_IN_ENUM(!ContainsDuplicates_t<typename RightOperand::FreeDimIndexTypeList>::V, RIGHT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES),
        STATIC_ASSERT_IN_ENUM((OPERATOR == '+' || OPERATOR == '-'), OPERATOR_IS_PLUS_OR_MINUS)
    };

public:

    ExpressionTemplate_Addition_t (LeftOperand const &left_operand, RightOperand const &right_operand)
        :
        m_left_operand(left_operand),
        m_right_operand(right_operand)
    { }

    operator Scalar () const
    {
        STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeDimIndexTypeList);
        if (OPERATOR == '+')
            return m_left_operand.operator Scalar() + m_right_operand.operator Scalar();
        else // OPERATOR == '-'
            return m_left_operand.operator Scalar() - m_right_operand.operator Scalar();
    }

    // read-only, because it doesn't make sense to assign to an expression which is a summation.
    Scalar operator [] (MultiIndex const &m) const
    {
        typedef MultiIndexMap_t<FreeDimIndexTypeList,typename LeftOperand::FreeDimIndexTypeList> LeftOperandIndexMap;
        typedef MultiIndexMap_t<FreeDimIndexTypeList,typename RightOperand::FreeDimIndexTypeList> RightOperandIndexMap;
        typename LeftOperandIndexMap::EvalMapType left_operand_index_map = LeftOperandIndexMap::eval;
        typename RightOperandIndexMap::EvalMapType right_operand_index_map = RightOperandIndexMap::eval;
        if (OPERATOR == '+')
            return m_left_operand[left_operand_index_map(m)] + m_right_operand[right_operand_index_map(m)];
        else // OPERATOR == '-'
            return m_left_operand[left_operand_index_map(m)] - m_right_operand[right_operand_index_map(m)];
    }

    bool overlaps_memory_range (Uint8 const *ptr, Uint32 range) const
    {
        return m_left_operand.overlaps_memory_range(ptr, range) || m_right_operand.overlaps_memory_range(ptr, range);
    }

    LeftOperand const &left_operand () const { return m_left_operand; }
    RightOperand const &right_operand () const { return m_right_operand; }

    static std::string type_as_string (bool verbose)
    {
        return "ExpressionTemplate_Addition_t<" + type_string_of<LeftOperand>() + ','
                                                + type_string_of<RightOperand>() + ','
                                                + '\'' + FORMAT(OPERATOR) + '\'' + '>';
    }

private:

    void operator = (ExpressionTemplate_Addition_t const &);

    LeftOperand m_left_operand;
    RightOperand m_right_operand;
};

template <typename LeftOperand_, typename RightOperand_, char OPERATOR_>
struct IsExpressionTemplate_f<ExpressionTemplate_Addition_t<LeftOperand_,RightOperand_,OPERATOR_> >
{
    static bool const V = true;
private:
    IsExpressionTemplate_f();
};

// ////////////////////////////////////////////////////////////////////////////
// scalar multiplication and division of expression templates
// ////////////////////////////////////////////////////////////////////////////

// it is assumed that scalar multiplication is commutative.
// OPERATOR can be '*' or '/'.
// NOTE: if this is ever subclassed, then it will be necessary to change the inheritance to pass in the Derived type
template <typename Operand, typename Scalar_, char OPERATOR>
struct ExpressionTemplate_ScalarMultiplication_t
    :
    public ExpressionTemplate_i<ExpressionTemplate_ScalarMultiplication_t<Operand,Scalar_,OPERATOR>,
                                typename Operand::Scalar,
                                typename Operand::FreeFactorTypeList,
                                typename Operand::FreeDimIndexTypeList,
                                typename Operand::UsedDimIndexTypeList>
{
    typedef ExpressionTemplate_i<ExpressionTemplate_ScalarMultiplication_t<Operand,Scalar_,OPERATOR>,
                                 typename Operand::Scalar,
                                 typename Operand::FreeFactorTypeList,
                                 typename Operand::FreeDimIndexTypeList,
                                 typename Operand::UsedDimIndexTypeList> Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeFactorTypeList FreeFactorTypeList;
    typedef typename Parent::FreeDimIndexTypeList FreeDimIndexTypeList;
    typedef typename Parent::UsedDimIndexTypeList UsedDimIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;

    enum
    {
        STATIC_ASSERT_IN_ENUM(IsExpressionTemplate_f<Operand>::V, OPERAND_IS_EXPRESSION_TEMPLATE),
        STATIC_ASSERT_IN_ENUM((TypesAreEqual_f<typename Operand::Scalar,Scalar_>::V), OPERAND_SCALAR_MATCHES_SCALAR),
        STATIC_ASSERT_IN_ENUM((OPERATOR == '*' || OPERATOR == '/'), OPERATOR_IS_VALID)
    };

    ExpressionTemplate_ScalarMultiplication_t (Operand const &operand, Scalar scalar_operand)
        :
        m_operand(operand),
        m_scalar_operand(scalar_operand)
    {
        // TODO: should there be a runtime check here if OPERATOR is '/' and scalar_operand is zero (or close to zero)?
    }

    operator Scalar () const
    {
        STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeDimIndexTypeList);
        return operator[](MultiIndex());
    }

    Scalar operator [] (MultiIndex const &m) const
    {
        if (OPERATOR == '*')
            return m_operand[m] * m_scalar_operand;
        else
            return m_operand[m] / m_scalar_operand;
    }

    bool overlaps_memory_range (Uint8 const *ptr, Uint32 range) const
    {
        return m_operand.overlaps_memory_range(ptr, range);
    }

    Operand const &operand () const { return m_operand; }
    Scalar const &scalar_operand () const { return m_scalar_operand; }

    static std::string type_as_string (bool verbose)
    {
        return "ExpressionTemplate_ScalarMultiplication_t<" + type_string_of<Operand>() + ','
                                                            + type_string_of<Scalar>() + ','
                                                            + '\'' + FORMAT(OPERATOR) + '\'' + '>';
    }

private:

    void operator = (ExpressionTemplate_ScalarMultiplication_t const &);

    Operand m_operand;
    Scalar m_scalar_operand;
};

template <typename Operand_, typename Scalar_, char OPERATOR_>
struct IsExpressionTemplate_f<ExpressionTemplate_ScalarMultiplication_t<Operand_,Scalar_,OPERATOR_> >
{
    static bool const V = true;
private:
    IsExpressionTemplate_f();
};

// ////////////////////////////////////////////////////////////////////////////
// multiplication of expression templates (tensor product and contraction)
// ////////////////////////////////////////////////////////////////////////////

// TODO: there is an issue to think about: while it is totally valid to do
// u(i)*v(j)*w(j) (this is an outer product contracted with a vector), the
// expression v(j)*w(j) can be computed first and factored out of the whole
// thing, instead of needing to be multiplied out for each access of the i index.
// this may be somewhat difficult to do, as it would require searching the
// expression template AST for such contractions and restructuring the AST.
// NOTE: if this is ever subclassed, then it will be necessary to change the inheritance to pass in the Derived type
template <typename LeftOperand, typename RightOperand>
struct ExpressionTemplate_Multiplication_t
    :
    public ExpressionTemplate_i<ExpressionTemplate_Multiplication_t<LeftOperand,RightOperand>,
                                typename LeftOperand::Scalar,
                                typename FreeFactorTypeListOfMultiplication_t<LeftOperand,RightOperand>::T,
                                typename FreeDimIndexTypeListOfMultiplication_t<LeftOperand,RightOperand>::T,
                                typename UsedDimIndexTypeListOfMultiplication_t<LeftOperand,RightOperand>::T>
{
    typedef ExpressionTemplate_i<ExpressionTemplate_Multiplication_t<LeftOperand,RightOperand>,
                                 typename LeftOperand::Scalar,
                                 typename FreeFactorTypeListOfMultiplication_t<LeftOperand,RightOperand>::T,
                                 typename FreeDimIndexTypeListOfMultiplication_t<LeftOperand,RightOperand>::T,
                                 typename UsedDimIndexTypeListOfMultiplication_t<LeftOperand,RightOperand>::T> Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeFactorTypeList FreeFactorTypeList;
    typedef typename Parent::FreeDimIndexTypeList FreeDimIndexTypeList;
    typedef typename Parent::UsedDimIndexTypeList UsedDimIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;

    typedef typename SummedDimIndexTypeListOfMultiplication_t<LeftOperand,RightOperand>::T SummedDimIndexTypeList;

    // TODO: check that the summed indices from each operand have no indices in common
    // though technically this is unnecessary, because the summed indices are "private"
    // to each contraction, so this is really for the human's benefit, not getting
    // confused by multiple repeated indices that have nothing to do with each other.
    enum
    {
        STATIC_ASSERT_IN_ENUM(IsExpressionTemplate_f<LeftOperand>::V, LEFT_OPERAND_IS_EXPRESSION_TEMPLATE),
        STATIC_ASSERT_IN_ENUM(IsExpressionTemplate_f<RightOperand>::V, RIGHT_OPERAND_IS_EXPRESSION_TEMPLATE),
        STATIC_ASSERT_IN_ENUM((TypesAreEqual_f<typename LeftOperand::Scalar,typename RightOperand::Scalar>::V), OPERAND_SCALAR_TYPES_ARE_EQUAL),
        STATIC_ASSERT_IN_ENUM((!HasNontrivialIntersectionAsSets_t<FreeDimIndexTypeList,UsedDimIndexTypeList>::V), FREE_INDICES_DONT_COLLIDE_WITH_USED)
    };
    // TODO: ensure there are no indices that occur 3+ times (?)

    ExpressionTemplate_Multiplication_t (LeftOperand const &left_operand, RightOperand const &right_operand)
        :
        m_left_operand(left_operand),
        m_right_operand(right_operand)
    { }

    // available ONLY if FreeDimIndexTypeList is EmptyTypeList
    operator Scalar () const
    {
        STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeDimIndexTypeList);
        return operator[](MultiIndex());
    }

    Scalar operator [] (MultiIndex const &m) const
    {
        return BinarySummation_t<LeftOperand,RightOperand,FreeDimIndexTypeList,SummedDimIndexTypeList>::eval(m_left_operand, m_right_operand, m);
    }

    bool overlaps_memory_range (Uint8 const *ptr, Uint32 range) const
    {
        return m_left_operand.overlaps_memory_range(ptr, range) || m_right_operand.overlaps_memory_range(ptr, range);
    }

    LeftOperand const &left_operand () const { return m_left_operand; }
    RightOperand const &right_operand () const { return m_right_operand; }

    static std::string type_as_string (bool verbose)
    {
        return "ExpressionTemplate_Multiplication_t<" + type_string_of<LeftOperand>() + ','
                                                      + type_string_of<RightOperand>() + '>';
    }

private:

    void operator = (ExpressionTemplate_Multiplication_t const &);

    LeftOperand m_left_operand;
    RightOperand m_right_operand;
};

template <typename LeftOperand_, typename RightOperand_>
struct IsExpressionTemplate_f<ExpressionTemplate_Multiplication_t<LeftOperand_,RightOperand_> >
{
    static bool const V = true;
private:
    IsExpressionTemplate_f();
};

// ////////////////////////////////////////////////////////////////////////////
// bundling multiple separate indices into a single vector index (downcasting)
// ////////////////////////////////////////////////////////////////////////////

// TODO: use a metafunction to define the parent class and clean this up
template <typename Operand,
          typename BundleAbstractIndexTypeList,
          typename ResultingFactorType,
          typename ResultingAbstractIndexType,
          bool DONT_CHECK_FACTOR_TYPES_>
struct ExpressionTemplate_IndexBundle_t
    :
    public ExpressionTemplate_IndexedObject_t<IndexBundle_t<Operand,BundleAbstractIndexTypeList,ResultingFactorType,ResultingAbstractIndexType,DONT_CHECK_FACTOR_TYPES_>,
                                              typename IndexBundle_t<Operand,BundleAbstractIndexTypeList,ResultingFactorType,ResultingAbstractIndexType,DONT_CHECK_FACTOR_TYPES_>::FactorTypeList,
                                              typename IndexBundle_t<Operand,BundleAbstractIndexTypeList,ResultingFactorType,ResultingAbstractIndexType,DONT_CHECK_FACTOR_TYPES_>::DimIndexTypeList,
                                              typename SummedIndexTypeList_t<typename IndexBundle_t<Operand,BundleAbstractIndexTypeList,ResultingFactorType,ResultingAbstractIndexType,DONT_CHECK_FACTOR_TYPES_>::DimIndexTypeList>::T,
                                              FORCE_CONST,
                                              CHECK_FOR_ALIASING, // irrelevant value
                                              ExpressionTemplate_IndexBundle_t<Operand,BundleAbstractIndexTypeList,ResultingFactorType,ResultingAbstractIndexType,DONT_CHECK_FACTOR_TYPES_> >
{
    typedef ExpressionTemplate_IndexedObject_t<IndexBundle_t<Operand,BundleAbstractIndexTypeList,ResultingFactorType,ResultingAbstractIndexType,DONT_CHECK_FACTOR_TYPES_>,
                                               typename IndexBundle_t<Operand,BundleAbstractIndexTypeList,ResultingFactorType,ResultingAbstractIndexType,DONT_CHECK_FACTOR_TYPES_>::FactorTypeList,
                                               typename IndexBundle_t<Operand,BundleAbstractIndexTypeList,ResultingFactorType,ResultingAbstractIndexType,DONT_CHECK_FACTOR_TYPES_>::DimIndexTypeList,
                                               typename SummedIndexTypeList_t<typename IndexBundle_t<Operand,BundleAbstractIndexTypeList,ResultingFactorType,ResultingAbstractIndexType,DONT_CHECK_FACTOR_TYPES_>::DimIndexTypeList>::T,
                                               FORCE_CONST,
                                               CHECK_FOR_ALIASING, // irrelevant value
                                               ExpressionTemplate_IndexBundle_t<Operand,BundleAbstractIndexTypeList,ResultingFactorType,ResultingAbstractIndexType,DONT_CHECK_FACTOR_TYPES_> > Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeFactorTypeList FreeFactorTypeList;
    typedef typename Parent::FreeDimIndexTypeList FreeDimIndexTypeList;
    typedef typename Parent::UsedDimIndexTypeList UsedDimIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;
    typedef typename Parent::SummedDimIndexTypeList SummedDimIndexTypeList;

private:

    typedef IndexBundle_t<Operand,BundleAbstractIndexTypeList,ResultingFactorType,ResultingAbstractIndexType,DONT_CHECK_FACTOR_TYPES_> IndexBundle;

public:

    // it's not great that m_bundler is being passed to Parent before
    // m_bundler is constructed, but Parent only stores a reference
    // to m_bundler, so it doesn't really matter.
    ExpressionTemplate_IndexBundle_t (Operand const &operand) : Parent(m_bundler), m_bundler(operand) { }

    operator Scalar () const
    {
        STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeDimIndexTypeList);
        return operator[](MultiIndex());
    }

    // read-only, because it doesn't make sense to assign to an index-bundled expression (which is possibly also a summation).
    Scalar operator [] (MultiIndex const &m) const
    {
        return UnarySummation_t<IndexBundle,typename IndexBundle::DimIndexTypeList,SummedDimIndexTypeList>::eval(m_bundler, m);
    }

    bool overlaps_memory_range (Uint8 const *ptr, Uint32 range) const
    {
        return m_bundler.overlaps_memory_range(ptr, range);
    }

    Operand const &operand () const { return m_bundler.operand(); }

    static std::string type_as_string (bool verbose)
    {
        return "ExpressionTemplate_IndexBundle_t<" + type_string_of<Operand>() + ','
                                                   + type_string_of<BundleAbstractIndexTypeList>() + ','
                                                   + type_string_of<ResultingFactorType>() + ','
                                                   + type_string_of<ResultingAbstractIndexType>() + '>';
    }

private:

    void operator = (ExpressionTemplate_IndexBundle_t const &);

    IndexBundle m_bundler;
};

template <typename Operand_,
          typename BundleAbstractIndexTypeList_,
          typename ResultingFactorType_,
          typename ResultingAbstractIndexType_,
          bool DONT_CHECK_FACTOR_TYPES_>
struct IsExpressionTemplate_f<ExpressionTemplate_IndexBundle_t<Operand_,
                                                               BundleAbstractIndexTypeList_,
                                                               ResultingFactorType_,
                                                               ResultingAbstractIndexType_,
                                                               DONT_CHECK_FACTOR_TYPES_> >
{
    static bool const V = true;
private:
    IsExpressionTemplate_f();
};

// ////////////////////////////////////////////////////////////////////////////
// splitting a single vector index into a multiple separate indices (upcasting)
// ////////////////////////////////////////////////////////////////////////////

// TODO: use a metafunction to define the parent class and clean this up
template <typename Operand, typename SourceAbstractIndexType, typename SplitAbstractIndexTypeList>
struct ExpressionTemplate_IndexSplit_t
    :
    public ExpressionTemplate_IndexedObject_t<IndexSplitter_t<Operand,SourceAbstractIndexType,SplitAbstractIndexTypeList>,
                                              typename IndexSplitter_t<Operand,SourceAbstractIndexType,SplitAbstractIndexTypeList>::FactorTypeList,
                                              typename IndexSplitter_t<Operand,SourceAbstractIndexType,SplitAbstractIndexTypeList>::DimIndexTypeList,
                                              typename SummedIndexTypeList_t<typename IndexSplitter_t<Operand,SourceAbstractIndexType,SplitAbstractIndexTypeList>::DimIndexTypeList>::T,
                                              FORCE_CONST,
                                              CHECK_FOR_ALIASING, // irrelevant value
                                              ExpressionTemplate_IndexSplit_t<Operand,SourceAbstractIndexType,SplitAbstractIndexTypeList> >
{
    enum
    {
        STATIC_ASSERT_IN_ENUM(IsAbstractIndex_f<SourceAbstractIndexType>::V, MUST_BE_ABSTRACT_INDEX),
    };

    typedef ExpressionTemplate_IndexedObject_t<IndexSplitter_t<Operand,SourceAbstractIndexType,SplitAbstractIndexTypeList>,
                                               typename IndexSplitter_t<Operand,SourceAbstractIndexType,SplitAbstractIndexTypeList>::FactorTypeList,
                                               typename IndexSplitter_t<Operand,SourceAbstractIndexType,SplitAbstractIndexTypeList>::DimIndexTypeList,
                                               typename SummedIndexTypeList_t<typename IndexSplitter_t<Operand,SourceAbstractIndexType,SplitAbstractIndexTypeList>::DimIndexTypeList>::T,
                                               FORCE_CONST,
                                               CHECK_FOR_ALIASING, // irrelevant value
                                               ExpressionTemplate_IndexSplit_t<Operand,SourceAbstractIndexType,SplitAbstractIndexTypeList> > Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeFactorTypeList FreeFactorTypeList;
    typedef typename Parent::FreeDimIndexTypeList FreeDimIndexTypeList;
    typedef typename Parent::UsedDimIndexTypeList UsedDimIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;
    typedef typename Parent::SummedDimIndexTypeList SummedDimIndexTypeList;

private:

    typedef IndexSplitter_t<Operand,SourceAbstractIndexType,SplitAbstractIndexTypeList> IndexSplitter;

public:

    // it's not great that m_splitter is being passed to Parent before
    // m_splitter is constructed, but Parent only stores a reference
    // to m_splitter, so it doesn't really matter.
    ExpressionTemplate_IndexSplit_t (Operand const &operand) : Parent(m_splitter), m_splitter(operand) { }

    operator Scalar () const
    {
        STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeDimIndexTypeList);
        return operator[](MultiIndex());
    }

    // read-only, because it doesn't make sense to assign to an index-bundled expression (which is possibly also a summation).
    Scalar operator [] (MultiIndex const &m) const
    {
        return UnarySummation_t<IndexSplitter,typename IndexSplitter::DimIndexTypeList,SummedDimIndexTypeList>::eval(m_splitter, m);
    }

    bool overlaps_memory_range (Uint8 const *ptr, Uint32 range) const
    {
        return m_splitter.overlaps_memory_range(ptr, range);
    }

    Operand const &operand () const { return m_splitter.operand(); }

    static std::string type_as_string (bool verbose)
    {
        return "ExpressionTemplate_IndexSplit_t<" + type_string_of<Operand>() + ','
                                                  + type_string_of<SourceAbstractIndexType>() + ','
                                                  + type_string_of<SplitAbstractIndexTypeList>() + '>';
    }

private:

    void operator = (ExpressionTemplate_IndexSplit_t const &);

    IndexSplitter m_splitter;
};

template <typename Operand_,
          typename SourceAbstractIndexType_,
          typename SplitAbstractIndexTypeList_>
struct IsExpressionTemplate_f<ExpressionTemplate_IndexSplit_t<Operand_,
                                                              SourceAbstractIndexType_,
                                                              SplitAbstractIndexTypeList_> >
{
    static bool const V = true;
private:
    IsExpressionTemplate_f();
};

// ////////////////////////////////////////////////////////////////////////////
// splitting a single vector index into a a single vector index for larger space
// ////////////////////////////////////////////////////////////////////////////

// TODO: this could probably combine with ExpressionTemplate_IndexSplit_t
// TODO: use a metafunction to define the parent class and clean this up
template <typename Operand, typename SourceAbstractIndexType, typename SplitAbstractIndexType>
struct ExpressionTemplate_IndexSplitToIndex_t
    :
    public ExpressionTemplate_IndexedObject_t<IndexSplitToIndex_t<Operand,SourceAbstractIndexType,SplitAbstractIndexType>,
                                              typename IndexSplitToIndex_t<Operand,SourceAbstractIndexType,SplitAbstractIndexType>::FactorTypeList,
                                              typename IndexSplitToIndex_t<Operand,SourceAbstractIndexType,SplitAbstractIndexType>::DimIndexTypeList,
                                              typename SummedIndexTypeList_t<typename IndexSplitToIndex_t<Operand,SourceAbstractIndexType,SplitAbstractIndexType>::DimIndexTypeList>::T,
                                              FORCE_CONST,
                                              CHECK_FOR_ALIASING, // irrelevant value
                                              ExpressionTemplate_IndexSplitToIndex_t<Operand,SourceAbstractIndexType,SplitAbstractIndexType> >
{
    enum
    {
        STATIC_ASSERT_IN_ENUM(IsAbstractIndex_f<SourceAbstractIndexType>::V, MUST_BE_ABSTRACT_INDEX),
    };

    typedef ExpressionTemplate_IndexedObject_t<IndexSplitToIndex_t<Operand,SourceAbstractIndexType,SplitAbstractIndexType>,
                                               typename IndexSplitToIndex_t<Operand,SourceAbstractIndexType,SplitAbstractIndexType>::FactorTypeList,
                                               typename IndexSplitToIndex_t<Operand,SourceAbstractIndexType,SplitAbstractIndexType>::DimIndexTypeList,
                                               typename SummedIndexTypeList_t<typename IndexSplitToIndex_t<Operand,SourceAbstractIndexType,SplitAbstractIndexType>::DimIndexTypeList>::T,
                                               FORCE_CONST,
                                               CHECK_FOR_ALIASING, // irrelevant value
                                               ExpressionTemplate_IndexSplitToIndex_t<Operand,SourceAbstractIndexType,SplitAbstractIndexType> > Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeFactorTypeList FreeFactorTypeList;
    typedef typename Parent::FreeDimIndexTypeList FreeDimIndexTypeList;
    typedef typename Parent::UsedDimIndexTypeList UsedDimIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;
    typedef typename Parent::SummedDimIndexTypeList SummedDimIndexTypeList;

private:

    typedef IndexSplitToIndex_t<Operand,SourceAbstractIndexType,SplitAbstractIndexType> IndexSplitToIndex;

public:

    // it's not great that m_splitter is being passed to Parent before
    // m_splitter is constructed, but Parent only stores a reference
    // to m_splitter, so it doesn't really matter.
    ExpressionTemplate_IndexSplitToIndex_t (Operand const &operand) : Parent(m_splitter), m_splitter(operand) { }

    operator Scalar () const
    {
        STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeDimIndexTypeList);
        return operator[](MultiIndex());
    }

    // read-only, because it doesn't make sense to assign to an index-bundled expression (which is possibly also a summation).
    Scalar operator [] (MultiIndex const &m) const
    {
        return UnarySummation_t<IndexSplitToIndex,typename IndexSplitToIndex::DimIndexTypeList,SummedDimIndexTypeList>::eval(m_splitter, m);
    }

    bool overlaps_memory_range (Uint8 const *ptr, Uint32 range) const
    {
        return m_splitter.overlaps_memory_range(ptr, range);
    }

    Operand const &operand () const { return m_splitter.operand(); }

    static std::string type_as_string (bool verbose)
    {
        return "ExpressionTemplate_IndexSplitToIndex_t<" + type_string_of<Operand>() + ','
                                                         + type_string_of<SourceAbstractIndexType>() + ','
                                                         + type_string_of<SplitAbstractIndexType>() + '>';
    }

private:

    void operator = (ExpressionTemplate_IndexSplitToIndex_t const &);

    IndexSplitToIndex m_splitter;
};

template <typename Operand_,
          typename SourceAbstractIndexType_,
          typename SplitAbstractIndexTypeList_>
struct IsExpressionTemplate_f<ExpressionTemplate_IndexSplitToIndex_t<Operand_,
                                                                     SourceAbstractIndexType_,
                                                                     SplitAbstractIndexTypeList_> >
{
    static bool const V = true;
private:
    IsExpressionTemplate_f();
};

// ////////////////////////////////////////////////////////////////////////////
// embedding single vector index into a single vector index for larger space
// ////////////////////////////////////////////////////////////////////////////

// TODO: use a metafunction to define the parent class and clean this up
template <typename Operand_,
          typename SourceAbstractIndexType_,
          typename EmbeddingCodomain_,
          typename EmbeddedAbstractIndexType_,
          typename EmbeddingId_>
struct ExpressionTemplate_IndexEmbed_t
    :
    public ExpressionTemplate_IndexedObject_t<IndexEmbedder_t<Operand_,SourceAbstractIndexType_,EmbeddingCodomain_,EmbeddedAbstractIndexType_,EmbeddingId_>,
                                              typename IndexEmbedder_t<Operand_,SourceAbstractIndexType_,EmbeddingCodomain_,EmbeddedAbstractIndexType_,EmbeddingId_>::FactorTypeList,
                                              typename IndexEmbedder_t<Operand_,SourceAbstractIndexType_,EmbeddingCodomain_,EmbeddedAbstractIndexType_,EmbeddingId_>::DimIndexTypeList,
                                              typename SummedIndexTypeList_t<typename IndexEmbedder_t<Operand_,SourceAbstractIndexType_,EmbeddingCodomain_,EmbeddedAbstractIndexType_,EmbeddingId_>::DimIndexTypeList>::T,
                                              FORCE_CONST,
                                              CHECK_FOR_ALIASING, // irrelevant value
                                              ExpressionTemplate_IndexEmbed_t<Operand_,SourceAbstractIndexType_,EmbeddingCodomain_,EmbeddedAbstractIndexType_,EmbeddingId_> >
{
    enum
    {
        STATIC_ASSERT_IN_ENUM(IsAbstractIndex_f<SourceAbstractIndexType_>::V, MUST_BE_ABSTRACT_INDEX),
    };

    typedef ExpressionTemplate_IndexedObject_t<IndexEmbedder_t<Operand_,SourceAbstractIndexType_,EmbeddingCodomain_,EmbeddedAbstractIndexType_,EmbeddingId_>,
                                               typename IndexEmbedder_t<Operand_,SourceAbstractIndexType_,EmbeddingCodomain_,EmbeddedAbstractIndexType_,EmbeddingId_>::FactorTypeList,
                                               typename IndexEmbedder_t<Operand_,SourceAbstractIndexType_,EmbeddingCodomain_,EmbeddedAbstractIndexType_,EmbeddingId_>::DimIndexTypeList,
                                               typename SummedIndexTypeList_t<typename IndexEmbedder_t<Operand_,SourceAbstractIndexType_,EmbeddingCodomain_,EmbeddedAbstractIndexType_,EmbeddingId_>::DimIndexTypeList>::T,
                                               FORCE_CONST,
                                               CHECK_FOR_ALIASING, // irrelevant value
                                               ExpressionTemplate_IndexEmbed_t<Operand_,SourceAbstractIndexType_,EmbeddingCodomain_,EmbeddedAbstractIndexType_,EmbeddingId_> > Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeFactorTypeList FreeFactorTypeList;
    typedef typename Parent::FreeDimIndexTypeList FreeDimIndexTypeList;
    typedef typename Parent::UsedDimIndexTypeList UsedDimIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;
    typedef typename Parent::SummedDimIndexTypeList SummedDimIndexTypeList;

private:

    typedef IndexEmbedder_t<Operand_,SourceAbstractIndexType_,EmbeddingCodomain_,EmbeddedAbstractIndexType_,EmbeddingId_> IndexEmbedder;

public:

    // it's not great that m_embedder is being passed to Parent before
    // m_embedder is constructed, but Parent only stores a reference
    // to m_embedder, so it doesn't really matter.
    ExpressionTemplate_IndexEmbed_t (Operand_ const &operand) : Parent(m_embedder), m_embedder(operand) { }

    operator Scalar () const
    {
        STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeDimIndexTypeList);
        return operator[](MultiIndex());
    }

    // read-only, because it doesn't make sense to assign to an index-bundled expression (which is possibly also a summation).
    Scalar operator [] (MultiIndex const &m) const
    {
        return UnarySummation_t<IndexEmbedder,typename IndexEmbedder::DimIndexTypeList,SummedDimIndexTypeList>::eval(m_embedder, m);
    }

    bool overlaps_memory_range (Uint8 const *ptr, Uint32 range) const
    {
        return m_embedder.overlaps_memory_range(ptr, range);
    }

    Operand_ const &operand () const { return m_embedder.operand(); }

    static std::string type_as_string (bool verbose)
    {
        return "ExpressionTemplate_IndexEmbed_t<" + type_string_of<Operand_>() + ','
                                                  + type_string_of<SourceAbstractIndexType_>() + ','
                                                  + type_string_of<EmbeddingCodomain_>() + ','
                                                  + type_string_of<EmbeddedAbstractIndexType_>() + ','
                                                  + type_string_of<EmbeddingId_>() + '>';
    }

private:

    void operator = (ExpressionTemplate_IndexEmbed_t const &);

    IndexEmbedder m_embedder;
};

template <typename Operand_,
          typename SourceAbstractIndexType_,
          typename EmbeddingCodomain_,
          typename EmbeddedAbstractIndexType_,
          typename EmbeddingId_>
struct IsExpressionTemplate_f<ExpressionTemplate_IndexEmbed_t<Operand_,
                                                              SourceAbstractIndexType_,
                                                              EmbeddingCodomain_,
                                                              EmbeddedAbstractIndexType_,
                                                              EmbeddingId_> >
{
    static bool const V = true;
private:
    IsExpressionTemplate_f();
};

// ////////////////////////////////////////////////////////////////////////////
// coembedding single vector index into a single vector index for smaller space
// ////////////////////////////////////////////////////////////////////////////

// TODO: use a metafunction to define the parent class and clean this up
template <typename Operand_,
          typename SourceAbstractIndexType_,
          typename CoembeddingCodomain_,
          typename CoembeddedAbstractIndexType_,
          typename EmbeddingId_>
struct ExpressionTemplate_IndexCoembed_t
    :
    public ExpressionTemplate_IndexedObject_t<IndexCoembedder_t<Operand_,SourceAbstractIndexType_,CoembeddingCodomain_,CoembeddedAbstractIndexType_,EmbeddingId_>,
                                              typename IndexCoembedder_t<Operand_,SourceAbstractIndexType_,CoembeddingCodomain_,CoembeddedAbstractIndexType_,EmbeddingId_>::FactorTypeList,
                                              typename IndexCoembedder_t<Operand_,SourceAbstractIndexType_,CoembeddingCodomain_,CoembeddedAbstractIndexType_,EmbeddingId_>::DimIndexTypeList,
                                              typename SummedIndexTypeList_t<typename IndexCoembedder_t<Operand_,SourceAbstractIndexType_,CoembeddingCodomain_,CoembeddedAbstractIndexType_,EmbeddingId_>::DimIndexTypeList>::T,
                                              FORCE_CONST,
                                              CHECK_FOR_ALIASING, // irrelevant value
                                              ExpressionTemplate_IndexCoembed_t<Operand_,SourceAbstractIndexType_,CoembeddingCodomain_,CoembeddedAbstractIndexType_,EmbeddingId_> >
{
    enum
    {
        STATIC_ASSERT_IN_ENUM(IsAbstractIndex_f<SourceAbstractIndexType_>::V, MUST_BE_ABSTRACT_INDEX),
    };

    typedef ExpressionTemplate_IndexedObject_t<IndexCoembedder_t<Operand_,SourceAbstractIndexType_,CoembeddingCodomain_,CoembeddedAbstractIndexType_,EmbeddingId_>,
                                               typename IndexCoembedder_t<Operand_,SourceAbstractIndexType_,CoembeddingCodomain_,CoembeddedAbstractIndexType_,EmbeddingId_>::FactorTypeList,
                                               typename IndexCoembedder_t<Operand_,SourceAbstractIndexType_,CoembeddingCodomain_,CoembeddedAbstractIndexType_,EmbeddingId_>::DimIndexTypeList,
                                               typename SummedIndexTypeList_t<typename IndexCoembedder_t<Operand_,SourceAbstractIndexType_,CoembeddingCodomain_,CoembeddedAbstractIndexType_,EmbeddingId_>::DimIndexTypeList>::T,
                                               FORCE_CONST,
                                               CHECK_FOR_ALIASING, // irrelevant value
                                               ExpressionTemplate_IndexCoembed_t<Operand_,SourceAbstractIndexType_,CoembeddingCodomain_,CoembeddedAbstractIndexType_,EmbeddingId_> > Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeFactorTypeList FreeFactorTypeList;
    typedef typename Parent::FreeDimIndexTypeList FreeDimIndexTypeList;
    typedef typename Parent::UsedDimIndexTypeList UsedDimIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;
    typedef typename Parent::SummedDimIndexTypeList SummedDimIndexTypeList;

private:

    typedef IndexCoembedder_t<Operand_,SourceAbstractIndexType_,CoembeddingCodomain_,CoembeddedAbstractIndexType_,EmbeddingId_> IndexCoembedder;

public:

    // it's not great that m_coembedder is being passed to Parent before
    // m_coembedder is constructed, but Parent only stores a reference
    // to m_coembedder, so it doesn't really matter.
    ExpressionTemplate_IndexCoembed_t (Operand_ const &operand) : Parent(m_coembedder), m_coembedder(operand) { }

    operator Scalar () const
    {
        STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeDimIndexTypeList);
        return operator[](MultiIndex());
    }

    // read-only, because it doesn't make sense to assign to an index-bundled expression (which is possibly also a summation).
    Scalar operator [] (MultiIndex const &m) const
    {
        return UnarySummation_t<IndexCoembedder,typename IndexCoembedder::DimIndexTypeList,SummedDimIndexTypeList>::eval(m_coembedder, m);
    }

    bool overlaps_memory_range (Uint8 const *ptr, Uint32 range) const
    {
        return m_coembedder.overlaps_memory_range(ptr, range);
    }

    Operand_ const &operand () const { return m_coembedder.operand(); }

    static std::string type_as_string (bool verbose)
    {
        return "ExpressionTemplate_IndexCoembed_t<" + type_string_of<Operand_>() + ','
                                                    + type_string_of<SourceAbstractIndexType_>() + ','
                                                    + type_string_of<CoembeddingCodomain_>() + ','
                                                    + type_string_of<CoembeddedAbstractIndexType_>() + ','
                                                    + type_string_of<EmbeddingId_>() + '>';
    }

private:

    void operator = (ExpressionTemplate_IndexCoembed_t const &);

    IndexCoembedder m_coembedder;
};

template <typename Operand_,
          typename SourceAbstractIndexType_,
          typename CoembeddingCodomain_,
          typename CoembeddedAbstractIndexType_,
          typename EmbeddingId_>
struct IsExpressionTemplate_f<ExpressionTemplate_IndexCoembed_t<Operand_,
                                                                SourceAbstractIndexType_,
                                                                CoembeddingCodomain_,
                                                                CoembeddedAbstractIndexType_,
                                                                EmbeddingId_> >
{
    static bool const V = true;
private:
    IsExpressionTemplate_f();
};

// ////////////////////////////////////////////////////////////////////////////
// operator overloads for expression templates
// ////////////////////////////////////////////////////////////////////////////

// expression template addition/subtractions

// addition
template <typename LeftDerived, typename LeftFactorTypeList, typename LeftFreeDimIndexTypeList, typename LeftUsedDimIndexTypeList,
          typename RightDerived, typename RightFactorTypeList, typename RightFreeDimIndexTypeList, typename RightUsedDimIndexTypeList>
ExpressionTemplate_Addition_t<LeftDerived,RightDerived,'+'>
    operator + (ExpressionTemplate_i<LeftDerived,typename LeftDerived::Scalar,LeftFactorTypeList,LeftFreeDimIndexTypeList,LeftUsedDimIndexTypeList> const &left_operand,
                ExpressionTemplate_i<RightDerived,typename RightDerived::Scalar,RightFactorTypeList,RightFreeDimIndexTypeList,RightUsedDimIndexTypeList> const &right_operand)
{
    return ExpressionTemplate_Addition_t<LeftDerived,RightDerived,'+'>(left_operand.as_derived(), right_operand.as_derived());
}

// subtraction
template <typename LeftDerived, typename LeftFactorTypeList, typename LeftFreeDimIndexTypeList, typename LeftUsedDimIndexTypeList,
          typename RightDerived, typename RightFactorTypeList, typename RightFreeDimIndexTypeList, typename RightUsedDimIndexTypeList>
ExpressionTemplate_Addition_t<LeftDerived,RightDerived,'-'>
    operator - (ExpressionTemplate_i<LeftDerived,typename LeftDerived::Scalar,LeftFactorTypeList,LeftFreeDimIndexTypeList,LeftUsedDimIndexTypeList> const &left_operand,
                ExpressionTemplate_i<RightDerived,typename RightDerived::Scalar,RightFactorTypeList,RightFreeDimIndexTypeList,RightUsedDimIndexTypeList> const &right_operand)
{
    return ExpressionTemplate_Addition_t<LeftDerived,RightDerived,'-'>(left_operand.as_derived(), right_operand.as_derived());
}

// scalar multiplication/division, including unary negation (multiplication by -1)

// scalar multiplication on the right
template <typename Derived, typename FactorTypeList, typename FreeDimIndexTypeList, typename UsedDimIndexTypeList>
ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>
    operator * (ExpressionTemplate_i<Derived,typename Derived::Scalar,FactorTypeList,FreeDimIndexTypeList,UsedDimIndexTypeList> const &operand,
                typename Derived::Scalar const &scalar_operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>(operand.as_derived(), scalar_operand);
}

// scalar multiplication on the left
template <typename Derived, typename FactorTypeList, typename FreeDimIndexTypeList, typename UsedDimIndexTypeList>
ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>
    operator * (typename Derived::Scalar scalar_operand,
                ExpressionTemplate_i<Derived,typename Derived::Scalar,FactorTypeList,FreeDimIndexTypeList,UsedDimIndexTypeList> const &operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>(operand.as_derived(), scalar_operand);
}

// scalar division on the right
template <typename Derived, typename FactorTypeList, typename FreeDimIndexTypeList, typename UsedDimIndexTypeList, typename ScalarOperand_>
ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'/'>
    operator / (ExpressionTemplate_i<Derived,typename Derived::Scalar,FactorTypeList,FreeDimIndexTypeList,UsedDimIndexTypeList> const &operand,
                ScalarOperand_ const &scalar_operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'/'>(operand.as_derived(), scalar_operand);
}

// unary negation
template <typename Derived, typename FactorTypeList, typename FreeDimIndexTypeList, typename UsedDimIndexTypeList>
ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>
    operator - (ExpressionTemplate_i<Derived,typename Derived::Scalar,FactorTypeList,FreeDimIndexTypeList,UsedDimIndexTypeList> const &operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>(operand.as_derived(), -1);
}

// expression template multiplication -- tensor contraction and product

template <typename LeftDerived, typename LeftFactorTypeList, typename LeftFreeDimIndexTypeList, typename LeftUsedDimIndexTypeList,
          typename RightDerived, typename RightFactorTypeList, typename RightFreeDimIndexTypeList, typename RightUsedDimIndexTypeList>
ExpressionTemplate_Multiplication_t<LeftDerived,RightDerived>
    operator * (ExpressionTemplate_i<LeftDerived,typename LeftDerived::Scalar,LeftFactorTypeList,LeftFreeDimIndexTypeList,LeftUsedDimIndexTypeList> const &left_operand,
                ExpressionTemplate_i<RightDerived,typename RightDerived::Scalar,RightFactorTypeList,RightFreeDimIndexTypeList,RightUsedDimIndexTypeList> const &right_operand)
{
    return ExpressionTemplate_Multiplication_t<LeftDerived,RightDerived>(left_operand.as_derived(), right_operand.as_derived());
}

} // end of namespace Tenh

#endif // TENH_EXPRESSION_TEMPLATES_HPP_
