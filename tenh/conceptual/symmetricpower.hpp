// ///////////////////////////////////////////////////////////////////////////
// tenh/conceptual/symmetricpower.hpp by Victor Dods, created 2013/07/29
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_CONCEPTUAL_SYMMETRICPOWER_HPP_
#define TENH_CONCEPTUAL_SYMMETRICPOWER_HPP_

#include "tenh/core.hpp"
#include "tenh/mathutil.hpp"

#include "tenh/conceptual/concept.hpp"
#include "tenh/conceptual/embeddableintensorpower.hpp"
#include "tenh/conceptual/tensorpower.hpp"
#include "tenh/conceptual/vectorspace.hpp"

namespace Tenh {

// symmetric power of formal symbols (e.g. identifiers, builtin C++ types, etc)
template <typename Factor_, Uint32 ORDER_>
struct SymmetricPower_c
{
    typedef EmptyTypeList ParentTypeList;

    static Uint32 const ORDER = ORDER_;
    typedef Factor_ Factor;
    typedef typename TypeListWithMultiplicity_t<Factor,ORDER>::T FactorTypeList;
    typedef typename DualOf_f<SymmetricPower_c>::T Dual; // the dual is not the symmetric power of Factor::Dual

    static std::string type_as_string ()
    {
        return "SymmetricPower_c<" + TypeStringOf_t<Factor>::eval() + ',' + AS_STRING(ORDER_) + '>';
    }
};

template <typename Factor_, Uint32 ORDER_>
struct IsConcept_f<SymmetricPower_c<Factor_, ORDER_> >
{ static bool const V = true; };

template <typename T> struct IsSymmetricPower_f { static bool const V = false; };
template <typename Factor, Uint32 ORDER> struct IsSymmetricPower_f<SymmetricPower_c<Factor,ORDER> > { static bool const V = true; };

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(SymmetricPower);
// special convenience macros
#define IS_SYMMETRIC_POWER_UNIQUELY(Concept) HasUniqueSymmetricPowerStructure_f<Concept>::V
#define AS_SYMMETRIC_POWER(Concept) UniqueSymmetricPowerStructureOf_f<Concept>::T


// FactorTypeList_ must be a TypeList_t of VectorSpace_c types
template <typename Factor_, Uint32 ORDER_>
struct SymmetricPowerOfVectorSpaces_c
{
private:
    enum { STATIC_ASSERT_IN_ENUM(IS_VECTOR_SPACE_UNIQUELY(Factor_), MUST_BE_VECTOR_SPACE), };
    typedef SymmetricPower_c<Factor_,ORDER_> As_SymmetricPower;
    typedef VectorSpace_c<typename Factor_::Field,BinomialCoefficient_t<Factor_::DIM + ORDER_ - 1, ORDER_>::V,SymmetricPower_c<typename Factor_::Id,ORDER_> > As_VectorSpace;
    typedef EmbeddableInTensorPowerOfVectorSpaces_c<TensorPowerOfVectorSpaces_c<Factor_, ORDER_> > As_EmbeddableInTensorPowerOfVectorSpaces;
public:
    typedef TypeList_t<As_SymmetricPower,
            TypeList_t<As_VectorSpace,
            TypeList_t<As_EmbeddableInTensorPowerOfVectorSpaces> > > ParentTypeList;

    typedef typename As_SymmetricPower::FactorTypeList FactorTypeList;
    typedef typename As_VectorSpace::Field Field;
    static Uint32 const DIM = As_VectorSpace::DIM;
    typedef typename As_VectorSpace::Id Id;
    typedef typename DualOf_f<SymmetricPowerOfVectorSpaces_c>::T Dual; // the dual is not the symmetric power of Factor::Dual
    typedef Factor_ Factor;

    static std::string type_as_string ()
    {
        return "SymmetricPowerOfVectorSpaces_c<" + TypeStringOf_t<Factor>::eval() + ',' + AS_STRING(ORDER_) + '>';
    }
};

template <typename Factor_, Uint32 ORDER_>
struct IsConcept_f<SymmetricPowerOfVectorSpaces_c<Factor_, ORDER_> >
{ static bool const V = true; };

template <typename T> struct IsSymmetricPowerOfVectorSpaces_f { static bool const V = false; };
template <typename Factor, Uint32 ORDER> struct IsSymmetricPowerOfVectorSpaces_f<SymmetricPowerOfVectorSpaces_c<Factor,ORDER> > { static bool const V = true; };

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(SymmetricPowerOfVectorSpaces);
// special convenience macros
#define IS_SYMMETRIC_POWER_OF_VECTOR_SPACES_UNIQUELY(Concept) HasUniqueSymmetricPowerOfVectorSpacesStructure_f<Concept>::V
#define AS_SYMMETRIC_POWER_OF_VECTOR_SPACES(Concept) UniqueSymmetricPowerOfVectorSpacesStructureOf_f<Concept>::T


// Factor_ must be a Basis_c type
template <typename Factor_, Uint32 ORDER_>
struct SymmetricPowerOfBases_c
{
private:
    enum { STATIC_ASSERT_IN_ENUM(IS_BASIS_UNIQUELY(Factor_), MUST_BE_BASIS) };
    typedef SymmetricPower_c<Factor_,ORDER_> As_SymmetricPower;
    typedef Basis_c<SymmetricPower_c<Factor_,ORDER_> > As_Basis;
public:
    typedef TypeList_t<As_SymmetricPower,
            TypeList_t<As_Basis> > ParentTypeList;

    typedef typename As_Basis::Id Id;
    typedef typename DualOf_f<SymmetricPowerOfBases_c>::T Dual; // the dual is not the symmetric power of Factor::Dual
    typedef Factor_ Factor;

    static std::string type_as_string ()
    {
        return "SymmetricPowerOfBases_c<" + TypeStringOf_t<Factor>::eval() + ',' + AS_STRING(ORDER_) + '>';
    }
};

template <typename Factor_, Uint32 ORDER_>
struct IsConcept_f<SymmetricPowerOfBases_c<Factor_, ORDER_> >
{ static bool const V = true; };

template <typename T> struct IsSymmetricPowerOfBases_f { static bool const V = false; };
template <typename Factor, Uint32 ORDER> struct IsSymmetricPowerOfBases_f<SymmetricPowerOfBases_c<Factor,ORDER> > { static bool const V = true; };

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(SymmetricPowerOfBases);
// special convenience macros
#define IS_SYMMETRIC_POWER_OF_BASES_UNIQUELY(Concept) HasUniqueSymmetricPowerOfBasesStructure_f<Concept>::V
#define AS_SYMMETRIC_POWER_OF_BASES(Concept) UniqueSymmetricPowerOfBasesStructureOf_f<Concept>::T


template <typename SymmetricPowerOfVectorSpaces_, typename Basis_>
struct BasedSymmetricPowerOfVectorSpaces_c
{
private:
    enum
    {
        STATIC_ASSERT_IN_ENUM(IS_SYMMETRIC_POWER_OF_VECTOR_SPACES_UNIQUELY(SymmetricPowerOfVectorSpaces_), MUST_BE_SYMMETRIC_POWER_OF_VECTOR_SPACES),
        STATIC_ASSERT_IN_ENUM(IS_BASIS_UNIQUELY(Basis_), MUST_BE_BASIS),
    };
    typedef SymmetricPowerOfVectorSpaces_ As_SymmetricPowerOfVectorSpaces;
    typedef BasedVectorSpace_c<SymmetricPowerOfVectorSpaces_,Basis_> As_BasedVectorSpace;
public:
    typedef TypeList_t<As_SymmetricPowerOfVectorSpaces,
            TypeList_t<As_BasedVectorSpace> > ParentTypeList;

    typedef typename As_BasedVectorSpace::Field Field;
    static Uint32 const DIM = As_BasedVectorSpace::DIM;
    typedef typename As_BasedVectorSpace::Id Id;
    typedef typename As_BasedVectorSpace::Basis Basis;
    typedef typename DualOf_f<BasedSymmetricPowerOfVectorSpaces_c>::T Dual; // relies on the template specialization below

    static std::string type_as_string ()
    {
        return "BasedSymmetricPowerOfVectorSpaces_c<" + TypeStringOf_t<SymmetricPowerOfVectorSpaces_>::eval() + ','
                                                     + TypeStringOf_t<Basis_>::eval() + '>';
    }
};

template <typename SymmetricPowerOfVectorSpaces, typename Basis_>
struct IsConcept_f<BasedSymmetricPowerOfVectorSpaces_c<SymmetricPowerOfVectorSpaces, Basis_> >
{ static bool const V = true; };

template <typename T> struct IsBasedSymmetricPowerOfVectorSpaces_f { static bool const V = false; };
template <typename SymmetricPowerOfVectorSpaces, typename Basis> struct IsBasedSymmetricPowerOfVectorSpaces_f<BasedSymmetricPowerOfVectorSpaces_c<SymmetricPowerOfVectorSpaces,Basis> > { static bool const V = true; };

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(BasedSymmetricPowerOfVectorSpaces);
// special convenience macros
#define IS_BASED_SYMMETRIC_POWER_OF_VECTOR_SPACES_UNIQUELY(Concept) HasUniqueBasedSymmetricPowerOfVectorSpacesStructure_f<Concept>::V
#define AS_BASED_SYMMETRIC_POWER_OF_VECTOR_SPACES(Concept) UniqueBasedSymmetricPowerOfVectorSpacesStructureOf_f<Concept>::T

// TODO: thinka bout this
// template <typename SymmetricPowerOfVectorSpaces, typename Basis>
// struct DualOf_f<BasedSymmetricPowerOfVectorSpaces_c<SymmetricPowerOfVectorSpaces,Basis> >
// {
//     typedef BasedSymmetricPowerOfVectorSpaces_c<typename DualOf_f<SymmetricPowerOfVectorSpaces>::T,typename DualOf_f<Basis>::T> T;
// };


// Factor_ must be a BasedVectorSpace_c type
template <typename Factor_, Uint32 ORDER_>
struct SymmetricPowerOfBasedVectorSpaces_c
{
private:
    enum { STATIC_ASSERT_IN_ENUM(IS_BASED_VECTOR_SPACE_UNIQUELY(Factor_), MUST_BE_BASED_VECTOR_SPACE) };
    typedef BasedSymmetricPowerOfVectorSpaces_c<SymmetricPowerOfVectorSpaces_c<Factor_,ORDER_>, SymmetricPowerOfBases_c<typename AS_BASED_VECTOR_SPACE(Factor_)::Basis, ORDER_> > As_BasedSymmetricPowerOfVectorSpaces;
    typedef EmbeddableInTensorPowerOfBasedVectorSpaces_c<TensorPowerOfBasedVectorSpaces_c<Factor_,ORDER_> > As_EmbeddableInTensorPowerOfBasedVectorSpaces;
public:
    typedef TypeList_t<As_BasedSymmetricPowerOfVectorSpaces,
            TypeList_t<As_EmbeddableInTensorPowerOfBasedVectorSpaces> > ParentTypeList;

    typedef typename As_EmbeddableInTensorPowerOfBasedVectorSpaces::FactorTypeList FactorTypeList;
    typedef typename As_BasedSymmetricPowerOfVectorSpaces::Field Field;
    static Uint32 const DIM = As_BasedSymmetricPowerOfVectorSpaces::DIM;
    typedef typename As_BasedSymmetricPowerOfVectorSpaces::Id Id;
    typedef typename As_BasedSymmetricPowerOfVectorSpaces::Basis Basis;
    typedef typename DualOf_f<SymmetricPowerOfBasedVectorSpaces_c>::T Dual; // the dual is not the symmetric power of Factor::Dual
    typedef Factor_ Factor;

    static std::string type_as_string ()
    {
        return "SymmetricProductOfBasedVectorSpaces_c<" + TypeStringOf_t<Factor_>::eval() + ',' + AS_STRING(ORDER_) + '>';
    }
};

template <typename Factor_, Uint32 ORDER_>
struct IsConcept_f<SymmetricPowerOfBasedVectorSpaces_c<Factor_, ORDER_> >
{ static bool const V = true; };

template <typename T> struct IsSymmetricPowerOfBasedVectorSpaces_f { static bool const V = false; };
template <typename Factor, Uint32 ORDER> struct IsSymmetricPowerOfBasedVectorSpaces_f<SymmetricPowerOfBasedVectorSpaces_c<Factor,ORDER> > { static bool const V = true; };

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(SymmetricPowerOfBasedVectorSpaces);
// special convenience macros
#define IS_SYMMETRIC_POWER_OF_BASED_VECTOR_SPACES_UNIQUELY(Concept) HasUniqueSymmetricPowerOfBasedVectorSpacesStructure_f<Concept>::V
#define AS_SYMMETRIC_POWER_OF_BASED_VECTOR_SPACES(Concept) UniqueSymmetricPowerOfBasedVectorSpacesStructureOf_f<Concept>::T

} // end of namespace Tenh

#endif // TENH_CONCEPTUAL_SYMMETRICPOWER_HPP_
