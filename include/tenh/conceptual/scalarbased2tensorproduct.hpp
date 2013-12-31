// ///////////////////////////////////////////////////////////////////////////
// tenh/conceptual/scalarbased2tensorproduct.hpp by Victor Dods, created 2013/12/30
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_CONCEPTUAL_SCALARBASED2TENSORPRODUCT_HPP_
#define TENH_CONCEPTUAL_SCALARBASED2TENSORPRODUCT_HPP_

#include "tenh/core.hpp"

#include "tenh/componentindex.hpp" // technically not conceptual code, but close enough.
#include "tenh/conceptual/concept.hpp"
#include "tenh/conceptual/diagonalbased2tensorproduct.hpp"
#include "tenh/conceptual/embeddableintensorproduct.hpp"
#include "tenh/conceptual/linearembedding.hpp"
#include "tenh/conceptual/symmetricpower.hpp"
#include "tenh/conceptual/tensorproduct.hpp"
#include "tenh/multiindex.hpp" // technically not conceptual code, but close enough.

namespace Tenh {

// ///////////////////////////////////////////////////////////////////////////
// Scalar2TensorProduct_c
// ///////////////////////////////////////////////////////////////////////////

// formal "scalar 2-tensor product" of symbols
template <typename Factor1_, typename Factor2_>
struct Scalar2TensorProduct_c
{
    typedef EmptyTypeList ParentTypeList;

    typedef Factor1_ Factor1;
    typedef Factor2_ Factor2;

    static std::string type_as_string ()
    {
        return "Scalar2TensorProduct_c<" + type_string_of<Factor1_>() + ',' + type_string_of<Factor2_>() + '>';
    }
};

template <typename Factor1_, typename Factor2_>
struct IsConcept_f<Scalar2TensorProduct_c<Factor1_,Factor2_> >
{
    static bool const V = true;
private:
    IsConcept_f();
};

template <typename T> struct IsScalar2TensorProduct_f
{
    static bool const V = false;
private:
    IsScalar2TensorProduct_f();
};

template <typename Factor1_, typename Factor2_>
struct IsScalar2TensorProduct_f<Scalar2TensorProduct_c<Factor1_,Factor2_> >
{
    static bool const V = true;
private:
    IsScalar2TensorProduct_f();
};

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(Scalar2TensorProduct);
// special convenience macros
#define IS_SCALAR_2_TENSOR_PRODUCT_UNIQUELY(Concept) HasUniqueScalar2TensorProductStructure_f<Concept>::V
#define AS_SCALAR_2_TENSOR_PRODUCT(Concept) UniqueScalar2TensorProductStructureOf_f<Concept>::T

// ///////////////////////////////////////////////////////////////////////////
// Scalar2TensorProductOfBases_c
// ///////////////////////////////////////////////////////////////////////////

template <typename Factor1_, typename Factor2_>
struct Scalar2TensorProductOfBases_c
{
private:
    enum
    {
        STATIC_ASSERT_IN_ENUM__UNIQUE(IS_BASIS_UNIQUELY(Factor1_), MUST_BE_BASIS, FACTOR1),
        STATIC_ASSERT_IN_ENUM__UNIQUE(IS_BASIS_UNIQUELY(Factor2_), MUST_BE_BASIS, FACTOR2)
    };
    typedef Basis_c<Scalar2TensorProduct_c<Factor1_,Factor2_> > As_Basis;
    typedef Scalar2TensorProduct_c<Factor1_,Factor2_> As_Scalar2TensorProduct;
public:
    typedef TypeList_t<As_Basis,
            TypeList_t<As_Scalar2TensorProduct> > ParentTypeList;

    typedef typename As_Basis::Id Id;
    typedef Factor1_ Factor1;
    typedef Factor2_ Factor2;

    static std::string type_as_string ()
    {
        return "Scalar2TensorProductOfBases_c<" + type_string_of<Factor1_>() + ',' + type_string_of<Factor2_>() + '>';
    }
};

template <typename Factor1_, typename Factor2_>
struct IsConcept_f<Scalar2TensorProductOfBases_c<Factor1_,Factor2_> >
{
    static bool const V = true;
private:
    IsConcept_f();
};

template <typename T> struct IsScalar2TensorProductOfBases_f
{
    static bool const V = false;
private:
    IsScalar2TensorProductOfBases_f();
};

template <typename Factor1_, typename Factor2_>
struct IsScalar2TensorProductOfBases_f<Scalar2TensorProductOfBases_c<Factor1_,Factor2_> >
{
    static bool const V = true;
private:
    IsScalar2TensorProductOfBases_f();
};

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(Scalar2TensorProductOfBases);
// special convenience macros
#define IS_SCALAR_2_TENSOR_PRODUCT_OF_BASES_UNIQUELY(Concept) HasUniqueScalar2TensorProductOfBasesStructure_f<Concept>::V
#define AS_SCALAR_2_TENSOR_PRODUCT_OF_BASES(Concept) UniqueScalar2TensorProductOfBasesStructureOf_f<Concept>::T

// ///////////////////////////////////////////////////////////////////////////
// Scalar2TensorProductOfBasedVectorSpaces_c
// ///////////////////////////////////////////////////////////////////////////

template <typename Factor1_, typename Factor2_>
struct Scalar2TensorProductOfBasedVectorSpaces_c
{
private:
    enum
    {
        STATIC_ASSERT_IN_ENUM__UNIQUE(IS_BASED_VECTOR_SPACE_UNIQUELY(Factor1_), MUST_BE_BASED_VECTOR_SPACE, FACTOR1),
        STATIC_ASSERT_IN_ENUM__UNIQUE(IS_BASED_VECTOR_SPACE_UNIQUELY(Factor2_), MUST_BE_BASED_VECTOR_SPACE, FACTOR2),
        STATIC_ASSERT_IN_ENUM((TypesAreEqual_f<typename ScalarFieldOf_f<Factor1_>::T,typename ScalarFieldOf_f<Factor2_>::T>::V), ALL_FACTORS_MUST_HAVE_SAME_FIELD),
    };
    typedef TypeList_t<Factor1_,TypeList_t<Factor2_> > FactorTypeList;

    typedef VectorSpace_c<typename ScalarFieldOf_f<Factor1_>::T,
                          1,
                          Scalar2TensorProduct_c<Factor1_,Factor2_> > UnderlyingVectorSpace;

    typedef BasedVectorSpace_c<UnderlyingVectorSpace,
                               Scalar2TensorProductOfBases_c<typename BasisOf_f<Factor1_>::T,
                                                             typename BasisOf_f<Factor2_>::T> > As_BasedVectorSpace;
    typedef EmbeddableInTensorProductOfBasedVectorSpaces_c<TensorProductOfBasedVectorSpaces_c<FactorTypeList>,
                                                           TensorProductOfVectorSpaces_c<FactorTypeList> > As_EmbeddableInTensorProductOfBasedVectorSpaces;
public:
    typedef TypeList_t<As_BasedVectorSpace,
            TypeList_t<As_EmbeddableInTensorProductOfBasedVectorSpaces> > ParentTypeList;

    typedef typename As_BasedVectorSpace::Id Id;
    typedef Factor1_ Factor1;
    typedef Factor2_ Factor2;

    static std::string type_as_string ()
    {
        return "Scalar2TensorProductOfBasedVectorSpaces_c<" + type_string_of<Factor1_>() + ',' + type_string_of<Factor2_>() + '>';
    }
};

template <typename Factor1_, typename Factor2_>
struct IsConcept_f<Scalar2TensorProductOfBasedVectorSpaces_c<Factor1_,Factor2_> >
{
    static bool const V = true;
private:
    IsConcept_f();
};

template <typename T> struct IsScalar2TensorProductOfBasedVectorSpaces_f
{
    static bool const V = false;
private:
    IsScalar2TensorProductOfBasedVectorSpaces_f();
};

template <typename Factor1_, typename Factor2_>
struct IsScalar2TensorProductOfBasedVectorSpaces_f<Scalar2TensorProductOfBasedVectorSpaces_c<Factor1_,Factor2_> >
{
    static bool const V = true;
private:
    IsScalar2TensorProductOfBasedVectorSpaces_f();
};

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(Scalar2TensorProductOfBasedVectorSpaces);
// special convenience macros
#define IS_SCALAR_2_TENSOR_PRODUCT_OF_BASED_VECTOR_SPACES_UNIQUELY(Concept) HasUniqueScalar2TensorProductOfBasedVectorSpacesStructure_f<Concept>::V
#define AS_SCALAR_2_TENSOR_PRODUCT_OF_BASED_VECTOR_SPACES(Concept) UniqueScalar2TensorProductOfBasedVectorSpacesStructureOf_f<Concept>::T

// TODO: verify that this mathematical claim is true
template <typename Factor1, typename Factor2>
struct DualOf_f<Scalar2TensorProductOfBasedVectorSpaces_c<Factor1,Factor2> >
{
    typedef Scalar2TensorProductOfBasedVectorSpaces_c<typename DualOf_f<Factor1>::T,typename DualOf_f<Factor2>::T> T;
private:
    DualOf_f();
};

// convenience metafunction for using a FactorTypeList_ instead of two separate factors
template <typename FactorTypeList_>
struct Scalar2TensorProductOfBasedVectorSpaces_f
{
private:
    enum { STATIC_ASSERT_IN_ENUM(FactorTypeList_::LENGTH == 2, LENGTH_MUST_BE_EXACTLY_2) };
    typedef typename FactorTypeList_::HeadType Factor1;
    typedef typename FactorTypeList_::BodyTypeList::HeadType Factor2;
    Scalar2TensorProductOfBasedVectorSpaces_f();
public:
    typedef Scalar2TensorProductOfBasedVectorSpaces_c<Factor1,Factor2> T;
};

// specialization for FactorTypeListOf_f
template <typename Factor1_, typename Factor2_>
struct BaseProperty_f<Scalar2TensorProductOfBasedVectorSpaces_c<Factor1_,Factor2_>,FactorTypeList>
{
    typedef TypeList_t<Factor1_,TypeList_t<Factor2_> > T;
private:
    BaseProperty_f();
};

// ///////////////////////////////////////////////////////////////////////////
// helper functions for easing use of the type system
// ///////////////////////////////////////////////////////////////////////////

// for now, just do scalar 2-tensor product of based vector spaces
template <typename Factor1_, typename Factor2_>
Scalar2TensorProductOfBasedVectorSpaces_c<Factor1_,Factor2_> scalar2 (TensorProductOfBasedVectorSpaces_c<TypeList_t<Factor1_,TypeList_t<Factor2_> > > const &)
{
    return Scalar2TensorProductOfBasedVectorSpaces_c<Factor1_,Factor2_>();
}

// ///////////////////////////////////////////////////////////////////////////
// linear embedding of scalar 2-tensor into corresponding tensor product
// ///////////////////////////////////////////////////////////////////////////

template <typename Factor1_, typename Factor2_, typename Scalar_>
struct LinearEmbedding_c<Scalar2TensorProductOfBasedVectorSpaces_c<Factor1_,Factor2_>,
                         TensorProductOfBasedVectorSpaces_c<TypeList_t<Factor1_,TypeList_t<Factor2_> > >,
                         Scalar_,
                         NaturalEmbedding>
{
private:
    typedef Scalar2TensorProductOfBasedVectorSpaces_c<Factor1_,Factor2_> Scalar2;
    typedef TensorProductOfBasedVectorSpaces_c<TypeList_t<Factor1_,TypeList_t<Factor2_> > > Tensor2;
    static bool const FACTOR1DIM_LEQ_FACTOR2DIM = DimensionOf_f<Factor1_>::V <= DimensionOf_f<Factor2_>::V;
    static Uint32 const DIM = DimensionOf_f<Factor1_>::V <= DimensionOf_f<Factor2_>::V ?
                              DimensionOf_f<Factor1_>::V :
                              DimensionOf_f<Factor2_>::V;
public:
    typedef ComponentIndex_t<DimensionOf_f<Scalar2>::V> Scalar2ComponentIndex;
    typedef ComponentIndex_t<DimensionOf_f<Tensor2>::V> Tensor2ComponentIndex;
    typedef ComponentIndex_t<DimensionOf_f<Factor1_>::V> Factor1ComponentIndex;
    typedef ComponentIndex_t<DimensionOf_f<Factor2_>::V> Factor2ComponentIndex;
private:
    typedef MultiIndex_t<TypeList_t<Factor1ComponentIndex,
                         TypeList_t<Factor2ComponentIndex> > > Tensor2MultiIndex;
public:
    static bool embedded_component_is_procedural_zero (Tensor2ComponentIndex const &i)
    {
        Tensor2MultiIndex m(i); // does the row-major indexing conversion
        // it's a procedural zero if the component is off the diagonal.
        return m.template el<0>().value() != m.template el<1>().value();
    }
    static Scalar_ scalar_factor_for_embedded_component (Tensor2ComponentIndex const &) { return Scalar_(1); }
    static Scalar2ComponentIndex source_component_index_for_embedded_component (Tensor2ComponentIndex const &i)
    {
        // there is only one component for a scalar 2-tensor -- zero.
        return Scalar2ComponentIndex(0, DONT_CHECK_RANGE);
    }

    static Uint32 term_count_for_coembedded_component (Scalar2ComponentIndex const &) { return DIM; }
    static Scalar_ scalar_factor_for_coembedded_component (Scalar2ComponentIndex const &, Uint32) { return Scalar_(1); }
    static Tensor2ComponentIndex source_component_index_for_coembedded_component (Scalar2ComponentIndex const &, Uint32 term)
    {
        return Tensor2MultiIndex(term, term, DONT_CHECK_RANGE).as_component_index();
    }
};

// ///////////////////////////////////////////////////////////////////////////
// linear embedding of scalar 2-tensor of uniform factor type into
// corresponding 2nd symmetric power
// ///////////////////////////////////////////////////////////////////////////

template <typename Factor_, typename Scalar_>
struct LinearEmbedding_c<Scalar2TensorProductOfBasedVectorSpaces_c<Factor_,Factor_>,
                         SymmetricPowerOfBasedVectorSpace_c<2,Factor_>,
                         Scalar_,
                         NaturalEmbedding>
{
private:
    typedef Scalar2TensorProductOfBasedVectorSpaces_c<Factor_,Factor_> Diag2;
    typedef SymmetricPowerOfBasedVectorSpace_c<2,Factor_> Sym2;
    static Uint32 const DIM = DimensionOf_f<Factor_>::V;
public:
    typedef ComponentIndex_t<DimensionOf_f<Diag2>::V> Scalar2ComponentIndex;
    typedef ComponentIndex_t<DimensionOf_f<Sym2>::V> Sym2ComponentIndex;
    typedef ComponentIndex_t<DimensionOf_f<Factor_>::V> FactorComponentIndex;

    static bool embedded_component_is_procedural_zero (Sym2ComponentIndex const &i)
    {
        // Sym2 uses the lower-triangular indexing (which doesn't depend on the dimension of Factor_)
        Uint32 t = index_of_greatest_simplicial_number_leq(i.value(), 2);
        // std::cout << FORMAT_VALUE(i.value()) << ", " << FORMAT_VALUE(t) << '\n';
        FactorComponentIndex row(t-1, DONT_CHECK_RANGE);
        FactorComponentIndex col(i.value() - t*(t-1)/2, DONT_CHECK_RANGE);
        // off-diagonals are procedural zeros
        return row.value() != col.value();
    }
    static Scalar_ scalar_factor_for_embedded_component (Sym2ComponentIndex const &) { return Scalar_(1); }
    static Scalar2ComponentIndex source_component_index_for_embedded_component (Sym2ComponentIndex const &i)
    {
        return Scalar2ComponentIndex(0, DONT_CHECK_RANGE);
    }

    static Uint32 term_count_for_coembedded_component (Scalar2ComponentIndex const &) { return DIM; }
    static Scalar_ scalar_factor_for_coembedded_component (Scalar2ComponentIndex const &, Uint32) { return Scalar_(1); }
    static Sym2ComponentIndex source_component_index_for_coembedded_component (Scalar2ComponentIndex const &, Uint32 term)
    {
        return Sym2ComponentIndex(term + (term*(term+1))/2, DONT_CHECK_RANGE);
    }
};

// ///////////////////////////////////////////////////////////////////////////
// linear embedding of scalar 2-tensor into corresponding diagonal 2-tensor
// ///////////////////////////////////////////////////////////////////////////

template <typename Factor1_, typename Factor2_, typename Scalar_>
struct LinearEmbedding_c<Scalar2TensorProductOfBasedVectorSpaces_c<Factor1_,Factor2_>,
                         Diagonal2TensorProductOfBasedVectorSpaces_c<Factor1_,Factor2_>,
                         Scalar_,
                         NaturalEmbedding>
{
private:
    typedef Scalar2TensorProductOfBasedVectorSpaces_c<Factor1_,Factor2_> Scalar2;
    typedef Diagonal2TensorProductOfBasedVectorSpaces_c<Factor1_,Factor2_> Diag2;
    static bool const FACTOR1DIM_LEQ_FACTOR2DIM = DimensionOf_f<Factor1_>::V <= DimensionOf_f<Factor2_>::V;
    static Uint32 const DIM = DimensionOf_f<Factor1_>::V <= DimensionOf_f<Factor2_>::V ?
                              DimensionOf_f<Factor1_>::V :
                              DimensionOf_f<Factor2_>::V;
public:
    typedef ComponentIndex_t<DimensionOf_f<Scalar2>::V> Scalar2ComponentIndex;
    typedef ComponentIndex_t<DimensionOf_f<Diag2>::V> Diag2ComponentIndex;
    typedef ComponentIndex_t<DimensionOf_f<Factor1_>::V> Factor1ComponentIndex;
    typedef ComponentIndex_t<DimensionOf_f<Factor2_>::V> Factor2ComponentIndex;

    static bool embedded_component_is_procedural_zero (Diag2ComponentIndex const &) { return false; }
    static Scalar_ scalar_factor_for_embedded_component (Diag2ComponentIndex const &) { return Scalar_(1); }
    static Scalar2ComponentIndex source_component_index_for_embedded_component (Diag2ComponentIndex const &i)
    {
        // there is only one component for a scalar 2-tensor -- zero.
        return Scalar2ComponentIndex(0, DONT_CHECK_RANGE);
    }

    static Uint32 term_count_for_coembedded_component (Scalar2ComponentIndex const &) { return DIM; }
    static Scalar_ scalar_factor_for_coembedded_component (Scalar2ComponentIndex const &, Uint32) { return Scalar_(1); }
    static Diag2ComponentIndex source_component_index_for_coembedded_component (Scalar2ComponentIndex const &, Uint32 term)
    {
        return Diag2ComponentIndex(term, DONT_CHECK_RANGE);
    }
};

} // end of namespace Tenh

#endif // TENH_CONCEPTUAL_SCALARBASED2TENSORPRODUCT_HPP_