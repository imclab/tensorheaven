#include "adhoc_test.hpp"

// ///////////////////////////////////////////////////////////////////////////////////////////
// 14
// ///////////////////////////////////////////////////////////////////////////////////////////

void test_embeddable_as_tensor_as_multilinear_form ()
{
    std::cout << "EmbeddableAsTensor_i as multilinear form\n";

    typedef BasedVectorSpace_c<VectorSpace_c<RealField,3,X>,OrthonormalBasis_c<X> > B3;
    typedef BasedVectorSpace_c<VectorSpace_c<RealField,4,X>,OrthonormalBasis_c<X> > B4;
    typedef DualOf_f<B3>::T DualOfB3;
    typedef DualOf_f<B4>::T DualOfB4;
    typedef TensorProductOfBasedVectorSpaces_c<TypeList_t<DualOfB3,TypeList_t<DualOfB4> > > TensorProduct;
    typedef Diagonal2TensorProductOfBasedVectorSpaces_c<DualOfB3,DualOfB4> Diagonal2TensorProduct;
    typedef SymmetricPowerOfBasedVectorSpace_c<3,DualOfB3> Sym;
    typedef ExteriorPowerOfBasedVectorSpace_c<2,DualOfB3> Alt;

    typedef float Scalar;
    typedef ImplementationOf_t<B3,Scalar> V3;
    typedef ImplementationOf_t<B4,Scalar> V4;
    typedef ImplementationOf_t<DualOfB3,Scalar> DualOfV3;
    typedef ImplementationOf_t<DualOfB4,Scalar> DualOfV4;
    typedef ImplementationOf_t<TensorProduct,Scalar> T;
    typedef ImplementationOf_t<Diagonal2TensorProduct,Scalar> D;
    typedef ImplementationOf_t<Sym,Scalar> S;
    typedef ImplementationOf_t<Alt,Scalar> A;

    T t(Static<WithoutInitialization>::SINGLETON);
    D d(Static<WithoutInitialization>::SINGLETON);
    S s(Static<WithoutInitialization>::SINGLETON);
    A a(Static<WithoutInitialization>::SINGLETON);

    for (T::ComponentIndex i; i.is_not_at_end(); ++i)
        t[i] = i.value();
    for (D::ComponentIndex i; i.is_not_at_end(); ++i)
        d[i] = i.value();
    for (S::ComponentIndex i; i.is_not_at_end(); ++i)
        s[i] = i.value();
    for (A::ComponentIndex i; i.is_not_at_end(); ++i)
        a[i] = i.value();

    V3 v1(uniform_tuple<Scalar>(8, 4, 1));
    V3 v2(uniform_tuple<Scalar>(0, 1, 3));
    V3 v3(uniform_tuple<Scalar>(-4, 2, 2));
    V4 w(uniform_tuple<Scalar>(8, -2, 5, -1));

    DualOfV3 x(uniform_tuple<Scalar>(-1, 3, 2));
    DualOfV4 y(uniform_tuple<Scalar>(1, -2, 3, 4));

    std::cout << FORMAT_VALUE(v1) << '\n';
    std::cout << FORMAT_VALUE(v2) << '\n';
    std::cout << FORMAT_VALUE(v3) << '\n';
    std::cout << FORMAT_VALUE(w) << '\n';
    std::cout << '\n';

    std::cout << FORMAT_VALUE(x) << '\n';
    std::cout << FORMAT_VALUE(y) << '\n';
    std::cout << '\n';

    AbstractIndex_c<'i'> i;
    AbstractIndex_c<'j'> j;
    AbstractIndex_c<'k'> k;
    std::cout << FORMAT_VALUE(t(i*j)) << '\n';
    std::cout << FORMAT_VALUE(d.split(i*j)) << '\n';
    std::cout << FORMAT_VALUE(s.split(i*j*k)) << '\n';
    std::cout << FORMAT_VALUE(a.split(i*j)) << '\n';
    std::cout << '\n';

    std::cout << FORMAT_VALUE(t(tuple(v1, w))) << '\n';
    std::cout << FORMAT_VALUE(d(tuple(v1, w))) << '\n';
    std::cout << FORMAT_VALUE(s(tuple(v1, v2, v3))) << '\n';
    std::cout << FORMAT_VALUE(a(tuple(v1, v2))) << '\n';
    std::cout << '\n';

    std::cout << FORMAT_VALUE(t(v1, w)) << '\n';
    std::cout << FORMAT_VALUE(d(v1, w)) << '\n';
    std::cout << FORMAT_VALUE(a(v1, v2)) << '\n';
    std::cout << '\n';

    std::cout << "the following numbers should be exactly zero:\n";
    std::cout << FORMAT_VALUE(x(tuple(v1)) - x(i)*v1(i)) << '\n';
    std::cout << FORMAT_VALUE(y(tuple(w)) - y(i)*w(i)) << '\n';
    std::cout << FORMAT_VALUE(t(tuple(v1, w)) - t(i*j)*v1(i)*w(j)) << '\n';
    std::cout << FORMAT_VALUE(d(tuple(v1, w)) - d.split(i*j)*v1(i)*w(j)) << '\n';
    std::cout << FORMAT_VALUE(s(tuple(v1, v2, v3)) - s.split(i*j*k)*v1(i)*v2(j)*v3(k)) << '\n';
    std::cout << FORMAT_VALUE(a(tuple(v1, v2)) - a.split(i*j)*v1(i)*v2(j)) << '\n';
    std::cout << FORMAT_VALUE(x(v1) - x(i)*v1(i)) << '\n';
    std::cout << FORMAT_VALUE(y(w) - y(i)*w(i)) << '\n';
    std::cout << FORMAT_VALUE(t(v1, w) - t(i*j)*v1(i)*w(j)) << '\n';
    std::cout << FORMAT_VALUE(d(v1, w) - d.split(i*j)*v1(i)*w(j)) << '\n';
    std::cout << FORMAT_VALUE(a(v1, v2) - a.split(i*j)*v1(i)*v2(j)) << '\n';
    std::cout << '\n';

    std::cout << '\n';
}

void test_product_of_abstract_indices ()
{
    AbstractIndex_c<'i'> i;
    AbstractIndex_c<'j'> j;
    AbstractIndex_c<'k'> k;
    AbstractIndex_c<'l'> l;

    std::cout << "testing product of abstract indices\n";
    std::cout << FORMAT_VALUE(symbol_string_of_abstract_index_type_list(i*j)) << '\n';
    std::cout << FORMAT_VALUE(symbol_string_of_abstract_index_type_list((i*j)*k)) << '\n';
    std::cout << FORMAT_VALUE(symbol_string_of_abstract_index_type_list(i*(j*k))) << '\n';
    std::cout << FORMAT_VALUE(symbol_string_of_abstract_index_type_list(i*j*k)) << '\n';
    std::cout << FORMAT_VALUE(symbol_string_of_abstract_index_type_list((i*j)*(k*l))) << '\n';
    std::cout << FORMAT_VALUE(symbol_string_of_abstract_index_type_list(i*(j*k*l))) << '\n';
    std::cout << FORMAT_VALUE(symbol_string_of_abstract_index_type_list((i*j*k)*l)) << '\n';
    std::cout << FORMAT_VALUE(symbol_string_of_abstract_index_type_list(i*j*k*l)) << '\n';
    std::cout << '\n';
}


