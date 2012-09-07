// bsl_ctime.h                                                        -*-C++-*-
#ifndef INCLUDED_BSL_CTIME
#define INCLUDED_BSL_CTIME

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#include <ctime>

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::size_t;
    using native_std::clock_t;
    using native_std::time_t;
    using native_std::tm;
    using native_std::clock;
    using native_std::asctime;
    using native_std::ctime;
    using native_std::gmtime;
    using native_std::difftime;
    using native_std::mktime;
    using native_std::localtime;
    using native_std::strftime;
    using native_std::time;
}

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
