#ifndef TENSOR2_HPP_
#define TENSOR2_HPP_

#include <iostream>

#include "core.hpp"
#include "expression_templates.hpp"
#include "vector.hpp"

// general 2-tensor with no symmetries -- most general type of 2-tensor
template <typename F1_, typename F2_>
struct Tensor2_t : Vector_t<typename F1_::Scalar,F1_::DIM*F2_::DIM>
{
    enum { _ = Lvd::Meta::Assert<Lvd::Meta::TypesAreEqual<typename F1_::Scalar,typename F2_::Scalar>::v>::v };

    typedef Vector_t<typename F1_::Scalar,F1_::DIM*F2_::DIM> Parent;
    typedef typename Parent::Scalar Scalar;
    using Parent::DIM;
//     static Uint32 const DEGREE = 2; // there are two factors in this tensor type (F1 and F2)

    typedef F1_ F1;
    typedef F2_ F2;

    Tensor2_t (WithoutInitialization const &w) : Parent(w) { }
    Tensor2_t (Scalar fill) : Parent(fill) { }

    struct IndexBlah : public Parent::Index // TODO: deprecate (it's only used in the test code below)
    {
        IndexBlah () { } // default constructor initializes to beginning
        explicit IndexBlah (Uint32 i) : Parent::Index(i) { }
        IndexBlah (typename F1::Index i1, typename F2::Index i2) : Parent::Index(F2::DIM*i1.value()+i2.value()) { }
        typename F1::Index subindex1 () const { return this->value() / F2::DIM; }
        typename F2::Index subindex2 () const { return this->value() % F2::DIM; }
    };

    // the argument is technically unnecessary, as its value is not used.  however,
    // this allows the template system to deduce the SYMBOL of the IndexType_t, so
    // it doesn't need to be specified explicitly.
    // in this, an outer product would be
    // IndexType_t<'i'> i;
    // IndexType_t<'j'> j;
    // u(i)*v(j)
    template <char SYMBOL>
    ExpressionTemplate_IndexAsVector_t<Tensor2_t,typename Parent::template Index_t<SYMBOL> > operator () (
        typename Parent::template Index_t<SYMBOL> const &) const
    {
        return expr<SYMBOL>();
    }
    // the corresponding outer product example here would be
    // u.expr<'i'>() * v.expr<'j'>()
    template <char SYMBOL>
    ExpressionTemplate_IndexAsVector_t<Tensor2_t,typename Parent::template Index_t<SYMBOL> > expr () const
    {
        Lvd::Meta::Assert<(SYMBOL != '\0')>();
        return ExpressionTemplate_IndexAsVector_t<Tensor2_t,typename Parent::template Index_t<SYMBOL> >(*this);
    }
    
    // a 2-tensor can be indexed by the pair of factor indices (F1::Index, F2::Index)
    template <char F1_SYMBOL, char F2_SYMBOL>
    ExpressionTemplate_IndexAsTensor2_t<Tensor2_t,
                                        typename F1::template Index_t<F1_SYMBOL>, 
                                        typename F2::template Index_t<F2_SYMBOL> > operator () (
        typename F1::template Index_t<F1_SYMBOL> const &, 
        typename F2::template Index_t<F2_SYMBOL> const &) const
    {
        return expr<F1_SYMBOL,F2_SYMBOL>();
    }
    // the 2-index analog of expr<SYMBOL>()
    template <char F1_SYMBOL, char F2_SYMBOL>
    ExpressionTemplate_IndexAsTensor2_t<Tensor2_t,
                                        typename F1::template Index_t<F1_SYMBOL>, 
                                        typename F2::template Index_t<F2_SYMBOL> > expr () const
    {
        Lvd::Meta::Assert<(F1_SYMBOL != '\0')>();
        Lvd::Meta::Assert<(F2_SYMBOL != '\0')>();
        return ExpressionTemplate_IndexAsTensor2_t<Tensor2_t,
                                                   typename F1::template Index_t<F1_SYMBOL>, 
                                                   typename F2::template Index_t<F2_SYMBOL> >(*this);
    }
};

template <typename F1, typename F2>
std::ostream &operator << (std::ostream &out, Tensor2_t<F1,F2> const &t)
{
    typedef Tensor2_t<F1,F2> Tensor2;

    if (Tensor2::DIM == 0)
        return out << "[]";

    Uint32 k = 0;
    out << '\n';
    for (typename F1::Index i; i.is_not_at_end(); ++i)
    {
        out << '[';
        for (typename F2::Index j; j.is_not_at_end(); ++j)
        {
            out << t[k] << '\t';
            ++k;
        }
        out << "]\n";
    }
    return out;
}


#endif // TENSOR2_HPP_
