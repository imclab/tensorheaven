#include <iostream>
#include <string>

#include "tenh/core.hpp"

#include "tenh/meta/typelist.hpp"
#include "tenh/meta/typelist_utility.hpp"
#include "tenh/meta/typestringof.hpp"

using namespace std;
using namespace Tenh;

/*
The point of this prototype is to test out a design for "concepts" which implement
a hierarchy of mathematical structures.  For example, a Ring has an additive Group
structure and a multiplicative Monoid structure, and because a Group has a unique
Monoid structure, a Ring has two distinct Monoid structures.

When mathematical structures/relationships are expressed in the form of concepts,
the various structures of a particular mathematical object can be retrieved.  This
is essentially the C++ implementation of the forgetful functors from a specific
category to a more general one (e.g. VectorSpace -> Group).
*/

// a Concept is a struct that has a ParentTypeList (which is a TypeList_t)
template <typename T> struct IsConcept_f { static bool const V = false; };

// parent-traversing metafunctions

template <typename ParentTypeList>
struct AncestorsOf_Recursive_f;

// "ancestors" of Concept include Concept as a trivial ancestor
template <typename Concept>
struct AncestorsOf_f
{
	enum { _ = Lvd::Meta::Assert<IsConcept_f<Concept>::V>::v };
	typedef TypeList_t<Concept,typename AncestorsOf_Recursive_f<typename Concept::ParentTypeList>::T> T;
};

template <typename ParentTypeList>
struct AncestorsOf_Recursive_f
{
	// depth-first traversal of the ancestor tree
	typedef typename ConcatenationOfTypeLists_t<typename AncestorsOf_f<typename ParentTypeList::HeadType>::T,
	                                            typename AncestorsOf_Recursive_f<typename ParentTypeList::BodyTypeList>::T>::T T;
};

template <typename HeadType>
struct AncestorsOf_Recursive_f<TypeList_t<HeadType> >
{
	// depth-first traversal of the ancestor tree
	typedef typename AncestorsOf_f<HeadType>::T T;
};

template <>
struct AncestorsOf_Recursive_f<EmptyTypeList>
{
	// depth-first traversal of the ancestor tree
	typedef EmptyTypeList T;
};

template <typename Concept, typename Predicate>
struct AncestorsSatisfyingPredicate_f
{
	typedef typename ElementsOfTypeListSatisfyingPredicate_t<typename AncestorsOf_f<Concept>::T,Predicate>::T T;
};

// for recursively retrieving various conceptual structures

template <typename Concept, typename ConceptualStructurePredicate>
struct ConceptualStructuresOf_f
{
	enum { _ = Lvd::Meta::Assert<IsConcept_f<Concept>::V>::v }; // TODO: check that ConceptualStructurePredicate actually is one
	typedef typename UniqueTypesIn_t<typename AncestorsSatisfyingPredicate_f<Concept,ConceptualStructurePredicate>::T>::T T;
};

template <typename Concept, typename ConceptualStructurePredicate>
struct HasConceptualStructureOf_f
{
	static bool const V = ConceptualStructuresOf_f<Concept,ConceptualStructurePredicate>::T::LENGTH > 0;
};

template <typename Concept, typename ConceptualStructurePredicate>
struct HasUniqueConceptualStructureOf_f
{
	static bool const V = ConceptualStructuresOf_f<Concept,ConceptualStructurePredicate>::T::LENGTH == 1;
};

template <typename Concept, typename ConceptualStructurePredicate>
struct UniqueConceptualStructureOf_f
{
	enum { _ = Lvd::Meta::Assert<HasUniqueConceptualStructureOf_f<Concept,ConceptualStructurePredicate>::V>::v };
	typedef typename ConceptualStructuresOf_f<Concept,ConceptualStructurePredicate>::T::HeadType T;
};

// Monoid_c

template <typename Identity_, typename Operation_, bool IS_ABELIAN_>
struct Monoid_c
{
	typedef EmptyTypeList ParentTypeList;

	typedef Identity_ Identity;
	typedef Operation_ Operation;
	static bool const IS_ABELIAN = IS_ABELIAN_;

	static std::string type_as_string ()
	{
		return "Monoid_c<" + TypeStringOf_t<Identity>::eval() + ','
		                   + TypeStringOf_t<Operation>::eval() + ','
		                   + (IS_ABELIAN ? "IS_ABELIAN" : "IS_NOT_ABELIAN") + '>';
	}
};

template <typename Identity, typename Operation, bool IS_ABELIAN> 
struct IsConcept_f<Monoid_c<Identity,Operation,IS_ABELIAN> >
{ static bool const V = true; };

template <typename T> struct IsMonoid_f { static bool const V = false; };

template <typename Identity, typename Operation, bool IS_ABELIAN> 
struct IsMonoid_f<Monoid_c<Identity,Operation,IS_ABELIAN> >
{ static bool const V = true; };

struct IsMonoid_p
{
	template <typename T>
	struct Eval_t
	{
		static bool const V = IsMonoid_f<T>::V;
	};
};

// Group_c

template <typename Identity_, typename Operation_, typename Inversion_, bool IS_ABELIAN_>
struct Group_c
{
private:
	typedef Monoid_c<Identity_,Operation_,IS_ABELIAN_> As_Monoid;
public:
	typedef TypeList_t<As_Monoid> ParentTypeList;

	typedef Identity_ Identity;
	typedef Operation_ Operation;
	typedef Inversion_ Inversion;
	static bool const IS_ABELIAN = IS_ABELIAN_;

	static std::string type_as_string ()
	{
		return "Group_c<" + TypeStringOf_t<Identity>::eval() + ','
		                  + TypeStringOf_t<Operation>::eval() + ','
		                  + TypeStringOf_t<Inversion>::eval() + ','
		                  + (IS_ABELIAN ? "IS_ABELIAN" : "IS_NOT_ABELIAN") + '>';
	}
};

template <typename Identity, typename Operation, typename Inversion, bool IS_ABELIAN> 
struct IsConcept_f<Group_c<Identity,Operation,Inversion,IS_ABELIAN> >
{ static bool const V = true; };

template <typename T> struct IsGroup_f { static bool const V = false; };

template <typename Identity, typename Operation, typename Inversion, bool IS_ABELIAN> 
struct IsGroup_f<Group_c<Identity,Operation,Inversion,IS_ABELIAN> >
{ static bool const V = true; };

struct IsGroup_p
{
	template <typename T>
	struct Eval_t
	{
		static bool const V = IsGroup_f<T>::V;
	};
};

// Ring_c

template <typename Zero_, 
          typename Addition_, 
          typename AdditiveInversion_, 
          typename One_, 
          typename Multiplication_, 
          bool IS_COMMUTATIVE_>
struct Ring_c
{
private:
	typedef Group_c<Zero_,Addition_,AdditiveInversion_,true> As_AdditiveGroup;
	typedef Monoid_c<One_,Multiplication_,IS_COMMUTATIVE_> As_MultiplicativeMonoid;
public:
	typedef TypeList_t<As_AdditiveGroup,TypeList_t<As_MultiplicativeMonoid> > ParentTypeList;

	typedef Zero_ Zero;
	typedef Addition_ Addition;
	typedef AdditiveInversion_ AdditiveInversion; 
	typedef One_ One;
	typedef Multiplication_ Multiplication;
	static bool const IS_COMMUTATIVE = IS_COMMUTATIVE_;

	static std::string type_as_string ()
	{
		return "Ring_c<" + TypeStringOf_t<Zero>::eval() + ','
		                 + TypeStringOf_t<Addition>::eval() + ','
		                 + TypeStringOf_t<AdditiveInversion>::eval() + ','
		                 + TypeStringOf_t<One>::eval() + ','
		                 + TypeStringOf_t<Multiplication>::eval() + ','
		                 + (IS_COMMUTATIVE ? "IS_COMMUTATIVE" : "IS_NOT_COMMUTATIVE") + '>';
	}
};

template <typename Zero, 
          typename Addition, 
          typename AdditiveInversion, 
          typename One, 
          typename Multiplication, 
          bool IS_COMMUTATIVE>
struct IsConcept_f<Ring_c<Zero,Addition,AdditiveInversion,One,Multiplication,IS_COMMUTATIVE> >
{ static bool const V = true; };

template <typename T> struct IsRing_f { static bool const V = false; };

template <typename Zero, 
          typename Addition, 
          typename AdditiveInversion, 
          typename One, 
          typename Multiplication, 
          bool IS_COMMUTATIVE>
struct IsRing_f<Ring_c<Zero,Addition,AdditiveInversion,One,Multiplication,IS_COMMUTATIVE> >
{ static bool const V = true; };

struct IsRing_p
{
	template <typename T>
	struct Eval_t
	{
		static bool const V = IsRing_f<T>::V;
	};
};

int main (int argc, char **argv)
{
	typedef Monoid_c<float,int,true> Monoid;
	typedef Group_c<float,int,double,false> Group;
	typedef Ring_c<float,int,double,char,bool,false> Ring;

	cout << FORMAT_VALUE(TypeStringOf_t<Monoid>::eval()) << '\n';
	cout << FORMAT_VALUE(TypeStringOf_t<Group>::eval()) << '\n';
	cout << FORMAT_VALUE(TypeStringOf_t<Ring>::eval()) << '\n';
	cout << '\n';

	cout << FORMAT_VALUE(IsMonoid_f<Monoid>::V) << '\n';
	cout << FORMAT_VALUE(IsMonoid_f<Group>::V) << '\n';
	cout << FORMAT_VALUE(IsGroup_f<Group>::V) << '\n';
	cout << FORMAT_VALUE(IsMonoid_f<Ring>::V) << '\n';
	cout << FORMAT_VALUE(IsGroup_f<Ring>::V) << '\n';
	cout << FORMAT_VALUE(IsRing_f<Ring>::V) << '\n';
	cout << '\n';

	typedef AncestorsOf_f<Monoid>::T AncestorsOfMonoid;
	typedef AncestorsOf_f<Group>::T AncestorsOfGroup;
	typedef AncestorsOf_f<Ring>::T AncestorsOfRing;
	cout << FORMAT_VALUE(TypeStringOf_t<AncestorsOfMonoid>::eval()) << '\n';
	cout << FORMAT_VALUE(TypeStringOf_t<AncestorsOfGroup>::eval()) << '\n';
	cout << FORMAT_VALUE(TypeStringOf_t<AncestorsOfRing>::eval()) << '\n';
	cout << '\n';

	cout << FORMAT_VALUE((TypeStringOf_t<AncestorsSatisfyingPredicate_f<Monoid,IsMonoid_p>::T>::eval())) << '\n';
	cout << FORMAT_VALUE((TypeStringOf_t<AncestorsSatisfyingPredicate_f<Group,IsMonoid_p>::T>::eval())) << '\n';
	cout << FORMAT_VALUE((TypeStringOf_t<AncestorsSatisfyingPredicate_f<Ring,IsMonoid_p>::T>::eval())) << '\n';
	cout << '\n';

	cout << FORMAT_VALUE((TypeStringOf_t<ConceptualStructuresOf_f<Monoid,IsMonoid_p>::T>::eval())) << '\n';
	cout << FORMAT_VALUE((HasConceptualStructureOf_f<Monoid,IsMonoid_p>::V)) << '\n';
	cout << FORMAT_VALUE((HasUniqueConceptualStructureOf_f<Monoid,IsMonoid_p>::V)) << '\n';
	cout << FORMAT_VALUE((TypeStringOf_t<UniqueConceptualStructureOf_f<Monoid,IsMonoid_p>::T>::eval())) << '\n';
	cout << '\n';

	cout << FORMAT_VALUE((TypeStringOf_t<ConceptualStructuresOf_f<Monoid,IsGroup_p>::T>::eval())) << '\n';
	cout << FORMAT_VALUE((HasConceptualStructureOf_f<Monoid,IsGroup_p>::V)) << '\n';
	cout << FORMAT_VALUE((HasUniqueConceptualStructureOf_f<Monoid,IsGroup_p>::V)) << '\n';
	cout << '\n';

	cout << FORMAT_VALUE((TypeStringOf_t<ConceptualStructuresOf_f<Group,IsMonoid_p>::T>::eval())) << '\n';
	cout << FORMAT_VALUE((HasConceptualStructureOf_f<Group,IsMonoid_p>::V)) << '\n';
	cout << FORMAT_VALUE((HasUniqueConceptualStructureOf_f<Group,IsMonoid_p>::V)) << '\n';
	cout << FORMAT_VALUE((TypeStringOf_t<UniqueConceptualStructureOf_f<Group,IsMonoid_p>::T>::eval())) << '\n';
	cout << '\n';

	cout << FORMAT_VALUE((TypeStringOf_t<ConceptualStructuresOf_f<Ring,IsMonoid_p>::T>::eval())) << '\n';
	cout << FORMAT_VALUE((HasConceptualStructureOf_f<Ring,IsMonoid_p>::V)) << '\n';
	cout << FORMAT_VALUE((HasUniqueConceptualStructureOf_f<Ring,IsMonoid_p>::V)) << '\n';
	cout << '\n';

	return 0;
}
