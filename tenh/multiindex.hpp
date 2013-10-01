// ///////////////////////////////////////////////////////////////////////////
// tenh/multiindex.hpp by Victor Dods, created 2013/03/12
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

// TODO: rename to componentmultiindex.hpp

#ifndef TENH_MULTIINDEX_HPP_
#define TENH_MULTIINDEX_HPP_

#include "tenh/core.hpp"

#include <algorithm>

#include "tenh/componentindex.hpp"
#include "tenh/list.hpp"
#include "tenh/meta/typelist_utility.hpp"

namespace Tenh {

// IndexTypeList should be a TypeList_t containing ComponentIndex_t types
// TODO: rename MultiIndex_t to ComponentMultiIndex_t
template <typename IndexTypeList_>
struct MultiIndex_t : List_t<IndexTypeList_>
{
    // TODO: assert that each type in the list is an Index_t
    enum
    {
        STATIC_ASSERT_IN_ENUM((EachTypeSatisfies_f<IndexTypeList_,IsComponentIndex_p>::V), EACH_TYPE_MUST_BE_INDEX)
    };

    typedef List_t<IndexTypeList_> Parent;
    using Parent::LENGTH;
    typedef IndexTypeList_ IndexTypeList;
    typedef typename IndexTypeList::HeadType HeadIndexType;
    typedef typename IndexTypeList::BodyTypeList BodyIndexTypeList;
    typedef MultiIndex_t<BodyIndexTypeList> BodyMultiIndex;
    static Uint32 const COMPONENT_COUNT = HeadIndexType::COMPONENT_COUNT * BodyMultiIndex::COMPONENT_COUNT;

    MultiIndex_t () { } // default constructor initializes to "first" component
    MultiIndex_t (HeadIndexType const &head, BodyMultiIndex const &body) : Parent(head, body) { }
    // tuple-like initializers for raw Uint32
    MultiIndex_t (Uint32 i0, Uint32 i1, bool check_range = CHECK_RANGE) // TODO: replace with IndexTypeList::HeadType i0, IndexTypeList::BodyTypeList::HeadType i1 ?
        :
        Parent(HeadIndexType(i0, check_range), BodyMultiIndex(i1, check_range))
    {
        STATIC_ASSERT((Parent::LENGTH == 2), LENGTH_DOES_NOT_MATCH_ARGUMENT_COUNT);
    }
    MultiIndex_t (Uint32 i0, Uint32 i1, Uint32 i2, bool check_range = CHECK_RANGE)
        :
        Parent(HeadIndexType(i0, check_range), BodyMultiIndex(i1, i2, check_range))
    {
        STATIC_ASSERT((Parent::LENGTH == 3), LENGTH_DOES_NOT_MATCH_ARGUMENT_COUNT);
    }
    MultiIndex_t (Uint32 i0, Uint32 i1, Uint32 i2, Uint32 i3, bool check_range = CHECK_RANGE)
        :
        Parent(HeadIndexType(i0, check_range), BodyMultiIndex(i1, i2, i3, check_range))
    {
        STATIC_ASSERT((Parent::LENGTH == 4), LENGTH_DOES_NOT_MATCH_ARGUMENT_COUNT);
    }
    // can really only provide tuple-like initializer for 2-multi-indices, due to the lack
    // of a guarantee that BodyMultiIndex has more than a head index type.
    MultiIndex_t (HeadIndexType const &i0, typename BodyMultiIndex::HeadIndexType const &i1)
        :
        Parent(i0, BodyMultiIndex(i1))
    {
        STATIC_ASSERT((Parent::LENGTH == 2), LENGTH_DOES_NOT_MATCH_ARGUMENT_COUNT);
    }

    MultiIndex_t (MultiIndex_t<EmptyTypeList> const &) { } // default construction
    MultiIndex_t (MultiIndex_t<TypeList_t<HeadIndexType> > const &leading_multi_index)
        :
        Parent(leading_multi_index)
    { }
    // this includes the copy constructor
    template <typename OtherHeadIndexType, typename OtherBodyIndexTypeList>
    MultiIndex_t (MultiIndex_t<TypeList_t<OtherHeadIndexType,OtherBodyIndexTypeList> > const &leading_multi_index)
        :
        Parent(leading_multi_index)
    { }

    // for converting a vector index into a multi-index via row-major ordering (most significant is head).
    MultiIndex_t (ComponentIndex_t<COMPONENT_COUNT> const &i)
        :
        Parent(HeadIndexType(i.value() / BodyMultiIndex::COMPONENT_COUNT),
               BodyMultiIndex(ComponentIndex_t<BodyMultiIndex::COMPONENT_COUNT>(i.value() % BodyMultiIndex::COMPONENT_COUNT)))
    { }

    bool operator == (MultiIndex_t const &m) const { return this->head() == m.head() && body() == m.body(); }
    bool operator != (MultiIndex_t const &m) const { return this->head() != m.head() || body() != m.body(); }
    bool operator < (MultiIndex_t const &m) const { return this->head() < m.head() && body() < m.body(); }

    bool is_at_end () const { return this->head().is_at_end(); } // because the head is the last one incremented
    bool is_not_at_end () const { return this->head().is_not_at_end(); } // because the head is the last one incremented
    Uint32 value () const { return BodyMultiIndex::COMPONENT_COUNT*this->head().value() + this->body().value(); }
    // TODO: think about adding a redundant single index that just increments and can be returned directly in value()
    void operator ++ ()
    {
        BodyMultiIndex &b = body();
        ++b; // increment the body index
        if (b.is_at_end()) // if it hit the end, reset it and increment the head
        {
            b.reset();
            ++(this->head());
        }
    }
    void reset () { this->head().reset(); body().reset(); }

    // slighty hacky way to use List_t's existing functionality -- NOTE: this only
    // works because MultiIndex_t<IndexTypeList> inherits non-virtually from
    // List_t<IndexTypeList> and has no members.
    BodyMultiIndex const &body () const { return *static_cast<BodyMultiIndex const *>(&Parent::body()); }
    BodyMultiIndex &body () { return *static_cast<BodyMultiIndex *>(&Parent::body()); }

    // if IndexTypeList is uniform (meaning all its types are the same), then this provides
    // run-time access to the ith index.
    HeadIndexType const &index (Uint32 i, bool check_range = CHECK_RANGE) const
    {
        STATIC_ASSERT(TypeListIsUniform_t<IndexTypeList>::V, ALL_TYPES_IN_TYPELIST_MUST_BE_THE_SAME);
        assert(this->is_layed_out_contiguously_in_memory());
        if (check_range && i >= LENGTH)
            throw std::out_of_range("index argument was out of range");
        HeadIndexType const *multi_index_as_array = &this->head();
        return multi_index_as_array[i];
    }
    HeadIndexType &index (Uint32 i, bool check_range = CHECK_RANGE)
    {
        STATIC_ASSERT(TypeListIsUniform_t<IndexTypeList>::V, ALL_TYPES_IN_TYPELIST_MUST_BE_THE_SAME);
        assert(this->is_layed_out_contiguously_in_memory());
        if (check_range && i >= LENGTH)
            throw std::out_of_range("index argument was out of range");
        HeadIndexType *multi_index_as_array = &this->head();
        return multi_index_as_array[i];
    }
    // if IndexTypeList is non-uniform (meaning not all its types are the same), then this provides
    // run-time access to the ith index in a weakly-typed way (via the Uint32-typed value())
    Uint32 value_of_index (Uint32 i, bool check_range = CHECK_RANGE) const
    {
        assert(this->is_layed_out_contiguously_in_memory());
        if (check_range && i >= LENGTH)
            throw std::out_of_range("index argument was out of range");
        Uint32 const *multi_index_as_array = reinterpret_cast<Uint32 const *>(&this->head());
        return multi_index_as_array[i];
    }
    Uint32 value_of_index (Uint32 i, bool check_range = CHECK_RANGE)
    {
        assert(this->is_layed_out_contiguously_in_memory());
        if (check_range && i >= LENGTH)
            throw std::out_of_range("index argument was out of range");
        Uint32 const *multi_index_as_array = reinterpret_cast<Uint32 const *>(&this->head());
        return multi_index_as_array[i];
    }

    // TODO: rename to RangeMultiIndex_t

    // slighty hacky way to use List_t's existing functionality -- NOTE: this only
    // works because MultiIndex_t<IndexTypeList> inherits non-virtually from
    // List_t<IndexTypeList> and has no members.
    // returns the type of the MultiIndex_t having the specified range
    template <Uint32 START_INDEX, Uint32 END_INDEX>
    struct RangeType_t
    {
        typedef MultiIndex_t<typename IndexTypeList::template Range_t<START_INDEX,END_INDEX>::T> T;
    };

    // returns the MultiIndex_t which is the given range of elements
    template <Uint32 START_INDEX, Uint32 END_INDEX>
    typename RangeType_t<START_INDEX,END_INDEX>::T const &range () const
    {
        return *static_cast<typename RangeType_t<START_INDEX,END_INDEX>::T const *>(&(this->Parent::template range<START_INDEX,END_INDEX>()));
    }
    template <Uint32 START_INDEX, Uint32 END_INDEX>
    typename RangeType_t<START_INDEX,END_INDEX>::T &range ()
    {
        return *static_cast<typename RangeType_t<START_INDEX,END_INDEX>::T *>(&(this->Parent::template range<START_INDEX,END_INDEX>()));
    }

    // slighty hacky way to use List_t's existing functionality -- NOTE: this only
    // works because MultiIndex_t<IndexTypeList> inherits non-virtually from
    // List_t<IndexTypeList> and has no members.
    // returns the type of the leading MultiIndex_t ending at the INDEXth element
    template <Uint32 INDEX>
    struct LeadingListType_t
    {
//         typedef List_t<typename TypeList::template LeadingTypeList_t<INDEX>::T> T;
        typedef MultiIndex_t<typename IndexTypeList::template LeadingTypeList_t<INDEX>::T> T;
    };

    // TODO: rename to leading_multi_index
    // returns the leading MultiIndex_t ending at the INDEXth element.
    template <Uint32 INDEX>
    typename LeadingListType_t<INDEX>::T const &leading_list () const
    {
        return *static_cast<typename LeadingListType_t<INDEX>::T const *>(&(this->Parent::template leading_list<INDEX>()));
    }
    template <Uint32 INDEX>
    typename LeadingListType_t<INDEX>::T &leading_list ()
    {
        return *static_cast<typename LeadingListType_t<INDEX>::T *>(&(this->Parent::template leading_list<INDEX>()));
    }

    // TODO: rename to TrailingMultiIndex_t

    // slighty hacky way to use List_t's existing functionality -- NOTE: this only
    // works because MultiIndex_t<IndexTypeList> inherits non-virtually from
    // List_t<IndexTypeList> and has no members.
    // returns the type of the trailing List_t starting at the INDEXth element
    template <Uint32 INDEX>
    struct TrailingListType_t
    {
        typedef MultiIndex_t<typename IndexTypeList::template TrailingTypeList_t<INDEX>::T> T;
    };

    // TODO: rename to trailing_multi_index
    // returns the trailing MultiIndex_t starting at the INDEXth element
    template <Uint32 INDEX>
    typename TrailingListType_t<INDEX>::T const &trailing_list () const
    {
        return *static_cast<typename TrailingListType_t<INDEX>::T const *>(&(this->Parent::template trailing_list<INDEX>()));
    };
    template <Uint32 INDEX>
    typename TrailingListType_t<INDEX>::T &trailing_list ()
    {
        return *static_cast<typename TrailingListType_t<INDEX>::T *>(&(this->Parent::template trailing_list<INDEX>()));
    };

    static std::string type_as_string () { return "MultiIndex_t<" + type_string_of<IndexTypeList>() + '>'; }
};

// template specializations for the IndexTypeList list corner cases
template <typename HeadIndexType_>
struct MultiIndex_t<TypeList_t<HeadIndexType_> > : public List_t<TypeList_t<HeadIndexType_> >
{
    enum
    {
        STATIC_ASSERT_IN_ENUM(IsComponentIndex_f<HeadIndexType_>::V, MUST_BE_INDEX)
    };

    typedef List_t<TypeList_t<HeadIndexType_> > Parent;
    typedef HeadIndexType_ HeadIndexType;
    using Parent::LENGTH;
    typedef TypeList_t<HeadIndexType> IndexTypeList;
    typedef typename IndexTypeList::BodyTypeList BodyIndexTypeList;
    typedef MultiIndex_t<BodyIndexTypeList> BodyMultiIndex;
    static Uint32 const COMPONENT_COUNT = HeadIndexType::COMPONENT_COUNT;

    MultiIndex_t () { } // default constructor initializes to "first" component
    // explicit because it has a range-check (in the HeadIndexType constructor)
    explicit MultiIndex_t (Uint32 i, bool check_range = CHECK_RANGE) : Parent(HeadIndexType(i, check_range)) { }
    explicit MultiIndex_t (HeadIndexType const &head) : Parent(head) { }

    MultiIndex_t (MultiIndex_t<EmptyTypeList> const &) { } // default construction
    // this is the copy constructor
    MultiIndex_t (MultiIndex_t const &m)
        :
        Parent(m)
    { }
    template <typename OtherHeadIndexType, typename OtherBodyIndexTypeList>
    MultiIndex_t (MultiIndex_t<TypeList_t<OtherHeadIndexType,OtherBodyIndexTypeList> > const &leading_multi_index)
        :
        Parent(leading_multi_index)
    { }

    bool operator == (MultiIndex_t const &m) const { return this->head() == m.head(); }
    bool operator != (MultiIndex_t const &m) const { return this->head() != m.head(); }
    bool operator < (MultiIndex_t const &m) const { return this->head() < m.head(); }

    bool is_at_end () const { return this->head().is_at_end(); }
    bool is_not_at_end () const { return this->head().is_not_at_end(); }
    Uint32 value () const { return this->head().value(); }
    void operator ++ () { ++(this->head()); }
    void reset () { this->head().reset(); }

    // slighty hacky way to use List_t's existing functionality -- NOTE: this only
    // works because MultiIndex_t<IndexTypeList> inherits non-virtually from
    // List_t<IndexTypeList> and has no members.
    BodyMultiIndex const &body () const { return *static_cast<BodyMultiIndex const *>(&Parent::body()); }
    BodyMultiIndex &body () { return *static_cast<BodyMultiIndex *>(&Parent::body()); }

    // if IndexTypeList is uniform (meaning all its types are the same), then this provides
    // run-time access to the ith index.
    HeadIndexType const &index (Uint32 i, bool check_range = CHECK_RANGE) const
    {
        STATIC_ASSERT(TypeListIsUniform_t<IndexTypeList>::V, ALL_TYPES_IN_TYPELIST_MUST_BE_THE_SAME);
        assert(this->is_layed_out_contiguously_in_memory());
        if (check_range && i >= LENGTH)
            throw std::out_of_range("index argument was out of range");
        HeadIndexType const *multi_index_as_array = &this->head();
        return multi_index_as_array[i];
    }
    HeadIndexType &index (Uint32 i, bool check_range = CHECK_RANGE)
    {
        STATIC_ASSERT(TypeListIsUniform_t<IndexTypeList>::V, ALL_TYPES_IN_TYPELIST_MUST_BE_THE_SAME);
        assert(this->is_layed_out_contiguously_in_memory());
        if (check_range && i >= LENGTH)
            throw std::out_of_range("index argument was out of range");
        HeadIndexType *multi_index_as_array = &this->head();
        return multi_index_as_array[i];
    }
    // if IndexTypeList is non-uniform (meaning not all its types are the same), then this provides
    // run-time access to the ith index in a weakly-typed way (via the Uint32-typed value())
    Uint32 value_of_index (Uint32 i, bool check_range = CHECK_RANGE) const
    {
        assert(this->is_layed_out_contiguously_in_memory());
        if (check_range && i >= LENGTH)
            throw std::out_of_range("index argument was out of range");
        Uint32 const *multi_index_as_array = reinterpret_cast<Uint32 const *>(&this->head());
        return multi_index_as_array[i];
    }
    Uint32 value_of_index (Uint32 i, bool check_range = CHECK_RANGE)
    {
        assert(this->is_layed_out_contiguously_in_memory());
        if (check_range && i >= LENGTH)
            throw std::out_of_range("index argument was out of range");
        Uint32 const *multi_index_as_array = reinterpret_cast<Uint32 const *>(&this->head());
        return multi_index_as_array[i];
    }

    // type conversion operator -- because this MultiIndex_t only has one component,
    // it can be canonically identified as its component type.
    operator HeadIndexType const & () const { return this->head(); }
    operator HeadIndexType & () { return this->head(); }

    // TODO: rename to LeadingMultiIndex_t

    // slighty hacky way to use List_t's existing functionality -- NOTE: this only
    // works because MultiIndex_t<IndexTypeList> inherits non-virtually from
    // List_t<IndexTypeList> and has no members.
    // returns the type of the MultiIndex_t having the specified range
    template <Uint32 START_INDEX, Uint32 END_INDEX>
    struct RangeType_t
    {
        typedef MultiIndex_t<typename IndexTypeList::template Range_t<START_INDEX,END_INDEX>::T> T;
    };

    // returns the MultiIndex_t which is the given range of elements
    template <Uint32 START_INDEX, Uint32 END_INDEX>
    typename RangeType_t<START_INDEX,END_INDEX>::T const &range () const
    {
        return *static_cast<typename RangeType_t<START_INDEX,END_INDEX>::T const *>(&(this->Parent::template range<START_INDEX,END_INDEX>()));
    }
    template <Uint32 START_INDEX, Uint32 END_INDEX>
    typename RangeType_t<START_INDEX,END_INDEX>::T &range ()
    {
        return *static_cast<typename RangeType_t<START_INDEX,END_INDEX>::T *>(&(this->Parent::template range<START_INDEX,END_INDEX>()));
    }

    // slighty hacky way to use List_t's existing functionality -- NOTE: this only
    // works because MultiIndex_t<IndexTypeList> inherits non-virtually from
    // List_t<IndexTypeList> and has no members.
    // returns the type of the leading MultiIndex_t ending at the INDEXth element
    template <Uint32 INDEX>
    struct LeadingListType_t
    {
//         typedef List_t<typename TypeList::template LeadingTypeList_t<INDEX>::T> T;
        typedef MultiIndex_t<typename IndexTypeList::template LeadingTypeList_t<INDEX>::T> T;
    };

    // TODO: rename to leading_multi_index
    // returns the leading MultiIndex_t ending at the INDEXth element.
    template <Uint32 INDEX>
    typename LeadingListType_t<INDEX>::T const &leading_list () const
    {
        return *static_cast<typename LeadingListType_t<INDEX>::T const *>(&(this->Parent::template leading_list<INDEX>()));
    }
    template <Uint32 INDEX>
    typename LeadingListType_t<INDEX>::T &leading_list ()
    {
        return *static_cast<typename LeadingListType_t<INDEX>::T *>(&(this->Parent::template leading_list<INDEX>()));
    }

    // slighty hacky way to use List_t's existing functionality -- NOTE: this only
    // works because MultiIndex_t<IndexTypeList> inherits non-virtually from
    // List_t<IndexTypeList> and has no members.
    // returns the type of the trailing List_t starting at the INDEXth element
    template <Uint32 INDEX>
    struct TrailingListType_t
    {
        typedef MultiIndex_t<typename IndexTypeList::template TrailingTypeList_t<INDEX>::T> T;
    };

    // returns the trailing MultiIndex_t starting at the INDEXth element
    template <Uint32 INDEX>
    typename TrailingListType_t<INDEX>::T const &trailing_list () const
    {
        return *static_cast<typename TrailingListType_t<INDEX>::T const *>(&(this->Parent::template trailing_list<INDEX>()));
    };
    template <Uint32 INDEX>
    typename TrailingListType_t<INDEX>::T &trailing_list ()
    {
        return *static_cast<typename TrailingListType_t<INDEX>::T *>(&(this->Parent::template trailing_list<INDEX>()));
    };

    static std::string type_as_string () { return "MultiIndex_t<" + type_string_of<IndexTypeList>() + '>'; }
};

template <>
struct MultiIndex_t<EmptyTypeList> : public List_t<EmptyTypeList>
{
    typedef List_t<EmptyTypeList> Parent;
    using Parent::LENGTH;
    typedef EmptyTypeList IndexTypeList;
    typedef EmptyTypeList BodyIndexTypeList;
    typedef MultiIndex_t<BodyIndexTypeList> BodyMultiIndex;
    static Uint32 const COMPONENT_COUNT = 0;

    MultiIndex_t () { }

    bool operator == (MultiIndex_t const &) const { return true; }  // there is only one of these, so it must be equal
    bool operator != (MultiIndex_t const &) const { return false; } // there is only one of these, so it must not be unequal
    bool operator < (MultiIndex_t const &) const { return false; }  // there is only one of these, so it can't be less than

    bool is_at_end () const { return true; }
    bool is_not_at_end () const { return false; }
    Uint32 value () const { return 0; } // vacuous value
    void operator ++ () { } // no-op
    void reset () { } // no-op

    // slighty hacky way to use List_t's existing functionality -- NOTE: this only
    // works because MultiIndex_t<IndexTypeList> inherits non-virtually from
    // List_t<IndexTypeList> and has no members.
    BodyMultiIndex const &body () const { return *static_cast<BodyMultiIndex const *>(&Parent::body()); }
    BodyMultiIndex &body () { return *static_cast<BodyMultiIndex *>(&Parent::body()); }

    static std::string type_as_string () { return "MultiIndex_t<" + type_string_of<IndexTypeList>() + '>'; }
};



template <typename T> struct IsAMultiIndex_t { static bool const V = false; };
template <typename IndexTypeList> struct IsAMultiIndex_t<MultiIndex_t<IndexTypeList> > { static bool const V = true; };



// type-specific analogs of all the operator overloads for List_t

// tack an element onto the beginning of a list (where the list is empty)
template <typename HeadType>
inline MultiIndex_t<TypeList_t<HeadType> > operator >>= (HeadType const &head, MultiIndex_t<EmptyTypeList> const &)
{
    return MultiIndex_t<TypeList_t<HeadType> >(head);
}

// tack an element onto the beginning of a list (catch-all case)
template <typename HeadType, typename BodyTypeList>
inline MultiIndex_t<TypeList_t<HeadType,BodyTypeList> > operator >>= (HeadType const &head, MultiIndex_t<BodyTypeList> const &body)
{
    return MultiIndex_t<TypeList_t<HeadType,BodyTypeList> >(head, body);
}



// concatenate two lists (where both are empty)
inline MultiIndex_t<EmptyTypeList> operator |= (MultiIndex_t<EmptyTypeList> const &, MultiIndex_t<EmptyTypeList> const &)
{
    return MultiIndex_t<EmptyTypeList>();
}

// concatenate two lists (where the second is empty)
template <typename LeadingHeadType, typename LeadingBodyTypeList>
inline MultiIndex_t<TypeList_t<LeadingHeadType,LeadingBodyTypeList> > operator |= (
    MultiIndex_t<TypeList_t<LeadingHeadType,LeadingBodyTypeList> > const &leading_list,
    MultiIndex_t<EmptyTypeList> const &)
{
    return leading_list;
}

// concatenate two lists (where the first is empty)
template <typename TrailingHeadType, typename TrailingBodyTypeList>
inline MultiIndex_t<TypeList_t<TrailingHeadType,TrailingBodyTypeList> > operator |= (
    MultiIndex_t<EmptyTypeList> const &,
    MultiIndex_t<TypeList_t<TrailingHeadType,TrailingBodyTypeList> > const &trailing_type_list)
{
    return trailing_type_list;
}

// concatenate two lists (where the first has only one element)
template <typename LeadingHeadType, typename TrailingHeadType, typename TrailingBodyTypeList>
inline MultiIndex_t<TypeList_t<LeadingHeadType,TypeList_t<TrailingHeadType,TrailingBodyTypeList> > > operator |= (
    MultiIndex_t<TypeList_t<LeadingHeadType> > const &leading_list,
    MultiIndex_t<TypeList_t<TrailingHeadType,TrailingBodyTypeList> > const &trailing_list)
{
    return MultiIndex_t<TypeList_t<LeadingHeadType,TypeList_t<TrailingHeadType,TrailingBodyTypeList> > >(
        leading_list.head(),
        trailing_list);
}

// concatenate two lists (catch-all case)
template <typename LeadingTypeList, typename TrailingTypeList>
inline MultiIndex_t<typename ConcatenationOfTypeLists_t<LeadingTypeList,TrailingTypeList>::T> operator |= (
    MultiIndex_t<LeadingTypeList> const &leading_list,
    MultiIndex_t<TrailingTypeList> const &trailing_list)
{
    return MultiIndex_t<typename ConcatenationOfTypeLists_t<LeadingTypeList,TrailingTypeList>::T>(
        leading_list.head(),
        (leading_list.body() |= trailing_list));
}





// in-place sort of a uniform MultiIndex_t -- the default type for Compare should be std::less<Uint32>
template <typename IndexTypeList, typename Compare>
void sort (MultiIndex_t<IndexTypeList> &m)
{
    typedef MultiIndex_t<IndexTypeList> MultiIndex;
    STATIC_ASSERT(TypeListIsUniform_t<IndexTypeList>::V, ALL_TYPES_IN_TYPELIST_MUST_BE_THE_SAME);
    // run-time check that the memory is actually layed out contiguously, which is necessary for the reinterpret_cast to work.
    assert(m.is_layed_out_contiguously_in_memory());
    Uint32 *starting_pointer = reinterpret_cast<Uint32*>(&m.head());
    std::sort(starting_pointer, starting_pointer + m.LENGTH, Compare());
}

// functional sort of a uniform MultiIndex_t -- the default type for Compare should be std::less<Uint32>
template <typename IndexTypeList, typename Compare>
MultiIndex_t<IndexTypeList> sorted (MultiIndex_t<IndexTypeList> const &m)
{
    MultiIndex_t<IndexTypeList> retval(m);
    sort<IndexTypeList,Compare>(retval);
    return retval;
}





// DomainIndexTypeList must be a TypeList_t of unique types
// CodomainIndexTypeList must be a TypeList_t consisting of a subset of DomainIndexTypeList
template <typename DomainIndexTypeList, typename CodomainIndexTypeList>
struct MultiIndexMap_t
{
    enum { STATIC_ASSERT_IN_ENUM((!ContainsDuplicates_t<DomainIndexTypeList>::V), DOMAIN_INDICES_MUST_NOT_CONTAIN_DUPLICATES) };

    typedef MultiIndex_t<DomainIndexTypeList> DomainIndex;
    typedef MultiIndex_t<CodomainIndexTypeList> CodomainIndex;
    typedef CodomainIndex (*EvalMapType) (DomainIndex const &);
    static CodomainIndex eval (DomainIndex const &i)
    {
        return CodomainIndex(i.template el<DomainIndexTypeList::template IndexOf_t<typename CodomainIndexTypeList::HeadType>::V>(),
                             MultiIndexMap_t<DomainIndexTypeList,typename CodomainIndexTypeList::BodyTypeList>::eval(i));
    }
};

template <typename DomainIndexTypeList, typename CodomainIndexType>
struct MultiIndexMap_t<DomainIndexTypeList,TypeList_t<CodomainIndexType> >
{
    enum { STATIC_ASSERT_IN_ENUM((!ContainsDuplicates_t<DomainIndexTypeList>::V), DOMAIN_INDICES_MUST_NOT_CONTAIN_DUPLICATES) };

    typedef TypeList_t<CodomainIndexType> CodomainIndexTypeList;
    typedef MultiIndex_t<DomainIndexTypeList> DomainIndex;
    typedef MultiIndex_t<CodomainIndexTypeList> CodomainIndex;
    typedef CodomainIndex (*EvalMapType) (DomainIndex const &);
    static CodomainIndex eval (DomainIndex const &i)
    {
        return CodomainIndex(i.template el<DomainIndexTypeList::template IndexOf_t<typename CodomainIndexTypeList::HeadType>::V>());
    }
};

template <typename MultiIndex>
struct MultiIndexMultiplicity_t
{
    static Uint32 eval (MultiIndex const &m, Uint32 count = 1)
    {
        STATIC_ASSERT(TypeListIsUniform_t<typename MultiIndex::IndexTypeList>::V, ALL_TYPES_IN_TYPELIST_MUST_BE_THE_SAME);

        return count * ((m.head() == m.body().head()) ? MultiIndexMultiplicity_t<typename MultiIndex::BodyMultiIndex>::eval(m.body(),count + 1) : MultiIndexMultiplicity_t<typename MultiIndex::BodyMultiIndex>::eval(m.body(),1));
    }
};

template<typename IndexType>
struct MultiIndexMultiplicity_t<MultiIndex_t<TypeList_t<IndexType, EmptyTypeList> > >
{
    static Uint32 eval (MultiIndex_t<TypeList_t<IndexType, EmptyTypeList> > const &m, Uint32 count = 1)
    {
        return count;
    }
};


template <typename IndexTypeList>
Uint32 trailing_zero_count (MultiIndex_t<IndexTypeList> const &m)
{
    if (IndexTypeList::LENGTH == 0)
        return 0;

    for (Uint32 i = 0; i < m.length(); ++i)
        if (m.value_of_index(m.length() - i - 1) != 0)
            return i;

    return m.length();
}

} // end of namespace Tenh

#endif // TENH_MULTIINDEX_HPP_
