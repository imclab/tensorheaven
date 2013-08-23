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
#include "tenh/conceptual/vectorspace.hpp"
#include "tenh/meta/typelist.hpp"

namespace Tenh {

// generic tensor product of formal symbols (e.g. identifiers, builtin C++ types, etc)
template <typename FactorTypeList_>
struct TensorProduct_c
{
private:
    enum { STATIC_ASSERT_IN_ENUM(IsATypeList_t<FactorTypeList_>::V, MUST_BE_TYPELIST) };

public:
    typedef EmptyTypeList ParentTypeList;

    typedef FactorTypeList_ FactorTypeList;
    static Uint32 const ORDER = FactorTypeList::LENGTH;
    typedef typename DualOf_f<TensorProduct_c>::T Dual; // relies on the template specialization below

    static std::string type_as_string ()
    {
        return "TensorProduct_c<" + TypeStringOf_t<FactorTypeList>::eval() + '>';
    }
};

template <typename FactorTypeList_>
struct IsConcept_f<TensorProduct_c<FactorTypeList_> >
{ static bool const V = true; };

template <typename T> struct IsTensorProduct_f { static bool const V = false; };
template <typename FactorTypeList> struct IsTensorProduct_f<TensorProduct_c<FactorTypeList> > { static bool const V = true; };

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(TensorProduct);
// special convenience macros
#define IS_TENSOR_PRODUCT_UNIQUELY(Concept) HasUniqueTensorProductStructure_f<Concept>::V
#define AS_TENSOR_PRODUCT(Concept) UniqueTensorProductStructureOf_f<Concept>::T

template <typename FactorTypeList>
struct DualOf_f<TensorProduct_c<FactorTypeList> >
{
    typedef TensorProduct_c<typename DualOf_f<FactorTypeList>::T> T;
};

// TODO: Replace with predicate-based thing
template <typename FactorTypeList>
struct AllFactorsAreVectorSpaces_f
{
    static bool const V = HasVectorSpaceStructure_f<typename FactorTypeList::HeadType>::V &&
                          AllFactorsAreVectorSpaces_f<typename FactorTypeList::BodyTypeList>::V;
};

template <>
struct AllFactorsAreVectorSpaces_f<EmptyTypeList>
{
    static bool const V = true;
};

template <typename FactorTypeList>
struct AllFactorsAreBases_f
{
    static bool const V = HasBasisStructure_f<typename FactorTypeList::HeadType>::V &&
                          AllFactorsAreBases_f<typename FactorTypeList::BodyTypeList>::V;
};

template <>
struct AllFactorsAreBases_f<EmptyTypeList>
{
    static bool const V = true;
};

template <typename FactorTypeList>
struct AllFactorsHaveTheSameField_f
{
    typedef typename FactorTypeList::HeadType HeadType;
    typedef typename FactorTypeList::BodyTypeList BodyTypeList;
    static bool const V = Lvd::Meta::TypesAreEqual<typename HeadType::Field,typename BodyTypeList::HeadType::Field>::v &&
                          AllFactorsHaveTheSameField_f<BodyTypeList>::V;
};

template <typename HeadType>
struct AllFactorsHaveTheSameField_f<TypeList_t<HeadType> >
{
    static bool const V = true;
};

template <>
struct AllFactorsHaveTheSameField_f<EmptyTypeList>
{
    static bool const V = true;
};

template <typename FactorTypeList>
struct ProductOfDimensions_t
{
    enum { STATIC_ASSERT_IN_ENUM(IS_VECTOR_SPACE_UNIQUELY(typename FactorTypeList::HeadType), MUST_BE_VECTOR_SPACE) };

    static Uint32 const V = AS_VECTOR_SPACE(typename FactorTypeList::HeadType)::DIM *
                            ProductOfDimensions_t<typename FactorTypeList::BodyTypeList>::V;
};

template <>
struct ProductOfDimensions_t<EmptyTypeList>
{
    static Uint32 const V = 1; // a 0-tensor is the scalar field by convention
};

template <typename TypeList>
struct IdsOfTypeList_t
{
    typedef TypeList_t<typename TypeList::HeadType::Id,
                       typename IdsOfTypeList_t<typename TypeList::BodyTypeList>::T> T;
};

template <>
struct IdsOfTypeList_t<EmptyTypeList>
{
    typedef EmptyTypeList T;
};


// TODO: get rid of the DIM and ORDER here.
// FactorTypeList_ must be a TypeList_t of VectorSpace_c types
template <typename FactorTypeList_>
struct TensorProductOfVectorSpaces_c
{
private:
    enum
    {
        STATIC_ASSERT_IN_ENUM(IsATypeList_t<FactorTypeList_>::V, MUST_BE_TYPELIST),
        STATIC_ASSERT_IN_ENUM(AllFactorsAreVectorSpaces_f<FactorTypeList_>::V, ALL_FACTORS_MUST_BE_VECTOR_SPACES),
        STATIC_ASSERT_IN_ENUM(AllFactorsHaveTheSameField_f<FactorTypeList_>::V, ALL_FACTORS_MUST_HAVE_SAME_FIELD)
    };

    typedef TensorProduct_c<FactorTypeList_> As_TensorProduct;
    typedef VectorSpace_c<typename FactorTypeList_::HeadType::Field,
                          ProductOfDimensions_t<FactorTypeList_>::V,
                          TensorProduct_c<typename IdsOfTypeList_t<FactorTypeList_>::T> > As_VectorSpace;
public:
    typedef TypeList_t<As_TensorProduct, TypeList_t<As_VectorSpace> > ParentTypeList;


    typedef FactorTypeList_ FactorTypeList;
    static Uint32 const ORDER = As_TensorProduct::ORDER;
    typedef typename As_VectorSpace::Field Field;
    static Uint32 const DIM = As_VectorSpace::DIM;
    typedef typename As_VectorSpace::Id Id;
    typedef typename DualOf_f<TensorProductOfVectorSpaces_c>::T Dual; // relies on the template specialization below

    static std::string type_as_string ()
    {
        return "TensorProductOfVectorSpaces_c<" + TypeStringOf_t<FactorTypeList>::eval() + '>';
    }
};

template <typename FactorTypeList_>
struct IsConcept_f<TensorProductOfVectorSpaces_c<FactorTypeList_> >
{ static bool const V = true; };

template <typename T> struct IsTensorProductOfVectorSpaces_f { static bool const V = false; };
template <typename FactorTypeList> struct IsTensorProductOfVectorSpaces_f<TensorProductOfVectorSpaces_c<FactorTypeList> > { static bool const V = true; };

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(TensorProductOfVectorSpaces);
// special convenience macros
#define IS_TENSOR_PRODUCT_OF_VECTOR_SPACES_UNIQUELY(Concept) HasUniqueTensorProductOfVectorSpacesStructure_f<Concept>::V
#define AS_TENSOR_PRODUCT_OF_VECTOR_SPACES(Concept) UniqueTensorProductOfVectorSpacesStructureOf_f<Concept>::T

template <typename FactorTypeList>
struct DualOf_f<TensorProductOfVectorSpaces_c<FactorTypeList> >
{
    typedef TensorProductOfVectorSpaces_c<typename DualOf_f<FactorTypeList>::T> T;
};


// FactorTypeList_ must be a TypeList_t of Basis_c types
template <typename FactorTypeList_>
struct TensorProductOfBases_c
{
private:
    enum
    {
        STATIC_ASSERT_IN_ENUM(IsATypeList_t<FactorTypeList_>::V, MUST_BE_TYPELIST),
        STATIC_ASSERT_IN_ENUM(AllFactorsAreBases_f<FactorTypeList_>::V, ALL_FACTORS_MUST_BE_BASES)
    };
    typedef TensorProduct_c<FactorTypeList_> As_TensorProduct;
    typedef Basis_c<TensorProduct_c<typename IdsOfTypeList_t<FactorTypeList_>::T> > As_Basis;
public:
    typedef TypeList_t<As_TensorProduct, TypeList_t<As_Basis> > ParentTypeList;

    typedef FactorTypeList_ FactorTypeList;
    static Uint32 const ORDER = As_TensorProduct::ORDER;
    typedef typename As_Basis::Id Id;
    typedef typename DualOf_f<TensorProductOfBases_c>::T Dual; // relies on the template specialization below

    static std::string type_as_string ()
    {
        return "TensorProductOfBases_c<" + TypeStringOf_t<FactorTypeList>::eval() + '>';
    }
};

template <typename FactorTypeList_>
struct IsConcept_f<TensorProductOfBases_c<FactorTypeList_> >
{ static bool const V = true; };

template <typename T> struct IsTensorProductOfBases_f { static bool const V = false; };
template <typename FactorTypeList> struct IsTensorProductOfBases_f<TensorProductOfBases_c<FactorTypeList> > { static bool const V = true; };

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(TensorProductOfBases);
// special convenience macros
#define IS_TENSOR_PRODUCT_OF_BASES_UNIQUELY(Concept) HasUniqueTensorProductOfBasesStructure_f<Concept>::V
#define AS_TENSOR_PRODUCT_OF_BASES(Concept) UniqueTensorProductOfBasesStructureOf_f<Concept>::T

template <typename FactorTypeList>
struct DualOf_f<TensorProductOfBases_c<FactorTypeList> >
{
    typedef TensorProductOfBases_c<typename DualOf_f<FactorTypeList>::T> T;
};


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

    typedef typename AS_TENSOR_PRODUCT_OF_VECTOR_SPACES(TensorProductOfVectorSpaces_) As_TensorProductOfVectorSpaces;
    typedef BasedVectorSpace_c<TensorProductOfVectorSpaces_,Basis_> As_BasedVectorSpace;
public:
    typedef TypeList_t<As_TensorProductOfVectorSpaces, TypeList_t<As_BasedVectorSpace> > ParentTypeList;

    typedef typename As_TensorProductOfVectorSpaces::FactorTypeList FactorTypeList;
    static Uint32 const ORDER = As_TensorProductOfVectorSpaces::ORDER;
    typedef typename As_BasedVectorSpace::Field Field;
    static Uint32 const DIM = As_BasedVectorSpace::DIM;
    typedef typename As_BasedVectorSpace::Id Id;
    typedef Basis_ Basis;
    typedef typename DualOf_f<BasedTensorProductOfVectorSpaces_c>::T Dual; // relies on the template specialization below

    static std::string type_as_string ()
    {
        return "BasedTensorProductOfVectorSpaces_c<" + TypeStringOf_t<TensorProductOfVectorSpaces_>::eval() + ','
                                                     + TypeStringOf_t<Basis>::eval() + '>';
    }
};

template <typename TensorProductOfVectorSpaces_, typename Basis_>
struct IsConcept_f<BasedTensorProductOfVectorSpaces_c<TensorProductOfVectorSpaces_, Basis_> >
{ static bool const V = true; };

template <typename T> struct IsBasedTensorProductOfVectorSpaces_f { static bool const V = false; };
template <typename TensorProductOfVectorSpaces_, typename Basis_> struct IsBasedTensorProductOfVectorSpaces_f<BasedTensorProductOfVectorSpaces_c<TensorProductOfVectorSpaces_,Basis_> > { static bool const V = true; };

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(BasedTensorProductOfVectorSpaces);
// special convenience macros
#define IS_BASED_TENSOR_PRODUCT_OF_VECTOR_SPACES_UNIQUELY(Concept) HasUniqueBasedTensorProductOfVectorSpacesStructure_f<Concept>::V
#define AS_BASED_TENSOR_PRODUCT_OF_VECTOR_SPACES(Concept) UniqueBasedTensorProductOfVectorSpacesStructureOf_f<Concept>::T

template <typename TensorProductOfVectorSpaces, typename Basis>
struct DualOf_f<BasedTensorProductOfVectorSpaces_c<TensorProductOfVectorSpaces,Basis> >
{
    typedef BasedTensorProductOfVectorSpaces_c<typename DualOf_f<TensorProductOfVectorSpaces>::T,typename DualOf_f<Basis>::T> T;
};


// TODO: Predicate thingy
template <typename FactorTypeList>
struct AllFactorsAreBasedVectorSpaces_f
{
    static bool const V = HasBasedVectorSpaceStructure_f<typename FactorTypeList::HeadType>::V &&
                          AllFactorsAreBasedVectorSpaces_f<typename FactorTypeList::BodyTypeList>::V;
};

template <>
struct AllFactorsAreBasedVectorSpaces_f<EmptyTypeList>
{
    static bool const V = true;
};

// TODO: Replace with some sort of map function.
template <typename TypeList>
struct VectorSpacesOfTypeList_t
{
    typedef TypeList_t<typename AS_VECTOR_SPACE(typename TypeList::HeadType),
                       typename VectorSpacesOfTypeList_t<typename TypeList::BodyTypeList>::T> T;
};

template <>
struct VectorSpacesOfTypeList_t<EmptyTypeList>
{
    typedef EmptyTypeList T;
};

template <typename TypeList>
struct BasesOfTypeList_t
{
    typedef TypeList_t<typename AS_BASED_VECTOR_SPACE(typename TypeList::HeadType)::Basis,
                       typename BasesOfTypeList_t<typename TypeList::BodyTypeList>::T> T;
};

template <>
struct BasesOfTypeList_t<EmptyTypeList>
{
    typedef EmptyTypeList T;
};


// FactorTypeList_ must be a TypeList_t of BasedVectorSpace_c types
template <typename FactorTypeList_>
struct TensorProductOfBasedVectorSpaces_c
{
private:
    enum
    {
        STATIC_ASSERT_IN_ENUM(IsATypeList_t<FactorTypeList_>::V, MUST_BE_TYPELIST),
        STATIC_ASSERT_IN_ENUM(AllFactorsAreBasedVectorSpaces_f<FactorTypeList_>::V, ALL_FACTORS_MUST_BE_BASED_VECTOR_SPACES),
    };

    typedef BasedTensorProductOfVectorSpaces_c<TensorProductOfVectorSpaces_c<FactorTypeList_>,
                                               TensorProductOfBases_c<typename BasesOfTypeList_t<FactorTypeList_>::T> > As_BasedTensorProductOfVectorSpaces;
public:
    typedef TypeList_t<As_BasedTensorProductOfVectorSpaces> ParentTypeList;

    typedef FactorTypeList_ FactorTypeList;
    static Uint32 const ORDER = As_BasedTensorProductOfVectorSpaces::ORDER;
    typedef typename As_BasedTensorProductOfVectorSpaces::Field Field;
    static Uint32 const DIM = As_BasedTensorProductOfVectorSpaces::DIM;
    typedef typename As_BasedTensorProductOfVectorSpaces::Id Id;
    typedef typename As_BasedTensorProductOfVectorSpaces::Basis Basis;
    typedef typename DualOf_f<TensorProductOfBasedVectorSpaces_c>::T Dual; // relies on the template specialization below

    static std::string type_as_string ()
    {
        return "TensorProductOfBasedVectorSpaces_c<" + TypeStringOf_t<FactorTypeList>::eval() + '>';
    }
};

template <typename FactorTypeList_>
struct IsConcept_f<TensorProductOfBasedVectorSpaces_c<FactorTypeList_> >
{ static bool const V = true; };

template <typename T> struct IsTensorProductOfBasedVectorSpaces_f { static bool const V = false; };
template <typename FactorTypeList> struct IsTensorProductOfBasedVectorSpaces_f<TensorProductOfBasedVectorSpaces_c<FactorTypeList> > { static bool const V = true; };

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(TensorProductOfBasedVectorSpaces);
// special convenience macros
#define IS_TENSOR_PRODUCT_OF_BASED_VECTOR_SPACES_UNIQUELY(Concept) HasUniqueTensorProductOfBasedVectorSpacesStructure_f<Concept>::V
#define AS_TENSOR_PRODUCT_OF_BASED_VECTOR_SPACES(Concept) UniqueTensorProductOfBasedVectorSpacesStructureOf_f<Concept>::T

template <typename FactorTypeList>
struct DualOf_f<TensorProductOfBasedVectorSpaces_c<FactorTypeList> >
{
    typedef TensorProductOfBasedVectorSpaces_c<typename DualOf_f<FactorTypeList>::T> T;
};

} // end of namespace Tenh

#endif // TENH_CONCEPTUAL_TENSORPRODUCT_HPP_
