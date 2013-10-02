#include "conceptual_prototype.hpp"

// ///////////////////////////////////////////////////////////////////////////////////////////
// 8
// ///////////////////////////////////////////////////////////////////////////////////////////

void test_poly_in_0_dim ()
{
    std::cout << "Polynomials in 0 dimensions." << '\n';
    typedef MultivariatePolynomial<2,0,X> PolyType;
    PolyType::SymDual w(0.0f);
    MultivariatePolynomial<1,0,X>::SymDual x(1);
    std::cout << FORMAT_VALUE(w) << '\n';
    PolyType roly(w,MultivariatePolynomial<1,0,X>(x,3));
    PolyType poly(0);
    poly = poly + MultivariatePolynomial<1,0,X>(x,3);
    PolyType::Vector v(0.0f);
    std::cout << FORMAT_VALUE(roly) << '\n';
    roly.as_array().print(std::cout);
    std::cout << '\n';
    std::cout << FORMAT_VALUE(poly) << '\n';
    std::cout << FORMAT_VALUE(v) << '\n';
    std::cout << FORMAT_VALUE(roly.evaluate(v)) << '\n';
    std::cout << FORMAT_VALUE(poly.evaluate(v)) << '\n';
    std::cout << FORMAT_VALUE((poly - roly).evaluate(v)) << '\n';
    std::cout << FORMAT_VALUE((3.0f*roly).evaluate(v)) << '\n';
    std::cout << FORMAT_VALUE(roly*poly) << '\n';
    std::cout << FORMAT_VALUE((roly*poly).evaluate(v)) << '\n';
    std::cout << '\n';
}

void test_poly_in_1_dim ()
{
    std::cout << "Polynomials in 1 dimension." << '\n';
    typedef MultivariatePolynomial<2,1,X> PolyType;
    PolyType::SymDual w(0.0f);
    MultivariatePolynomial<1,1,X>::SymDual x(1);
    w[PolyType::SymDual::ComponentIndex(0, CHECK_RANGE)] = 1; // x^2
    std::cout << FORMAT_VALUE(w) << '\n';
    PolyType roly(w,MultivariatePolynomial<1,1,X>(x,3));
    PolyType poly(0);
    poly = poly + MultivariatePolynomial<1,1,X>(x,3);
    PolyType::Vector v(1.0f);
    std::cout << FORMAT_VALUE(roly) << '\n';
    roly.as_array().print(std::cout);
    std::cout << '\n';
    std::cout << FORMAT_VALUE(poly) << '\n';
    std::cout << FORMAT_VALUE(v) << '\n';
    std::cout << FORMAT_VALUE(roly.evaluate(v)) << '\n';
    std::cout << FORMAT_VALUE(poly.evaluate(v)) << '\n';
    std::cout << FORMAT_VALUE((poly - roly).evaluate(v)) << '\n';
    std::cout << FORMAT_VALUE((3.0f*roly).evaluate(v)) << '\n';
    std::cout << FORMAT_VALUE(roly*poly) << '\n';
    std::cout << FORMAT_VALUE((roly*poly).evaluate(v)) << '\n';
    std::cout << '\n';
}

