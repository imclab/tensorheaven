// ///////////////////////////////////////////////////////////////////////////
// applications/polynomial_utility.hpp by Ted Nitz, created 2013/09/27
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef APPLICATIONS_POLYNOMIAL_UTILITY_HPP_
#define APPLICATIONS_POLYNOMIAL_UTILITY_HPP_

#include <ostream>

#include "applications/polynomial.hpp"
#include "applications/homogeneouspolynomial.hpp"

//    operator<< for ostream
template <Uint32 DEG, typename BasedVectorSpace_, typename Scalar>
struct VariableStringComputer
{
    static std::string compute(typename HomogeneousPolynomial<DEG,BasedVectorSpace_,Scalar>::Sym::MultiIndex const &n)
    {
        typedef typename HomogeneousPolynomial<DEG,BasedVectorSpace_,Scalar>::Sym::MultiIndex MultiIndex;
        MultiIndex m = Tenh::sorted<typename MultiIndex::IndexTypeList,std::less<Uint32> >(n);
        Uint32 last = m.value_of_index(0, Tenh::DONT_CHECK_RANGE);
        Uint32 count = 1;
        std::string result = "x" + AS_STRING(last);
        for (Uint32 i = 1; i < MultiIndex::LENGTH; ++i)
        {
            if (m.value_of_index(i, Tenh::DONT_CHECK_RANGE) == last)
            {
                ++count;
            }
            else
            {
                last = m.value_of_index(i, Tenh::DONT_CHECK_RANGE);
                if (count != 1)
                {
                    result += "^" + AS_STRING(count);
                }
                result += "*x" + AS_STRING(last);
                count = 1;
            }
        }
        if (count != 1)
        {
            result += "^" + AS_STRING(count);
        }

        return result;
    }
private:
    VariableStringComputer();
};

template <Uint32 DEG, typename ScalarField_, typename VectorSpaceId_, typename Basis_, typename Scalar>
struct VariableStringComputer<DEG,Tenh::BasedVectorSpace_c<Tenh::VectorSpace_c<ScalarField_,0,VectorSpaceId_>,Basis_>,Scalar>
{
    static std::string compute(typename HomogeneousPolynomial<DEG,Tenh::BasedVectorSpace_c<Tenh::VectorSpace_c<ScalarField_,0,VectorSpaceId_>,Basis_>,Scalar>::Sym::MultiIndex const &n)
    {
        return "";
    }
private:
    VariableStringComputer();
};


template <Uint32 DEG, typename ScalarField_, typename VectorSpaceId_, typename Basis_, typename Scalar>
struct VariableStringComputer<DEG,Tenh::BasedVectorSpace_c<Tenh::VectorSpace_c<ScalarField_,1,VectorSpaceId_>,Basis_>,Scalar>
{
    static std::string compute(typename HomogeneousPolynomial<DEG,Tenh::BasedVectorSpace_c<Tenh::VectorSpace_c<ScalarField_,1,VectorSpaceId_>,Basis_>,Scalar>::Sym::MultiIndex const &n)
    {
        typedef typename HomogeneousPolynomial<DEG,Tenh::BasedVectorSpace_c<Tenh::VectorSpace_c<ScalarField_,1,VectorSpaceId_>,Basis_>,Scalar>::Sym::MultiIndex MultiIndex;
        return "x" + ((MultiIndex::LENGTH > 1) ? "^" + AS_STRING(MultiIndex::LENGTH) : "");
    }
private:
    VariableStringComputer();
};

template <Uint32 DEG, typename ScalarField_, typename VectorSpaceId_, typename Basis_, typename Scalar>
struct VariableStringComputer<DEG,Tenh::BasedVectorSpace_c<Tenh::VectorSpace_c<ScalarField_,2,VectorSpaceId_>,Basis_>,Scalar>
{
    static std::string compute(typename HomogeneousPolynomial<DEG,Tenh::BasedVectorSpace_c<Tenh::VectorSpace_c<ScalarField_,2,VectorSpaceId_>,Basis_>,Scalar>::Sym::MultiIndex const &n)
    {
        typedef typename HomogeneousPolynomial<DEG,Tenh::BasedVectorSpace_c<Tenh::VectorSpace_c<ScalarField_,2,VectorSpaceId_>,Basis_>,Scalar>::Sym::MultiIndex MultiIndex;
        MultiIndex m = Tenh::sorted<typename MultiIndex::IndexTypeList,std::less<Uint32> >(n);
        char vars[3] = "xy";
        Uint32 last = m.value_of_index(0, Tenh::DONT_CHECK_RANGE);
        Uint32 count = 1;
        std::string result;
        result.push_back(vars[last]);
        for (Uint32 i = 1; i < MultiIndex::LENGTH; ++i)
        {
            if (m.value_of_index(i, Tenh::DONT_CHECK_RANGE) == last)
            {
                ++count;
            }
            else
            {
                last = m.value_of_index(i, Tenh::DONT_CHECK_RANGE);
                if (count != 1)
                {
                    result += "^" + AS_STRING(count);
                }
                result += "*";
                result.push_back(vars[last]);
                count = 1;
            }
        }
        if (count != 1)
        {
            result += "^" + AS_STRING(count);
        }

        return result;
    }
private:
    VariableStringComputer();
};

template <Uint32 DEG, typename ScalarField_, typename VectorSpaceId_, typename Basis_, typename Scalar>
struct VariableStringComputer<DEG,Tenh::BasedVectorSpace_c<Tenh::VectorSpace_c<ScalarField_,3,VectorSpaceId_>,Basis_>,Scalar>
{
    static std::string compute(typename HomogeneousPolynomial<DEG,Tenh::BasedVectorSpace_c<Tenh::VectorSpace_c<ScalarField_,3,VectorSpaceId_>,Basis_>,Scalar>::Sym::MultiIndex const &n)
    {
        typedef typename HomogeneousPolynomial<DEG,Tenh::BasedVectorSpace_c<Tenh::VectorSpace_c<ScalarField_,3,VectorSpaceId_>,Basis_>,Scalar>::Sym::MultiIndex MultiIndex;
        MultiIndex m = Tenh::sorted<typename MultiIndex::IndexTypeList,std::less<Uint32> >(n);
        char vars[4] = "xyz";
        Uint32 last = m.value_of_index(0, Tenh::DONT_CHECK_RANGE);
        Uint32 count = 1;
        std::string result;
        result.push_back(vars[last]);
        for (Uint32 i = 1; i < MultiIndex::LENGTH; ++i)
        {
            if (m.value_of_index(i, Tenh::DONT_CHECK_RANGE) == last)
            {
                ++count;
            }
            else
            {
                last = m.value_of_index(i, Tenh::DONT_CHECK_RANGE);
                if (count != 1)
                {
                    result += "^" + AS_STRING(count);
                }
                result += "*";
                result.push_back(vars[last]);
                count = 1;
            }

        }
        if (count != 1)
        {
            result += "^" + AS_STRING(count);
        }

        return result;
    }
private:
    VariableStringComputer();
};

template <Uint32 DEG, typename BasedVectorSpace_, typename Scalar>
std::ostream &operator << (std::ostream &out, HomogeneousPolynomial<DEG,BasedVectorSpace_,Scalar> const &poly)
{
    typedef HomogeneousPolynomial<DEG,BasedVectorSpace_,Scalar> PolyType;
    typedef typename PolyType::Sym::ComponentIndex ComponentIndex;
    typedef typename PolyType::Sym::MultiIndex MultiIndex;

    bool first_loop = true;

    for (ComponentIndex it; it.is_not_at_end(); ++it)
    {

        MultiIndex m = PolyType::Sym::template bundle_index_map<typename MultiIndex::IndexTypeList, ComponentIndex>(it);
        Scalar coeff = poly.coefficients()[it] * Scalar(Tenh::Factorial_t<DEG>::V / (Tenh::MultiIndexMultiplicity_t<MultiIndex>::eval(m)));
        if (coeff == Scalar(1))
        {
            if (first_loop)
            {
                first_loop = false;
            }
            else
            {
                out << " + ";
            }
            out << VariableStringComputer<DEG,BasedVectorSpace_,Scalar>::compute(m);
        }
        else if (coeff != Scalar(0))
        {
            if (first_loop)
            {
                first_loop = false;
            }
            else
            {
                out << " + ";
            }
            out << coeff
                << "*"
                <<  VariableStringComputer<DEG,BasedVectorSpace_,Scalar>::compute(m);
        }
    }

    return out;
}

template <typename BasedVectorSpace_, typename Scalar>
std::ostream &operator << (std::ostream &out, MultivariatePolynomial<0,BasedVectorSpace_,Scalar> const &m)
{
    return out << m.term();
}

template <Uint32 DEG, typename BasedVectorSpace_, typename Scalar>
std::ostream &operator << (std::ostream &out, MultivariatePolynomial<DEG,BasedVectorSpace_,Scalar> const &poly)
{
    typedef MultivariatePolynomial<DEG,BasedVectorSpace_,Scalar> PolyType;
    typedef typename PolyType::Sym::ComponentIndex ComponentIndex;
    typedef typename PolyType::Sym::MultiIndex MultiIndex;

    if (poly.term().is_exactly_zero())
    {
        if (poly.body().is_exactly_zero())
        {
            return out;
        }
        else
        {
            return out << poly.body();
        }
    }
    else
    {
        if (poly.body().is_exactly_zero())
        {
            return out << poly.term();
        }
        else
        {
            return out << poly.term() << " + " << poly.body();
        }
    }
}

#endif // APPLICATIONS_POLYNOMIAL_UTILITY_HPP_
