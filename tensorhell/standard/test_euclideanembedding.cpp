// ///////////////////////////////////////////////////////////////////////////
// test_euclideanembedding.cpp by Victor Dods, created 2013/04/13
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#include "test_euclideanembedding.hpp"

#include <string>

#include "tenh/euclideanembedding.hpp"
#include "tenh/tensor2.hpp"
#include "tenh/tensor2antisymmetric.hpp"
#include "tenh/tensor2diagonal.hpp"
#include "tenh/tensor2symmetric.hpp"
#include "tenh/vector.hpp"

// this is included last because it redefines the `assert` macro,
// which would be bad for the above includes.
#include "lvd_testsystem.hpp"

using namespace Lvd;
using namespace std;
using namespace TestSystem;

namespace Test {
namespace EuclideanEmbedding {

// this ensures that the dot product of vectors is the same as the Euclideanly embedded dot product
template <typename VectorType>
void verify_on_vector_type (Context const &context)
{
    typedef typename VectorType::Scalar Scalar;
    typedef VectorType V;

    Tenh::TypedIndex_t<typename V::WithStandardEuclideanBasis,'p'> p;
    Tenh::TypedIndex_t<V,'q'> q;
    Tenh::EuclideanEmbedding_t<V> e;

    // TODO: if InnerProduct_t is ever implemented like EuclideanEmbedding_t (as an implementation of
    // Tensor_i), then this nested for-loop could be replaced by showing that E^T E = InnerProduct,
    // where E is EuclideanEmbedding_t.

    for (typename V::Index i; i.is_not_at_end(); ++i)
    {
        V v_i(0);
        v_i[i] = Scalar(1);
        typename V::WithStandardEuclideanBasis w_i(Tenh::Static<Tenh::WithoutInitialization>::SINGLETON);
        w_i(p).no_alias() = e(p|q)*v_i(q);

        for (typename V::Index j; j.is_not_at_end(); ++j)
        {
            V v_j(0);
            v_j[j] = Scalar(1);
            typename V::WithStandardEuclideanBasis w_j(Tenh::Static<Tenh::WithoutInitialization>::SINGLETON);
            w_j(p).no_alias() = e(p|q)*v_j(q);

            Scalar w_i_dot_w_j(0);
            for (typename V::WithStandardEuclideanBasis::Index k; k.is_not_at_end(); ++k)
                w_i_dot_w_j += w_i[k]*w_j[k]; // NOT an expression template

            // now to compute the same dot product, but using an expression template
            Scalar v_i_dot_v_j = v_i(q)*v_j(q);

//             std::cerr << FORMAT_VALUE(i) << ", " << FORMAT_VALUE(j) << ", "
//                       << FORMAT_VALUE(w_i_dot_w_j) << ", " << FORMAT_VALUE(v_i_dot_v_j) << ", "
//                       << FORMAT_VALUE(abs(w_i_dot_w_j - v_i_dot_v_j)) << '\n';

            assert_about_eq(w_i_dot_w_j, v_i_dot_v_j);
        }
    }
}

template <typename VectorType>
void add_test_for_verify_on_vector_type (Directory *parent)
{
    LVD_ADD_NAMED_TEST_CASE_FUNCTION(parent,
                                     Tenh::type_string_of<VectorType>(),
                                     verify_on_vector_type<VectorType>,
                                     RESULT_NO_ERROR);
}

// vector types and tensor types having identical factor types
template <typename Scalar, Uint32 DIM>
void add_tests_for_verify_on_vector_type_1 (Directory *parent)
{
    typedef Tenh::Vector_t<Scalar,DIM> V;
    typedef Tenh::Tensor2Antisymmetric_t<V,V> A;
    typedef Tenh::Tensor2Symmetric_t<V,V> S;

    add_test_for_verify_on_vector_type<V>(parent);
    add_test_for_verify_on_vector_type<A>(parent);
    add_test_for_verify_on_vector_type<S>(parent);
//     add_test_for_verify_on_vector_type<Tenh::Tensor2Antisymmetric_t<A> >(parent); // TODO: allow zero-dimensional vector spaces
    add_test_for_verify_on_vector_type<Tenh::Tensor2Antisymmetric_t<S> >(parent);
    add_test_for_verify_on_vector_type<Tenh::Tensor2Symmetric_t<A> >(parent);
    add_test_for_verify_on_vector_type<Tenh::Tensor2Symmetric_t<S> >(parent);
}

// tensor types whose factors don't have to be identical
template <typename Scalar, Uint32 DIM1, Uint32 DIM2>
void add_tests_for_verify_on_vector_type_2 (Directory *parent)
{
    typedef Tenh::Vector_t<Scalar,DIM1> Factor1;
    typedef Tenh::Vector_t<Scalar,DIM2> Factor2;
    typedef Tenh::Tensor2_t<Factor1,Factor2> T;
    typedef Tenh::Tensor2Diagonal_t<Factor1,Factor2> D;

    add_test_for_verify_on_vector_type<T>(parent);
    add_test_for_verify_on_vector_type<D>(parent);
}

// this ensures that the "blown up" dot product (Frobenius inner product) is the same as the Euclideanly embedded dot product
template <typename TensorType>
void verify_on_tensor_type (Context const &context)
{
    // for now, only deal with 2-tensor types
    Assert<TensorType::ORDER == 2>();

    typedef typename TensorType::Scalar Scalar;
    typedef TensorType S;
    typedef typename TensorType::Factor1 Factor1;
    typedef typename TensorType::Factor2 Factor2;
    S s(Tenh::Static<Tenh::WithoutInitialization>::SINGLETON);

    Tenh::TypedIndex_t<Factor1,'a'> a;
    Tenh::TypedIndex_t<Factor2,'b'> b;
    Tenh::TypedIndex_t<typename S::WithStandardEuclideanBasis,'p'> p;
    Tenh::TypedIndex_t<S,'q'> q;
    Tenh::EuclideanEmbedding_t<S> e;
    for (typename S::Index i; i.is_not_at_end(); ++i)
    {
        S v_i(0);
        v_i[i] = Scalar(1);
        typename S::WithStandardEuclideanBasis w_i(Tenh::Static<Tenh::WithoutInitialization>::SINGLETON);
        w_i(p).no_alias() = e(p|q)*v_i(q);
        for (typename S::Index j; j.is_not_at_end(); ++j)
        {
            S v_j(0);
            v_j[j] = Scalar(1);
            typename S::WithStandardEuclideanBasis w_j(Tenh::Static<Tenh::WithoutInitialization>::SINGLETON);
            w_j(p).no_alias() = e(p|q)*v_j(q);

            // have to compute the dot product by hand because InnerProduct_t
            // may not be the standard inner product.
            Scalar w_i_dot_w_j(0);
            for (typename S::WithStandardEuclideanBasis::Index k; k.is_not_at_end(); ++k)
                w_i_dot_w_j += w_i[k]*w_j[k]; // NOT an expression template

            // now to compute the same dot product, but in the "blown up" space of nonsymmetric tensors
            Scalar v_i_dot_v_j = v_i.split(a|b) * v_j.split(a|b);

//             std::cerr << FORMAT_VALUE(i) << ", " << FORMAT_VALUE(j) << ", "
//                       << FORMAT_VALUE(w_i_dot_w_j) << ", " << FORMAT_VALUE(v_i_dot_v_j) << ", "
//                       << FORMAT_VALUE(abs(w_i_dot_w_j - v_i_dot_v_j)) << '\n';

            assert_about_eq(w_i_dot_w_j, v_i_dot_v_j);
        }
    }
}

template <typename TensorType>
void add_test_for_verify_on_tensor_type (Directory *parent)
{
    LVD_ADD_NAMED_TEST_CASE_FUNCTION(parent,
                                     Tenh::type_string_of<TensorType>(),
                                     verify_on_tensor_type<TensorType>,
                                     RESULT_NO_ERROR);
}

// vector types and tensor types having identical factor types
template <typename Scalar, Uint32 DIM>
void add_tests_for_verify_on_tensor_type_1 (Directory *parent)
{
    typedef Tenh::Vector_t<Scalar,DIM> V;
    typedef Tenh::Tensor2Antisymmetric_t<V,V> A;
    typedef Tenh::Tensor2Symmetric_t<V,V> S;

    add_test_for_verify_on_tensor_type<A>(parent);
    add_test_for_verify_on_tensor_type<S>(parent);
//     add_test_for_verify_on_tensor_type<Tenh::Tensor2Antisymmetric_t<A> >(parent); // TODO: allow zero-dimensional vector spaces
    add_test_for_verify_on_tensor_type<Tenh::Tensor2Antisymmetric_t<S> >(parent);
    add_test_for_verify_on_tensor_type<Tenh::Tensor2Symmetric_t<A> >(parent);
    add_test_for_verify_on_tensor_type<Tenh::Tensor2Symmetric_t<S> >(parent);
}

// tensor types whose factors don't have to be identical
template <typename Scalar, Uint32 DIM1, Uint32 DIM2>
void add_tests_for_verify_on_tensor_type_2 (Directory *parent)
{
    typedef Tenh::Vector_t<Scalar,DIM1> Factor1;
    typedef Tenh::Vector_t<Scalar,DIM2> Factor2;
    typedef Tenh::Tensor2_t<Factor1,Factor2> T;
    typedef Tenh::Tensor2Diagonal_t<Factor1,Factor2> D;

    add_test_for_verify_on_tensor_type<T>(parent);
    add_test_for_verify_on_tensor_type<D>(parent);
}

void AddTests (Directory *parent)
{
    Directory *euclidean_embedding = new Directory("EuclideanEmbedding", parent);

    {
        Directory *d = new Directory("verify_as_vector", euclidean_embedding);

//         add_tests_for_verify_on_vector_type_1<float,1>(d); // TODO: allow 0-dimensional vector spaces
        add_tests_for_verify_on_vector_type_1<float,2>(d);
        add_tests_for_verify_on_vector_type_1<float,3>(d);
        add_tests_for_verify_on_vector_type_1<float,4>(d);

//         add_tests_for_verify_on_vector_type_1<double,1>(d); // TODO: allow 0-dimensional vector spaces
        add_tests_for_verify_on_vector_type_1<double,2>(d);
        add_tests_for_verify_on_vector_type_1<double,3>(d);
        add_tests_for_verify_on_vector_type_1<double,4>(d);


        add_tests_for_verify_on_vector_type_2<float,1,1>(d);
        add_tests_for_verify_on_vector_type_2<float,1,3>(d);
        add_tests_for_verify_on_vector_type_2<float,1,4>(d);
        add_tests_for_verify_on_vector_type_2<float,3,1>(d);
        add_tests_for_verify_on_vector_type_2<float,3,3>(d);
        add_tests_for_verify_on_vector_type_2<float,3,4>(d);
        add_tests_for_verify_on_vector_type_2<float,4,1>(d);
        add_tests_for_verify_on_vector_type_2<float,4,3>(d);
        add_tests_for_verify_on_vector_type_2<float,4,4>(d);

        add_tests_for_verify_on_vector_type_2<double,1,1>(d);
        add_tests_for_verify_on_vector_type_2<double,1,3>(d);
        add_tests_for_verify_on_vector_type_2<double,1,4>(d);
        add_tests_for_verify_on_vector_type_2<double,3,1>(d);
        add_tests_for_verify_on_vector_type_2<double,3,3>(d);
        add_tests_for_verify_on_vector_type_2<double,3,4>(d);
        add_tests_for_verify_on_vector_type_2<double,4,1>(d);
        add_tests_for_verify_on_vector_type_2<double,4,3>(d);
        add_tests_for_verify_on_vector_type_2<double,4,4>(d);
    }

    {
        Directory *d = new Directory("verify_as_tensor", euclidean_embedding);

//         add_tests_for_verify_on_tensor_type_1<float,1>(d); // TODO: allow 0-dimensional vector spaces
        add_tests_for_verify_on_tensor_type_1<float,2>(d);
        add_tests_for_verify_on_tensor_type_1<float,3>(d);
        add_tests_for_verify_on_tensor_type_1<float,4>(d);

//         add_tests_for_verify_on_tensor_type_1<double,1>(d); // TODO: allow 0-dimensional vector spaces
        add_tests_for_verify_on_tensor_type_1<double,2>(d);
        add_tests_for_verify_on_tensor_type_1<double,3>(d);
        add_tests_for_verify_on_tensor_type_1<double,4>(d);


        add_tests_for_verify_on_tensor_type_2<float,1,1>(d);
        add_tests_for_verify_on_tensor_type_2<float,1,3>(d);
        add_tests_for_verify_on_tensor_type_2<float,1,4>(d);
        add_tests_for_verify_on_tensor_type_2<float,3,1>(d);
        add_tests_for_verify_on_tensor_type_2<float,3,3>(d);
        add_tests_for_verify_on_tensor_type_2<float,3,4>(d);
        add_tests_for_verify_on_tensor_type_2<float,4,1>(d);
        add_tests_for_verify_on_tensor_type_2<float,4,3>(d);
        add_tests_for_verify_on_tensor_type_2<float,4,4>(d);

        add_tests_for_verify_on_tensor_type_2<double,1,1>(d);
        add_tests_for_verify_on_tensor_type_2<double,1,3>(d);
        add_tests_for_verify_on_tensor_type_2<double,1,4>(d);
        add_tests_for_verify_on_tensor_type_2<double,3,1>(d);
        add_tests_for_verify_on_tensor_type_2<double,3,3>(d);
        add_tests_for_verify_on_tensor_type_2<double,3,4>(d);
        add_tests_for_verify_on_tensor_type_2<double,4,1>(d);
        add_tests_for_verify_on_tensor_type_2<double,4,3>(d);
        add_tests_for_verify_on_tensor_type_2<double,4,4>(d);
    }
}

} // end of namespace EuclideanEmbedding
} // end of namespace Test