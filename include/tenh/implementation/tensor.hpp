// ///////////////////////////////////////////////////////////////////////////
// tenh/implementation/tensor.hpp by Victor Dods, created 2013/08/10
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_IMPLEMENTATION_TENSOR_HPP_
#define TENH_IMPLEMENTATION_TENSOR_HPP_

#include "tenh/core.hpp"

#include "tenh/conceptual/tensorproduct.hpp"
#include "tenh/implementation/implementationof.hpp"
#include "tenh/interface/tensor.hpp"
#include "tenh/tuple.hpp"

namespace Tenh {

template <typename FactorTyple_, typename Scalar_, typename UseArrayType_, typename Derived_>
struct ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<FactorTyple_>,Scalar_,UseArrayType_,Derived_>
    :
    public Tensor_i<typename DerivedType_f<Derived_,ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<FactorTyple_>,Scalar_,UseArrayType_,Derived_>>::T,
                    Scalar_,
                    TensorProductOfBasedVectorSpaces_c<FactorTyple_>,
                    ComponentQualifierOfArrayType_f<UseArrayType_>::V>,
    // privately inherited because it is an implementation detail
    private ArrayStorage_f<Scalar_,
                           DimensionOf_f<TensorProductOfBasedVectorSpaces_c<FactorTyple_>>::V,
                           UseArrayType_,
                           typename DerivedType_f<Derived_, ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<FactorTyple_>,Scalar_,UseArrayType_,Derived_>>::T >::T
{
    typedef Tensor_i<typename DerivedType_f<Derived_,ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<FactorTyple_>,Scalar_,UseArrayType_,Derived_>>::T,
                     Scalar_,
                     TensorProductOfBasedVectorSpaces_c<FactorTyple_>,
                     ComponentQualifierOfArrayType_f<UseArrayType_>::V> Parent_Tensor_i;
    typedef typename ArrayStorage_f<Scalar_,
                                    DimensionOf_f<TensorProductOfBasedVectorSpaces_c<FactorTyple_>>::V,
                                    UseArrayType_,
                                    typename DerivedType_f<Derived_, ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<FactorTyple_>,Scalar_,UseArrayType_,Derived_>>::T >::T Parent_Array_i;

    typedef TensorProductOfBasedVectorSpaces_c<FactorTyple_> Concept;
    typedef UseArrayType_ UseArrayType;
    typedef typename Parent_Tensor_i::Derived Derived;
    typedef typename Parent_Tensor_i::Scalar Scalar;
    using Parent_Tensor_i::DIM;
    typedef typename Parent_Tensor_i::ComponentIndex ComponentIndex;

    typedef typename Parent_Tensor_i::FactorTyple FactorTyple;
    typedef typename Parent_Tensor_i::MultiIndex MultiIndex;
    using Parent_Tensor_i::ORDER;
    using Parent_Tensor_i::IS_TENSOR_I;

    using Parent_Array_i::COMPONENT_QUALIFIER;
    typedef typename Parent_Array_i::ComponentAccessConstReturnType ComponentAccessConstReturnType;
    typedef typename Parent_Array_i::ComponentAccessNonConstReturnType ComponentAccessNonConstReturnType;
    typedef typename Parent_Array_i::QualifiedComponent QualifiedComponent;

    typedef typename DualOf_f<ImplementationOf_t>::T Dual; // relies on the template specialization below

    typedef ImplementationOf_t<Concept,
                               Scalar_,
                               UseProceduralArray_t<typename ComponentGenerator_Constant_f<Scalar_,DIM,0>::T>,
                               Derived_> Zero;
    static Zero const ZERO;

    template <Uint32 INDEX_>
    struct BasisVector_f
    {
    private:
        enum { STATIC_ASSERT_IN_ENUM((INDEX_ < Parent_Tensor_i::DIM), INDEX_OUT_OF_RANGE) };
        BasisVector_f () { }
        typedef typename ComponentGenerator_Characteristic_f<Scalar_,Parent_Tensor_i::DIM,INDEX_>::T ComponentGenerator;
    public:
        typedef ImplementationOf_t<Concept,
                                   Scalar_,
                                   UseProceduralArray_t<ComponentGenerator>,
                                   Derived_> T;
        static T const V;
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
        STATIC_ASSERT(IsUseMemberArray_f<UseArrayType_>::V, MUST_BE_USE_MEMBER_ARRAY);
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
        STATIC_ASSERT(IsUseMemberArray_f<UseArrayType_>::V, MUST_BE_USE_MEMBER_ARRAY);
    }
    // this is the tuple-based constructor
    template <typename... Types_>
    ImplementationOf_t (Tuple_t<Typle_t<Types_...>> const &x)
        :
        Parent_Array_i(x.as_member_array())
    {
        STATIC_ASSERT(IsUseMemberArray_f<UseArrayType_>::V, MUST_BE_USE_MEMBER_ARRAY);
    }

    // only use these if UsePreallocatedArray_t<...> is specified

    explicit ImplementationOf_t (QualifiedComponent *pointer_to_allocation, CheckPointer check_pointer = CheckPointer::TRUE)
        :
        Parent_Array_i(pointer_to_allocation, check_pointer)
    {
        STATIC_ASSERT(IsUsePreallocatedArray_f<UseArrayType_>::V, MUST_BE_USE_PREALLOCATED_ARRAY);
    }
    // similar to a copy constructor, except initializes from a Vector_i.
    // this was chosen to be explicit to avoid unnecessary copies.
    template <typename OtherDerived_, ComponentQualifier OTHER_COMPONENT_QUALIFIER_>
    ImplementationOf_t (Vector_i<OtherDerived_,Scalar_,Concept,OTHER_COMPONENT_QUALIFIER_> const &x,
                        QualifiedComponent *pointer_to_allocation, CheckPointer check_pointer = CheckPointer::TRUE)
        :
        Parent_Array_i(pointer_to_allocation, check_pointer)
    {
        STATIC_ASSERT(IsUsePreallocatedArray_f<UseArrayType_>::V, MUST_BE_USE_PREALLOCATED_ARRAY);
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
        STATIC_ASSERT(IsUsePreallocatedArray_f<UseArrayType_>::V, MUST_BE_USE_PREALLOCATED_ARRAY);
    }
    // this is the tuple-based constructor
    template <typename... Types_>
    ImplementationOf_t (Tuple_t<Typle_t<Types_...>> const &x,
                        QualifiedComponent *pointer_to_allocation, CheckPointer check_pointer = CheckPointer::TRUE)
        :
        Parent_Array_i(x, pointer_to_allocation, check_pointer)
    {
        STATIC_ASSERT(IsUsePreallocatedArray_f<UseArrayType_>::V, MUST_BE_USE_PREALLOCATED_ARRAY);
    }

    // only use this if UseProceduralArray_t<...> is specified
    ImplementationOf_t ()
        :
        Parent_Array_i(WithoutInitialization()) // sort of meaningless constructor
    {
        STATIC_ASSERT(IsUseProceduralArray_f<UseArrayType_>::V, MUST_BE_USE_PROCEDURAL_ARRAY);
    }

    template <typename BundleIndexTyple, typename BundledIndex>
    static MultiIndex_t<BundleIndexTyple> bundle_index_map (BundledIndex const &b)
    {
        STATIC_ASSERT(IsComponentIndex_f<BundledIndex>::V, MUST_BE_COMPONENT_INDEX);
        // this constructor breaks the vector index apart into a row-major multi-index
        return MultiIndex_t<BundleIndexTyple>(b);
    }

    using Parent_Array_i::as_derived;
    using Parent_Array_i::allocation_size_in_bytes;
    using Parent_Array_i::pointer_to_allocation;
    using Parent_Array_i::overlaps_memory_range;

    // can't do "using Parent_Array_i::operator[];" due to overloading ambiguity.
    ComponentAccessConstReturnType operator [] (ComponentIndex const &i) const { return Parent_Array_i::operator[](i); }
    ComponentAccessNonConstReturnType operator [] (ComponentIndex const &i) { return Parent_Array_i::operator[](i); }

    using Parent_Tensor_i::operator[];
    // these are what provide indexed expressions -- via expression templates
    using Parent_Tensor_i::operator();

    using Parent_Tensor_i::component_is_procedural_zero;
    using Parent_Tensor_i::scalar_factor_for_component;
    using Parent_Tensor_i::vector_index_of;
};

template <typename FactorTyple_, typename Scalar_, typename UseArrayType_, typename Derived_>
typename ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<FactorTyple_>,Scalar_,UseArrayType_,Derived_>::Zero const ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<FactorTyple_>,Scalar_,UseArrayType_,Derived_>::ZERO;

template <typename FactorTyple_, typename Scalar_, typename UseArrayType_, typename Derived_>
template <Uint32 INDEX_>
typename ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<FactorTyple_>,Scalar_,UseArrayType_,Derived_>::template BasisVector_f<INDEX_>::T const ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<FactorTyple_>,Scalar_,UseArrayType_,Derived_>::BasisVector_f<INDEX_>::V;

template <typename FactorTyple_, typename Scalar_, typename UseArrayType_, typename Derived_>
struct DualOf_f<ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<FactorTyple_>,Scalar_,UseArrayType_,Derived_>>
{
    typedef ImplementationOf_t<typename DualOf_f<TensorProductOfBasedVectorSpaces_c<FactorTyple_>>::T,Scalar_,typename DualOf_f<UseArrayType_>::T,typename DualOf_f<Derived_>::T> T;
private:
    DualOf_f();
};

} // end of namespace Tenh

#endif // TENH_IMPLEMENTATION_TENSOR_HPP_
