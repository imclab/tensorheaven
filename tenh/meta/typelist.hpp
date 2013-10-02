// ///////////////////////////////////////////////////////////////////////////
// tenh/meta/typelist.hpp by Victor Dods, created 2013/02/16
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////
/// @file typelist.hpp
/// @headerfile typelist.hpp "tenh/meta/typelist.hpp"
/// @brief Contains basic classes for creating compile type lists of types.
#ifndef TENH_META_TYPELIST_HPP_
#define TENH_META_TYPELIST_HPP_

#include <ostream>
#include <string>

#include "tenh/core.hpp"
#include "tenh/meta/typestringof.hpp"

namespace Tenh {

/// @struct EmptyTypeList typelist.hpp "tenh/meta/typelist.hpp"
/// @brief A type which acts like a TypeList_t which contains no types.
/// @details For detailed information on the members of EmptyTypeList see the documentation for TypeList_t
struct EmptyTypeList
{
    /// @cond false
    // there is no HeadType
    typedef EmptyTypeList BodyTypeList;
    static Uint32 const LENGTH = 0;

    static Uint32 length () { return LENGTH; } // this is necessary to avoid a linker error complaining about undefined LENGTH member

    template <typename Type_>
    struct IndexOf_t
    {
        static Uint32 const V = 0; // this is past the end of the indexing
        operator bool () const { return V; }
    };

    static std::string type_as_string (bool with_angle_brackets = true) { return with_angle_brackets ? "TypeList_t<>" : ""; }
    /// @endcond
};


/// @struct TypeList_t typelist.hpp "tenh/meta/typelist.hpp"
/// @brief A compile time list of types, generated with tail recursion.
/// @tparam HeadType_ the first type in the list.
/// @tparam BodyTypeList_ a TypeList_t (or EmptyTypeList) of the remaining types.
template <typename HeadType_, typename BodyTypeList_ = EmptyTypeList>
struct TypeList_t
{
    /// Accessor for the first type in the list.
    typedef HeadType_ HeadType;
    /// Accessor for the body TypeList_t.
    typedef BodyTypeList_ BodyTypeList;
    /// The length of the typelist
    static Uint32 const LENGTH = 1+BodyTypeList::LENGTH;

    static Uint32 length () { return LENGTH; }

    /// @brief Returns the first index at which a type occurs.
    /// @headerfile typelist.hpp "tenh/meta/typelist.hpp"
    template <typename Type_>
    struct IndexOf_t
    {
        static Uint32 const V = TypesAreEqual_f<Type_,HeadType>::V ? 0 : (1 + BodyTypeList::template IndexOf_t<Type_>::V);
        operator Uint32 () const { return V; }
    };

    /// For use with TypeStringOf_t.
    static std::string type_as_string (bool with_angle_brackets = true)
    {
        return std::string(with_angle_brackets ? "TypeList_t<" : "") +
               type_string_of<HeadType>() +
               std::string(BodyTypeList::LENGTH > 0 ? "," : "") +
               BodyTypeList::type_as_string(false) + // no brackets, so it's not cluttered from nesting
               std::string(with_angle_brackets ? ">" : "");
    }
};


/// @struct IsTypeList_f typelist.hpp "tenh/meta/typelist.hpp"
/// @brief Simple function to determine if a type is either a TypeList_t or an EmptyTypeList.
/// @tparam T the type to test.
template <typename T> struct IsTypeList_f { static bool const V = false; };
/// @cond false
template <typename HeadType, typename BodyTypeList> struct IsTypeList_f<TypeList_t<HeadType,BodyTypeList> > { static bool const V = IsTypeList_f<BodyTypeList>::V; };
template <typename HeadType> struct IsTypeList_f<TypeList_t<HeadType> > { static bool const V = true; };
template <> struct IsTypeList_f<EmptyTypeList> { static bool const V = true; };
/// @endcond

} // end of namespace Tenh

#endif // TENH_META_TYPELIST_HPP_
