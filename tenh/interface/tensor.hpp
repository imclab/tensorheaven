// ///////////////////////////////////////////////////////////////////////////
// tenh/interface/tensor.hpp by Victor Dods, created 2013/04/05
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_INTERFACE_TENSOR_HPP_
#define TENH_INTERFACE_TENSOR_HPP_

#include <ostream>

#include "tenh/core.hpp"
#include "tenh/expression_templates.hpp"
#include "tenh/index.hpp"
#include "tenh/interface/vector.hpp"

namespace Tenh {

template <typename FactorTypeList>
struct AllFactorTypeScalarsAreEqual_t
{
    static bool const V = Lvd::Meta::TypesAreEqual<typename FactorTypeList::HeadType::Scalar,
                                                   typename FactorTypeList::BodyTypeList::HeadType::Scalar>::v &&
                          AllFactorTypeScalarsAreEqual_t<typename FactorTypeList::BodyTypeList>::V;
};

template <typename Factor1, typename Factor2>
struct AllFactorTypeScalarsAreEqual_t<TypeList_t<Factor1,TypeList_t<Factor2> > >
{
    static bool const V = Lvd::Meta::TypesAreEqual<typename Factor1::Scalar,typename Factor2::Scalar>::v;
};

template <typename Factor>
struct AllFactorTypeScalarsAreEqual_t<TypeList_t<Factor> >
{
    static bool const V = true;
};

template <>
struct AllFactorTypeScalarsAreEqual_t<EmptyTypeList>
{
    static bool const V = true;
};

template <typename FactorTypeList>
struct FactorIndexTypeList_t
{
    typedef TypeList_t<typename FactorTypeList::HeadType::Index,typename FactorIndexTypeList_t<typename FactorTypeList::BodyTypeList>::T> T;
};

template <typename HeadType>
struct FactorIndexTypeList_t<TypeList_t<HeadType> >
{
    typedef TypeList_t<typename HeadType::Index> T;
};

template <>
struct FactorIndexTypeList_t<EmptyTypeList>
{
    typedef EmptyTypeList T;
};

// compile-time interface for a tensor product class.  the factors should each
// be a vector space type (and NOT for example an affine space type or other non-
// vector space type) -- the tensor product is only defined on vector spaces
template <typename Derived_, typename FactorTypeList_, Uint32 DIM_, typename Basis_>
struct Tensor_i : public Vector_i<Derived_,typename FactorTypeList_::HeadType::Scalar,DIM_,Basis_>
{
    enum
    {
        STATIC_ASSERT_IN_ENUM((!Lvd::Meta::TypesAreEqual<Derived_,NullType>::v), DERIVED_MUST_NOT_BE_NULL_TYPE),
        STATIC_ASSERT_IN_ENUM((AllFactorTypeScalarsAreEqual_t<FactorTypeList_>::V), ALL_FACTOR_TYPE_SCALARS_ARE_EQUAL),
        STATIC_ASSERT_IN_ENUM((FactorTypeList_::LENGTH > 0), FACTOR_TYPE_LIST_IS_NONEMPTY)
    };

    typedef Vector_i<Derived_,typename FactorTypeList_::HeadType::Scalar,DIM_,Basis_> Parent_Vector_i;
    typedef typename Parent_Vector_i::Derived Derived;
    typedef typename Parent_Vector_i::Scalar Scalar;
    using Parent_Vector_i::DIM;
    typedef typename Parent_Vector_i::Basis Basis;
    typedef typename Parent_Vector_i::Index Index;

    typedef FactorTypeList_ FactorTypeList;
    typedef typename FactorIndexTypeList_t<FactorTypeList>::T FactorIndexTypeList;
    typedef MultiIndex_t<FactorIndexTypeList> MultiIndex;
    // this is not the "fully expanded" degree, but the number of [what you could think of
    // as "parenthesized"] factors that formed this tensor product type.
    static Uint32 const DEGREE = FactorTypeList::LENGTH;

    // TODO: could put canonical as_factorX conversions here

    using Parent_Vector_i::as_derived;
    using Parent_Vector_i::operator[];
    Scalar operator [] (MultiIndex const &m) const { return as_derived().Derived::operator[](m); }
    Scalar operator [] (MultiIndex const &m) { return as_derived().Derived::operator[](m); }

    using Parent_Vector_i::operator();
    // the two separate head/body template arguments are necessary to disambiguate this method
    // from one that takes a single index (i.e. the index-by-vector-index one).
    template <typename IndexTypeListHeadType, typename IndexTypeListBodyTypeList>
    ExpressionTemplate_IndexedObject_t<
        Derived,
        TypeList_t<IndexTypeListHeadType,IndexTypeListBodyTypeList>,
        typename SummedIndexTypeList_t<TypeList_t<IndexTypeListHeadType,IndexTypeListBodyTypeList> >::T,
        FORCE_CONST,
        CHECK_FOR_ALIASING> operator () (TypeList_t<IndexTypeListHeadType,IndexTypeListBodyTypeList> const &) const
    {
        typedef TypeList_t<IndexTypeListHeadType,IndexTypeListBodyTypeList> ArgumentIndexTypeList;
        // ensure the length matches FactorIndexTypeList
        STATIC_ASSERT((ArgumentIndexTypeList::LENGTH == FactorIndexTypeList::LENGTH), ARGUMENT_LENGTH_MUST_EQUAL_FACTOR_LENGTH);
        // make sure that each type in the index list is actually a TypedIndex_t
        AssertThatEachTypeIsATypedIndex_t<ArgumentIndexTypeList>();
        // make sure there are type conversions for all types in the index lists
        compile_time_check_that_there_is_a_type_conversion(ArgumentIndexTypeList(), FactorIndexTypeList());
        // TEMP KLUDGE - forbit contraction with this tensor if its basis is StandardEuclideanBasis,
        // because its components are then not necessarily those that should be contracted with --
        // they would be some tensor-implementation-dependent scalar multiples of the components,
        // and this particular capability is too complicated to implement currently.
        STATIC_ASSERT((!Lvd::Meta::TypesAreEqual<Basis,StandardEuclideanBasis>::v), CANT_CONTRACT_WITH_EUCLIDEANLY_EMBEDDED_TENSOR);
        return ExpressionTemplate_IndexedObject_t<
            Derived,
            TypeList_t<IndexTypeListHeadType,IndexTypeListBodyTypeList>,
            typename SummedIndexTypeList_t<TypeList_t<IndexTypeListHeadType,IndexTypeListBodyTypeList> >::T,
            FORCE_CONST,
            CHECK_FOR_ALIASING>(as_derived());
    }
    template <typename IndexTypeListHeadType, typename IndexTypeListBodyTypeList>
    ExpressionTemplate_IndexedObject_t<
        Derived,
        TypeList_t<IndexTypeListHeadType,IndexTypeListBodyTypeList>,
        typename SummedIndexTypeList_t<TypeList_t<IndexTypeListHeadType,IndexTypeListBodyTypeList> >::T,
        DONT_FORCE_CONST,
        CHECK_FOR_ALIASING> operator () (TypeList_t<IndexTypeListHeadType,IndexTypeListBodyTypeList> const &)
    {
        typedef TypeList_t<IndexTypeListHeadType,IndexTypeListBodyTypeList> ArgumentIndexTypeList;
        // ensure the length matches FactorIndexTypeList
        STATIC_ASSERT((ArgumentIndexTypeList::LENGTH == FactorIndexTypeList::LENGTH), ARGUMENT_LENGTH_MUST_EQUAL_FACTOR_LENGTH);
        // make sure that each type in the index list is actually a TypedIndex_t
        AssertThatEachTypeIsATypedIndex_t<ArgumentIndexTypeList>();
        // make sure there are type conversions for all types in the index lists
        compile_time_check_that_there_is_a_type_conversion(ArgumentIndexTypeList(), FactorIndexTypeList());
        // TEMP KLUDGE - forbit contraction with this tensor if its basis is StandardEuclideanBasis,
        // because its components are then not necessarily those that should be contracted with --
        // they would be some tensor-implementation-dependent scalar multiples of the components,
        // and this particular capability is too complicated to implement currently.
        STATIC_ASSERT((!Lvd::Meta::TypesAreEqual<Basis,StandardEuclideanBasis>::v), CANT_CONTRACT_WITH_EUCLIDEANLY_EMBEDDED_TENSOR);
        return ExpressionTemplate_IndexedObject_t<
            Derived,
            TypeList_t<IndexTypeListHeadType,IndexTypeListBodyTypeList>,
            typename SummedIndexTypeList_t<TypeList_t<IndexTypeListHeadType,IndexTypeListBodyTypeList> >::T,
            DONT_FORCE_CONST,
            CHECK_FOR_ALIASING>(as_derived());
    }

    using Parent_Vector_i::component_is_immutable_zero;
    using Parent_Vector_i::scalar_factor_for_component;
    using Parent_Vector_i::vector_index_of;

    static std::string type_as_string ()
    {
        return "Tensor_i<" + TypeStringOf_t<Derived>::eval() + ',' + TypeStringOf_t<FactorTypeList>::eval() + '>';
    }
};

// TODO: figure out the maximum width any particular element takes up (or do this by column)
// and use iomanip::setw to do really nice formatting that isn't the hackyness of using tabs.

// specialization for 1-tensors
template <typename Derived, typename Factor1, Uint32 DIM, typename Basis>
std::ostream &operator << (std::ostream &out, Tensor_i<Derived,TypeList_t<Factor1>,DIM,Basis> const &t)
{
    typedef Tensor_i<Derived,TypeList_t<Factor1>,DIM,Basis> Tensor;

    // determine the max size that a component takes up
    Uint32 max_component_width = 0;
    for (typename Tensor::MultiIndex m; m.is_not_at_end(); ++m)
    {
        std::ostringstream sout;
        sout << t[m];
        if (sout.str().length() > max_component_width)
            max_component_width = sout.str().length();
    }

    out << "[ ";
    for (typename Tensor::MultiIndex m; m.is_not_at_end(); ++m)
    {
        out.setf(std::ios_base::right);
        out.width(max_component_width);
        out << t[m] << ' ';
    }
    return out << ']';
}

// specialization for 2-tensors
template <typename Derived, typename Factor1, typename Factor2, Uint32 DIM, typename Basis>
std::ostream &operator << (std::ostream &out, Tensor_i<Derived,TypeList_t<Factor1,TypeList_t<Factor2> >,DIM,Basis> const &t)
{
    typedef Tensor_i<Derived,TypeList_t<Factor1,TypeList_t<Factor2> >,DIM,Basis> Tensor;
    
    // determine the max size that a component takes up
    Uint32 max_component_width = 0;
    for (typename Tensor::MultiIndex m; m.is_not_at_end(); ++m)
    {
        std::ostringstream sout;
        sout << t[m];
        if (sout.str().length() > max_component_width)
            max_component_width = sout.str().length();
    }

    typename Tensor::MultiIndex m;
    if (m.is_at_end())
        return out << "\n[[ ]]\n";

    out << "\n[";
    for (typename Factor1::Index i; i.is_not_at_end(); ++i)
    {
        if (i.value() != 0)
            out << ' ';
        out << "[ ";
        for (typename Factor2::Index j; j.is_not_at_end(); ++j)
        {
            out.setf(std::ios_base::right);
            out.width(max_component_width);
            out << t[m] << ' ';
            ++m;
        }
        out << ']';
        typename Factor1::Index next(i);
        ++next;
        if (next.is_not_at_end())
            out << '\n';
    }
    return out << "]\n";
}

// specialization for 3-tensors
template <typename Derived, typename Factor1, typename Factor2, typename Factor3, Uint32 DIM, typename Basis>
std::ostream &operator << (std::ostream &out, Tensor_i<Derived,TypeList_t<Factor1,TypeList_t<Factor2,TypeList_t<Factor3> > >,DIM,Basis> const &t)
{
    typedef Tensor_i<Derived,TypeList_t<Factor1,TypeList_t<Factor2,TypeList_t<Factor3> > >,DIM,Basis> Tensor;

    // determine the max size that a component takes up
    Uint32 max_component_width = 0;
    for (typename Tensor::MultiIndex m; m.is_not_at_end(); ++m)
    {
        std::ostringstream sout;
        sout << t[m];
        if (sout.str().length() > max_component_width)
            max_component_width = sout.str().length();
    }
    
    typename Tensor::MultiIndex m;
    if (m.is_at_end())
        return out << "\n[[[ ]]]\n";

    out << "\n[";
    for (typename Factor1::Index i; i.is_not_at_end(); ++i)
    {
        if (i.value() != 0)
            out << ' ';
        out << '[';
        for (typename Factor2::Index j; j.is_not_at_end(); ++j)
        {
            if (j.value() != 0)
                out << "  ";
            out << "[ ";
            for (typename Factor3::Index k; k.is_not_at_end(); ++k)
            {
                out.setf(std::ios_base::right);
                out.width(max_component_width);
                out << t[m] << ' ';
                ++m;
            }
            out << ']';
            typename Factor2::Index next(j);
            ++next;
            if (next.is_not_at_end())
                out << '\n';
        }
        out << ']';
        typename Factor1::Index next(i);
        ++next;
        if (next.is_not_at_end())
            out << "\n\n";
    }
    return out << "]\n";
}

// TODO: implement recursive operator << for arbitrary degree tensors

} // end of namespace Tenh

#endif // TENH_INTERFACE_TENSOR_HPP_
