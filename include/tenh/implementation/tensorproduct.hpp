// ///////////////////////////////////////////////////////////////////////////
// tenh/implementation/tensorproduct.hpp by Victor Dods, created 2013/09/18
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_IMPLEMENTATION_TENSORPRODUCT_HPP_
#define TENH_IMPLEMENTATION_TENSORPRODUCT_HPP_

#include "tenh/core.hpp"

#include "tenh/conceptual/abstractindex.hpp"
#include "tenh/conceptual/diagonalbased2tensorproduct.hpp"
#include "tenh/conceptual/tensorproduct.hpp"
#include "tenh/implementation/implementationof.hpp"
#include "tenh/implementation/tensor.hpp"
#include "tenh/implementation/vector.hpp"

namespace Tenh {

// ///////////////////////////////////////////////////////////////////////////
// TensorProductOfProceduralVectors_f
// ///////////////////////////////////////////////////////////////////////////

template <typename ProceduralVectorImplementationTypeList_> struct TensorProductOfProceduralVectors_f;

// base case (1-tensor product)

namespace ComponentGeneratorEvaluator {

template <typename ProceduralVectorImplementationTypeList_, typename TensorProductOfBasedVectorSpaces_, typename Scalar_>
Scalar_ tensor_product_of_single_procedural_vector (ComponentIndex_t<DimensionOf_f<TensorProductOfBasedVectorSpaces_>::V> const &i)
{
    typedef ComponentIndex_t<DimensionOf_f<TensorProductOfBasedVectorSpaces_>::V> ComponentIndex;
    typedef typename ConceptOfEachTypeIn_f<ProceduralVectorImplementationTypeList_>::T ConceptTypeList;
    typedef TensorProductOfBasedVectorSpaces_c<ConceptTypeList> TensorProductOfBasedVectorSpaces;
    STATIC_ASSERT_TYPES_ARE_EQUAL(TensorProductOfBasedVectorSpaces_, TensorProductOfBasedVectorSpaces);
    typedef typename ProceduralVectorImplementationTypeList_::HeadType HeadImplementation;
    HeadImplementation head_implementation;
    AbstractIndex_c<'a'> a;
    return head_implementation(a)[MultiIndex_t<TypeList_t<ComponentIndex> >(i)];
}

} // end of namespace ComponentGeneratorEvaluator

template <typename HeadProceduralVectorImplementationType_>
struct TensorProductOfProceduralVectors_f<TypeList_t<HeadProceduralVectorImplementationType_> >
{
private:
    typedef TypeList_t<HeadProceduralVectorImplementationType_> ProceduralVectorImplementationTypeList_;
    typedef typename ConceptOfEachTypeIn_f<ProceduralVectorImplementationTypeList_>::T ConceptTypeList;
    typedef typename ScalarOfEachTypeIn_f<ProceduralVectorImplementationTypeList_>::T ScalarTypeList;

    enum
    {
        STATIC_ASSERT_IN_ENUM((ProceduralVectorImplementationTypeList_::LENGTH > 0), LENGTH_MUST_BE_POSITIVE),
        STATIC_ASSERT_IN_ENUM((EachTypeSatisfies_f<ConceptTypeList,IsBasedVectorSpace_p>::V), MUST_BE_TYPELIST_OF_BASED_VECTOR_SPACES),
        STATIC_ASSERT_IN_ENUM(TypeListIsUniform_t<ScalarTypeList>::V, ALL_FACTOR_TYPE_SCALARS_ARE_EQUAL),
        STATIC_ASSERT_IN_ENUM((EachTypeUsesProceduralArray_f<ProceduralVectorImplementationTypeList_>::V), MUST_BE_TYPELIST_OF_PROCEDURAL_IMPLEMENTATIONS)
    };

    typedef TensorProductOfBasedVectorSpaces_c<ConceptTypeList> TensorProductOfBasedVectorSpaces;
    typedef typename ScalarTypeList::HeadType Scalar;
    typedef ComponentGenerator_t<Scalar,
                                 DimensionOf_f<TensorProductOfBasedVectorSpaces>::V,
                                 ComponentGeneratorEvaluator::tensor_product_of_single_procedural_vector<ProceduralVectorImplementationTypeList_,
                                                                                                        TensorProductOfBasedVectorSpaces,
                                                                                                        Scalar>,
                                 TensorProduct_c<ProceduralVectorImplementationTypeList_> > ComponentGenerator;
    TensorProductOfProceduralVectors_f();
public:
    typedef ImplementationOf_t<TensorProductOfBasedVectorSpaces,Scalar,UseProceduralArray_t<ComponentGenerator> > T;
};

// recursive case

namespace ComponentGeneratorEvaluator {

template <typename ProceduralVectorImplementationTypeList_, typename TensorProductOfBasedVectorSpaces_, typename Scalar_>
Scalar_ tensor_product_of_procedural_vectors (ComponentIndex_t<DimensionOf_f<TensorProductOfBasedVectorSpaces_>::V> const &i)
{
    typedef ComponentIndex_t<DimensionOf_f<TensorProductOfBasedVectorSpaces_>::V> ComponentIndex;
    typedef typename ConceptOfEachTypeIn_f<ProceduralVectorImplementationTypeList_>::T ConceptTypeList;
    typedef TensorProductOfBasedVectorSpaces_c<ConceptTypeList> TensorProductOfBasedVectorSpaces;
    STATIC_ASSERT_TYPES_ARE_EQUAL(TensorProductOfBasedVectorSpaces_, TensorProductOfBasedVectorSpaces);
    typedef typename ProceduralVectorImplementationTypeList_::HeadType HeadImplementation;
    typedef typename TensorProductOfProceduralVectors_f<typename ProceduralVectorImplementationTypeList_::BodyTypeList>::T BodyImplementation;
    HeadImplementation head_implementation;
    BodyImplementation body_implementation;
    AbstractIndex_c<'a'> a;
    AbstractIndex_c<'b'> b;
    AbstractIndex_c<'P'> P;
    return (head_implementation(a)*body_implementation(b))
           .bundle_with_no_type_check(a*b,TensorProductOfBasedVectorSpaces_(),P)
           [MultiIndex_t<TypeList_t<ComponentIndex> >(i)];
}

} // end of namespace ComponentGeneratorEvaluator

template <typename ProceduralVectorImplementationTypeList_>
struct TensorProductOfProceduralVectors_f
{
private:
    typedef typename ConceptOfEachTypeIn_f<ProceduralVectorImplementationTypeList_>::T ConceptTypeList;
    typedef typename ScalarOfEachTypeIn_f<ProceduralVectorImplementationTypeList_>::T ScalarTypeList;

    enum
    {
        STATIC_ASSERT_IN_ENUM((ProceduralVectorImplementationTypeList_::LENGTH > 0), LENGTH_MUST_BE_POSITIVE),
        STATIC_ASSERT_IN_ENUM((EachTypeSatisfies_f<ConceptTypeList,IsBasedVectorSpace_p>::V), MUST_BE_TYPELIST_OF_BASED_VECTOR_SPACES),
        STATIC_ASSERT_IN_ENUM(TypeListIsUniform_t<ScalarTypeList>::V, ALL_FACTOR_TYPE_SCALARS_ARE_EQUAL),
        STATIC_ASSERT_IN_ENUM((EachTypeUsesProceduralArray_f<ProceduralVectorImplementationTypeList_>::V), MUST_BE_TYPELIST_OF_PROCEDURAL_IMPLEMENTATIONS)
    };

    typedef TensorProductOfBasedVectorSpaces_c<ConceptTypeList> TensorProductOfBasedVectorSpaces;
    typedef typename ScalarTypeList::HeadType Scalar;
    typedef ComponentGenerator_t<Scalar,
                                 DimensionOf_f<TensorProductOfBasedVectorSpaces>::V,
                                 ComponentGeneratorEvaluator::tensor_product_of_procedural_vectors<ProceduralVectorImplementationTypeList_,
                                                                                                  TensorProductOfBasedVectorSpaces,
                                                                                                  Scalar>,
                                 TensorProduct_c<ProceduralVectorImplementationTypeList_> > ComponentGenerator;
    TensorProductOfProceduralVectors_f();
public:
    typedef ImplementationOf_t<TensorProductOfBasedVectorSpaces,Scalar,UseProceduralArray_t<ComponentGenerator> > T;
};

// ///////////////////////////////////////////////////////////////////////////
// "parallel" tensor product of 2-tensors (i.e. tensor product of linear maps)
// ///////////////////////////////////////////////////////////////////////////

template <typename Procedural2TensorImplementationTypeList_> struct TensorProductOfProcedural2Tensors_f;

// if all the procedural 2-tensor implementations are diagonal 2-tensors, then
// the result can be a diagonal 2-tensor.  if all the implementations are
// symmetric 2-tensors, the result can be a symmetric 2-tensor, etc.  if there
// are only symmetric and diagonal 2-tensors, then the result has to be
// symmetric.  in general, there is a "meet" of 2-tensor types which gives the
// common tensor type in which their tensor product lives.  for example, the
// tensor product of a symmetric and an antisymmetric can only be (in the
// current state of the conceptual hierarchy) a non-symmetric tensor.

template <typename Procedural2TensorImplementationTypeList_> struct ConceptualTypeOfTensorProductOfProcedural2Tensors_f;

// template specialization for tensor products of diagonal 2-tensors
template <typename Factor0_,
          typename Factor1_,
          typename Scalar_,
          typename UseArrayType_,
          typename Derived_,
          typename Procedural2TensorImplementationBodyTypeList_>
struct ConceptualTypeOfTensorProductOfProcedural2Tensors_f<
    TypeList_t<ImplementationOf_t<Diagonal2TensorProductOfBasedVectorSpaces_c<Factor0_,Factor1_>,Scalar_,UseArrayType_,Derived_>,
               Procedural2TensorImplementationBodyTypeList_> >
{
private:
    typedef TypeList_t<ImplementationOf_t<Diagonal2TensorProductOfBasedVectorSpaces_c<Factor0_,Factor1_>,Scalar_,UseArrayType_,Derived_>,
                       Procedural2TensorImplementationBodyTypeList_> Procedural2TensorImplementationTypeList;
    typedef typename ConceptOfEachTypeIn_f<Procedural2TensorImplementationTypeList>::T ConceptTypeList;
    enum { STATIC_ASSERT_IN_ENUM((EachTypeSatisfies_f<ConceptTypeList,IsDiagonal2TensorProductOfBasedVectorSpaces_p>::V), MUST_BE_TYPELIST_OF_DIAGONAL_2_TENSORS) };
    typedef typename FactorNOfEachTypeIn_f<0,ConceptTypeList>::T Factor0TypeList;
    typedef typename FactorNOfEachTypeIn_f<1,ConceptTypeList>::T Factor1TypeList;
    typedef TensorProductOfBasedVectorSpaces_c<Factor0TypeList> Factor0TensorProduct;
    typedef TensorProductOfBasedVectorSpaces_c<Factor1TypeList> Factor1TensorProduct;
    ConceptualTypeOfTensorProductOfProcedural2Tensors_f();
public:
    typedef Diagonal2TensorProductOfBasedVectorSpaces_c<Factor0TensorProduct,Factor1TensorProduct> T;
};

// template specialization for tensor products of general 2-tensors
template <typename Factor0_,
          typename Factor1_,
          typename Scalar_,
          typename UseArrayType_,
          typename Derived_,
          typename Procedural2TensorImplementationBodyTypeList_>
struct ConceptualTypeOfTensorProductOfProcedural2Tensors_f<
    TypeList_t<ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<TypeList_t<Factor0_,TypeList_t<Factor1_> > >,Scalar_,UseArrayType_,Derived_>,
               Procedural2TensorImplementationBodyTypeList_> >
{
private:
    typedef TypeList_t<ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<TypeList_t<Factor0_,TypeList_t<Factor1_> > >,Scalar_,UseArrayType_,Derived_>,
                       Procedural2TensorImplementationBodyTypeList_> Procedural2TensorImplementationTypeList;
    typedef typename ConceptOfEachTypeIn_f<Procedural2TensorImplementationTypeList>::T ConceptTypeList;
    enum
    {
        STATIC_ASSERT_IN_ENUM((EachTypeSatisfies_f<ConceptTypeList,IsTensorProductOfBasedVectorSpaces_p>::V), MUST_BE_TYPELIST_OF_DIAGONAL_2_TENSORS),
        STATIC_ASSERT_IN_ENUM(EachTypeIsA2TensorProductOfBasedVectorSpaces_f<ConceptTypeList>::V, MUST_BE_TYPELIST_OF_2_TENSORS)
    };
    typedef typename FactorNOfEachTypeIn_f<0,ConceptTypeList>::T Factor0TypeList;
    typedef typename FactorNOfEachTypeIn_f<1,ConceptTypeList>::T Factor1TypeList;
    typedef TensorProductOfBasedVectorSpaces_c<Factor0TypeList> Factor0TensorProduct;
    typedef TensorProductOfBasedVectorSpaces_c<Factor1TypeList> Factor1TensorProduct;
    ConceptualTypeOfTensorProductOfProcedural2Tensors_f();
public:
    typedef TensorProductOfBasedVectorSpaces_c<TypeList_t<Factor0TensorProduct,TypeList_t<Factor1TensorProduct> > > T;
};

namespace ComponentGeneratorEvaluator {

template <typename Procedural2TensorImplementationTypeList_, typename ConceptualTypeOfTensorProduct_, typename Scalar_>
Scalar_ tensor_product_of_2_tensors (ComponentIndex_t<DimensionOf_f<ConceptualTypeOfTensorProduct_>::V> const &i)
{
    typedef ComponentIndex_t<DimensionOf_f<ConceptualTypeOfTensorProduct_>::V> ComponentIndex;
    typedef typename ConceptualTypeOfTensorProductOfProcedural2Tensors_f<Procedural2TensorImplementationTypeList_>::T ConceptualTypeOfTensorProduct;
    STATIC_ASSERT_TYPES_ARE_EQUAL(ConceptualTypeOfTensorProduct_, ConceptualTypeOfTensorProduct);
    typedef typename FactorTypeListOf_f<ConceptualTypeOfTensorProduct_>::T FactorTypeList;
    STATIC_ASSERT((FactorTypeList::LENGTH == 2), LENGTH_MUST_BE_EXACTLY_2);
    typedef typename FactorTypeList::HeadType Factor0;
    typedef typename FactorTypeList::BodyTypeList::HeadType Factor1;
    typedef typename Procedural2TensorImplementationTypeList_::HeadType HeadImplementation;
    typedef typename TensorProductOfProcedural2Tensors_f<typename Procedural2TensorImplementationTypeList_::BodyTypeList>::T BodyImplementation;
    HeadImplementation head_implementation;
    BodyImplementation body_implementation;
    AbstractIndex_c<'u'> u;
    AbstractIndex_c<'v'> v;
    AbstractIndex_c<'k'> k;
    AbstractIndex_c<'l'> l;
    AbstractIndex_c<'P'> P;
    AbstractIndex_c<'Q'> Q;
    AbstractIndex_c<'r'> r;
    return (head_implementation.split(u*v)*body_implementation.split(k*l))
           .bundle_with_no_type_check(u*k,Factor0(),P)
           .bundle_with_no_type_check(v*l,Factor1(),Q)
           .bundle(P*Q,ConceptualTypeOfTensorProduct_(),r)
           [MultiIndex_t<TypeList_t<ComponentIndex> >(i)];
}

} // end of namespace ComponentGeneratorEvaluator

// general/recursive case
template <typename Procedural2TensorImplementationTypeList_>
struct TensorProductOfProcedural2Tensors_f
{
private:
    typedef typename ConceptOfEachTypeIn_f<Procedural2TensorImplementationTypeList_>::T ConceptTypeList;
    typedef typename ScalarOfEachTypeIn_f<Procedural2TensorImplementationTypeList_>::T ScalarTypeList;
    typedef typename ConceptualTypeOfTensorProductOfProcedural2Tensors_f<Procedural2TensorImplementationTypeList_>::T ConceptualTypeOfTensorProduct;

    enum
    {
        STATIC_ASSERT_IN_ENUM(TypeListIsUniform_t<ScalarTypeList>::V, ALL_FACTOR_TYPE_SCALARS_ARE_EQUAL),
        STATIC_ASSERT_IN_ENUM((EachTypeUsesProceduralArray_f<Procedural2TensorImplementationTypeList_>::V), MUST_BE_TYPELIST_OF_PROCEDURAL_IMPLEMENTATIONS)
    };

    typedef typename ScalarTypeList::HeadType Scalar;
    typedef ComponentGenerator_t<Scalar,
                                 DimensionOf_f<ConceptualTypeOfTensorProduct>::V,
                                 ComponentGeneratorEvaluator::tensor_product_of_2_tensors<Procedural2TensorImplementationTypeList_,
                                                                                          ConceptualTypeOfTensorProduct,
                                                                                          Scalar>,
                                 TensorProduct_c<Procedural2TensorImplementationTypeList_> > ComponentGenerator;
private:
    TensorProductOfProcedural2Tensors_f();
public:
    typedef ImplementationOf_t<ConceptualTypeOfTensorProduct,Scalar,UseProceduralArray_t<ComponentGenerator> > T;
};

namespace ComponentGeneratorEvaluator {

template <typename Procedural2TensorImplementationTypeList_, typename ConceptualTypeOfTensorProduct_, typename Scalar_>
Scalar_ tensor_product_of_single_2_tensor (ComponentIndex_t<DimensionOf_f<ConceptualTypeOfTensorProduct_>::V> const &i)
{
    typedef ComponentIndex_t<DimensionOf_f<ConceptualTypeOfTensorProduct_>::V> ComponentIndex;
    typedef typename ConceptualTypeOfTensorProductOfProcedural2Tensors_f<Procedural2TensorImplementationTypeList_>::T ConceptualTypeOfTensorProduct;
    STATIC_ASSERT_TYPES_ARE_EQUAL(ConceptualTypeOfTensorProduct_, ConceptualTypeOfTensorProduct);
    typedef typename Procedural2TensorImplementationTypeList_::HeadType HeadImplementation;
    HeadImplementation head_implementation;
    AbstractIndex_c<'a'> a;
    return head_implementation(a)[MultiIndex_t<TypeList_t<ComponentIndex> >(i)];
}

} // end of namespace ComponentGeneratorEvaluator

// template specialization for tensor product of a single 2-tensor
template <typename HeadProcedural2TensorImplementationType_>
struct TensorProductOfProcedural2Tensors_f<TypeList_t<HeadProcedural2TensorImplementationType_> >
{
private:
    typedef TypeList_t<HeadProcedural2TensorImplementationType_> Procedural2TensorImplementationTypeList;
    typedef typename ConceptOfEachTypeIn_f<Procedural2TensorImplementationTypeList>::T ConceptTypeList;
    typedef typename ScalarOfEachTypeIn_f<Procedural2TensorImplementationTypeList>::T ScalarTypeList;
    typedef typename ConceptualTypeOfTensorProductOfProcedural2Tensors_f<Procedural2TensorImplementationTypeList>::T ConceptualTypeOfTensorProduct;

    enum
    {
        STATIC_ASSERT_IN_ENUM(TypeListIsUniform_t<ScalarTypeList>::V, ALL_FACTOR_TYPE_SCALARS_ARE_EQUAL),
        STATIC_ASSERT_IN_ENUM((EachTypeUsesProceduralArray_f<Procedural2TensorImplementationTypeList>::V), MUST_BE_TYPELIST_OF_PROCEDURAL_IMPLEMENTATIONS)
    };

    typedef typename ScalarTypeList::HeadType Scalar;
    typedef ComponentGenerator_t<Scalar,
                                 DimensionOf_f<ConceptualTypeOfTensorProduct>::V,
                                 ComponentGeneratorEvaluator::tensor_product_of_single_2_tensor<Procedural2TensorImplementationTypeList,
                                                                                                ConceptualTypeOfTensorProduct,
                                                                                                Scalar>,
                                 TensorProduct_c<Procedural2TensorImplementationTypeList> > ComponentGenerator;
public:
    typedef ImplementationOf_t<ConceptualTypeOfTensorProduct,Scalar,UseProceduralArray_t<ComponentGenerator> > T;
};

} // end of namespace Tenh

#endif // TENH_IMPLEMENTATION_TENSORPRODUCT_HPP_
