// ///////////////////////////////////////////////////////////////////////////
// tenh/implementation/wedge.hpp by Victor Dods, created 2013/08/15
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_IMPLEMENTATION_WEDGE_HPP_
#define TENH_IMPLEMENTATION_WEDGE_HPP_

#include "tenh/core.hpp"

#include "tenh/array.hpp"
#include "tenh/mathutil.hpp"
#include "tenh/conceptual/exteriorpower.hpp"
#include "tenh/interface/embeddableastensor.hpp"

namespace Tenh {

template <typename Scalar, typename Space> struct ImplementationOf_t;

// Factor_ should be a BasedVectorSpace_c type
template <typename Scalar_, typename Factor_, Uint32 ORDER_>
struct ImplementationOf_t<Scalar_,ExteriorPowerOfBasedVectorSpaces_c<Factor_,ORDER_> >
    :
    public EmbeddableAsTensor_i<ImplementationOf_t<Scalar_,ExteriorPowerOfBasedVectorSpaces_c<Factor_,ORDER_> >,
                                Scalar_,
                                ExteriorPowerOfBasedVectorSpaces_c<Factor_,ORDER_> >,
    // Array_t is privately inherited because it is an implementation detail
    private Array_t<Scalar_,ExteriorPowerOfBasedVectorSpaces_c<Factor_,ORDER_>::DIM>
{
    enum { STATIC_ASSERT_IN_ENUM(IsABasedVectorSpace_c<Factor_>::V, MUST_BE_BASED_VECTOR_SPACE) };

    typedef EmbeddableAsTensor_i<ImplementationOf_t<Scalar_,ExteriorPowerOfBasedVectorSpaces_c<Factor_,ORDER_> >,
                                 Scalar_,
                                 ExteriorPowerOfBasedVectorSpaces_c<Factor_,ORDER_> > Parent_EmbeddableAsTensor_i;
    typedef Array_t<Scalar_,ExteriorPowerOfBasedVectorSpaces_c<Factor_,ORDER_>::DIM> Parent_Array_t;

    typedef typename Parent_EmbeddableAsTensor_i::Derived Derived;
    typedef typename Parent_EmbeddableAsTensor_i::Scalar Scalar;
    typedef typename Parent_EmbeddableAsTensor_i::BasedVectorSpace BasedVectorSpace;
    using Parent_EmbeddableAsTensor_i::DIM;
    typedef typename Parent_EmbeddableAsTensor_i::ComponentIndex ComponentIndex;

    typedef typename Parent_EmbeddableAsTensor_i::TensorProductOfBasedVectorSpaces TensorProductOfBasedVectorSpaces;
    typedef typename Parent_EmbeddableAsTensor_i::FactorTypeList FactorTypeList;
    typedef typename Parent_EmbeddableAsTensor_i::MultiIndex MultiIndex;
    using Parent_EmbeddableAsTensor_i::DEGREE;
    static Uint32 const ORDER = ORDER_;
    typedef Factor_ Factor;
    typedef ExteriorPowerOfBasedVectorSpaces_c<Factor,ORDER> ExteriorPowerOfBasedVectorSpaces;

    typedef typename DualOf_c<ImplementationOf_t>::T Dual; // relies on the template specialization below

    explicit ImplementationOf_t (WithoutInitialization const &w) : Parent_Array_t(w) { }
    // probably only useful for zero element
    explicit ImplementationOf_t (Scalar fill_with) : Parent_Array_t(fill_with) { }

    template <typename BundleIndexTypeList, typename BundledIndex>
    static MultiIndex_t<BundleIndexTypeList> bundle_index_map (BundledIndex const &b)
    {
        STATIC_ASSERT(IsADimIndex_t<BundledIndex>::V, MUST_BE_COMPONENT_INDEX);
        // this constructor breaks the vector index apart into a row-major multi-index
        return compute_bundle_index<BundleIndexTypeList, BundledIndex, ORDER>(b);
    }

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

    // these are what provide indexed expressions -- via expression templates
    using Parent_EmbeddableAsTensor_i::operator();

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
    // using Parent_EmbeddableAsTensor_i::component_is_immutable_zero;
    // using Parent_EmbeddableAsTensor_i::scalar_factor_for_component;
    // using Parent_EmbeddableAsTensor_i::vector_index_of;
    // all components are stored in memory (in the array m), and have scalar factor 1
    // TODO: this should probably go into the conceptual layer

    // TODO: Ted -- here is where the other stuff goes
    static bool component_is_immutable_zero (MultiIndex const &m) { return false; }
    static Scalar scalar_factor_for_component (MultiIndex const &m) { return Scalar(1/Factorial_t<ORDER>::V); }
    static ComponentIndex vector_index_of (MultiIndex const &m) {
      // TODO: Sort m descending!
      return ComponentIndex(compute_vector_index(m), DONT_CHECK_RANGE); }

    static std::string type_as_string ()
    {
        return "ImplementationOf_t<" + TypeStringOf_t<Scalar>::eval() + ',' + TypeStringOf_t<ExteriorPowerOfBasedVectorSpaces>::eval() + '>';
    }

private:

    // this has no definition, and is designed to generate a compiler error if used (use the one accepting WithoutInitialization instead).
    ImplementationOf_t ();

    template <typename BundleIndexTypeList, typename BundledIndex, Uint32 ORD>
    static MultiIndex_t<BundleIndexTypeList> compute_bundle_index (BundledIndex const &b)
    {
        if (BundleIndexTypeList::LENGTH == 1)
        {
            return MultiIndex_t<BundleIndexTypeList>(index_of_greatest_triangular_number_less_than(b.value(),ORD));
        }
        else
        {
            return MultiIndex_t<BundleIndexTypeList>(index_of_greatest_triangular_number_less_than(b.value(),ORD), bundle_index_map<BundleIndexTypeList::BodyType, BundledIndex,ORD-1>(b.value() - binomial_coefficient(index_of_greatest_triangular_number_less_than(b.value(),ORD), ORD)));
        }
    }

    template<typename T>
    static Uint32 compute_vector_index (T const &m)
    {
      if (T::LENGTH > 1)
      {
        return compute_vector_index(m.body()) + binomial_coefficient(m.head().value(),T::LENGTH);
      }
      else
      {
        return m.head().value();
      }
    }

};

template <typename Scalar, typename Factor, Uint32 ORDER>
struct DualOf_c<ImplementationOf_t<Scalar,ExteriorPowerOfBasedVectorSpaces_c<Factor,ORDER> > >
{
    typedef ImplementationOf_t<Scalar,typename DualOf_c<ExteriorPowerOfBasedVectorSpaces_c<Factor,ORDER> >::T> T;
};

} // end of namespace Tenh

#endif // TENH_IMPLEMENTATION_WEDGE_HPP_
