#include <cassert>
#include <iostream>

#include "tenh/core.hpp"
#include "tenh/meta/function.hpp"
#include "tenh/meta/typelist.hpp"
#include "tenh/meta/typestringof.hpp"

using namespace std;
using namespace Tenh;

typedef unsigned long long int uint;

/*
// assuming Function_f_<K_>::V is increasing w.r.t. K_, this returns the
// K_ such that Function_f_<K_>::V <= N_ < Function_f_<K_+1>::V.
template <uint N_, template <uint K_> class Function_f_>
struct Largest_K_SuchThat_Function_K_LTE_N_f
{
private:

};
*/
// prototype design goal(s):
// - supply a metafunction and have it generate a static lookup table at compile time
// - ideally this would work for non-integer datatypes, though this may require an
//   index map, e.g. mapping the index type into a contiguous set of integers.
//   call this index -> integer map the "hash" map, and require it to be injective.
// - the problem with using a generic index is that it needs to be a type that is
//   allowable both as a template parameter and as a runtime value.  so maybe two
//   versions of the index type must be available, as well as two versions of the
//   hash map -- one a compile-time TMP valued thing, and the other a runtime valued
//   thing.

// generates a lookup table for Function_f_ in range [START_, END_).
// Domain_ should be an integral type.
template <typename Domain_,
          template <Domain_ N_> class Function_f_,
          Domain_ START_,
          Domain_ END_,
          typename Codomain_>
struct LookupTable_t
{
    LookupTable_t ()
        :
        m_start_value(Function_f_<START_>::V),
        m_body_lookup_table()
    {
        cout << Function_f_<START_>::V << '\n';
        // make sure the range is valid
        assert(START_ <= END_);
        // make sure that each m_start_value is actually layed out contiguously in memory
        assert(&m_start_value + 1 == m_body_lookup_table.as_array());
    }
    Codomain_ const *as_array () const
    {
        return &m_start_value;
    }
    Codomain_ operator [] (Domain_ i) const
    {
        assert(START_ <= i && i < END_);
        return as_array()[i - START_];
    }
private:
    typedef LookupTable_t<Domain_,Function_f_,START_+1,END_,Codomain_> BodyLookupTable;

    Codomain_ const m_start_value;
    BodyLookupTable m_body_lookup_table;
};

template <typename Domain_,
          template <Domain_ N_> class Function_f_,
          Domain_ START_END_,
          typename Codomain_>
struct LookupTable_t<Domain_,Function_f_,START_END_,START_END_,Codomain_>
{
    Codomain_ const *as_array () const { return reinterpret_cast<Codomain_ const *>(this); }
};

// test metafunction
template <uint N_> struct Factorial_f { static uint const V = N_ * Factorial_f<N_-1>::V; };
template <> struct Factorial_f<0> { static uint const V = 1; };

// other test metafunction
template <typename N_> struct Square_f { typedef Tenh::Value_t<uint,N_::V*N_::V> T; };

///////////////////////////////////////////////////////////////////////////////

template <typename T_> struct Cube_f;
template <uint VALUE_> struct Cube_f<Value_t<uint,VALUE_> >
{
    static uint const V = VALUE_*VALUE_*VALUE_;
    // typedef Value_t<uint,V> T;
};

template <typename T_> struct PlusOne_f;
template <uint VALUE_> struct PlusOne_f<Value_t<uint,VALUE_> >
{
    static uint const V = VALUE_+1;
    // typedef Value_t<uint,V> T;
};

MAKE_1_ARY_VALUE_EVALUATOR(Cube, uint);
MAKE_1_ARY_VALUE_EVALUATOR(PlusOne, uint);

///////////////////////////////////////////////////////////////////////////////

uint triangular_number (uint n) { return (n * (n + 1)) / 2; }

struct SymmetricIndex
{
    uint i, j;
    //SymmetricIndex (SymmetricIndex const &s) : i(s.i), j(s.j) { }
    SymmetricIndex (uint i_, uint j_) : i(i_), j(j_) { assert (i >= j); }
};

uint symmetric_index_hash (SymmetricIndex const &s)
{
    return triangular_number(s.i) + s.j;
}

std::ostream &operator << (std::ostream &out, SymmetricIndex const &s)
{
    return out << "SymmetricIndex(" << s.i << ',' << s.j << ')';
}

///////////////////////////////////////////////////////////////////////////////

template <uint N_>
struct TriangularNumber_f
{
    static uint const V = (N_ * (N_ + 1)) / 2;
};

// here's a nontrivial index type
template <uint I_, uint J_>
struct SymmetricIndex_t
{
    // the indices must be non-increasing (lower triangular)
    enum { _ = Tenh::Assert<(I_ >= J_)>::V };

    static SymmetricIndex const V;
    // operator SymmetricIndex () const { return V; }

    static std::string type_as_string () { return "SymmetricIndex_t<" + AS_STRING(I_) + ',' + AS_STRING(J_) + '>'; }
};

template <uint I_, uint J_>
SymmetricIndex const SymmetricIndex_t<I_,J_>::V(SymmetricIndex(I_,J_));

template <typename SymmetricIndex_> struct SymmetricIndexHash_f;

template <uint I_, uint J_>
struct SymmetricIndexHash_f<SymmetricIndex_t<I_,J_> >
{
    static uint const V = TriangularNumber_f<I_>::V + J_;
};

template <uint N_, uint K_>
struct SymmetricIndexHashInverse_Helper_f
{
    // 0
    // 1 2
    // 3 4 5
    // 6 7 8 9
    // . . . . .
    typedef typename Tenh::If_f<(N_ < K_),
                                Tenh::Type_t<SymmetricIndex_t<K_-1,N_> >,
                                SymmetricIndexHashInverse_Helper_f<N_-K_,K_+1> >::T::T T;
};

template <uint K_>
struct SymmetricIndexHashInverse_Helper_f<0,K_>
{
    typedef SymmetricIndex_t<K_-1,0> T;
};

template <uint N_>
struct SymmetricIndexHashInverse_f
{
    // 0
    // 1 2
    // 3 4 5
    // 6 7 8 9
    // . . . . .
    typedef typename SymmetricIndexHashInverse_Helper_f<N_,1>::T T;
    static SymmetricIndex const V;
};

template <uint N_>
SymmetricIndex const SymmetricIndexHashInverse_f<N_>::V = SymmetricIndexHashInverse_f<N_>::T::V;

///////////////////////////////////////////////////////////////////////////////

// some function on SymmetricIndex_t
// template <typename SymmetricIndex_> WhateverFunction_f;

// template <uint I_, uint J_>
// struct WhateverFunction_f<SymmetricIndex_t<I_,J_> >
// {
//     typedef SymmetricIndex_t<I_+1,J_+1> T;
// };

///////////////////////////////////////////////////////////////////////////////

int main (int argc, char **argv)
{

    cout << "0! = " << Factorial_f<0>::V << '\n';
    cout << "1! = " << Factorial_f<1>::V << '\n';
    cout << "2! = " << Factorial_f<2>::V << '\n';
    cout << "3! = " << Factorial_f<3>::V << '\n';
    cout << "4! = " << Factorial_f<4>::V << '\n';
    cout << "5! = " << Factorial_f<5>::V << '\n';
    cout << '\n';

    {
        typedef LookupTable_t<uint,Factorial_f,0,20,uint> LookupTable;
        LookupTable lookup;
        for (uint i = 0; i < 20; ++i)
            cout << "lookup(" << i << ") = " << lookup[i] << '\n';
    }

    typedef SymmetricIndex_t<2,1> S;
#define PRINT_SYMMETRIC_INDEX_HASH(i,j) cout << "SymmetricIndexHash_f<" << i << "," << j << ">::V = " \
                                             << SymmetricIndexHash_f<SymmetricIndex_t<i,j> >::V << '\n'
    PRINT_SYMMETRIC_INDEX_HASH(0,0);
    PRINT_SYMMETRIC_INDEX_HASH(1,0);
    PRINT_SYMMETRIC_INDEX_HASH(1,1);
    PRINT_SYMMETRIC_INDEX_HASH(2,0);
    PRINT_SYMMETRIC_INDEX_HASH(2,1);
    PRINT_SYMMETRIC_INDEX_HASH(2,2);
    cout << '\n';

    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<0>::T>())) << '\n';
    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<1>::T>())) << '\n';
    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<2>::T>())) << '\n';
    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<3>::T>())) << '\n';
    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<4>::T>())) << '\n';
    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<5>::T>())) << '\n';
    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<6>::T>())) << '\n';
    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<7>::T>())) << '\n';
    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<8>::T>())) << '\n';
    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<9>::T>())) << '\n';
    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<10>::T>())) << '\n';
    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<11>::T>())) << '\n';
    cout << '\n';

    cout << FORMAT_VALUE(SymmetricIndexHash_f<SymmetricIndexHashInverse_f<0>::T>::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHash_f<SymmetricIndexHashInverse_f<1>::T>::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHash_f<SymmetricIndexHashInverse_f<2>::T>::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHash_f<SymmetricIndexHashInverse_f<3>::T>::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHash_f<SymmetricIndexHashInverse_f<4>::T>::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHash_f<SymmetricIndexHashInverse_f<5>::T>::V) << '\n';
    cout << '\n';

    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<SymmetricIndexHash_f<SymmetricIndex_t<0,0> >::V>::T>())) << '\n';
    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<SymmetricIndexHash_f<SymmetricIndex_t<1,0> >::V>::T>())) << '\n';
    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<SymmetricIndexHash_f<SymmetricIndex_t<1,1> >::V>::T>())) << '\n';
    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<SymmetricIndexHash_f<SymmetricIndex_t<2,0> >::V>::T>())) << '\n';
    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<SymmetricIndexHash_f<SymmetricIndex_t<2,1> >::V>::T>())) << '\n';
    cout << FORMAT_VALUE((Tenh::type_string_of<SymmetricIndexHashInverse_f<SymmetricIndexHash_f<SymmetricIndex_t<2,2> >::V>::T>())) << '\n';
    cout << '\n';

    cout << FORMAT_VALUE((SymmetricIndex_t<0,0>::V)) << '\n';
    cout << FORMAT_VALUE((SymmetricIndex_t<1,0>::V)) << '\n';
    cout << FORMAT_VALUE((SymmetricIndex_t<1,1>::V)) << '\n';
    cout << FORMAT_VALUE((SymmetricIndex_t<2,0>::V)) << '\n';
    cout << FORMAT_VALUE((SymmetricIndex_t<2,1>::V)) << '\n';
    cout << FORMAT_VALUE((SymmetricIndex_t<2,2>::V)) << '\n';
    cout << FORMAT_VALUE((SymmetricIndex_t<3,0>::V)) << '\n';
    cout << '\n';

    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<0>::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<1>::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<2>::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<3>::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<4>::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<5>::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<6>::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<7>::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<8>::V) << '\n';
    cout << '\n';

    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<0>::T::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<1>::T::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<2>::T::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<3>::T::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<4>::T::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<5>::T::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<6>::T::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<7>::T::V) << '\n';
    cout << FORMAT_VALUE(SymmetricIndexHashInverse_f<8>::T::V) << '\n';
    cout << '\n';

    cout << FORMAT_VALUE((SymmetricIndex_t<3,2>::V)) << '\n';
    cout << FORMAT_VALUE((SymmetricIndex_t<4,2>::V)) << '\n';
    cout << FORMAT_VALUE((SymmetricIndex_t<5,3>::V)) << '\n';
    cout << '\n';

    {
        static uint const LOOKUP_TABLE_SIZE = 40;
        typedef LookupTable_t<uint,SymmetricIndexHashInverse_f,0,LOOKUP_TABLE_SIZE,SymmetricIndex> LookupTable;
        LookupTable lookup;
        for (uint i = 0; i < LOOKUP_TABLE_SIZE; ++i)
            cout << "lookup(" << i << ") = " << lookup[i] << '\n';
    }

    // typedef TypeList_t<Square_f,
    //         TypeList_t<Square_f> > FunctionTypeList;
    // cout << FORMAT_VALUE(Tenh::type_string_of<Composition_f<FunctionTypeList,Tenh::Value_t<uint,0> >::T>()) << '\n';

    typedef CompositionOf_e<TypeList_t<Cube_e,
                            TypeList_t<PlusOne_e> > > AddOneThenCube_e;
    typedef CompositionOf_e<TypeList_t<PlusOne_e,
                            TypeList_t<Cube_e,
                            TypeList_t<PlusOne_e> > > > AddOneThenCubeThenAddOne_e;

    cout << FORMAT_VALUE((AddOneThenCube_e::Eval_f<Value_t<uint,0> >::T::V)) << '\n';
    cout << FORMAT_VALUE((AddOneThenCube_e::Eval_f<Value_t<uint,1> >::T::V)) << '\n';
    cout << FORMAT_VALUE((AddOneThenCube_e::Eval_f<Value_t<uint,2> >::T::V)) << '\n';
    cout << FORMAT_VALUE((AddOneThenCube_e::Eval_f<Value_t<uint,3> >::T::V)) << '\n';
    cout << FORMAT_VALUE((AddOneThenCube_e::Eval_f<Value_t<uint,4> >::T::V)) << '\n';
    cout << FORMAT_VALUE((AddOneThenCube_e::Eval_f<Value_t<uint,5> >::T::V)) << '\n';
    cout << '\n';

    cout << FORMAT_VALUE((AddOneThenCubeThenAddOne_e::Eval_f<Value_t<uint,0> >::T::V)) << '\n';
    cout << FORMAT_VALUE((AddOneThenCubeThenAddOne_e::Eval_f<Value_t<uint,1> >::T::V)) << '\n';
    cout << FORMAT_VALUE((AddOneThenCubeThenAddOne_e::Eval_f<Value_t<uint,2> >::T::V)) << '\n';
    cout << FORMAT_VALUE((AddOneThenCubeThenAddOne_e::Eval_f<Value_t<uint,3> >::T::V)) << '\n';
    cout << FORMAT_VALUE((AddOneThenCubeThenAddOne_e::Eval_f<Value_t<uint,4> >::T::V)) << '\n';
    cout << FORMAT_VALUE((AddOneThenCubeThenAddOne_e::Eval_f<Value_t<uint,5> >::T::V)) << '\n';

    return 0;
}
