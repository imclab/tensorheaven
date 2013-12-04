#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>

#include "applications/polynomial.hpp"

#include "tenh/implementation/identity.hpp"
#include "tenh/implementation/innerproduct.hpp"
#include "tenh/implementation/vector.hpp"
#include "tenh/implementation/vee.hpp"
#include "tenh/expressiontemplate_eval.hpp"

using namespace Tenh;

// ///////////////////////////////////////////////////////////////////////////
// norm and squared norm functions, which require an inner product to use
// ///////////////////////////////////////////////////////////////////////////

// this is sort of hacky -- it would probably be better to make an
// InnerProductSpace_c conceptual type and make ImplementationOf_t aware
// of it.
// NOTE: this won't work for complex types
template <typename InnerProductId_,
          typename Derived_,
          typename Scalar_,
          typename BasedVectorSpace_,
          bool COMPONENTS_ARE_IMMUTABLE_>
Scalar_ squared_norm (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &v)
{
    AbstractIndex_c<'i'> i;
    AbstractIndex_c<'j'> j;
    AbstractIndex_c<'p'> p;
    typename InnerProduct_f<BasedVectorSpace_,InnerProductId_,Scalar_>::T inner_product;
    return inner_product(p).split(p,i|j)*v(i)*v(j); // doesn't take advantage of symmetry
}

// NOTE: this won't work for complex types
template <typename InnerProductId_,
          typename Derived_,
          typename Scalar_,
          typename BasedVectorSpace_,
          bool COMPONENTS_ARE_IMMUTABLE_>
typename AssociatedFloatingPointType_t<Scalar_>::T
    norm (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &v)
{
    return std::sqrt(squared_norm<InnerProductId_>(v));
}

// ///////////////////////////////////////////////////////////////////////////

/*
design goals
------------

- given a function and a gradient, verify that the gradient is the gradient for the function
  via numerical estimation -- using asymptotic bounds
- given a function and a gradient and a hessian, verify that the gradient and hessian are
  actually the ones for that function, analogous to previous method.
- implement Newton's method
- implement conjugate gradient method
- implement gradient descent
- implement adaptive thingy (Newton's method, congjugate gradient method, gradient descent)
*/

// uniform distribution in [0,1]
void randomize (float &x)
{
    x = float(rand()) / RAND_MAX;
}

// uniform distribution in [0,1]
void randomize (double &x)
{
    x = double(rand()) / RAND_MAX;
}

// uniform distribution in [0,1]
void randomize (long double &x)
{
    x = static_cast<long double>(rand()) / RAND_MAX;
}

// open annulus of given inner and outer radii in the vector space -- false
// is the parameter value for COMPONENTS_ARE_IMMUTABLE
template <typename InnerProductId_, typename Derived_, typename Scalar_, typename BasedVectorSpace_>
void randomize (Vector_i<Derived_,Scalar_,BasedVectorSpace_,false> &x,
                Scalar_ const &inner_radius,
                Scalar_ const &outer_radius)
{
    Scalar_ squared_inner_radius = sqr(inner_radius);
    Scalar_ squared_outer_radius = sqr(outer_radius);
    Scalar_ sqn;
    do
    {
        // generate a random vector in the cube [-1,1]^n, where n is the dimension of the space
        for (typename Vector_i<Derived_,Scalar_,BasedVectorSpace_,false>::ComponentIndex i; i.is_not_at_end(); ++i)
        {
            randomize(x[i]); // puts x[i] within [0,1]
            x[i] *= Scalar_(2)*outer_radius;
            x[i] -= outer_radius;
        }
        sqn = squared_norm<InnerProductId_>(x);
    } // if the randomly generated point is outside of the annulus, try again
    while (sqn <= squared_inner_radius || squared_outer_radius <= sqn);
}

// for arbitrary codomain
template <typename ParameterSpace_, typename CodomainSpace_, typename CodomainInnerProductId_, typename Scalar_, typename Derived_>
struct TaylorPolynomialVerifier_t
{
    typedef typename DualOf_f<ParameterSpace_>::T DualOfBasedVectorSpace;
    typedef SymmetricPowerOfBasedVectorSpace_c<2,DualOfBasedVectorSpace> Sym2Dual;
    typedef TensorProductOfBasedVectorSpaces_c<TypeList_t<CodomainSpace_,
                                               TypeList_t<DualOfBasedVectorSpace> > > Differential1;
    typedef TensorProductOfBasedVectorSpaces_c<TypeList_t<CodomainSpace_,
                                               TypeList_t<Sym2Dual> > > Differential2;

    typedef ImplementationOf_t<ParameterSpace_,Scalar_,UseMemberArray> V;
    typedef ImplementationOf_t<DualOfBasedVectorSpace,Scalar_,UseMemberArray> DualOfV;
    typedef ImplementationOf_t<Sym2Dual,Scalar_,UseMemberArray> Sym2_DualOfV;
    typedef ImplementationOf_t<CodomainSpace_,Scalar_,UseMemberArray> Out;
    typedef ImplementationOf_t<Differential1,Scalar_,UseMemberArray> D1;
    typedef ImplementationOf_t<Differential2,Scalar_,UseMemberArray> D2;

    template <typename Derived0_, bool COMPONENTS_ARE_IMMUTABLE0_,
              typename Derived1_, bool COMPONENTS_ARE_IMMUTABLE1_>
    Out evaluate_0th_order (Vector_i<Derived0_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE0_> const &based_at_point,
                            Vector_i<Derived1_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE1_> const &evaluation_point) const
    {
        return as_derived().function(based_at_point);
    }

    template <typename Derived0_, bool COMPONENTS_ARE_IMMUTABLE0_,
              typename Derived1_, bool COMPONENTS_ARE_IMMUTABLE1_>
    Out evaluate_1st_order (Vector_i<Derived0_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE0_> const &based_at_point,
                            Vector_i<Derived1_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE1_> const &evaluation_point) const
    {
        AbstractIndex_c<'i'> i;
        V delta(Static<WithoutInitialization>::SINGLETON);
        delta(i).no_alias() = evaluation_point(i) - based_at_point(i);
        return evaluate_1st_order_via_delta(based_at_point, delta);
    }

    template <typename Derived0_, bool COMPONENTS_ARE_IMMUTABLE0_,
              typename Derived1_, bool COMPONENTS_ARE_IMMUTABLE1_>
    Out evaluate_2nd_order (Vector_i<Derived0_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE0_> const &based_at_point,
                            Vector_i<Derived1_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE1_> const &evaluation_point) const
    {
        AbstractIndex_c<'i'> i;
        Out retval(Static<WithoutInitialization>::SINGLETON);
        V delta(Static<WithoutInitialization>::SINGLETON);
        delta(i).no_alias() = evaluation_point(i) - based_at_point(i);
        return evaluate_2nd_order_via_delta(based_at_point, delta);
    }

    template <typename Derived0_, bool COMPONENTS_ARE_IMMUTABLE0_,
              typename Derived1_, bool COMPONENTS_ARE_IMMUTABLE1_>
    Out evaluate_0th_order_via_delta (Vector_i<Derived0_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE0_> const &based_at_point,
                                      Vector_i<Derived1_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE1_> const &delta) const
    {
        return as_derived().function(based_at_point);
    }

    template <typename Derived0_, bool COMPONENTS_ARE_IMMUTABLE0_,
              typename Derived1_, bool COMPONENTS_ARE_IMMUTABLE1_>
    Out evaluate_1st_order_via_delta (Vector_i<Derived0_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE0_> const &based_at_point,
                                      Vector_i<Derived1_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE1_> const &delta) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        Out retval(Static<WithoutInitialization>::SINGLETON);
        retval(i).no_alias() = as_derived().function(based_at_point)(i)
                             + as_derived().D_function(based_at_point)(i|j)*delta(j);
        return retval;
    }

    template <typename Derived0_, bool COMPONENTS_ARE_IMMUTABLE0_,
              typename Derived1_, bool COMPONENTS_ARE_IMMUTABLE1_>
    Out evaluate_2nd_order_via_delta (Vector_i<Derived0_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE0_> const &based_at_point,
                                      Vector_i<Derived1_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE1_> const &delta) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'k'> k;
        AbstractIndex_c<'p'> p;
        Out retval(Static<WithoutInitialization>::SINGLETON);
        retval(i).no_alias() = as_derived().function(based_at_point)(i)
                             + as_derived().D_function(based_at_point)(i|j)*delta(j)
                             + as_derived().D2_function(based_at_point)(i|p).split(p,j|k)*delta(j)*delta(k)/2;
        return retval;
    }

    template <typename Derived0_, bool COMPONENTS_ARE_IMMUTABLE_>
    Scalar_ verify_gradient (Vector_i<Derived0_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE_> const &at_point) const
    {
        Scalar_ big_o_bound(0);
        static Uint32 const SAMPLES = 100;
        for (Uint32 i = 0; i < SAMPLES; ++i)
        {
            V delta(Static<WithoutInitialization>::SINGLETON);
            V evaluation_point(Static<WithoutInitialization>::SINGLETON);
            AbstractIndex_c<'j'> j;
            randomize<StandardInnerProduct>(delta, Scalar_(1)/100, Scalar_(2)/100);
            evaluation_point(j).no_alias() = at_point(j) + delta(j);
            // Scalar_ actual_function_value(function(at_point + delta)); // TODO: make this work?
            Out actual_function_value(as_derived().function(evaluation_point));
//             Scalar_ estimated_function_value(evaluate_1st_order(at_point, evaluation_point));
            Out estimated_function_value(evaluate_1st_order_via_delta(at_point, delta));
//             std::cerr << FORMAT_VALUE(actual_function_value) << ", " << FORMAT_VALUE(estimated_function_value) << '\n';
            Out error(Static<WithoutInitialization>::SINGLETON);
            error(j).no_alias() = actual_function_value(j) - estimated_function_value(j);
            Scalar_ asymptotic_ratio = norm<CodomainInnerProductId_>(error)
                                       /
                                       squared_norm<StandardInnerProduct>(delta);
//                                        norm<StandardInnerProduct>(delta);
            big_o_bound = std::max(big_o_bound, asymptotic_ratio);
        }
        //std::cerr << "verify_gradient(" << FORMAT_VALUE(at_point) << "): " << FORMAT_VALUE(big_o_bound) << '\n';
        return big_o_bound;
    }

    template <typename Derived0_, bool COMPONENTS_ARE_IMMUTABLE_>
    Scalar_ verify_hessian (Vector_i<Derived0_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE_> const &at_point) const
    {
        Scalar_ big_o_bound(0);
        static Uint32 const SAMPLES = 100;
        for (Uint32 i = 0; i < SAMPLES; ++i)
        {
            V delta(Static<WithoutInitialization>::SINGLETON);
            V evaluation_point(Static<WithoutInitialization>::SINGLETON);
            AbstractIndex_c<'j'> j;
            // use a solid annulus of radius [1/100, 2/200] around at_point
            randomize<StandardInnerProduct>(delta, Scalar_(1)/100, Scalar_(2)/100);
            evaluation_point(j).no_alias() = at_point(j) + delta(j);
            // Scalar_ actual_function_value(function(at_point + delta)); // TODO: make this work?
            Out actual_function_value(as_derived().function(evaluation_point));
//             Scalar_ estimated_function_value(evaluate_2nd_order(at_point, evaluation_point));
            Out estimated_function_value(evaluate_2nd_order_via_delta(at_point, delta));
            Out error(Static<WithoutInitialization>::SINGLETON);
            error(j).no_alias() = actual_function_value(j) - estimated_function_value(j);
//             std::cerr << FORMAT_VALUE(actual_function_value) << ", " << FORMAT_VALUE(estimated_function_value) << '\n';
            Scalar_ asymptotic_ratio = norm<CodomainInnerProductId_>(error)
                                       /
                                       cube(norm<StandardInnerProduct>(delta));
//                                        squared_norm<StandardInnerProduct>(delta);

            big_o_bound = std::max(big_o_bound, asymptotic_ratio);
//             std::cerr << FORMAT_VALUE(at_point) << ", " << FORMAT_VALUE(norm<StandardInnerProduct>(delta)) << ", " << FORMAT_VALUE(big_o_bound) << '\n';
        }
        //std::cerr << "verify_hessian(" << FORMAT_VALUE(at_point) << "): " << FORMAT_VALUE(big_o_bound) << '\n';
        return big_o_bound;
    }

    Derived_ const &as_derived () const { return *static_cast<Derived_ const *>(this); }
    Derived_ &as_derived () { return *static_cast<Derived_ *>(this); }
};

// for when the codomain is Scalar_
template <typename ParameterSpace_, typename Scalar_, typename Derived_>
struct TaylorPolynomialVerifier_t<ParameterSpace_,Scalar_,StandardInnerProduct,Scalar_,Derived_>
{
    typedef typename DualOf_f<ParameterSpace_>::T DualOfBasedVectorSpace;
    typedef SymmetricPowerOfBasedVectorSpace_c<2,DualOfBasedVectorSpace> Sym2Dual;
    typedef DualOfBasedVectorSpace Differential1;
    typedef Sym2Dual Differential2;

    typedef ImplementationOf_t<ParameterSpace_,Scalar_,UseMemberArray> V;
    typedef ImplementationOf_t<DualOfBasedVectorSpace,Scalar_,UseMemberArray> DualOfV;
    typedef ImplementationOf_t<Sym2Dual,Scalar_,UseMemberArray> Sym2_DualOfV;
    typedef Scalar_ Out;
    typedef DualOfV D1;
    typedef Sym2_DualOfV D2;

    template <typename Derived0_, bool COMPONENTS_ARE_IMMUTABLE0_,
              typename Derived1_, bool COMPONENTS_ARE_IMMUTABLE1_>
    Scalar_ evaluate_0th_order (Vector_i<Derived0_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE0_> const &based_at_point,
                                Vector_i<Derived1_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE1_> const &evaluation_point) const
    {
        return as_derived().function(based_at_point);
    }

    template <typename Derived0_, bool COMPONENTS_ARE_IMMUTABLE0_,
              typename Derived1_, bool COMPONENTS_ARE_IMMUTABLE1_>
    Scalar_ evaluate_1st_order (Vector_i<Derived0_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE0_> const &based_at_point,
                                Vector_i<Derived1_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE1_> const &evaluation_point) const
    {
        AbstractIndex_c<'i'> i;
        V delta(Static<WithoutInitialization>::SINGLETON);
        delta(i).no_alias() = evaluation_point(i) - based_at_point(i);
        return evaluate_1st_order_via_delta(based_at_point, delta);
    }

    template <typename Derived0_, bool COMPONENTS_ARE_IMMUTABLE0_,
              typename Derived1_, bool COMPONENTS_ARE_IMMUTABLE1_>
    Scalar_ evaluate_2nd_order (Vector_i<Derived0_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE0_> const &based_at_point,
                                Vector_i<Derived1_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE1_> const &evaluation_point) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'p'> p;
        V delta(Static<WithoutInitialization>::SINGLETON);
        delta(i).no_alias() = evaluation_point(i) - based_at_point(i);
        return evaluate_2nd_order_via_delta(based_at_point, delta);
    }

    template <typename Derived0_, bool COMPONENTS_ARE_IMMUTABLE0_,
              typename Derived1_, bool COMPONENTS_ARE_IMMUTABLE1_>
    Scalar_ evaluate_0th_order_via_delta (Vector_i<Derived0_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE0_> const &based_at_point,
                                          Vector_i<Derived1_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE1_> const &delta) const
    {
        return as_derived().function(based_at_point);
    }

    template <typename Derived0_, bool COMPONENTS_ARE_IMMUTABLE0_,
              typename Derived1_, bool COMPONENTS_ARE_IMMUTABLE1_>
    Scalar_ evaluate_1st_order_via_delta (Vector_i<Derived0_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE0_> const &based_at_point,
                                          Vector_i<Derived1_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE1_> const &delta) const
    {
        AbstractIndex_c<'i'> i;
        return as_derived().function(based_at_point)
             + as_derived().D_function(based_at_point)(i)*delta(i);
    }

    template <typename Derived0_, bool COMPONENTS_ARE_IMMUTABLE0_,
              typename Derived1_, bool COMPONENTS_ARE_IMMUTABLE1_>
    Scalar_ evaluate_2nd_order_via_delta (Vector_i<Derived0_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE0_> const &based_at_point,
                                          Vector_i<Derived1_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE1_> const &delta) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'p'> p;
        return as_derived().function(based_at_point)
             + as_derived().D_function(based_at_point)(i)*delta(i)
             + as_derived().D2_function(based_at_point)(p).split(p,i|j)*delta(i)*delta(j)/2;
    }

    template <typename Derived0_, bool COMPONENTS_ARE_IMMUTABLE_>
    Scalar_ verify_gradient (Vector_i<Derived0_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE_> const &at_point) const
    {
        Scalar_ big_o_bound(0);
        static Uint32 const SAMPLES = 10;
        for (Uint32 i = 0; i < SAMPLES; ++i)
        {
            V delta(Static<WithoutInitialization>::SINGLETON);
            V evaluation_point(Static<WithoutInitialization>::SINGLETON);
            AbstractIndex_c<'j'> j;
            randomize<StandardInnerProduct>(delta, Scalar_(1)/100, Scalar_(2)/100);
            evaluation_point(j).no_alias() = at_point(j) + delta(j);
            // Scalar_ actual_function_value(function(at_point + delta)); // TODO: make this work?
            Scalar_ actual_function_value(as_derived().function(evaluation_point));
//             Scalar_ estimated_function_value(evaluate_1st_order(at_point, evaluation_point));
            Scalar_ estimated_function_value(evaluate_1st_order_via_delta(at_point, delta));
            Scalar_ error(actual_function_value - estimated_function_value);
//             std::cerr << FORMAT_VALUE(actual_function_value) << ", " << FORMAT_VALUE(estimated_function_value) << '\n';
            Scalar_ asymptotic_ratio = fabs(error)
                                       /
                                       squared_norm<StandardInnerProduct>(delta);
//                                        norm<StandardInnerProduct>(delta);
            big_o_bound = std::max(big_o_bound, asymptotic_ratio);
        }
        //std::cerr << "verify_gradient(" << FORMAT_VALUE(at_point) << "): " << FORMAT_VALUE(big_o_bound) << '\n';
        return big_o_bound;
    }

    template <typename Derived0_, bool COMPONENTS_ARE_IMMUTABLE_>
    Scalar_ verify_hessian (Vector_i<Derived0_,Scalar_,ParameterSpace_,COMPONENTS_ARE_IMMUTABLE_> const &at_point) const
    {
        Scalar_ big_o_bound(0);
        static Uint32 const SAMPLES = 100;
        for (Uint32 i = 0; i < SAMPLES; ++i)
        {
            V delta(Static<WithoutInitialization>::SINGLETON);
            V evaluation_point(Static<WithoutInitialization>::SINGLETON);
            AbstractIndex_c<'j'> j;
            // use a solid annulus of radius [1/100, 2/200] around at_point
            randomize<StandardInnerProduct>(delta, Scalar_(1)/100, Scalar_(2)/100);
            evaluation_point(j) = at_point(j) + delta(j);
            // Scalar_ actual_function_value(function(at_point + delta)); // TODO: make this work?
            Scalar_ actual_function_value(as_derived().function(evaluation_point));
//             Scalar_ estimated_function_value(evaluate_2nd_order(at_point, evaluation_point));
            Scalar_ estimated_function_value(evaluate_2nd_order_via_delta(at_point, delta));
            Scalar_ error(actual_function_value - estimated_function_value);
//             std::cerr << FORMAT_VALUE(actual_function_value) << ", " << FORMAT_VALUE(estimated_function_value) << '\n';
            Scalar_ asymptotic_ratio = fabs(error)
                                       /
                                       cube(norm<StandardInnerProduct>(delta));
//                                        squared_norm<StandardInnerProduct>(delta);

            big_o_bound = std::max(big_o_bound, asymptotic_ratio);
//             std::cerr << FORMAT_VALUE(at_point) << ", " << FORMAT_VALUE(norm<StandardInnerProduct>(delta)) << ", " << FORMAT_VALUE(big_o_bound) << '\n';
        }
        //std::cerr << "verify_hessian(" << FORMAT_VALUE(at_point) << "): " << FORMAT_VALUE(big_o_bound) << '\n';
        return big_o_bound;
    }

    Derived_ const &as_derived () const { return *static_cast<Derived_ const *>(this); }
    Derived_ &as_derived () { return *static_cast<Derived_ *>(this); }
};

template <typename BasedVectorSpace_, typename Scalar_>
struct QuadraticFunction_t
    :
    public TaylorPolynomialVerifier_t<BasedVectorSpace_,Scalar_,StandardInnerProduct,Scalar_,QuadraticFunction_t<BasedVectorSpace_,Scalar_> >
{
    typedef TaylorPolynomialVerifier_t<BasedVectorSpace_,Scalar_,StandardInnerProduct,Scalar_,QuadraticFunction_t<BasedVectorSpace_,Scalar_> > Parent;

    typedef typename Parent::V V;
    typedef typename Parent::DualOfV DualOfV;
    typedef typename Parent::Sym2_DualOfV Sym2_DualOfV;
    typedef typename Parent::D1 D1;
    typedef typename Parent::D2 D2;

    QuadraticFunction_t ()
        :
//         m(Static<WithoutInitialization>::SINGLETON)
        m(Scalar_(0))
    {
        // won't necessarily be positive definite or even nondegenerate.
        //randomize<StandardInnerProduct>(m, Scalar_(1), Scalar_(2));
//         for (typename Sym2_DualOfV::ComponentIndex i; i.is_not_at_end(); ++i)
//             randomize(m[i]);

//         m[typename Sym2_DualOfV::ComponentIndex(0)] = 1;
//         m[typename Sym2_DualOfV::ComponentIndex(2)] = 1;
//         m[typename Sym2_DualOfV::ComponentIndex(5)] = 1;

        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'p'> p;
        for (Uint32 k = 0; k < V::DIM; ++k)
        {
            DualOfV a(Static<WithoutInitialization>::SINGLETON);
            randomize<StandardInnerProduct>(a, Scalar_(1), Scalar_(2));
            m(p) += (a(i)*a(j)).bundle(i|j,typename Sym2_DualOfV::Concept(),p);
        }
//         std::cerr << FORMAT_VALUE(m(p).split(p,i|j)) << '\n';
    }

    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    Scalar_ function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'p'> p;
        // "canonical" quadratic form whose hessian is equal to m.
        return x(i)*m(p).split(p,i|j)*x(j) / 2;
    }

    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    D1 D_function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'p'> p;
        D1 retval(Static<WithoutInitialization>::SINGLETON);
        retval(j) = x(i)*m(p).split(p,i|j);
        return retval;
    }

    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    D2 D2_function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
        return m;
    }

private:

    Sym2_DualOfV m;
};

template <typename BasedVectorSpace_, typename Scalar_>
struct SombreroFunction_t
    :
    public TaylorPolynomialVerifier_t<BasedVectorSpace_,Scalar_,StandardInnerProduct,Scalar_,SombreroFunction_t<BasedVectorSpace_,Scalar_> >
{
    typedef TaylorPolynomialVerifier_t<BasedVectorSpace_,Scalar_,StandardInnerProduct,Scalar_,SombreroFunction_t<BasedVectorSpace_,Scalar_> > Parent;

    typedef typename Parent::V V;
    typedef typename Parent::DualOfV DualOfV;
    typedef typename Parent::Sym2_DualOfV Sym2_DualOfV;
    typedef typename Parent::D1 D1;
    typedef typename Parent::D2 D2;

    SombreroFunction_t ()
        :
//         m(Static<WithoutInitialization>::SINGLETON)
        m(Scalar_(0))
    {
        // won't necessarily be positive definite or even nondegenerate.
        //randomize<StandardInnerProduct>(m, Scalar_(1), Scalar_(2));
//         for (typename Sym2_DualOfV::ComponentIndex i; i.is_not_at_end(); ++i)
//             randomize(m[i]);

//         m[typename Sym2_DualOfV::ComponentIndex(0)] = Scalar_(1);
//         m[typename Sym2_DualOfV::ComponentIndex(2)] = Scalar_(1);
//         m[typename Sym2_DualOfV::ComponentIndex(5)] = Scalar_(1);
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'p'> p;
        for (Uint32 k = 0; k < V::DIM; ++k)
        {
            DualOfV a(Static<WithoutInitialization>::SINGLETON);
            randomize<StandardInnerProduct>(a, Scalar_(1), Scalar_(2));
            m(p) += (a(i)*a(j)).bundle(i|j,typename Sym2_DualOfV::Concept(),p);
        }
//         std::cerr << FORMAT_VALUE(m(p).split(p,i|j)) << '\n';
    }

    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    Scalar_ function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'p'> p;
        return Scalar_(1) / (Scalar_(1) + x(i)*m(p).split(p,i|j)*x(j) / 2);
    }

    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    D1 D_function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'p'> p;
        D1 retval(Static<WithoutInitialization>::SINGLETON);
        retval(j) = -sqr(function(x)) * x(i)*m(p).split(p,i|j);
        return retval;
    }

    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    D2 D2_function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'k'> k;
        AbstractIndex_c<'l'> l;
        AbstractIndex_c<'p'> p;
        AbstractIndex_c<'q'> q;
        AbstractIndex_c<'r'> r;
        D2 retval(Static<WithoutInitialization>::SINGLETON);
        Scalar_ f_of_x(function(x));
        retval(r) = sqr(f_of_x) * (Scalar_(2) * f_of_x * (m(p).split(p,i|j)*x(j)*x(k)*m(q).split(q,k|l))
                                                         .bundle(i|l,typename Sym2_DualOfV::Concept(),r) - m(r));
        return retval;
    }

private:

    Sym2_DualOfV m;
};

// ///////////////////////////////////////////////////////////////////////////
// "hat" Lie algebra morphism from (V, \times) to (so(3), [.,.]), where
// so(3) \subset V \otimes V*, where V = R^3.  This has the property that
// hat(x)*y = x \times y (cross product).
// ///////////////////////////////////////////////////////////////////////////

namespace ComponentGeneratorEvaluator {

template <typename BasedVectorSpace_, typename HatMorphism_, typename Scalar_>
Scalar_ hat (ComponentIndex_t<DimensionOf_f<HatMorphism_>::V> const &i)
{
    typedef typename DualOf_f<BasedVectorSpace_>::T DualOfBasedVectorSpace;
    typedef TensorProductOfBasedVectorSpaces_c<TypeList_t<BasedVectorSpace_,
                                               TypeList_t<DualOfBasedVectorSpace> > > EndomorphismOfBasedVectorSpace;
    typedef TensorProductOfBasedVectorSpaces_c<TypeList_t<EndomorphismOfBasedVectorSpace,
                                               TypeList_t<DualOfBasedVectorSpace> > > HatMorphism;
    STATIC_ASSERT_TYPES_ARE_EQUAL(HatMorphism, HatMorphism_);

    // with EndomorphismOfBasedVectorSpaces indexed as
    // [ [ 0 1 2 ]
    //   [ 3 4 5 ]
    //   [ 6 7 8 ] ]
    // it follows that the hat morphism is indexed as
    // [ [  0  1  2  3  4  5  6  7  8 ]
    //   [  9 10 11 12 13 14 15 16 17 ]
    //   [ 18 19 20 21 22 23 24 25 26 ] ]
    // and the hat morphism's matrix looks like
    // [ [  0  0  0  0  0 -1  0  1  0 ]
    //   [  0  0  1  0  0  0 -1  0  0 ]
    //   [  0 -1  0  1  0  0  0  0  0 ] ]
    // so the 1 constants go in slots 7, 11, 21, while the -1 constants go in slots 5, 15, 19.
    switch (i.value())
    {
        case 7:
        case 11:
        case 21:
            return Scalar_(1);
        case 5:
        case 15:
        case 19:
            return Scalar_(-1);
        default:
            return Scalar_(0);
    }
}

} // end of namespace ComponentGeneratorEvaluator

struct HatId { static std::string type_as_string () { return "Hat"; } };

template <typename BasedVectorSpace_, typename Scalar_>
struct HatTensor_f
{
private:

    enum { STATIC_ASSERT_IN_ENUM((DimensionOf_f<BasedVectorSpace_>::V == 3), DIMENSION_MUST_BE_EXACTLY_3) };

    typedef typename DualOf_f<BasedVectorSpace_>::T DualOfBasedVectorSpace;
    typedef TensorProductOfBasedVectorSpaces_c<TypeList_t<BasedVectorSpace_,
                                               TypeList_t<DualOfBasedVectorSpace> > > Endomorphism;
    typedef TensorProductOfBasedVectorSpaces_c<TypeList_t<Endomorphism,
                                               TypeList_t<DualOfBasedVectorSpace> > > HatMorphism;

    typedef ComponentGenerator_t<Scalar_,
                                 DimensionOf_f<HatMorphism>::V,
                                 ::ComponentGeneratorEvaluator::hat<BasedVectorSpace_,HatMorphism,Scalar_>,
                                 HatId> ComponentGenerator;
    HatTensor_f();
public:
    typedef ImplementationOf_t<HatMorphism,Scalar_,UseImmutableArray_t<ComponentGenerator> > T;
};

// Lie algebra morphism from (R^3, \times) to (so(3), [.,.]) - as a 2-tensor
// TODO: figure out if this make sense to have for arbitrary 3-dimensional based vector spaces
template <typename Derived_, typename Scalar_, typename BasedVectorSpace_, bool COMPONENTS_ARE_IMMUTABLE_>
ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<TypeList_t<BasedVectorSpace_,
                                                      TypeList_t<typename DualOf_f<BasedVectorSpace_>::T> > >,
                   Scalar_,
                   UseMemberArray>
    hat (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x)
{
    STATIC_ASSERT(DimensionOf_f<BasedVectorSpace_>::V == 3, DIMENSION_MUST_BE_EXACTLY_3);
    // for brevity
    typedef ImplementationOf_t<TensorProductOfBasedVectorSpaces_c<TypeList_t<BasedVectorSpace_,
                                                      TypeList_t<typename DualOf_f<BasedVectorSpace_>::T> > >,
                               Scalar_,
                               UseMemberArray> T;
    typedef typename Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_>::ComponentIndex c;
    typedef typename T::ComponentIndex C;
    T retval(Static<WithoutInitialization>::SINGLETON);
    retval[C(0)] = Scalar_(0);
    retval[C(1)] = -x[c(2)];
    retval[C(2)] =  x[c(1)];
    retval[C(3)] =  x[c(2)];
    retval[C(4)] = Scalar_(0);
    retval[C(5)] = -x[c(0)];
    retval[C(6)] = -x[c(1)];
    retval[C(7)] =  x[c(0)];
    retval[C(8)] = Scalar_(0);
    return retval;
}

// ///////////////////////////////////////////////////////////////////////////
// end of "hat" stuff
// ///////////////////////////////////////////////////////////////////////////

template <typename BasedVectorSpace_, typename Scalar_>
struct J_t
    :
    public TaylorPolynomialVerifier_t<BasedVectorSpace_,Scalar_,StandardInnerProduct,Scalar_,J_t<BasedVectorSpace_,Scalar_> >
{
private:

    enum { _ = Assert<DimensionOf_f<BasedVectorSpace_>::V == 3>::V };

public:

    typedef TaylorPolynomialVerifier_t<BasedVectorSpace_,Scalar_,StandardInnerProduct,Scalar_,J_t<BasedVectorSpace_,Scalar_> > Parent;

    typedef typename Parent::DualOfBasedVectorSpace DualOfBasedVectorSpace;
    typedef typename Parent::Sym2Dual Sym2Dual;

    typedef typename Parent::V V;
    typedef typename Parent::DualOfV DualOfV;
    typedef typename Parent::Sym2_DualOfV Sym2_DualOfV;
    typedef typename Parent::Out Out;
    typedef typename Parent::D1 D1;
    typedef typename Parent::D2 D2;

    J_t ()
        :
        m_D2(Static<WithoutInitialization>::SINGLETON)
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'p'> p;
        AbstractIndex_c<'q'> q;
        // this really should use the natural embedding of the diagonal (inner product) into Sym2Dual
        m_D2(p) = (Scalar_(-2)*m_inner_product(q)).split(q,i|j).bundle(i|j,typename D2::Concept(),p);
    }

    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    Out function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'p'> p;
        return Scalar_(1) - x(i)*m_inner_product(p).split(p,i|j)*x(j);
    }
    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    D1 D_function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'p'> p;
        D1 retval(Static<WithoutInitialization>::SINGLETON);
        retval(j).no_alias() = x(i)*m_D2(p).split(p,i|j); //Scalar_(-2)*x(i)*m_inner_product(p).split(p,i|j);
        return retval;
    }
    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    D2 D2_function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
//         AbstractIndex_c<'i'> i;
//         AbstractIndex_c<'j'> j;
//         AbstractIndex_c<'p'> p;
//         D2 retval(Static<WithoutInitialization>::SINGLETON);
//         retval(p).no_alias() = Scalar_(-2)*m_inner_product(p);
//         return retval;
        return m_D2;
    }

private:

    typename InnerProduct_f<BasedVectorSpace_,StandardInnerProduct,Scalar_>::T m_inner_product;
    D2 m_D2;
};

template <typename BasedVectorSpace_, typename Scalar_>
struct K_t
    :
    public TaylorPolynomialVerifier_t<BasedVectorSpace_,Scalar_,StandardInnerProduct,Scalar_,K_t<BasedVectorSpace_,Scalar_> >
{
private:

    enum { _ = Assert<DimensionOf_f<BasedVectorSpace_>::V == 3>::V };

public:

    typedef TaylorPolynomialVerifier_t<BasedVectorSpace_,Scalar_,StandardInnerProduct,Scalar_,K_t<BasedVectorSpace_,Scalar_> > Parent;

    typedef typename Parent::DualOfBasedVectorSpace DualOfBasedVectorSpace;
    typedef typename Parent::Sym2Dual Sym2Dual;

    typedef typename Parent::V V;
    typedef typename Parent::DualOfV DualOfV;
    typedef typename Parent::Sym2_DualOfV Sym2_DualOfV;
    typedef typename Parent::Out Out;
    typedef typename Parent::D1 D1;
    typedef typename Parent::D2 D2;

    K_t ()
        :
        m_form(Static<WithoutInitialization>::SINGLETON)
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'p'> p;
        AbstractIndex_c<'q'> q;
        // this really should use the natural embedding of the diagonal (inner product) into Sym2Dual
        m_form(p) = (Scalar_(2)*m_inner_product(q)).split(q,i|j).bundle(i|j,typename D2::Concept(),p);
    }

    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    Out function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'p'> p;
        return Scalar_(1) / (Scalar_(1) + x(i)*m_inner_product(p).split(p,i|j)*x(j));
    }
    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    D1 D_function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'p'> p;
        D1 retval(Static<WithoutInitialization>::SINGLETON);
        retval(j).no_alias() = -sqr(function(x))*x(i)*m_form(p).split(p,i|j);
        return retval;
    }
    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    D2 D2_function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
        Scalar_ f_of_x(function(x));
        AbstractIndex_c<'a'> a;
        AbstractIndex_c<'b'> b;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'k'> k;
        AbstractIndex_c<'p'> p;
        AbstractIndex_c<'q'> q;
        AbstractIndex_c<'r'> r;
        D2 retval(Static<WithoutInitialization>::SINGLETON);
        retval(p).no_alias() = sqr(f_of_x)
                             * (  Scalar_(2)*f_of_x*(m_form(q).split(q,j|a)*x(a)*x(b)*m_form(r).split(r,b|k)).bundle(j|k,typename D2::Concept(),p)
                                - m_form(p));
        return retval;
    }

private:

    typename InnerProduct_f<BasedVectorSpace_,StandardInnerProduct,Scalar_>::T m_inner_product;
    D2 m_form;
};

template <typename BasedVectorSpace_, typename Scalar_>
struct N_t
    :
    public TaylorPolynomialVerifier_t<BasedVectorSpace_,
                                      TensorProductOfBasedVectorSpaces_c<TypeList_t<BasedVectorSpace_,
                                                                         TypeList_t<typename DualOf_f<BasedVectorSpace_>::T> > >,
                                      TensorProduct_c<TypeList_t<StandardInnerProduct,
                                                      TypeList_t<StandardInnerProduct> > >,
                                      Scalar_,
                                      N_t<BasedVectorSpace_,Scalar_> >
{
private:

    enum { _ = Assert<DimensionOf_f<BasedVectorSpace_>::V == 3>::V };

public:

    typedef TaylorPolynomialVerifier_t<BasedVectorSpace_,
                                       TensorProductOfBasedVectorSpaces_c<TypeList_t<BasedVectorSpace_,
                                                                          TypeList_t<typename DualOf_f<BasedVectorSpace_>::T> > >,
                                       TensorProduct_c<TypeList_t<StandardInnerProduct,
                                                       TypeList_t<StandardInnerProduct> > >,
                                       Scalar_,
                                       N_t<BasedVectorSpace_,Scalar_> > Parent;

    typedef typename Parent::DualOfBasedVectorSpace DualOfBasedVectorSpace;
    typedef typename Parent::Sym2Dual Sym2Dual;

    typedef typename Parent::V V;
    typedef typename Parent::DualOfV DualOfV;
    typedef typename Parent::Sym2_DualOfV Sym2_DualOfV;
    typedef typename Parent::Out Out;
    typedef typename Parent::D1 D1;
    typedef typename Parent::D2 D2;

    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    Out function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'p'> p;
        AbstractIndex_c<'A'> A;
        Out retval(Static<WithoutInitialization>::SINGLETON);
        retval(i|j).no_alias() = m_J.function(x)*m_identity(A).split(A,i|j)
                               + Scalar_(2) * (x(i)*m_inner_product(A).split(A,j|p)*x(p) + hat(x)(i|j));
        return retval;
    }
    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    D1 D_function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'k'> k;
        AbstractIndex_c<'p'> p;
        AbstractIndex_c<'A'> A;
        AbstractIndex_c<'B'> B;
        AbstractIndex_c<'C'> C;
        D1 retval(Static<WithoutInitialization>::SINGLETON);
        retval(C|k).no_alias() = (  m_J.D_function(x)(k)*m_identity(A).split(A,i|j)
                                  + Scalar_(2) * (  m_identity(A).split(A,i|k)*m_inner_product(B).split(B,j|p)*x(p)
                                                  + x(i)*m_inner_product(B).split(B,j|k)
                                                  + m_hat_tensor(A|k).split(A,i|j)))
                                 .bundle(i|j,typename Out::Concept(),C);
        return retval;
    }
    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    D2 D2_function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'k'> k;
        AbstractIndex_c<'l'> l;
        AbstractIndex_c<'p'> p;
        AbstractIndex_c<'q'> q;
        AbstractIndex_c<'r'> r;
        AbstractIndex_c<'B'> B;
        AbstractIndex_c<'C'> C;
        D2 retval(Static<WithoutInitialization>::SINGLETON);
        retval(C|p).no_alias() = m_identity(B).split(B,C)*m_J.D2_function(x)(p)
                               + Scalar_(2)
                               * (  (m_identity(q).split(q,i|k)*m_inner_product(r).split(r,j|l))
                                    .bundle(i|j,typename Out::Concept(),C)
                                    .bundle(k|l,typename Sym2_DualOfV::Concept(),p)
                                  + (m_identity(q).split(q,i|l)*m_inner_product(r).split(r,j|k))
                                    .bundle(i|j,typename Out::Concept(),C)
                                    .bundle(k|l,typename Sym2_DualOfV::Concept(),p));
        return retval;
    }

private:

    J_t<BasedVectorSpace_,Scalar_> m_J;
    typename Identity_f<BasedVectorSpace_,Scalar_>::T m_identity;
    typename InnerProduct_f<BasedVectorSpace_,StandardInnerProduct,Scalar_>::T m_inner_product;
    typename InnerProduct_f<DualOfBasedVectorSpace,StandardInnerProduct,Scalar_>::T m_inner_product_inverse;
    typename HatTensor_f<BasedVectorSpace_,Scalar_>::T m_hat_tensor;
};

template <typename BasedVectorSpace_, typename Scalar_>
struct CayleyTransform_t
    :
    public TaylorPolynomialVerifier_t<BasedVectorSpace_,
                                      TensorProductOfBasedVectorSpaces_c<TypeList_t<BasedVectorSpace_,
                                                                         TypeList_t<typename DualOf_f<BasedVectorSpace_>::T> > >,
                                      TensorProduct_c<TypeList_t<StandardInnerProduct,
                                                      TypeList_t<StandardInnerProduct> > >,
                                      Scalar_,
                                      CayleyTransform_t<BasedVectorSpace_,Scalar_> >
{
private:

    enum { _ = Assert<DimensionOf_f<BasedVectorSpace_>::V == 3>::V };

public:

    typedef TaylorPolynomialVerifier_t<BasedVectorSpace_,
                                       TensorProductOfBasedVectorSpaces_c<TypeList_t<BasedVectorSpace_,
                                                                          TypeList_t<typename DualOf_f<BasedVectorSpace_>::T> > >,
                                       TensorProduct_c<TypeList_t<StandardInnerProduct,
                                                       TypeList_t<StandardInnerProduct> > >,
                                       Scalar_,
                                       CayleyTransform_t<BasedVectorSpace_,Scalar_> > Parent;

    typedef typename Parent::DualOfBasedVectorSpace DualOfBasedVectorSpace;
    typedef typename Parent::Sym2Dual Sym2Dual;

    typedef typename Parent::V V;
    typedef typename Parent::DualOfV DualOfV;
    typedef typename Parent::Sym2_DualOfV Sym2_DualOfV;
    typedef typename Parent::Out Out;
    typedef typename Parent::D1 D1;
    typedef typename Parent::D2 D2;

    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    Out function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
        AbstractIndex_c<'i'> i;
        Out retval(Static<WithoutInitialization>::SINGLETON);
        retval(i).no_alias() = m_K.function(x) * m_N.function(x)(i);
        return retval;
    }
    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    D1 D_function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        D1 retval(Static<WithoutInitialization>::SINGLETON);
//         retval(i|j).no_alias() = D_K(x)(j)*N(x)(i) + K(x)*D_N(x)(i|j); // this should work but Tenh complains about the ordering
        retval(i|j).no_alias() = m_N.function(x)(i)*m_K.D_function(x)(j) + m_K.function(x)*m_N.D_function(x)(i|j);
        return retval;
    }
    template <typename Derived_, bool COMPONENTS_ARE_IMMUTABLE_>
    D2 D2_function (Vector_i<Derived_,Scalar_,BasedVectorSpace_,COMPONENTS_ARE_IMMUTABLE_> const &x) const
    {
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'k'> k;
        AbstractIndex_c<'p'> p;
        AbstractIndex_c<'C'> C;
        D2 retval(Static<WithoutInitialization>::SINGLETON);
        retval(C|p).no_alias() = m_N.function(x)(C)*m_K.D2_function(x)(p)
                               + (m_N.D_function(x)(C|k)*m_K.D_function(x)(j)).bundle(j|k,typename Sym2_DualOfV::Concept(),p)
                               + (m_K.D_function(x)(k)*m_N.D_function(x)(C|j)).bundle(j|k,typename Sym2_DualOfV::Concept(),p)
                               + m_K.function(x)*m_N.D2_function(x)(C|p);
        return retval;
    }

private:

    K_t<BasedVectorSpace_,Scalar_> m_K;
    N_t<BasedVectorSpace_,Scalar_> m_N;
    typename Identity_f<BasedVectorSpace_,Scalar_>::T m_identity;
    typename InnerProduct_f<BasedVectorSpace_,StandardInnerProduct,Scalar_>::T m_inner_product;
    typename InnerProduct_f<DualOfBasedVectorSpace,StandardInnerProduct,Scalar_>::T m_inner_product_inverse;
    typename HatTensor_f<BasedVectorSpace_,Scalar_>::T m_hat_tensor;
};

int main (int argc, char **argv)
{
    typedef double Scalar;
    static Uint32 const DIM = 3;
    typedef BasedVectorSpace_c<VectorSpace_c<RealField,DIM,Generic>,OrthonormalBasis_c<Generic> > BasedVectorSpace;
    typedef ImplementationOf_t<BasedVectorSpace,Scalar> V;

    typedef CayleyTransform_t<BasedVectorSpace,Scalar> C;
    C c;
    V x(Static<WithoutInitialization>::SINGLETON);
    randomize<StandardInnerProduct>(x, Scalar(0), Scalar(10));
//     std::cerr << FORMAT_VALUE(c.N(x)) << "\n\n";
//     std::cerr << FORMAT_VALUE(c.D_N(x)) << "\n\n";
    std::cerr << FORMAT_VALUE(c.function(x)) << "\n\n";
    std::cerr << FORMAT_VALUE(c.D_function(x)) << "\n\n";
//     std::cerr << FORMAT_VALUE(c.D2_function(x)) << "\n\n";

    InnerProduct_f<BasedVectorSpace,StandardInnerProduct,Scalar>::T inner_product;
    InnerProduct_f<DualOf_f<BasedVectorSpace>::T,StandardInnerProduct,Scalar>::T inner_product_inverse;
    for (Uint32 it = 0; it < 10; ++it)
    {
        randomize<StandardInnerProduct>(x, Scalar(0), Scalar(10));
        C::Out cayley_transform(c.function(x));
//         std::cerr << FORMAT_VALUE(cayley_transform) << "\n\n";
        AbstractIndex_c<'i'> i;
        AbstractIndex_c<'j'> j;
        AbstractIndex_c<'k'> k;
        AbstractIndex_c<'l'> l;
        AbstractIndex_c<'p'> p;
//         std::cerr << FORMAT_VALUE(cayley_transform(i|j)*inner_product(p).split(p,i|k)*cayley_transform(k|l)) << "\n\n";
//         std::cerr << FORMAT_VALUE((cayley_transform(i|j)*inner_product(p).split(p,i|k)*cayley_transform(k|l)).eval().tensor_value()) << "\n\n";
//         std::cerr << FORMAT_VALUE(cayley_transform(i|j)*inner_product(p).split(p,i|k)*cayley_transform(k|l) - inner_product_inverse(p).split(p,j|l)) << "\n\n";
//         std::cerr << FORMAT_VALUE((cayley_transform(i|j)*inner_product(p).split(p,i|k)*cayley_transform(k|l) - inner_product_inverse(p).split(p,j|l)).eval().tensor_value()) << "\n\n";
        typedef TypeList_t<StandardInnerProduct,
                TypeList_t<StandardInnerProduct> > InnerProductFactorList;
        std::cerr << "this value should be about equal to 0: " << FORMAT_VALUE(squared_norm<TensorProduct_c<InnerProductFactorList> >((cayley_transform(i|j)*inner_product(p).split(p,i|k)*cayley_transform(k|l) - inner_product_inverse(p).split(p,j|l)).eval().tensor_value())) << "\n\n";
    }

    static Uint32 const SAMPLES = 1000;
//     std::cerr << "QuadraticFunction_t\n";
//     for (Uint32 j = 0; j < 10; ++j)
//     {
//         Scalar big_o_bound(0);
//         QuadraticFunction_t<BasedVectorSpace,Scalar> helpy;
//         for (Uint32 i = 0; i < SAMPLES; ++i)
//         {
//             V at_point(Static<WithoutInitialization>::SINGLETON);
//             randomize<StandardInnerProduct>(at_point, Scalar(0), Scalar(10));
//             big_o_bound = std::max(big_o_bound, helpy.verify_gradient(at_point));
//         }
//         std::cerr << "max big-o-bound for gradient verification = " << big_o_bound << '\n';
//         big_o_bound = Scalar(0);
//         for (Uint32 i = 0; i < SAMPLES; ++i)
//         {
//             V at_point(Static<WithoutInitialization>::SINGLETON);
//             randomize<StandardInnerProduct>(at_point, Scalar(0), Scalar(10));
//             big_o_bound = std::max(big_o_bound, helpy.verify_hessian(at_point));
//         }
//         std::cerr << "max big-o-bound for hessian verification = " << big_o_bound << '\n';
//     }
//     std::cerr << '\n';
//     std::cerr << "SombreroFunction_t\n";
//     for (Uint32 j = 0; j < 10; ++j)
//     {
//         Scalar big_o_bound(0);
//         SombreroFunction_t<BasedVectorSpace,Scalar> helpy;
//         for (Uint32 i = 0; i < SAMPLES; ++i)
//         {
//             V at_point(Static<WithoutInitialization>::SINGLETON);
//             randomize<StandardInnerProduct>(at_point, Scalar(0), Scalar(10));
//             big_o_bound = std::max(big_o_bound, helpy.verify_gradient(at_point));
//         }
//         std::cerr << "max big-o-bound for gradient verification = " << big_o_bound << '\n';
//         big_o_bound = Scalar(0);
//         for (Uint32 i = 0; i < SAMPLES; ++i)
//         {
//             V at_point(Static<WithoutInitialization>::SINGLETON);
//             randomize<StandardInnerProduct>(at_point, Scalar(0), Scalar(10));
//             big_o_bound = std::max(big_o_bound, helpy.verify_hessian(at_point));
//         }
//         std::cerr << "max big-o-bound for hessian verification = " << big_o_bound << '\n';
//     }
    std::cerr << "J_t\n";
    {
        Scalar big_o_bound(0);
        J_t<BasedVectorSpace,Scalar> helpy;
        for (Uint32 i = 0; i < SAMPLES; ++i)
        {
            V at_point(Static<WithoutInitialization>::SINGLETON);
            randomize<StandardInnerProduct>(at_point, Scalar(0), Scalar(100));
            big_o_bound = std::max(big_o_bound, helpy.verify_gradient(at_point));
        }
        std::cerr << "max big-o-bound for gradient verification = " << big_o_bound << '\n';
        big_o_bound = Scalar(0);
        for (Uint32 i = 0; i < SAMPLES; ++i)
        {
            V at_point(Static<WithoutInitialization>::SINGLETON);
            randomize<StandardInnerProduct>(at_point, Scalar(0), Scalar(100));
            big_o_bound = std::max(big_o_bound, helpy.verify_hessian(at_point));
        }
        std::cerr << "max big-o-bound for hessian verification = " << big_o_bound << '\n';
    }
    std::cerr << '\n';
    std::cerr << "K_t\n";
    {
        Scalar big_o_bound(0);
        K_t<BasedVectorSpace,Scalar> helpy;
        for (Uint32 i = 0; i < SAMPLES; ++i)
        {
            V at_point(Static<WithoutInitialization>::SINGLETON);
            randomize<StandardInnerProduct>(at_point, Scalar(1)/2, Scalar(100));
            big_o_bound = std::max(big_o_bound, helpy.verify_gradient(at_point));
        }
        std::cerr << "max big-o-bound for gradient verification = " << big_o_bound << '\n';
        big_o_bound = Scalar(0);
        for (Uint32 i = 0; i < SAMPLES; ++i)
        {
            V at_point(Static<WithoutInitialization>::SINGLETON);
            randomize<StandardInnerProduct>(at_point, Scalar(1)/2, Scalar(100));
            big_o_bound = std::max(big_o_bound, helpy.verify_hessian(at_point));
        }
        std::cerr << "max big-o-bound for hessian verification = " << big_o_bound << '\n';
    }
    std::cerr << '\n';
    std::cerr << "N_t\n";
    {
        Scalar big_o_bound(0);
        N_t<BasedVectorSpace,Scalar> helpy;
        for (Uint32 i = 0; i < SAMPLES; ++i)
        {
            V at_point(Static<WithoutInitialization>::SINGLETON);
            randomize<StandardInnerProduct>(at_point, Scalar(1)/2, Scalar(100));
            big_o_bound = std::max(big_o_bound, helpy.verify_gradient(at_point));
        }
        std::cerr << "max big-o-bound for gradient verification = " << big_o_bound << '\n';
        big_o_bound = Scalar(0);
        for (Uint32 i = 0; i < SAMPLES; ++i)
        {
            V at_point(Static<WithoutInitialization>::SINGLETON);
            randomize<StandardInnerProduct>(at_point, Scalar(1)/2, Scalar(100));
            big_o_bound = std::max(big_o_bound, helpy.verify_hessian(at_point));
        }
        std::cerr << "max big-o-bound for hessian verification = " << big_o_bound << '\n';
    }
    std::cerr << '\n';
    std::cerr << "CayleyTransform_t\n";
    {
        Scalar big_o_bound(0);
        CayleyTransform_t<BasedVectorSpace,Scalar> helpy;
        for (Uint32 i = 0; i < SAMPLES; ++i)
        {
            V at_point(Static<WithoutInitialization>::SINGLETON);
            randomize<StandardInnerProduct>(at_point, Scalar(1)/2, Scalar(100));
            big_o_bound = std::max(big_o_bound, helpy.verify_gradient(at_point));
        }
        std::cerr << "max big-o-bound for gradient verification = " << big_o_bound << '\n';
        big_o_bound = Scalar(0);
        for (Uint32 i = 0; i < SAMPLES; ++i)
        {
            V at_point(Static<WithoutInitialization>::SINGLETON);
            randomize<StandardInnerProduct>(at_point, Scalar(1)/2, Scalar(100));
            big_o_bound = std::max(big_o_bound, helpy.verify_hessian(at_point));
        }
        std::cerr << "max big-o-bound for hessian verification = " << big_o_bound << '\n';
    }
    std::cerr << '\n';
    return 0;
}
