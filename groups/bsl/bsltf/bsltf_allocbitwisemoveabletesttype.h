// bsltf_allocbitwisemoveabletesttype.h                               -*-C++-*-
#ifndef INCLUDED_BSLTF_ALLOCBITWISEMOVEABLETESTTYPE
#define INCLUDED_BSLTF_ALLOCBITWISEMOVEABLETESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class that allocates memory and is bitwise-moveable.
//
//@CLASSES:
//   bsltf::AllocBitwiseMoveableTestType: allocating bitwise-moveable class
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@AUTHOR: Chen He (che2)
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, 'AllocBitwiseMoveableTestType', that is
// bitwise-moveable, uses a 'bslma::Allocator' to allocate memory, and defines
// the type traits 'bslalg::TypeTraitUsesBslmaAllocator' and
// 'bslalg::TypeTraitBitwiseMoveable'.
//
///Attributes
///----------
//..
//  Name                Type         Default
//  ------------------  -----------  -------
//  data                int          0
//..
//: o 'data': representation of the class value
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Printing the Supported Traits
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to print the supported traits of this test type.
//
// First, we create a function template 'printTypeTraits' with a parameterized
// 'TYPE':
//..
//  template <class TYPE>
//  void printTypeTraits()
//      // Prints the traits of the parameterized 'TYPE' to the console.
//  {
//      if (bslmf::IsConvertible<bslalg_TypeTraits<TYPE>,
//          bslalg::TypeTraitUsesBslmaAllocator>::VALUE) {
//          printf("Type defines bslalg::TypeTraitUsesBslmaAllocator.\n");
//      }
//      else {
//          printf(
//              "Type does not define bslalg::TypeTraitUsesBslmaAllocator.\n");
//      }
//
//      if (bslmf::IsConvertible<bslalg_TypeTraits<TYPE>,
//          bslalg::TypeTraitBitwiseMoveable>::VALUE) {
//          printf("Type defines bslalg::TypeTraitBitwiseMoveable.\n");
//      }
//      else {
//          printf("Type does not define bslalg::TypeTraitBitwiseMoveable.\n");
//      }
//  }
// ..
// Now, we invoke the 'printTypeTraits' function template using
// 'AllocBitwiseMoveableTestType' as the parameterized 'TYPE':
//..
//  printTypeTraits<AllocBitwiseMoveableTestType>();
//..
// Finally, we observe the console output:
//..
//  Type defines bslalg::TypeTraitUsesBslmaAllocator.
//  Type defines bslalg::TypeTraitBitwiseMoveable.
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

namespace BloombergLP {
namespace bsltf {

                        // ==================================
                        // class AllocBitwiseMoveableTestType
                        // ==================================

class AllocBitwiseMoveableTestType {
    // This unconstrained (value-semantic) attribute class that is
    // bitwise-moveable, uses a 'bslma::Allocator' to allocate memory, and
    // defines the type traits 'bslalg::TypeTraitUsesBslmaAllocator' and
    // 'bslalg::TypeTraitBitwiseMoveable'.  See the Attributes section under
    // @DESCRIPTION in the component-level documentation for information on the
    // class attributes.

    // DATA
    int             *d_data_p;       // pointer to the integer class value

    bslma::Allocator *d_allocator_p;  // allocator used to supply memory (held,
                                     // not owned)

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(AllocBitwiseMoveableTestType,
                                  bslalg::TypeTraitUsesBslmaAllocator,
                                  bslalg::TypeTraitBitwiseMoveable);

    // CREATORS
    explicit AllocBitwiseMoveableTestType(
                                         bslma::Allocator *basicAllocator = 0);
        // Create a 'AllocBitwiseMoveableTestType' object having the (default)
        // attribute values:
        //..
        //  data() == 0
        //..
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.


    explicit AllocBitwiseMoveableTestType(
                                         int               data,
                                         bslma::Allocator *basicAllocator = 0);
        // Create a 'AllocBitwiseMoveableTestType' object having the specified
        // 'data' attribute value.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    AllocBitwiseMoveableTestType(
                     const AllocBitwiseMoveableTestType&   original,
                     bslma::Allocator                     *basicAllocator = 0);
        // Create a 'AllocBitwiseMoveableTestType' object having the same value
        // as the specified 'original' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~AllocBitwiseMoveableTestType();
        // Destroy this object.

    // MANIPULATORS
    AllocBitwiseMoveableTestType& operator=(
                                      const AllocBitwiseMoveableTestType& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the currently
        // installed default allocator is used.
};

// FREE OPERATORS
bool operator==(const AllocBitwiseMoveableTestType& lhs,
                const AllocBitwiseMoveableTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'AllocBitwiseMoveableTestType'
    // objects have the same if their 'data' attributes are the same.

bool operator!=(const AllocBitwiseMoveableTestType& lhs,
                const AllocBitwiseMoveableTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'AllocBitwiseMoveableTestType'
    // objects do not have the same value if their 'data' attributes are not
    // the same.

// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================

                        // ----------------------------------
                        // class AllocBitwiseMoveableTestType
                        // ----------------------------------

// CREATORS
inline
AllocBitwiseMoveableTestType::AllocBitwiseMoveableTestType(
                                              bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_data_p = reinterpret_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = 0;
}

inline
AllocBitwiseMoveableTestType::AllocBitwiseMoveableTestType(
                                              int              data,
                                              bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_data_p = reinterpret_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = data;
}

inline
AllocBitwiseMoveableTestType::AllocBitwiseMoveableTestType(
                          const AllocBitwiseMoveableTestType&  original,
                          bslma::Allocator                     *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_data_p = reinterpret_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = *original.d_data_p;
}

inline
AllocBitwiseMoveableTestType::~AllocBitwiseMoveableTestType()
{
    d_allocator_p->deallocate(d_data_p);
}

// MANIPULATORS
inline
AllocBitwiseMoveableTestType& AllocBitwiseMoveableTestType::operator=(
                                       const AllocBitwiseMoveableTestType& rhs)
{
    if (&rhs != this)
    {
        int *newData = reinterpret_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
        d_allocator_p->deallocate(d_data_p);
        d_data_p = newData;
        *d_data_p = *rhs.d_data_p;
    }
    return *this;
}

inline
void AllocBitwiseMoveableTestType::setData(int value)
{
    *d_data_p = value;
}

// ACCESSORS
inline
int AllocBitwiseMoveableTestType::data() const
{
    return *d_data_p;
}

                                  // Aspects

inline
bslma::Allocator *AllocBitwiseMoveableTestType::allocator() const
{
    return d_allocator_p;
}

// FREE OPERATORS
inline
bool operator==(const AllocBitwiseMoveableTestType& lhs,
                const AllocBitwiseMoveableTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool operator!=(const AllocBitwiseMoveableTestType& lhs,
                const AllocBitwiseMoveableTestType& rhs)
{
    return lhs.data() != rhs.data();
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
