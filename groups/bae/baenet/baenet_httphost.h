// baenet_httphost.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#ifndef INCLUDED_BAENET_HTTPHOST
#define INCLUDED_BAENET_HTTPHOST

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(baenet_httphost_h,"$Id$ $CSID$ $CCId$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes
//
//@AUTHOR: Rohan Bhindwale (rbhindwale@bloomberg.net)
//
//@SEE_ALSO: RFC 2616
//
//@DESCRIPTION:  This component provides a value-semantic type for representing
// the HTTP Host field value, as described in RFC 2616.

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_SELECTIONINFO
#include <bdeat_selectioninfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {


                         // =========================                          
                         // class baenet_HttpHost                          
                         // =========================                          

class baenet_HttpHost {

    // INSTANCE DATA
    bsl::string               d_name;
    bdeut_NullableValue<int>  d_port;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_NAME = 0
      , ATTRIBUTE_ID_PORT = 1
    };

    enum {
        NUM_ATTRIBUTES = 2
    };

    enum {
        ATTRIBUTE_INDEX_NAME = 0
      , ATTRIBUTE_INDEX_PORT = 1
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit baenet_HttpHost(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'baenet_HttpHost' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    baenet_HttpHost(const baenet_HttpHost& original,
                        bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'baenet_HttpHost' having the value of
        // the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~baenet_HttpHost();
        // Destroy this object.

    // MANIPULATORS
    baenet_HttpHost& operator=(const baenet_HttpHost& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    bsl::string& name();
        // Return a reference to the modifiable "Name" attribute of this
        // object.

    bdeut_NullableValue<int>& port();
        // Return a reference to the modifiable "Port" attribute of this
        // object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bsl::string& name() const;
        // Return a reference to the non-modifiable "Name" attribute of this
        // object.

    const bdeut_NullableValue<int>& port() const;
        // Return a reference to the non-modifiable "Port" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const baenet_HttpHost& lhs, const baenet_HttpHost& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const baenet_HttpHost& lhs, const baenet_HttpHost& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const baenet_HttpHost& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.


// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baenet_HttpHost)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================


                         // -------------------------                          
                         // class baenet_HttpHost                          
                         // -------------------------                          

// CLASS METHODS
inline
int baenet_HttpHost::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& baenet_HttpHost::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_name, 1);
            bdex_InStreamFunctions::streamIn(stream, d_port, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int baenet_HttpHost::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_port, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PORT]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int baenet_HttpHost::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return manipulator(&d_name,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
      } break;
      case ATTRIBUTE_ID_PORT: {
        return manipulator(&d_port,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PORT]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int baenet_HttpHost::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::string& baenet_HttpHost::name()
{
    return d_name;
}

inline
bdeut_NullableValue<int>& baenet_HttpHost::port()
{
    return d_port;
}

// ACCESSORS
template <class STREAM>
STREAM& baenet_HttpHost::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_name, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_port, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int baenet_HttpHost::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_port, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PORT]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int baenet_HttpHost::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
      } break;
      case ATTRIBUTE_ID_PORT: {
        return accessor(d_port, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PORT]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int baenet_HttpHost::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::string& baenet_HttpHost::name() const
{
    return d_name;
}

inline
const bdeut_NullableValue<int>& baenet_HttpHost::port() const
{
    return d_port;
}


// FREE FUNCTIONS

inline
bool operator==(
        const baenet_HttpHost& lhs,
        const baenet_HttpHost& rhs)
{
    return  lhs.name() == rhs.name()
         && lhs.port() == rhs.port();
}

inline
bool operator!=(
        const baenet_HttpHost& lhs,
        const baenet_HttpHost& rhs)
{
    return  lhs.name() != rhs.name()
         || lhs.port() != rhs.port();
}

inline
bsl::ostream& operator<<(
        bsl::ostream& stream,
        const baenet_HttpHost& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP
#endif

// GENERATED BY BLP_BAS_CODEGEN_3.4.4 Wed Feb 10 17:14:02 2010
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
