// balxml_encodingstyle.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALXML_ENCODINGSTYLE
#define INCLUDED_BALXML_ENCODINGSTYLE

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baexml_encodingstyle_h,"$Id$ $CSID$ $CCId$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes.
//
//@AUTHOR: Rohan Bhindwale (rbhindwale@bloomberg.net)
//
//@DESCRIPTION: Value-semantic attribute classes.

#include <balscm_version.h>

#include <bdlat_attributeinfo.h>
#include <bdlat_enumeratorinfo.h>
#include <bdlat_typetraits.h>

#include <bsls_assert.h>

#include <bsl_iosfwd.h>
#include <bsl_ostream.h>
#include <bsl_string.h>

namespace BloombergLP {

namespace balxml {

                            // ===================
                            // class EncodingStyle
                            // ===================

struct EncodingStyle {
    // Enumeration of encoding style (COMPACT or PRETTY).
    // This struct is generated using baexml_generateoptions.pl

  public:
    // TYPES
    enum Value {
        e_COMPACT = 0
      , e_PRETTY  = 1
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BAEXML_COMPACT = e_COMPACT
      , BAEXML_PRETTY = e_PRETTY
      , COMPACT = e_COMPACT
      , PRETTY  = e_PRETTY
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    enum {
        k_NUM_ENUMERATORS = 2
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , NUM_ENUMERATORS = k_NUM_ENUMERATORS
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];

    // CLASS METHODS
    static const char *toString(Value value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

    static int fromString(Value        *result,
                          const char   *string,
                          int           stringLength);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string' of the specified 'stringLength'.  Return 0 on
        // success, and a non-zero value with no effect on 'result' otherwise
        // (i.e., 'string' does not match any enumerator).

    static int fromString(Value              *result,
                          const bsl::string&  string);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' otherwise (i.e., 'string' does not match any
        // enumerator).

    static int fromInt(Value *result, int number);
        // Load into the specified 'result' the enumerator matching the
        // specified 'number'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' otherwise (i.e., 'number' does not match any
        // enumerator).

    static bsl::ostream& print(bsl::ostream& stream, Value value);
        // Write to the specified 'stream' the string representation of
        // the specified enumeration 'value'.  Return a reference to
        // the modifiable 'stream'.
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, EncodingStyle::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.


// TRAITS
}  // close package namespace

BDLAT_DECL_ENUMERATION_TRAITS(balxml::EncodingStyle)

namespace balxml {


// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================


                            // -------------------
                            // class EncodingStyle
                            // -------------------

// CLASS METHODS
inline
int EncodingStyle::fromString(Value *result, const bsl::string& string)
{
    return fromString(result,
                      string.c_str(),
                      static_cast<int>(string.length()));
}

inline
bsl::ostream& EncodingStyle::print(bsl::ostream&      stream,
                                 EncodingStyle::Value value)
{
    return stream << toString(value);
}
}  // close package namespace


// FREE FUNCTIONS

inline
bsl::ostream& balxml::operator<<(
        bsl::ostream& stream,
        EncodingStyle::Value rhs)
{
    return EncodingStyle::print(stream, rhs);
}

}  // close enterprise namespace
#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
