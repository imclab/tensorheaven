// ///////////////////////////////////////////////////////////////////////////
// tenh/conceptual/tensorproduct.hpp by Victor Dods, created 2013/07/22
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_CONCEPTUAL_TENSORPRODUCT_HPP_
#define TENH_CONCEPTUAL_TENSORPRODUCT_HPP_

#include "tenh/core.hpp"

#include "tenh/conceptual/concept.hpp"
#include "tenh/conceptual/basis.hpp"
#include "tenh/conceptual/dual.hpp"
#include "tenh/conceptual/embeddableintensorproduct.hpp"
#include "tenh/conceptual/vectorspace.hpp"
#include "tenh/meta/typelist.hpp"

namespace Tenh {

// ///////////////////////////////////////////////////////////////////////////
// TensorProduct_c
// ///////////////////////////////////////////////////////////////////////////

// generic tensor product of formal symbols (e.g. identifiers, builtin C++ types, etc)
template <typename FactorTypeList_>
struct TensorProduct_c
{
private:
    enum { STATIC_ASSERT_IN_ENUM(IsTypeList_f<FactorTypeList_>::V, MUST_BE_TYPELIST) };

public:
    typedef EmptyTypeList ParentTypeList;

    typedef FactorTypeList_ FactorTypeList;
    static Uint32 const ORDER = FactorTypeList::LENGTH;

    static std::string type_as_string ()
    {
        return "TensorProduct_c<" + type_string_of<FactorTypeList_>() + '>';
    }
};

template <typename FactorTypeList_>
struct IsConcept_f<TensorProduct_c<FactorTypeList_> >
{
    static bool const V = true;
private:
    IsConcept_f();
};

template <typename T> struct IsTensorProduct_f
{
    static bool const V = false;
private:
    IsTensorProduct_f();
};
template <typename FactorTypeList_> struct IsTensorProduct_f<TensorProduct_c<FactorTypeList_> >
{
    static bool const V = true;
private:
    IsTensorProduct_f();
};

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(TensorProduct);
// special convenience macros
#define IS_TENSOR_PRODUCT_UNIQUELY(Concept) HasUniqueTensorProductStructure_f<Concept>::V
#define AS_TENSOR_PRODUCT(Concept) UniqueTensorProductStructureOf_f<Concept>::T

template <typename FactorTypeList_>
struct DualOf_f<TensorProduct_c<FactorTypeList_> >
{
    typedef TensorProduct_c<typename DualOf_f<FactorTypeList_>::T> T;
private:
    DualOf_f();
};

// property IDs

struct Order; // could this be a forward declaration? Looks like yes.
struct FactorTypeList; // could this be a forward declaration? Looks like yes.
struct TensorPowerFactor; // could this be a forward declaration? Looks like yes.

// BaseProperty_f accessors

template <typename FactorTypeList_>
struct BaseProperty_f<TensorProduct_c<FactorTypeList_>,Order>
{
    typedef Value_t<Uint32,FactorTypeList_::LENGTH> T;
private:
    BaseProperty_f();
};

template <typename FactorTypeList_>
struct BaseProperty_f<TensorProduct_c<FactorTypeList_>,FactorTypeList>
{
    typedef FactorTypeList_ T;
private:
    BaseProperty_f();
};

// this function is only well-defined if FactorTypeList_ is uniform and has at least one element
template <typename FactorTypeList_>
struct BaseProperty_f<TensorProduct_c<FactorTypeList_>,TensorPowerFactor>
{
private:
    static bool const THERE_IS_A_UNIQUE_FACTOR = FactorTypeList_::LENGTH >= 1 &&
                                                 TypeListIsUniform_t<FactorTypeList_>::V;
    BaseProperty_f();
public:
    typedef typename If_f<THERE_IS_A_UNIQUE_FACTOR,
                          typename FactorTypeList_::HeadType,
                          NullValue>::T T;
};

// named property accessors

template <typename Concept_> struct OrderOf_f
{
    static Uint32 const V = PropertyValue_f<Concept_,Order>::V;
private:
    OrderOf_f();
};
template <typename Concept_> struct FactorTypeListOf_f
{
    typedef typename Property_f<Concept_,FactorTypeList>::T T;
private:
    FactorTypeListOf_f();
};
template <typename Concept_> struct TensorPowerFactorOf_f
{
    typedef typename Property_f<Concept_,TensorPowerFactor>::T T;
private:
    TensorPowerFactorOf_f();
};

// ///////////////////////////////////////////////////////////////////////////
// TensorProductOfVectorSpaces_c
// ///////////////////////////////////////////////////////////////////////////

// TODO: Replace with predicate-based thing
template <typename FactorTypeList_>
struct AllFactorsAreVectorSpaces_f
{
    static bool const V = HasVectorSpaceStructure_f<typename FactorTypeList_::HeadType>::V &&
                          AllFactorsAreVectorSpaces_f<typename FactorTypeList_::BodyTypeList>::V;
private:
    AllFactorsAreVectorSpaces_f();
};

template <>
struct AllFactorsAreVectorSpaces_f<EmptyTypeList>
{
    static bool const V = true;
private:
    AllFactorsAreVectorSpaces_f();
};

template <typename FactorTypeList_>
struct AllFactorsAreBases_f
{
    static bool const V = HasBasisStructure_f<typename FactorTypeList_::HeadType>::V &&
                          AllFactorsAreBases_f<typename FactorTypeList_::BodyTypeList>::V;
private:
    AllFactorsAreBases_f();
};

template <>
struct AllFactorsAreBases_f<EmptyTypeList>
{
    static bool const V = true;
private:
    AllFactorsAreBases_f();
};

template <typename FactorTypeList_>
struct AllFactorsHaveTheSameField_f
{
private:
    typedef typename FactorTypeList_::HeadType HeadType;
    typedef typename FactorTypeList_::BodyTypeList BodyTypeList;
    AllFactorsHaveTheSameField_f();
public:
    static bool const V = TypesAreEqual_f<typename ScalarFieldOf_f<HeadType>::T,
                                          typename ScalarFieldOf_f<typename BodyTypeList::HeadType>::T>::V &&
                          AllFactorsHaveTheSameField_f<BodyTypeList>::V;
};

template <typename HeadType>
struct AllFactorsHaveTheSameField_f<TypeList_t<HeadType> >
{
    static bool const V = true;
private:
    AllFactorsHaveTheSameField_f();
};

template <>
struct AllFactorsHaveTheSameField_f<EmptyTypeList>
{
    static bool const V = true;
private:
    AllFactorsHaveTheSameField_f();
};

template <typename FactorTypeList_>
struct ProductOfDimensions_t
{
private:
    enum { STATIC_ASSERT_IN_ENUM(IS_VECTOR_SPACE_UNIQUELY(typename FactorTypeList_::HeadType), MUST_BE_VECTOR_SPACE) };
    ProductOfDimensions_t();
public:
    static Uint32 const V = DimensionOf_f<typename FactorTypeList_::HeadType>::V *
                            ProductOfDimensions_t<typename FactorTypeList_::BodyTypeList>::V;
};

template <>
struct ProductOfDimensions_t<EmptyTypeList>
{
    static Uint32 const V = 1; // a 0-tensor is the scalar field by convention
private:
    ProductOfDimensions_t();
};

template <typename TypeList>
struct IdsOfTypeList_t
{
    typedef TypeList_t<typename TypeList::HeadType::Id,
                       typename IdsOfTypeList_t<typename TypeList::BodyTypeList>::T> T;
private:
    IdsOfTypeList_t();
};

template <>
struct IdsOfTypeList_t<EmptyTypeList>
{
    typedef EmptyTypeList T;
private:
    IdsOfTypeList_t();
};


// FactorTypeList_ must be a TypeList_t of VectorSpace_c types
template <typename FactorTypeList_>
struct TensorProductOfVectorSpaces_c
{
private:
    enum
    {
        STATIC_ASSERT_IN_ENUM(IsTypeList_f<FactorTypeList_>::V, MUST_BE_TYPELIST),
        STATIC_ASSERT_IN_ENUM(AllFactorsAreVectorSpaces_f<FactorTypeList_>::V, ALL_FACTORS_MUST_BE_VECTOR_SPACES),
        STATIC_ASSERT_IN_ENUM(AllFactorsHaveTheSameField_f<FactorTypeList_>::V, ALL_FACTORS_MUST_HAVE_SAME_FIELD)
    };

    typedef TensorProduct_c<FactorTypeList_> As_TensorProduct;
    typedef VectorSpace_c<typename ScalarFieldOf_f<typename FactorTypeList_::HeadType>::T,
                          ProductOfDimensions_t<FactorTypeList_>::V,
                          TensorProduct_c<typename IdsOfTypeList_t<FactorTypeList_>::T> > As_VectorSpace;
    typedef EmbeddableInTensorProductOfVectorSpaces_c<TensorProductOfVectorSpaces_c> As_EmbeddableInTensorProductOfVectorSpaces;
public:
    typedef TypeList_t<As_TensorProduct,
            TypeList_t<As_VectorSpace,
            TypeList_t<As_EmbeddableInTensorProductOfVectorSpaces> > > ParentTypeList;

    typedef typename As_VectorSpace::Id Id;

    static std::string type_as_string ()
    {
        return "TensorProductOfVectorSpaces_c<" + type_string_of<FactorTypeList_>() + '>';
    }
};

template <typename FactorTypeList_>
struct IsConcept_f<TensorProductOfVectorSpaces_c<FactorTypeList_> >
{
    static bool const V = true;
private:
    IsConcept_f();
};

template <typename T> struct IsTensorProductOfVectorSpaces_f
{
    static bool const V = false;
private:
    IsTensorProductOfVectorSpaces_f();
};
template <typename FactorTypeList_> struct IsTensorProductOfVectorSpaces_f<TensorProductOfVectorSpaces_c<FactorTypeList_> >
{
    static bool const V = true;
private:
    IsTensorProductOfVectorSpaces_f();
};

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(TensorProductOfVectorSpaces);
// special convenience macros
#define IS_TENSOR_PRODUCT_OF_VECTOR_SPACES_UNIQUELY(Concept) HasUniqueTensorProductOfVectorSpacesStructure_f<Concept>::V
#define AS_TENSOR_PRODUCT_OF_VECTOR_SPACES(Concept) UniqueTensorProductOfVectorSpacesStructureOf_f<Concept>::T

template <typename FactorTypeList_>
struct DualOf_f<TensorProductOfVectorSpaces_c<FactorTypeList_> >
{
    typedef TensorProductOfVectorSpaces_c<typename DualOf_f<FactorTypeList_>::T> T;
private:
    DualOf_f();
};

// ///////////////////////////////////////////////////////////////////////////
// TensorProductOfBases_c
// ///////////////////////////////////////////////////////////////////////////

// FactorTypeList_ must be a TypeList_t of Basis_c types
template <typename FactorTypeList_>
struct TensorProductOfBases_c
{
private:
    enum
    {
        STATIC_ASSERT_IN_ENUM(IsTypeList_f<FactorTypeList_>::V, MUST_BE_TYPELIST),
        STATIC_ASSERT_IN_ENUM(AllFactorsAreBases_f<FactorTypeList_>::V, ALL_FACTORS_MUST_BE_BASES)
    };
    typedef TensorProduct_c<FactorTypeList_> As_TensorProduct;
    typedef Basis_c<TensorProduct_c<typename IdsOfTypeList_t<FactorTypeList_>::T> > As_Basis;
public:
    typedef TypeList_t<As_TensorProduct, TypeList_t<As_Basis> > ParentTypeList;

    typedef typename As_Basis::Id Id;

    static std::string type_as_string ()
    {
        return "TensorProductOfBases_c<" + type_string_of<FactorTypeList_>() + '>';
    }
};

template <typename FactorTypeList_>
struct IsConcept_f<TensorProductOfBases_c<FactorTypeList_> >
{
    static bool const V = true;
private:
    IsConcept_f();
};

template <typename T> struct IsTensorProductOfBases_f
{
    static bool const V = false;
private:
    IsTensorProductOfBases_f();
};
template <typename FactorTypeList_> struct IsTensorProductOfBases_f<TensorProductOfBases_c<FactorTypeList_> >
{
    static bool const V = true;
private:
    IsTensorProductOfBases_f();
};

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(TensorProductOfBases);
// special convenience macros
#define IS_TENSOR_PRODUCT_OF_BASES_UNIQUELY(Concept) HasUniqueTensorProductOfBasesStructure_f<Concept>::V
#define AS_TENSOR_PRODUCT_OF_BASES(Concept) UniqueTensorProductOfBasesStructureOf_f<Concept>::T

template <typename FactorTypeList_>
struct DualOf_f<TensorProductOfBases_c<FactorTypeList_> >
{
    typedef TensorProductOfBases_c<typename DualOf_f<FactorTypeList_>::T> T;
private:
    DualOf_f();
};

// ///////////////////////////////////////////////////////////////////////////
// BasedTensorProductOfVectorSpaces_c
// ///////////////////////////////////////////////////////////////////////////

// TODO: this should be an EmbeddableInTensorProductOfVectorSpaces
template <typename TensorProductOfVectorSpaces_, typename Basis_>
struct BasedTensorProductOfVectorSpaces_c
{
private:
    enum
    {
        STATIC_ASSERT_IN_ENUM(IS_TENSOR_PRODUCT_OF_VECTOR_SPACES_UNIQUELY(TensorProductOfVectorSpaces_), MUST_BE_TENSOR_PRODUCT_OF_VECTOR_SPACES),
        STATIC_ASSERT_IN_ENUM(IS_BASIS_UNIQUELY(Basis_), MUST_BE_BASIS),
    };

    typedef TensorProductOfVectorSpaces_ As_TensorProductOfVectorSpaces;
    typedef BasedVectorSpace_c<TensorProductOfVectorSpaces_,Basis_> As_BasedVectorSpace;
public:
    typedef TypeList_t<As_TensorProductOfVectorSpaces,
            TypeList_t<As_BasedVectorSpace> > ParentTypeList;

    typedef typename As_BasedVectorSpace::Id Id;

    static std::string type_as_string ()
    {
        return "BasedTensorProductOfVectorSpaces_c<" + type_string_of<TensorProductOfVectorSpaces_>() + ','
                                                     + type_string_of<Basis_>() + '>';
    }
};

template <typename TensorProductOfVectorSpaces_, typename Basis_>
struct IsConcept_f<BasedTensorProductOfVectorSpaces_c<TensorProductOfVectorSpaces_, Basis_> >
{
    static bool const V = true;
private:
    IsConcept_f();
};

template <typename T> struct IsBasedTensorProductOfVectorSpaces_f
{
    static bool const V = false;
private:
    IsBasedTensorProductOfVectorSpaces_f();
};
template <typename TensorProductOfVectorSpaces_, typename Basis_> struct IsBasedTensorProductOfVectorSpaces_f<BasedTensorProductOfVectorSpaces_c<TensorProductOfVectorSpaces_,Basis_> >
{
    static bool const V = true;
private:
    IsBasedTensorProductOfVectorSpaces_f();
};

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(BasedTensorProductOfVectorSpaces);
// special convenience macros
#define IS_BASED_TENSOR_PRODUCT_OF_VECTOR_SPACES_UNIQUELY(Concept) HasUniqueBasedTensorProductOfVectorSpacesStructure_f<Concept>::V
#define AS_BASED_TENSOR_PRODUCT_OF_VECTOR_SPACES(Concept) UniqueBasedTensorProductOfVectorSpacesStructureOf_f<Concept>::T

template <typename TensorProductOfVectorSpaces, typename Basis>
struct DualOf_f<BasedTensorProductOfVectorSpaces_c<TensorProductOfVectorSpaces,Basis> >
{
    typedef BasedTensorProductOfVectorSpaces_c<typename DualOf_f<TensorProductOfVectorSpaces>::T,typename DualOf_f<Basis>::T> T;
private:
    DualOf_f();
};

// ///////////////////////////////////////////////////////////////////////////
// TensorProductOfBasedVectorSpaces_c
// ///////////////////////////////////////////////////////////////////////////

// TODO: Predicate thingy
template <typename FactorTypeList_>
struct AllFactorsAreBasedVectorSpaces_f
{
    static bool const V = HasBasedVectorSpaceStructure_f<typename FactorTypeList_::HeadType>::V &&
                          AllFactorsAreBasedVectorSpaces_f<typename FactorTypeList_::BodyTypeList>::V;
private:
    AllFactorsAreBasedVectorSpaces_f();
};

template <>
struct AllFactorsAreBasedVectorSpaces_f<EmptyTypeList>
{
    static bool const V = true;
private:
    AllFactorsAreBasedVectorSpaces_f();
};

// FactorTypeList_ must be a TypeList_t of BasedVectorSpace_c types
template <typename FactorTypeList_>
struct TensorProductOfBasedVectorSpaces_c
{
private:
    enum
    {
        STATIC_ASSERT_IN_ENUM(IsTypeList_f<FactorTypeList_>::V, MUST_BE_TYPELIST),
        STATIC_ASSERT_IN_ENUM(AllFactorsAreBasedVectorSpaces_f<FactorTypeList_>::V, ALL_FACTORS_MUST_BE_BASED_VECTOR_SPACES),
    };

    typedef typename PropertyOfEachInTypeList_f<FactorTypeList_,Basis>::T BasisTypeList;
    typedef BasedTensorProductOfVectorSpaces_c<TensorProductOfVectorSpaces_c<FactorTypeList_>,
                                               TensorProductOfBases_c<BasisTypeList> > As_BasedTensorProductOfVectorSpaces;
    typedef EmbeddableInTensorProductOfBasedVectorSpaces_c<TensorProductOfBasedVectorSpaces_c,TensorProductOfVectorSpaces_c<FactorTypeList_> > As_EmbeddableInTensorProductOfBasedVectorSpaces;
public:
    typedef TypeList_t<As_BasedTensorProductOfVectorSpaces,
            TypeList_t<As_EmbeddableInTensorProductOfBasedVectorSpaces> > ParentTypeList;

    typedef typename As_BasedTensorProductOfVectorSpaces::Id Id;

    static std::string type_as_string ()
    {
        return "TensorProductOfBasedVectorSpaces_c<" + type_string_of<FactorTypeList_>() + '>';
    }
};

template <typename FactorTypeList_>
struct IsConcept_f<TensorProductOfBasedVectorSpaces_c<FactorTypeList_> >
{
    static bool const V = true;
private:
    IsConcept_f();
};

template <typename T> struct IsTensorProductOfBasedVectorSpaces_f
{
    static bool const V = false;
private:
    IsTensorProductOfBasedVectorSpaces_f();
};
template <typename FactorTypeList_> struct IsTensorProductOfBasedVectorSpaces_f<TensorProductOfBasedVectorSpaces_c<FactorTypeList_> >
{
    static bool const V = true;
private:
    IsTensorProductOfBasedVectorSpaces_f();
};

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(TensorProductOfBasedVectorSpaces);
// special convenience macros
#define IS_TENSOR_PRODUCT_OF_BASED_VECTOR_SPACES_UNIQUELY(Concept) HasUniqueTensorProductOfBasedVectorSpacesStructure_f<Concept>::V
#define AS_TENSOR_PRODUCT_OF_BASED_VECTOR_SPACES(Concept) UniqueTensorProductOfBasedVectorSpacesStructureOf_f<Concept>::T

// there is a natural identification of a tensor product of vector spaces with the
// tensor product of the dual of the vector spaces
template <typename FactorTypeList_>
struct DualOf_f<TensorProductOfBasedVectorSpaces_c<FactorTypeList_> >
{
    typedef TensorProductOfBasedVectorSpaces_c<typename DualOf_f<FactorTypeList_>::T> T;
private:
    DualOf_f();
};

// ///////////////////////////////////////////////////////////////////////////
// meta-functions for taking tensor powers of stuff
// ///////////////////////////////////////////////////////////////////////////

template <Uint32 ORDER_, typename Factor_>
struct TensorPower_f
{
    typedef TensorProduct_c<typename UniformTypeListOfLength_t<Factor_,ORDER_>::T> T;
private:
    TensorPower_f();
};

template <Uint32 ORDER_, typename Factor_>
struct TensorPowerOfVectorSpace_f
{
private:
    enum { STATIC_ASSERT_IN_ENUM(IS_VECTOR_SPACE_UNIQUELY(Factor_), MUST_BE_VECTOR_SPACE) };
    TensorPowerOfVectorSpace_f();
public:
    typedef TensorProductOfVectorSpaces_c<typename UniformTypeListOfLength_t<Factor_,ORDER_>::T> T;
};

template <Uint32 ORDER_, typename Factor_>
struct TensorPowerOfBasedVectorSpace_f
{
private:
    enum { STATIC_ASSERT_IN_ENUM(IS_BASED_VECTOR_SPACE_UNIQUELY(Factor_), MUST_BE_BASED_VECTOR_SPACE) };
    TensorPowerOfBasedVectorSpace_f();
public:
    typedef TensorProductOfBasedVectorSpaces_c<typename UniformTypeListOfLength_t<Factor_,ORDER_>::T> T;
};

} // end of namespace Tenh

#endif // TENH_CONCEPTUAL_TENSORPRODUCT_HPP_
