// ///////////////////////////////////////////////////////////////////////////
// test_dimindex.cpp by Victor Dods, created 2013/10/02
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#include "test_dimindex.hpp"

#include "tenh/dimindex.hpp"
#include "tenh/indexrenamer.hpp"
#include "tenh/meta/typetuple.hpp"

// this is included last because it redefines the `assert` macro,
// which would be bad for the above includes.
#include "lvd_testsystem.hpp"

using namespace Lvd;
using namespace std;
using namespace TestSystem;

namespace Test {
namespace DimIndex {

void test_IndexRenamer_e (Context const &context)
{
    typedef Tenh::AbstractIndex_c<'i'> i;
    typedef Tenh::AbstractIndex_c<'j'> j;
    typedef Tenh::AbstractIndex_c<'k'> k;
    typedef Tenh::AbstractIndex_c<'P'> P;
    typedef Tenh::TypeTuple_f<i,j,P>::T DomainIndexTypeList;
    typedef Tenh::TypeTuple_f<j,P,k>::T CodomainIndexTypeList;
    typedef Tenh::IndexRenamer_e<DomainIndexTypeList,CodomainIndexTypeList> IndexRenamer;

    assert((Tenh::TypesAreEqual_f<IndexRenamer::Eval_f<Tenh::DimIndex_t<'i',42> >::T,Tenh::DimIndex_t<'j',42> >::V));
    assert((Tenh::TypesAreEqual_f<IndexRenamer::Eval_f<Tenh::DimIndex_t<'j',8> >::T,Tenh::DimIndex_t<'P',8> >::V));
    assert((Tenh::TypesAreEqual_f<IndexRenamer::Eval_f<Tenh::DimIndex_t<'P',3> >::T,Tenh::DimIndex_t<'k',3> >::V));

    static Tenh::AbstractIndexSymbol const OFFSET = 'k'; // this is the max of {'j','k','P'}
    assert((Tenh::TypesAreEqual_f<IndexRenamer::Eval_f<Tenh::DimIndex_t<'a',123> >::T,
                                  Tenh::DimIndex_t<OFFSET+'a',123> >::V));
    assert((Tenh::TypesAreEqual_f<IndexRenamer::Eval_f<Tenh::DimIndex_t<'k',666> >::T,
                                  Tenh::DimIndex_t<OFFSET+'k',666> >::V));
    assert((Tenh::TypesAreEqual_f<IndexRenamer::Eval_f<Tenh::DimIndex_t<12345,67890> >::T,
                                  Tenh::DimIndex_t<OFFSET+12345,67890> >::V));
}

void test_induced_TypeList_IndexRenamer_e (Context const &context)
{
    typedef Tenh::AbstractIndex_c<'i'> i;
    typedef Tenh::AbstractIndex_c<'j'> j;
    typedef Tenh::AbstractIndex_c<'k'> k;
    typedef Tenh::AbstractIndex_c<'P'> P;
    typedef Tenh::TypeTuple_f<i,j,P>::T DomainIndexTypeList;
    typedef Tenh::TypeTuple_f<j,P,k>::T CodomainIndexTypeList;
    typedef Tenh::IndexRenamer_e<DomainIndexTypeList,CodomainIndexTypeList> IndexRenamer;

    assert((Tenh::TypesAreEqual_f<IndexRenamer::Eval_f<i>::T,j>::V));
    assert((Tenh::TypesAreEqual_f<IndexRenamer::Eval_f<j>::T,P>::V));
    assert((Tenh::TypesAreEqual_f<IndexRenamer::Eval_f<P>::T,k>::V));

    static Tenh::AbstractIndexSymbol const OFFSET = 'k'; // this is the max of {'j','k','P'}

    typedef Tenh::TypeTuple_f<Tenh::DimIndex_t<'i',10>,
                              Tenh::DimIndex_t<'j',20>,
                              Tenh::DimIndex_t<'i',30>,
                              Tenh::DimIndex_t<'k',40>,
                              Tenh::DimIndex_t<'P',50> >::T TypeList;
    typedef IndexRenamer::Eval_f<TypeList>::T MappedTypeList;
    typedef Tenh::TypeTuple_f<Tenh::DimIndex_t<'j',10>,
                              Tenh::DimIndex_t<'P',20>,
                              Tenh::DimIndex_t<'j',30>,
                              Tenh::DimIndex_t<OFFSET+'k',40>,
                              Tenh::DimIndex_t<'k',50> >::T ExpectedTypeList;
    assert((Tenh::TypesAreEqual_f<MappedTypeList,ExpectedTypeList>::V));
}

void AddTests (Directory *parent)
{
    Directory *dimindex_dir = new Directory("dimindex", parent);

    LVD_ADD_TEST_CASE_FUNCTION(dimindex_dir, test_IndexRenamer_e, RESULT_NO_ERROR);
    LVD_ADD_TEST_CASE_FUNCTION(dimindex_dir, test_induced_TypeList_IndexRenamer_e, RESULT_NO_ERROR);
}

} // end of namespace DimIndex
} // end of namespace Test
