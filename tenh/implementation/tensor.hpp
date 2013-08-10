// ///////////////////////////////////////////////////////////////////////////
// tenh/implementation/tensor.hpp by Victor Dods, created 2013/08/10
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_IMPLEMENTATION_TENSOR_HPP_
#define TENH_IMPLEMENTATION_TENSOR_HPP_

#include "tenh/core.hpp"

#include "tenh/array.hpp"
#include "tenh/conceptual/tensorproduct.hpp"
#include "tenh/interface/tensor.hpp"

namespace Tenh {

template <typename Scalar, typename Space> struct ImplementationOf_t;

template <typename Scalar_, typename FactorTypeList_>
struct ImplementationOf_t<Scalar_,TensorProductOfBasedVectorSpaces_c<FactorTypeList_> >
    :
    public Tensor_i<ImplementationOf_t<Scalar_,TensorProductOfBasedVectorSpaces_c<FactorTypeList_> >,
                    Scalar_,
                    TensorProductOfBasedVectorSpaces_c<FactorTypeList_> >,
    // Array_t is privately inherited because it is an implementation detail
    private Array_t<Scalar_,TensorProductOfBasedVectorSpaces_c<FactorTypeList_>::DIM>
{
    typedef Tensor_i<ImplementationOf_t<Scalar_,TensorProductOfBasedVectorSpaces_c<FactorTypeList_> >,
                     Scalar_,
                     TensorProductOfBasedVectorSpaces_c<FactorTypeList_> > Parent_Tensor_i;
    typedef Array_t<Scalar_,TensorProductOfBasedVectorSpaces_c<FactorTypeList_>::DIM> Parent_Array_t;

    typedef typename Parent_Tensor_i::Derived Derived;
    typedef typename Parent_Tensor_i::Scalar Scalar;
    typedef typename Parent_Tensor_i::BasedVectorSpace BasedVectorSpace;
    using Parent_Tensor_i::DIM;
    typedef typename Parent_Tensor_i::Basis Basis;
    typedef typename Parent_Tensor_i::ComponentIndex ComponentIndex;

    typedef typename Parent_Tensor_i::TensorProductOfBasedVectorSpaces TensorProductOfBasedVectorSpaces;
    typedef typename Parent_Tensor_i::FactorTypeList FactorTypeList;
    typedef typename Parent_Tensor_i::MultiIndex MultiIndex;
    using Parent_Tensor_i::DEGREE;
    using Parent_Tensor_i::IS_TENSOR_I;    

    typedef typename DualOf_c<ImplementationOf_t>::T Dual; // relies on the template specialization below

    ImplementationOf_t (WithoutInitialization const &w) : Parent_Array_t(w) { }
    ImplementationOf_t (Scalar fill_with) : Parent_Array_t(fill_with) { }

    // TODO: do later
    // template <typename BundleIndexTypeList, typename BundledIndex>
    // static MultiIndex_t<BundleIndexTypeList> bundle_index_map (BundledIndex const &b)
    // {
    //     typedef typename BundleIndexTypeList::template El_t<0>::T Index1;
    //     typedef typename BundleIndexTypeList::template El_t<1>::T Index2;
    //     // this is just to check that there is a valid conversion to the requested MultiIndex type.
    //     // it doesn't actually produce any side-effects, and should be optimized out.
    //     {
    //         STATIC_ASSERT((BundleIndexTypeList::LENGTH == 2), CAN_ONLY_BUNDLE_TWO_INDICES);
    //         Index1 i1;
    //         Index2 i2;
    //         typename Factor1::Index f1(i1);
    //         typename Factor2::Index f2(i2);
    //         // check that the parameter BundleIndex type is compatible with Index
    //         Index i(b);
    //     }

    //     Uint32 row;
    //     Uint32 col;
    //     contiguous_index_to_rowcol_index(b.value(), row, col);
    //     return MultiIndex_t<BundleIndexTypeList>(Index1(row, DONT_CHECK_RANGE), Index2(col, DONT_CHECK_RANGE));
    // }

    using Parent_Array_t::operator[];
    using Parent_Array_t::data_size_in_bytes;
    using Parent_Array_t::data_pointer;

    // // Index1 could be Factor1::Index or Factor1::MultiIndex (checked by its use in the other functions)
    // // Index2 could be Factor2::Index or Factor2::MultiIndex (checked by its use in the other functions)
    // template <typename Index1, typename Index2>
    // Scalar operator [] (MultiIndex_t<TypeList_t<Index1,TypeList_t<Index2> > > const &m) const
    // {
    //     return component(m.template el<0>(), m.template el<1>());
    // }
    // template <typename Index1, typename Index2>
    // Scalar &operator [] (MultiIndex_t<TypeList_t<Index1,TypeList_t<Index2> > > const &m)
    // {
    //     assert(m.is_not_at_end() && "you used Index_t(x, DONT_RANGE_CHECK) inappropriately");
    //     // NOTE: this construction is unnecessary to the code, but IS necessary to the compile-time type checking
    //     // the compiler should optimize it out anyway.
    //     MultiIndex x(m);
    //     return operator[](Index(m.value(), DONT_CHECK_RANGE));
    // }
    template <typename OtherIndexTypeList>
    Scalar operator [] (MultiIndex_t<OtherIndexTypeList> const &m) const
    {
        STATIC_ASSERT(IsATypeList_t<OtherIndexTypeList>::V, MUST_BE_TYPELIST);
        typedef MultiIndex_t<OtherIndexTypeList> OtherMultiIndex;
        STATIC_ASSERT((OtherMultiIndex::LENGTH == MultiIndex::LENGTH), MUST_HAVE_EQUAL_LENGTHS);
        //std::cout << OtherMultiIndex::LENGTH << ", " << MultiIndex::LENGTH << '\n';
        assert(m.is_not_at_end() && "you used ComponentIndex_t(x, DONT_RANGE_CHECK) inappropriately");
        // NOTE: this construction is unnecessary to the code, but IS necessary to the compile-time type checking
        // the compiler should optimize it out anyway.
        MultiIndex x(m);
        // m.value() is what does the multi-index-to-vector-index computation
        return operator[](ComponentIndex(m.value(), DONT_CHECK_RANGE));
    }
    template <typename OtherIndexTypeList>
    Scalar &operator [] (MultiIndex_t<OtherIndexTypeList> const &m)
    {
        STATIC_ASSERT(IsATypeList_t<OtherIndexTypeList>::V, MUST_BE_TYPELIST);
        typedef MultiIndex_t<OtherIndexTypeList> OtherMultiIndex;
        STATIC_ASSERT((OtherMultiIndex::LENGTH == MultiIndex::LENGTH), MUST_HAVE_EQUAL_LENGTHS);
        //std::cout << OtherMultiIndex::LENGTH << ", " << MultiIndex::LENGTH << '\n';
        assert(m.is_not_at_end() && "you used ComponentIndex_t(x, DONT_RANGE_CHECK) inappropriately");
        // NOTE: this construction is unnecessary to the code, but IS necessary to the compile-time type checking
        // the compiler should optimize it out anyway.
        MultiIndex x(m);
        // m.value() is what does the multi-index-to-vector-index computation
        return operator[](ComponentIndex(m.value(), DONT_CHECK_RANGE));
        // std::cout << TypeStringOf_t<OtherMultiIndex>::eval() << ", " << TypeStringOf_t<MultiIndex>::eval() << '\n';
        // return Scalar(0);
    }

    // these are what provide indexed expressions -- via expression templates
    using Parent_Tensor_i::operator();

/*
    // access 2-tensor components
    // Index1 could be Factor1::Index or Factor1::MultiIndex (checked by its use in the other functions)
    // Index2 could be Factor2::Index or Factor2::MultiIndex (checked by its use in the other functions)
    template <typename Index1, typename Index2>
    Scalar component (Index1 const &i1, Index2 const &i2) const
    {
        assert(i1.is_not_at_end() && i2.is_not_at_end() && "you used Index_t(x, DONT_RANGE_CHECK) inappropriately");

        if (Factor1::component_is_immutable_zero(i1) || Factor2::component_is_immutable_zero(i2))
        {
            return Scalar(0);
        }
        else
        {
            return Factor1::scalar_factor_for_component(i1) *
                   Factor2::scalar_factor_for_component(i2) *
                   operator[](vector_index_of(MultiIndex(Factor1::vector_index_of(i1), Factor2::vector_index_of(i2))));
        }
    }
    // write 2-tensor components -- will throw if a component doesn't correspond to a memory location
    // Index1 could be Factor1::Index or Factor1::MultiIndex (checked by its use in the other functions)
    // Index2 could be Factor2::Index or Factor2::MultiIndex (checked by its use in the other functions)
    template <typename Index1, typename Index2>
    void set_component (Index1 const &i1, Index2 const &i2, Scalar s)
    {
        assert(i1.is_not_at_end() && i2.is_not_at_end() && "you used Index_t(x, DONT_RANGE_CHECK) inappropriately");

        if (Factor1::component_is_immutable_zero(i1) || Factor2::component_is_immutable_zero(i2))
        {
            throw std::invalid_argument("this tensor component is not writable");
        }

        MultiIndex m(Factor1::vector_index_of(i1), Factor2::vector_index_of(i2));
        // write to the component, but divide through by the total scale factor for the component.
        operator[](vector_index_of(m)) = s / (Factor1::scalar_factor_for_component(i1) * Factor2::scalar_factor_for_component(i2));
    }
    */
    using Parent_Tensor_i::component_is_immutable_zero;
    using Parent_Tensor_i::scalar_factor_for_component;
    using Parent_Tensor_i::vector_index_of;
    // all components are stored in memory (in the array m), and have scalar factor 1
    static bool component_is_immutable_zero (MultiIndex const &m) { return false; }
    static Scalar scalar_factor_for_component (MultiIndex const &m) { return Scalar(1); }
    static ComponentIndex vector_index_of (MultiIndex const &m) { return ComponentIndex(m.value(), DONT_CHECK_RANGE); }

    static std::string type_as_string ()
    {
        return "ImplementationOf_t<" + TypeStringOf_t<Scalar>::eval() + ',' + TypeStringOf_t<TensorProductOfBasedVectorSpaces>::eval() + '>';
    }

private:

/*
    // functions between the indexing schemes -- multi-index is (row,col) with row > col and vector index is contiguous.
    static Uint32 rowcol_index_to_contiguous_index (Uint32 row, Uint32 col)
    {
        return Factor2::DIM*row + col;
    }
    static void contiguous_index_to_rowcol_index (Uint32 i, Uint32 &row, Uint32 &col)
    {
        row = i / Factor2::DIM;
        col = i % Factor2::DIM;
    }

    friend struct InnerProduct_t<Tensor2_t,Basis>;
*/
    // this has no definition, and is designed to generate a compiler error if used (use the one accepting WithoutInitialization instead).
    ImplementationOf_t ();
};

template <typename Scalar, typename FactorTypeList>
struct DualOf_c<ImplementationOf_t<Scalar,TensorProductOfBasedVectorSpaces_c<FactorTypeList> > >
{
    typedef ImplementationOf_t<Scalar,typename TensorProductOfBasedVectorSpaces_c<FactorTypeList>::Dual> T;
};

} // end of namespace Tenh

#endif // TENH_IMPLEMENTATION_TENSOR_HPP_
