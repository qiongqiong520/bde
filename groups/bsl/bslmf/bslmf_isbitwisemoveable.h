// bslmf_isbitwisemoveable.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#define INCLUDED_BSLMF_ISBITWISEMOVEABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for bitwise moveable classes.
//
//@CLASSES:
//  bslmf::IsBitwiseMoveable<TYPE>: bitwise moveable trait metafunction
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides a single trait metafunction,
// 'bslmf::IsBitwiseMoveable<TYPE>', which allows generic code to determine
// whether 'TYPE' can be destructively moved using 'memcpy'.  Given a pointer,
// 'p1', to an object of 'TYPE', and a pointer 'p2' of the same type pointing
// to allocated but uninitialized storage, a destructive move from 'p1' to
// 'p2' comprises the following pair of operations:
//..
//  new ((void*) p2) TYPE(*p1);  // Or new ((void*) p2) TYPE(std::move(*p1));
//  p1->~TYPE();
//..
// An object of a 'TYPE' is *bitwise* *moveable*, if the above operation
// can be replaced by the following operation without affecting correctness:
//..
//  std::memcpy(p2, p1, sizeof(TYPE));
//..
// If 'IsBitwiseMoveable<TYPE>::value' inherits from 'true_type' for a given
// 'TYPE', then a generic algorithm can infer that 'TYPE' is bitwise
// moveable.
//
// This trait is used by various components for providing optimizations for
// types that can be bitwise moved.  The major benefit of this trait is not
// for a single object but for an array of such types, as a loop of
// copy/destroy operations can be replaced by a single call to 'memcpy'.  This
// replacement is not only faster, but is guaranteed not to throw an exception.
//
// 'IsBitwiseMoveable<TYPE>' will inherit from 'true_type' if 'TYPE' is a
// fundamental type, enumeration type, or pointer type.  Most user-defined
// classes are are bitwise moveable, but generic code must assume that an
// arbitrary 'TYPE' is not bitwise-moveable, as bitwise moving a type that is
// not bitwise moveable is likely to result in a dangling pointer.  Thus, it
// is necessary to explicitly associate the bitwise moveable trait with a
// class (via template specialization or by use of the
// 'BSLMF_DECLARE_NESTED_TRAIT' macro) in order to generic algorithms to
// recognize that class as bitwise moveable.  As a special case, one-byte
// objects are deduced as bitwise moveable unless explicitly annotated
// otherwise (see-below).
//
///What classes are not bitwise moveable?
///---------------------------------------
// A class that has any of the following attributes is !not! bitwise
// moveable:
//
//: o Its address is one of the salient attributes that comprise its value.
//: o It contains a pointer that might (directly or indirectly) point to
//:   itself or to one of its own members or which stores an encoding of its
//:   own address or the address of one of its members.  For example, a list
//:   implementation that includes an embedded sentinel node such that the
//:   last node in the list points back to the sentinel node within the list
//:   class object is not bitwise moveable.
//: o Its constructor registers a pointer to itself in some static registry.
//: o Its constructor or destructor have some side effect that, if omited
//:   during destructive move, would render the program incorrect.
//: o It contains a data member or base class that is not bitwise moveable.
//
// Because of the destructive nature of a bitwise move (the original object
// must be treated as uninitialized storage after the move), a class can be
// bitwise moveable but not also bitwise copyable.  For example, a class
// that contains a pointer to heap-allocated storage is generally bitwise
// moveable.  The moved object simply refers to the same storage as the
// (defunct) original.  However a bitwise copy of the same object would
// incorrectly cause the original and the copy to share the same
// heap-allocated storage.
//
///One-byte objects
///- - - - - - - -
// An object whose size does not exceede one byte are deduced to be bitwise
// moveable. The validity of this heuristic can be deduced by examining the
// criteria for non-bitwise moveable classes above:
//
//: o Very few types have their own address as a salient attribute.
//: o It is not possible for an object to store a pointer to itself in only
//:   one byte. It is difficult to conceive of why an object would store a
//:   (compressed) encoding of own address in one byte.
//: o Static registries of objects of any size are rare and are almost never
//:   a good design.
//: o Constructors and destructors with side effects are also rare, and appear
//:   almost entirely within test code (i.e., counting constructions and
//:   destructions).  Even in those cases, it is often acceptable or even
//:   preferable to skip the balanced side effects of the constructor and
//:   destructor during a destructive move.
//: o Any data member or base class of a one-byte class must also be either
//:   an empty base class or a one-byte object, so the above rationalle
//:   applies recursively to them.
//
// The purpose of this heuristic is to deduce bitwise moveability for an
// important category of empty classes that are not explicitly annotated as
// being bitwise moveable: standard predicate classes such as 'std::less<T>'.
// Being able to treat these classes as bitwise moveable means that 'bsl::set'
// and 'bsl::map' objects can be deduced as bitwise moveable and that
// 'bsl::function' objects wrapping these classes can use the small-object
// optimization.  It can be argued that any type with size less than the size
// of a pointer should be deduced as bitwise moveable by the logic above.
// However, is is primarily the common case of empty classes that we are
// trying to handle.  By limiting ourselves to the smallest-possible type,
// we reduce the chance of false positives (see next paragraph).
//
// Note that the word "rare" appears several times in the list above.  Rare
// implies non-zero, so we must provide  a way to annotate non-bitwise moveable
// one-byte classes so that the 'IsBitwiseMoveable' trait is not deduced for
// them.  This annotation is accomplished simply by specializing
// 'IsBitwiseMoveable' to inherit from 'false_type' for these rare classes.
//
// In C++11 and later, it is possible to accurately deduce a class is bitwise
// moveable without relying on the one-byte heuristic.  If the deduction with
// the one-byte heuristic yields true and the deduction without the one-byte
// heuristic yields false, then a static assert fires and the progam is
// ill-formed.  This error can be corrected by specializing the trait to false
// for the type in question.
//
///Usage
///-----
///Example 1: Using the trait to implement `destructiveMoveArray`
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Here, we use this trait in a simple algorithm called 'destructiveMoveArray',
// which moves elements from one array to another.  The algorithm is
// implemented using two implementation functions, one for types that are
// known to be bit-wise moveable, and one for other types.  THe first takes an
// extra function argument of type 'true_type', the second takes and extra
// function argument of type 'false_type':
//..
//  namespace BloombergLP {
//
//  template <class TYPE>
//  void destructiveMoveArrayImp(TYPE *to,
//                               TYPE *from,
//                               int   size,
//                               bsl::true_type)
//  {
//      // Bitwise moveable types can be moved using memcpy
//      memcpy(to, from, size * sizeof(TYPE));
//  }
//
//  template <class TYPE>
//  void destructiveMoveArrayImp(TYPE *to,
//                               TYPE *from,
//                               int   size,
//                               bsl::false_type)
//  {
//      for (int i = 0; i < size; ++i) {
//          ::new(to + i) TYPE(from[i]);
//          from[i].~TYPE();
//      }
//  }
//..
// Now we can dispatch between the two Imp functions, using the
// 'IsBitwiseMoveable' trait metafunction to determine at compile time which
// of the implementations should be used:
//..
//  template <class TYPE>
//  void destructiveMoveArray(TYPE *to, TYPE *from, int size)
//  {
//      destructiveMoveArrayImp(to, from, size,
//                              bslmf::IsBitwiseMoveable<TYPE>());
//  }
//..
// Next, to check our work, we create three classes that we will use to
// instantiate 'destructiveMoveArray'.  All of the classes will log the number
// of constructor and destructor calls.  The first class will not be decorated
// with the 'IsBitwiseMoveable' trait:
//..
//  class NonMoveableClass
//  {
//  private:
//      int d_value;
//
//      static int d_ctorCount;
//      static int d_dtorCount;
//
//  public:
//      static int ctorCount() { return d_ctorCount; }
//      static int dtorCount() { return d_dtorCount; }
//
//      NonMoveableClass(int val = 0) : d_value(val) { ++d_ctorCount; }
//      NonMoveableClass(const NonMoveableClass& other)
//          : d_value(other.d_value) { ++d_ctorCount; }
//      ~NonMoveableClass() { d_dtorCount++; }
//
//      int value() const { return d_value; }
//  };
//
//  int NonMoveableClass::d_ctorCount = 0;
//  int NonMoveableClass::d_dtorCount = 0;
//..
// The second class is similar except that we declare it to be bit-wise
// moveable by specializing 'IsBitwiseMoveable':
//..
//  class MoveableClass1
//  {
//  private:
//      int d_value;
//
//      static int d_ctorCount;
//      static int d_dtorCount;
//
//  public:
//      static int ctorCount() { return d_ctorCount; }
//      static int dtorCount() { return d_dtorCount; }
//
//      MoveableClass1(int val = 0) : d_value(val) { ++d_ctorCount; }
//      MoveableClass1(const MoveableClass1& other)
//          : d_value(other.d_value) { ++d_ctorCount; }
//      ~MoveableClass1() { d_dtorCount++; }
//
//      int value() const { return d_value; }
//  };
//
//  int MoveableClass1::d_ctorCount = 0;
//  int MoveableClass1::d_dtorCount = 0;
//
//  namespace bslmf {
//      template <> struct IsBitwiseMoveable<MoveableClass1> : bsl::true_type {
//      };
//  }
//..
// The third class is also declared to be bitwise moveable, but this time we
// do it using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro:
//..
//  class MoveableClass2
//  {
//  private:
//      int d_value;
//
//      static int d_ctorCount;
//      static int d_dtorCount;
//
//  public:
//      BSLMF_NESTED_TRAIT_DECLARATION(MoveableClass2,
//                                     bslmf::IsBitwiseMoveable);
//
//      static int ctorCount() { return d_ctorCount; }
//      static int dtorCount() { return d_dtorCount; }
//
//      MoveableClass2(int val = 0) : d_value(val) { ++d_ctorCount; }
//      MoveableClass2(const MoveableClass2& other)
//          : d_value(other.d_value) { ++d_ctorCount; }
//      ~MoveableClass2() { d_dtorCount++; }
//
//      int value() const { return d_value; }
//  };
//
//  int MoveableClass2::d_ctorCount = 0;
//  int MoveableClass2::d_dtorCount = 0;
//..
// Finally, invoke 'destructiveMoveArray' on arrays of all three classes:
//..
//  enum MoveableEnum { A_VALUE };
//
//  int main()
//  {
//      using namespace bslmf;
//
//      // First, check the basic operation of 'IsBitwiseMoveable':
//      assert(  IsBitwiseMoveable<int>::value);
//      assert(  IsBitwiseMoveable<int*>::value);
//      assert(  IsBitwiseMoveable<const int*>::value);
//      assert(  IsBitwiseMoveable<MoveableEnum>::value);
//      assert(! IsBitwiseMoveable<int&>::value);
//      assert(! IsBitwiseMoveable<const int&>::value);
//      assert(  IsBitwiseMoveable<MoveableClass1>::value);
//      assert(  IsBitwiseMoveable<const MoveableClass1>::value);
//      assert(  IsBitwiseMoveable<MoveableClass2>::value);
//      assert(  IsBitwiseMoveable<volatile MoveableClass2>::value);
//      assert(! IsBitwiseMoveable<NonMoveableClass>::value);
//      assert(! IsBitwiseMoveable<const NonMoveableClass>::value);
//
//      // For each of our test classes, allocate an array, construct three
//      // objects into it, then move it into another array.
//      const int nObj = 3;
//
//      {
//          NonMoveableClass* p1 = (NonMoveableClass*)
//              ::operator new(nObj * sizeof(NonMoveableClass));
//          NonMoveableClass* p2 =  (NonMoveableClass*)
//              ::operator new(nObj * sizeof(NonMoveableClass));
//
//          for (int i = 0; i < nObj; ++i) {
//              new(p1 + i) NonMoveableClass(i);
//          }
//
//          assert(nObj == NonMoveableClass::ctorCount());
//          assert(0    == NonMoveableClass::dtorCount());
//
//          assert(! IsBitwiseMoveable<NonMoveableClass>::value);
//          destructiveMoveArray(p2, p1, nObj);
//
//          // Verify that constructor and destructor were called on each move
//          assert(2 * nObj == NonMoveableClass::ctorCount());
//          assert(nObj     == NonMoveableClass::dtorCount());
//
//          // Verify contents
//          for (int i = 0; i < nObj; ++i) {
//              assert(i == p2[i].value());
//          }
//
//          // Destroy and deallocate
//          for (int i = 0; i < nObj; ++i) {
//              p2[i].~NonMoveableClass();
//          }
//          ::operator delete(p1);
//          ::operator delete(p2);
//      }
//
//      {
//          MoveableClass1* p1 = (MoveableClass1*)
//              ::operator new(nObj * sizeof(MoveableClass1));
//          MoveableClass1* p2 = (MoveableClass1*)
//              ::operator new(nObj * sizeof(MoveableClass1));
//
//          for (int i = 0; i < nObj; ++i) {
//              ::new(p1 + i) MoveableClass1(i);
//          }
//
//          assert(nObj == MoveableClass1::ctorCount());
//          assert(0    == MoveableClass1::dtorCount());
//
//          assert(IsBitwiseMoveable<MoveableClass1>::value);
//          destructiveMoveArray(p2, p1, nObj);
//
//          // Verify that constructor and destructor were NOT called on each
//          // move
//          assert(nObj == MoveableClass1::ctorCount());
//          assert(0    == MoveableClass1::dtorCount());
//
//          // Verify contents
//          for (int i = 0; i < nObj; ++i) {
//              assert(i == p2[i].value());
//          }
//
//          // Destroy and deallocate
//          for (int i = 0; i < nObj; ++i) {
//              p2[i].~MoveableClass1();
//          }
//          ::operator delete(p1);
//          ::operator delete(p2);
//      }
//
//      {
//          MoveableClass2* p1 = (MoveableClass2*)
//              ::operator new(nObj * sizeof(MoveableClass2));
//          MoveableClass2* p2 = (MoveableClass2*)
//              ::operator new(nObj * sizeof(MoveableClass2));
//
//          for (int i = 0; i < nObj; ++i) {
//              ::new(p1 + i) MoveableClass2(i);
//          }
//
//          assert(nObj == MoveableClass2::ctorCount());
//          assert(0    == MoveableClass2::dtorCount());
//
//          assert(IsBitwiseMoveable<MoveableClass2>::value);
//          destructiveMoveArray(p2, p1, nObj);
//
//          // Verify that constructor and destructor were NOT called on each
//          // move
//          assert(nObj == MoveableClass2::ctorCount());
//          assert(0    == MoveableClass2::dtorCount());
//
//          // Verify contents
//          for (int i = 0; i < nObj; ++i) {
//              assert(i == p2[i].value());
//          }
//
//          // Destroy and deallocate
//          for (int i = 0; i < nObj; ++i) {
//              p2[i].~MoveableClass2();
//          }
//          ::operator delete(p1);
//          ::operator delete(p2);
//      }
//
//      return 0;
//  }
//
//  } // Close enterprise namespace
//..
//
///Example 2: Associating a trait with a class template
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we associate a trait not with a class, but with a class
// *template*.  We create three class templates, each of which uses a
// different mechanisms for being associated with the 'IsBitwiseMoveable'
// trait, plus a "control" template that is not bit-wise moveable.  First, we
// define the non-bit-wise-moveable template, 'NonMoveableTemplate':
//..
//  namespace BloombergLP {
//
//  template <class TYPE>
//  struct NonMoveableTemplate
//  {
//      TYPE d_p;
//  };
//..
// Second, we define a 'MoveableTemplate1', which uses partial template
// specialization to associate the 'IsBitwiseMoveable' trait with each
// instantiation:
//..
//  template <class TYPE>
//  struct MoveableTemplate1
//  {
//      TYPE *d_p;
//  };
//
//  namespace bslmf {
//      template <class TYPE>
//      struct IsBitwiseMoveable<MoveableTemplate1<TYPE> > : bsl::true_type {
//      };
//  }
//..
// Third, we define 'MoveableTemplate2', which uses the
// 'BSLMF_NESTED_TRAIT_DECLARATION' macro to associate the 'IsBitwiseMoveable'
// trait with each instantiation:
//..
//  template <class TYPE>
//  struct MoveableTemplate2
//  {
//      TYPE *d_p;
//
//      BSLMF_NESTED_TRAIT_DECLARATION(MoveableTemplate2,
//                                     bslmf::IsBitwiseMoveable);
//  };
//..
// Fourth, we define 'MoveableTemplate3', which is bit-wise moveable iff its
// 'TYPE' template parameter is bit-wise moveable.  There is no way to get
// this effect using 'BSLMF_NESTED_TRAIT_DECLARATION', so we use partial
// specialization combined with inheritance to "inherit" the trait from
// 'TYPE':
//..
//  template <class TYPE>
//  struct MoveableTemplate3
//  {
//      TYPE d_p;
//  };
//
//  namespace bslmf {
//      template <class TYPE>
//      struct IsBitwiseMoveable<MoveableTemplate3<TYPE> > :
//          IsBitwiseMoveable<TYPE>::type { };
//  }
//..
// Now, we check that the traits are correctly associated by instantiating
// each class with both bit-wise moveable and non-moveable types and verifying
// the value of 'IsBitwiseMoveable<T>::value':
//..
//  int main()
//  {
//      using namespace bslmf;
//
//      assert(! IsBitwiseMoveable<
//             NonMoveableTemplate<NonMoveableClass> >::value);
//      assert(! IsBitwiseMoveable<
//             NonMoveableTemplate<MoveableClass1> >::value);
//
//      assert(  IsBitwiseMoveable<
//             MoveableTemplate1<NonMoveableClass> >::value);
//      assert(  IsBitwiseMoveable<
//             MoveableTemplate1<MoveableClass1> >::value);
//
//      assert(  IsBitwiseMoveable<
//             MoveableTemplate2<NonMoveableClass> >::value);
//      assert(  IsBitwiseMoveable<
//             MoveableTemplate2<MoveableClass1> >::value);
//
//      assert(! IsBitwiseMoveable<
//             MoveableTemplate3<NonMoveableClass> >::value);
//      assert(  IsBitwiseMoveable<
//             MoveableTemplate3<MoveableClass1> >::value);
//
//      return 0;
//  }
//
//  } // Close enterprise namespace
//..
//
///Example 3: Avoiding false positives on one-byte clases
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we define an empty class that has a non-trivial copy
// constructor that has a global side effect.  The side effect should not be
// omitted, even in a destructive-move situation, so 'IsBitwiseMoveable'
// should be false.  However, the heuristic described above would deduce any
// one-byte class (including an empty class) as bitwise-moveable by default,
// so we must take specific action to set the trait to false in this (rare)
// case.
//
// First, we declare a normal empty class which *is* bitwise moveable:
//..
//  namespace BloombergLP {
//
//  class MoveableEmptyClass
//  {
//      // This class is implicitly moveable by virtue of being only one byte
//      // in size.
//  };
//..
// ..
// The class above requires no special treatment.  Next, we define an empty
// class that is not bitwise moveable:
//..
//  class NonMoveableEmptyClass
//  {
//      // This class is empty, which normally would imply bitwise
//      // moveability.  However, because it has a non-trivial move/copy
//      // constructor, it should not be bitwise moved.
//
//      static int d_count;
//
//  public:
//      NonMoveableEmptyClass() { ++d_count; }
//      NonMoveableEmptyClass(const NonMoveableEmptyClass&) { ++d_count; }
//  };
//
//  int NonMoveableEmptyClass::d_count = 0;
//..
// Next, we specialize the 'IsBitwiseMoveable' trait so that
// 'NonMoveableEmptyClass' is not incorrectly flagged by trait deduction as
// having the 'IsBitwiseMoveable' trait:
//..
//  namespace bslmf {
//
//  template <>
//  struct IsBitwiseMoveable<NonMoveableEmptyClass> : bsl::false_type
//  {
//  };
//
//  } // close namespace bslmf
//..
// Finally, we show that the first class has the 'IsBitwiseMoveable' trait and
// the second class does not:
//..
//  int main()
//  {
//      using namespace bslmf;
//
//      assert(  IsBitwiseMoveable<MoveableEmptyClass>::value);
//      assert(! IsBitwiseMoveable<NonMoveableEmptyClass>::value);
//  }
//
//  } // Close enterprise namespace
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_TYPE_TRAITS
#include <type_traits>
#endif
#endif // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER

namespace BloombergLP {

namespace bslmf {

template <class TYPE>
struct IsBitwiseMoveable;

template <class TYPE>
struct IsBitwiseMoveable_Imp
{
    // Core implementation of the 'IsBitwiseMoveable' trait. A class is
    // detected as being bitwise moveable iff it is trivially copyable or it
    // has a nested trait declaration for the 'IsBitwiseMoveable' trait.
    // In C++03 however, detection of trivially copyable classes is imperfect
    // and depends on programmer intervention. As many empty classes
    // (including standard classes like 'std::less<T>' would not be detected
    // as being trivially copyable and, therefore, bitwise moveable, a
    // heuristic is put in place whereby any type of one byte size is assumed
    // to be bitwise moveable.  See component-level documentation for this
    // component for more details on this heuristic and how to avoid false
    // positives.

private:
    static const bool k_NestedBitwiseMoveableTrait =
        DetectNestedTrait<TYPE, IsBitwiseMoveable>::value;

public:
    static const bool value = !bsl::is_reference<TYPE>::value
                          && (  bsl::is_trivially_copyable<TYPE>::value
                             || sizeof(TYPE) == 1
                             || k_NestedBitwiseMoveableTrait);

    typedef bsl::integral_constant<bool, value> type;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT) &&     \
    defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
    // In C++11 and beyond, we can accurately detect trivial-copiable types
    // without relying on the one-byte heuristic.  If we detect that this
    // trait yields true with the one-byte heuristic but false without it in
    // C++11 mode, trigger a static assert; 'IsBitwiseMoveable<TYPE>' should be
    // specialized to inherit from 'false_type'.
    static const bool k_ValueWithoutOnebyteHeuristic =
        !bsl::is_reference<TYPE>::value
            && (  native_std::is_trivially_copyable<TYPE>::value
               || k_NestedBitwiseMoveableTrait);

    static_assert(! (value && !k_ValueWithoutOnebyteHeuristic),
                  "False positive IsBitwiseMoveable for one-byte type");
#endif
};

                        // ========================
                        // struct IsBitwiseMoveable
                        // ========================

template <class TYPE>
struct IsBitwiseMoveable : IsBitwiseMoveable_Imp<TYPE>::type
{
    // Trait metafunction that determines whether the specified parameter
    // 'TYPE' is bitwise moveable.  If 'IsBitwiseMoveable<TYPE>' is derived
    // from 'bsl::true_type' then 'TYPE' is bitwise moveable.  Otherwise,
    // bitwise moveability cannot be inferred for 'TYPE'.  This trait can be
    // associated with a bitwise moveable user-defined class by specializing
    // this class or by using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro.
};

template <class TYPE>
struct IsBitwiseMoveable<const TYPE> : IsBitwiseMoveable<TYPE>::type
{
    // Trait metafunction that determines whether the specified parameter
    // 'TYPE' is bitwise moveable by stripping off the 'const' qualifier and
    // forwarding to the base-case of 'IsBitwiseMoveable'.
};

template <class TYPE>
struct IsBitwiseMoveable<volatile TYPE> : IsBitwiseMoveable<TYPE>::type
{
    // Trait metafunction that determines whether the specified parameter
    // 'TYPE' is bitwise moveable by stripping off the 'volatile' qualifier
    // and forwarding to the base-case of 'IsBitwiseMoveable'.
};

template <class TYPE>
struct IsBitwiseMoveable<const volatile TYPE> : IsBitwiseMoveable<TYPE>::type
{
    // Trait metafunction that determines whether the specified parameter
    // 'TYPE' is bitwise moveable by stripping off the 'const' and 'volatile'
    // qualifiers and forwarding to the base-case of 'IsBitwiseMoveable'.
};

}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMF_ISBITWISEMOVEABLE)

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
