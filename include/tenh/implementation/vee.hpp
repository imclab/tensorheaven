// ///////////////////////////////////////////////////////////////////////////
// tenh/implementation/vee.hpp by Ted Nitz, created 2013/08/19
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_IMPLEMENTATION_VEE_HPP_
#define TENH_IMPLEMENTATION_VEE_HPP_

#include "tenh/core.hpp"

#include <functional>

#include "tenh/mathutil.hpp"
#include "tenh/conceptual/symmetricpower.hpp"
#include "tenh/implementation/implementationof.hpp"
#include "tenh/interface/embeddableastensor.hpp"
#include "tenh/tuple.hpp"

namespace Tenh {

// Factor_ should be a BasedVectorSpace_c type
template <Uint32 ORDER_, typename Factor_, typename Scalar_, typename UseArrayType_, typename Derived_>
struct ImplementationOf_t<SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_>,Scalar_,UseArrayType_,Derived_>
    :
    public EmbeddableAsTensor_i<typename DerivedType_f<Derived_,ImplementationOf_t<SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_>,Scalar_,UseArrayType_,Derived_>>::T,
                                Scalar_,
                                SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_>,
                                ComponentQualifierOfArrayType_f<UseArrayType_>::V>,
    // privately inherited because it is an implementation detail
    private ArrayStorage_f<Scalar_,
                           DimensionOf_f<SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_>>::V,
                           UseArrayType_,
                           typename DerivedType_f<Derived_,ImplementationOf_t<SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_>,Scalar_,UseArrayType_,Derived_>>::T >::T
{
    static_assert(HasBasedVectorSpaceStructure_f<Factor_>::V, "Factor_ must have unique based vector space structure");

    typedef EmbeddableAsTensor_i<typename DerivedType_f<Derived_,ImplementationOf_t<SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_>,Scalar_,UseArrayType_,Derived_>>::T,
                                 Scalar_,
                                 SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_>,
                                 ComponentQualifierOfArrayType_f<UseArrayType_>::V> Parent_EmbeddableAsTensor_i;
    typedef typename ArrayStorage_f<Scalar_,
                                    DimensionOf_f<SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_>>::V,
                                    UseArrayType_,
                                    typename DerivedType_f<Derived_,ImplementationOf_t<SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_>,Scalar_,UseArrayType_,Derived_>>::T >::T Parent_Array_i;

    typedef SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_> Concept;
    typedef UseArrayType_ UseArrayType;
    typedef typename Parent_EmbeddableAsTensor_i::Derived Derived;
    typedef typename Parent_EmbeddableAsTensor_i::Scalar Scalar;
    using Parent_EmbeddableAsTensor_i::DIM;
    typedef typename Parent_EmbeddableAsTensor_i::ComponentIndex ComponentIndex;
    using Parent_EmbeddableAsTensor_i::COMPONENT_QUALIFIER;
    typedef typename Parent_EmbeddableAsTensor_i::ComponentAccessConstReturnType ComponentAccessConstReturnType;
    typedef typename Parent_EmbeddableAsTensor_i::ComponentAccessNonConstReturnType ComponentAccessNonConstReturnType;
    typedef typename Parent_EmbeddableAsTensor_i::QualifiedComponent QualifiedComponent;

    typedef typename Parent_EmbeddableAsTensor_i::FactorTyple FactorTyple;
    typedef typename Parent_EmbeddableAsTensor_i::MultiIndex MultiIndex;
    static Uint32 const ORDER = ORDER_;
    typedef Factor_ Factor;

    typedef typename DualOf_f<ImplementationOf_t>::T Dual; // relies on the template specialization below

    typedef ImplementationOf_t<Concept,
                               Scalar_,
                               UseProceduralArray_t<typename ComponentGenerator_Constant_f<Scalar_,DIM,0>::T>,
                               Derived_> Zero;
    static Zero const ZERO;

    template <Uint32 INDEX_>
    struct BasisVector_f
    {
        static_assert(INDEX_ < Parent_EmbeddableAsTensor_i::DIM, "index out of range");
        typedef ImplementationOf_t<Concept,
                                   Scalar_,
                                   UseProceduralArray_t<typename ComponentGenerator_Characteristic_f<Scalar_,Parent_EmbeddableAsTensor_i::DIM,INDEX_>::T>,
                                   Derived_> T;
        static T const V;
    private:
        BasisVector_f () { }
    };

    explicit ImplementationOf_t (WithoutInitialization const &w) : Parent_Array_i(w) { }

    // only use these if UseMemberArray_t<...> is specified

    // similar to a copy constructor, except initializes from a Vector_i.
    // this was chosen to be explicit to avoid unnecessary copies.
    template <typename OtherDerived_, ComponentQualifier OTHER_COMPONENT_QUALIFIER_>
    explicit ImplementationOf_t (Vector_i<OtherDerived_,Scalar_,Concept,OTHER_COMPONENT_QUALIFIER_> const &x)
        :
        Parent_Array_i(Static<WithoutInitialization>::SINGLETON)
    {
        static_assert(IsUseMemberArray_f<UseArrayType_>::V, "UseArrayType_ must be a UseMemberArray_t type");
        // TODO: could make this use MemoryArray_i::copy_from (?)
        for (ComponentIndex i; i.is_not_at_end(); ++i)
            (*this)[i] = x[i];
    }
    // probably only useful for zero element (because this is basis-dependent)
    template <typename T_>
    explicit ImplementationOf_t (FillWith_t<T_> const &fill_with)
        :
        Parent_Array_i(fill_with)
    {
        static_assert(IsUseMemberArray_f<UseArrayType_>::V, "UseArrayType_ must be a UseMemberArray_t type");
    }
    // this is the tuple-based constructor
    template <typename Typle_>
    ImplementationOf_t (Tuple_t<Typle_> const &x)
        :
        Parent_Array_i(x.as_member_array())
    {
        static_assert(IsUseMemberArray_f<UseArrayType_>::V, "UseArrayType_ must be a UseMemberArray_t type");
    }

    // only use these if UsePreallocatedArray_t<...> is specified

    explicit ImplementationOf_t (QualifiedComponent *pointer_to_allocation, CheckPointer check_pointer = CheckPointer::TRUE)
        :
        Parent_Array_i(pointer_to_allocation, check_pointer)
    {
        static_assert(IsUsePreallocatedArray_f<UseArrayType_>::V, "UseArrayType_ must be a UsePreallocatedArray_t type");
    }
    // similar to a copy constructor, except initializes from a Vector_i.
    // this was chosen to be explicit to avoid unnecessary copies.
    template <typename OtherDerived_, ComponentQualifier OTHER_COMPONENT_QUALIFIER_>
    ImplementationOf_t (Vector_i<OtherDerived_,Scalar_,Concept,OTHER_COMPONENT_QUALIFIER_> const &x,
                        QualifiedComponent *pointer_to_allocation, CheckPointer check_pointer = CheckPointer::TRUE)
        :
        Parent_Array_i(pointer_to_allocation, check_pointer)
    {
        static_assert(IsUsePreallocatedArray_f<UseArrayType_>::V, "UseArrayType_ must be a UsePreallocatedArray_t type");
        // TODO: could make this use MemoryArray_i::copy_from (?)
        for (ComponentIndex i; i.is_not_at_end(); ++i)
            (*this)[i] = x[i];
    }
    template <typename T_>
    ImplementationOf_t (FillWith_t<T_> const &fill_with,
                        QualifiedComponent *pointer_to_allocation, CheckPointer check_pointer = CheckPointer::TRUE)
        :
        Parent_Array_i(fill_with, pointer_to_allocation, check_pointer)
    {
        static_assert(IsUsePreallocatedArray_f<UseArrayType_>::V, "UseArrayType_ must be a UsePreallocatedArray_t type");
    }
    // this is the tuple-based constructor
    template <typename Typle_>
    ImplementationOf_t (Tuple_t<Typle_> const &x,
                        QualifiedComponent *pointer_to_allocation, CheckPointer check_pointer = CheckPointer::TRUE)
        :
        Parent_Array_i(x, pointer_to_allocation, check_pointer)
    {
        static_assert(IsUsePreallocatedArray_f<UseArrayType_>::V, "UseArrayType_ must be a UsePreallocatedArray_t type");
    }

    // only use this if UseProceduralArray_t<...> is specified
    ImplementationOf_t ()
        :
        Parent_Array_i(WithoutInitialization()) // sort of meaningless constructor
    {
        static_assert(IsUseProceduralArray_f<UseArrayType_>::V || DIM == 0, "UseArrayType_ must be UseProceduralArray_t or space must be 0-dimensional");
    }

    template <typename BundleIndexTyple, typename BundledIndex>
    static MultiIndex_t<BundleIndexTyple> bundle_index_map (BundledIndex const &b)
    {
        // this constructor breaks the vector index apart into a row-major multi-index
        return BundleIndexComputer_t<BundleIndexTyple, BundledIndex, ORDER_>::compute(b);
    }

    using Parent_Array_i::as_derived;
    using Parent_Array_i::operator[];
    using Parent_Array_i::allocation_size_in_bytes;
    using Parent_Array_i::pointer_to_allocation;
    using Parent_Array_i::overlaps_memory_range;

    // TODO (maybe): Scalar &operator [] (MultiIndex const &) -- writeable access to components,
    // which would need to throw if the multi-index corresponds to an procedural zero.  there is
    // the complication that components with non-unit scale factors would need to be set with
    // a division by that scale factor.

    // these are what provide indexed expressions -- via expression templates
    using Parent_EmbeddableAsTensor_i::operator();

    static bool component_is_procedural_zero (MultiIndex const &m) { return false; }
    static Scalar scalar_factor_for_component (MultiIndex const &m) { return Scalar(1); }
    static ComponentIndex vector_index_of (MultiIndex const &m)
    {
        MultiIndex n = sorted<std::greater<Uint32>>(m);
        return ComponentIndex(VectorIndexComputer_t<MultiIndex>::compute(n), CheckRange::FALSE);
    }

private:

    template <typename BundleIndexTyple, typename BundledIndex, Uint32 ORD> struct BundleIndexComputer_t;
    template<typename T, typename I = int> struct VectorIndexComputer_t;
};

template <Uint32 ORDER_, typename Factor_, typename Scalar_, typename UseArrayType_, typename Derived_>
typename ImplementationOf_t<SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_>,Scalar_,UseArrayType_,Derived_>::Zero const ImplementationOf_t<SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_>,Scalar_,UseArrayType_,Derived_>::ZERO;

template <Uint32 ORDER_, typename Factor_, typename Scalar_, typename UseArrayType_, typename Derived_>
template <Uint32 INDEX_>
typename ImplementationOf_t<SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_>,Scalar_,UseArrayType_,Derived_>::template BasisVector_f<INDEX_>::T const ImplementationOf_t<SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_>,Scalar_,UseArrayType_,Derived_>::BasisVector_f<INDEX_>::V;

template <Uint32 ORDER_, typename Factor_, typename Scalar_, typename UseArrayType_, typename Derived_>
template <typename BundleIndexTyple, typename BundledIndex, Uint32 ORD>
struct ImplementationOf_t<SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_>,Scalar_,UseArrayType_,Derived_>::BundleIndexComputer_t
{
    static MultiIndex_t<BundleIndexTyple> compute (BundledIndex const &b)
    {
        typedef typename Head_f<BundleIndexTyple>::T BundleIndexHead;
        typedef typename BodyTyple_f<BundleIndexTyple>::T BundleIndexBodyTyple;
        return MultiIndex_t<BundleIndexTyple>(BundleIndexHead(index_of_greatest_simplicial_number_leq(b.value(),ORD) - ORD + 1, CheckRange::FALSE), BundleIndexComputer_t<BundleIndexBodyTyple,BundledIndex,ORD-1>::compute(BundledIndex(b.value() - binomial_coefficient(index_of_greatest_simplicial_number_leq(b.value(),ORD), ORD), CheckRange::FALSE)));
    }
};

template <Uint32 ORDER_, typename Factor_, typename Scalar_, typename UseArrayType_, typename Derived_>
template <typename FactorType, typename BundledIndex, Uint32 ORD>
struct ImplementationOf_t<SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_>,Scalar_,UseArrayType_,Derived_>::BundleIndexComputer_t<Typle_t<FactorType>, BundledIndex, ORD>
{
    static MultiIndex_t<Typle_t<FactorType>> compute (BundledIndex const &b)
    {
        return MultiIndex_t<Typle_t<FactorType>>(FactorType(index_of_greatest_simplicial_number_leq(b.value(),ORD) - ORD + 1, CheckRange::FALSE));
    }
};

template <Uint32 ORDER_, typename Factor_, typename Scalar_, typename UseArrayType_, typename Derived_>
template <typename T, typename I>
struct ImplementationOf_t<SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_>,Scalar_,UseArrayType_,Derived_>::VectorIndexComputer_t
{
    static Uint32 compute (T const &m)
    {
        return VectorIndexComputer_t<typename T::BodyMultiIndex>::compute(m.body()) + binomial_coefficient(m.head().value() + T::LENGTH - 1,T::LENGTH);
    }
};

template <Uint32 ORDER_, typename Factor_, typename Scalar_, typename UseArrayType_, typename Derived_>
template <typename I>
struct ImplementationOf_t<SymmetricPowerOfBasedVectorSpace_c<ORDER_,Factor_>,Scalar_,UseArrayType_,Derived_>::VectorIndexComputer_t<MultiIndex_t<Typle_t<>>, I>
{
    static Uint32 compute (MultiIndex_t<Typle_t<>> const &)
    {
        return 0;
    }
};

template <Uint32 ORDER, typename Factor, typename Scalar, typename UseArrayType_, typename Derived_>
struct DualOf_f<ImplementationOf_t<SymmetricPowerOfBasedVectorSpace_c<ORDER,Factor>,Scalar,UseArrayType_,Derived_>>
{
    typedef ImplementationOf_t<typename DualOf_f<SymmetricPowerOfBasedVectorSpace_c<ORDER,Factor>>::T,Scalar,typename DualOf_f<UseArrayType_>::T,typename DualOf_f<Derived_>::T> T;
};

} // end of namespace Tenh

#endif // TENH_IMPLEMENTATION_VEE_HPP_
