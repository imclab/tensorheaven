// ///////////////////////////////////////////////////////////////////////////
// tenh/core.hpp by Victor Dods, created 2013/03/13
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////
/// @file tenh/core.hpp
/// @headerfile core.hpp "tenh/core.hpp"
/// @brief Contains code for dealing with component indices.
#ifndef TENH_CORE_HPP_
#define TENH_CORE_HPP_

#if _WIN32
#pragma warning( disable : 4503 )
#endif

#include <cassert>
#include <complex>

#include "tenh/meta/core.hpp"
#include "tenh/meta/static_assert.hpp"

#define FORMAT(expr) static_cast<std::ostringstream &>(std::ostringstream().flush() << expr).str()
#define FORMAT_VALUE(expr) #expr << " = " << (expr) // TODO: move this out into test code

namespace Tenh {

/// @brief A type for use in places where a typename must be placed, but where no typename is correct.
/// @note This type should never actually be constructed.
/// @headerfile core.hpp "tenh/core.hpp"
struct NullType
{
private:

    NullType () { }

public:

    static std::string type_as_string () { return "NullType"; }
};

/// @brief For making the construction of something without initialization explicit and ugly.
/// @headerfile core.hpp "tenh/core.hpp"
struct WithoutInitialization { };

/// @brief For use when you want to return a const reference to various constants.
///  The default implementation is to provide a default-constructed static
///  const SINGLETON of the given value.
///
/// @note You can template-specialize this to provide particular customizations,
///  for example, Static<float> and Static<double> provide static constant
///  scalar values ZERO, ONE, and so forth.
/// @headerfile core.hpp "tenh/core.hpp"
template <typename T>
struct Static
{
    static T const SINGLETON;
};

template <typename T>
T const Static<T>::SINGLETON = T();

/// @brief Convenient notation for the square of something without evaluating the expression twice.
/// @tparam T the type of the value to square.
/// @headerfile core.hpp "tenh/core.hpp"
template <typename T>
T sqr (T const &t)
{
    return t*t;
}

/// @brief These are used in constructors for determining if a range check should be done.
///  The default check parameter value should be CHECK_RANGE, which is more expensive,
///  but if you know what you're doing, you can pass in DONT_CHECK_RANGE to avoid the
///  range check and gain efficiency (e.g. if you know for a fact that the value is
///  within the correct range).  This is a compromise between completely correct program
///  behavior and program efficiency.
static bool const CHECK_RANGE = true;
static bool const DONT_CHECK_RANGE = false;

/// @brief These are used in constructors for determining if a pointer check should be done.
///  The default check parameter value should be CHECK_POINTER, which is more expensive,
///  but if you know what you're doing, you can pass in DONT_CHECK_POINTER to avoid the
///  pointer check and gain efficiency (e.g. if you know for a fact that the pointer is
///  non-NULL).  This is a compromise between completely correct program behavior and
///  program efficiency.
/// @todo TODO: should these be moved to preallocatedarray.hpp? (if they're only used there)
static bool const CHECK_POINTER = true;
static bool const DONT_CHECK_POINTER = false;

/// @brief Used in the curiously recurring template pattern, where the derived type is passed
///  to parent classes as a template parameter, so that the baseclass can access the
///  derived type's methods.  Will "return" (as a typedef for T) Derived_ if Derived_
///  is not NullType, otherwise DefaultType_.
/// @tparam Derived_ the derived type to potentially return.
/// @tparam DefaultType_ the default type to use if Derived_ is NullType.
/// @headerfile core.hpp "tenh/core.hpp"
template <typename Derived_, typename DefaultType_>
struct DerivedType_t
{
    typedef typename If_f<TypesAreEqual_f<Derived_,NullType>::V,
                          DefaultType_,
                          Derived_>::T T;
};

/// @brief Used to find a floating point type associated to an associated scalar type.
/// @notes Provide an implementation of this for any custom scalar type (e.g. an arbitrary-precision
///  floating point type).
/// @headerfile core.hpp "tenh/core.hpp"
template <typename Scalar_>
struct AssociatedFloatingPointType_t;

/// @cond false
template <>
struct AssociatedFloatingPointType_t<float> { typedef float T; };

template <>
struct AssociatedFloatingPointType_t<double> { typedef double T; };

template <>
struct AssociatedFloatingPointType_t<long double> { typedef long double T; };

template <typename RealScalar>
struct AssociatedFloatingPointType_t<std::complex<RealScalar> > { typedef RealScalar T; };

template <>
struct AssociatedFloatingPointType_t<Sint8> { typedef float T; }; // smallest lossless floating point conversion

template <>
struct AssociatedFloatingPointType_t<Uint8> { typedef float T; }; // smallest lossless floating point conversion

template <>
struct AssociatedFloatingPointType_t<Sint16> { typedef float T; }; // smallest lossless floating point conversion

template <>
struct AssociatedFloatingPointType_t<Uint16> { typedef float T; }; // smallest lossless floating point conversion

template <>
struct AssociatedFloatingPointType_t<Sint32> { typedef double T; }; // smallest lossless floating point conversion

template <>
struct AssociatedFloatingPointType_t<Uint32> { typedef double T; }; // smallest lossless floating point conversion

template <>
struct AssociatedFloatingPointType_t<Sint64> { typedef long double T; }; // smallest lossless floating point conversion

template <>
struct AssociatedFloatingPointType_t<Uint64> { typedef long double T; }; // smallest lossless floating point conversion
///@endcond

} // end of namespace Tenh

#endif // TENH_CORE_HPP_
