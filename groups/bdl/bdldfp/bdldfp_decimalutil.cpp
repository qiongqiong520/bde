// bdldfp_decimalutil.cpp                                             -*-C++-*-
#include <bdldfp_decimalutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdldfp_decimalutil_cpp,"$Id$ $CSID$")

#include <bdldfp_decimalplatform.h>
#include <bdldfp_decimalimputil.h>

#include <bsls_assert.h>
#include <bslmf_assert.h>

#include <bsl_cmath.h>
#include <bsl_cstring.h>
#include <errno.h>
#include <math.h>  // For the  FP_* macros

#ifdef BDLDFP_DECIMALPLATFORM_DECNUMBER
extern "C" {
#include <decnumber/decSingle.h>
}
#endif

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
#  ifndef  __STDC_WANT_DEC_FP__
#    error __STDC_WANT_DEC_FP__ must be defined on the command line!
     BSLMF_ASSERT(false);
#  endif
#endif

#define BDLDFP_DISABLE_COMPILE BSLMF_ASSERT(false)

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
#include <bdldfp_intelimpwrapper.h>
#endif


#include <errno.h>

namespace BloombergLP {
namespace bdldfp {


namespace {

bool isNanString(const char *str) {
    // Return 'true' if the specified 'str' represents a NaN value, and 'false'
    // otherwise.  Note that the IEEE 754 standard specifies sequence of
    // characters equivalent to "NaN" or "sNaN" except for case is a valid
    // representation of NaN.

    // For the sake of efficiency, we rely on the trick that any alphabetic
    // character [a-zA-Z] in ASCII encoding can be bit-wise 'or'ed with '_'
    // (0x20) to get the corresponding lower case character.

    bsl::size_t len = bsl::strlen(str);
    if (len < 3) {
        return false;
    }

    if ((str[0] | ' ') == 's') {
        ++str;
        if (len != 4) {
            return false;
        }
    }
    else if (len != 3) {
        return false;
    }

    return ((str[0] | ' ') == 'n' &&
            (str[1] | ' ') == 'a' &&
            (str[2] | ' ') == 'n');
}

}  // close unnamed namespace


                             // Creator functions


int DecimalUtil::parseDecimal32(Decimal32 *out, const char *str)
{
    BSLS_ASSERT(out != 0);
    BSLS_ASSERT(str != 0);

    Decimal32 d = DecimalImpUtil::parse32(str);
    if (isNan(d) && !isNanString(str)) {
        return -1;
    }
    *out = d;
    return 0;
}

int DecimalUtil::parseDecimal64(Decimal64 *out, const char *str)
{

    BSLS_ASSERT(out != 0);
    BSLS_ASSERT(str != 0);

    Decimal64 d = DecimalImpUtil::parse64(str);
    if (isNan(d) && !isNanString(str)) {
        return -1;
    }
    *out = d;
    return 0;
}

int DecimalUtil::parseDecimal128(Decimal128 *out, const char *str)
{
    BSLS_ASSERT(out != 0);
    BSLS_ASSERT(str != 0);

    Decimal128 d = DecimalImpUtil::parse128(str);

    if (isNan(d) && !isNanString(str)) {
        return -1;
    }
    *out = d;
    return 0;
}


int DecimalUtil::parseDecimal32(Decimal32 *out, const bsl::string& str)
{
    BSLS_ASSERT(out != 0);

    return parseDecimal32(out, str.c_str());
}
int DecimalUtil::parseDecimal64(Decimal64 *out, const bsl::string& str)
{
    BSLS_ASSERT(out != 0);

    return parseDecimal64(out, str.c_str());
}
int DecimalUtil::parseDecimal128(Decimal128 *out, const bsl::string& str)
{
    BSLS_ASSERT(out != 0);

    return parseDecimal128(out, str.c_str());
}

                                // Formatting functions

void DecimalUtil::format(Decimal32 value, bsl::string *out)
{
    char buffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];

    DenselyPackedDecimalImpUtil::StorageType32 dpdStorage;
    dpdStorage = DecimalImpUtil::convertToDPD(*value.data());

    DecimalImpUtil_DecNumber::ValueType32 dpdValue;
    bsl::memcpy(&dpdValue, &dpdStorage, sizeof(dpdValue));

    DecimalImpUtil_DecNumber::format(dpdValue, buffer);
    out->assign(buffer);
}

void DecimalUtil::format(Decimal64 value, bsl::string *out)
{
    char buffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];

    DenselyPackedDecimalImpUtil::StorageType64 dpdStorage;
    dpdStorage = DecimalImpUtil::convertToDPD(*value.data());

    DecimalImpUtil_DecNumber::ValueType64 dpdValue;
    bsl::memcpy(&dpdValue, &dpdStorage, sizeof(dpdValue));

    DecimalImpUtil_DecNumber::format(dpdValue, buffer);
    out->assign(buffer);
}

void DecimalUtil::format(Decimal128 value, bsl::string *out)
{
    char buffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];
    DenselyPackedDecimalImpUtil::StorageType128 dpdStorage;
    dpdStorage = DecimalImpUtil::convertToDPD(*value.data());

    DecimalImpUtil_DecNumber::ValueType128 dpdValue;
    bsl::memcpy(&dpdValue, &dpdStorage, sizeof(dpdValue));

    DecimalImpUtil_DecNumber::format(dpdValue, buffer);
    out->assign(buffer);
}

                                // Math functions

Decimal64 DecimalUtil::fma(Decimal64 x, Decimal64 y, Decimal64 z)
{
    Decimal64 rv;
#if defined(BDLDFP_DECIMALPLATFORM_C99_TR) \
 && defined(BDLDFP_DECIMALPLATFORM_C99_NO_FMAD64)
    // TODO TBD Is this OK?  Why don't we have fmad64 on IBM???
    // TODO: I believe that it is not okay -- fma exists not just for
    // performance, but for accuracy, by keeping "ideal" precision, until
    // the operation completes.  -- ADAM
    *rv.data() = (x.value() * y.value()) + z.value();
#elif defined(BDLDFP_DECIMALPLATFORM_C99_TR)
    *rv.data() = fmad64(x.value(), y.value(), z.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    decDoubleFMA(rv.data(),
                 x.data(),
                 y.data(),
                 z.data(),
                 DecimalImpUtil_DecNumber::getDecNumberContext());
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    rv.data()->d_raw = __bid64_fma(x.data()->d_raw,
                                   y.data()->d_raw,
                                   z.data()->d_raw,
                                   &flags);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
    return rv;
}

Decimal128 DecimalUtil::fma(Decimal128 x, Decimal128 y, Decimal128 z)
{
    Decimal128 rv;
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    *rv.data()= fmad128(x.value(), y.value(), z.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    decQuadFMA(rv.data(),
               x.data(),
               y.data(),
               z.data(),
               DecimalImpUtil_DecNumber::getDecNumberContext());
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    rv.data()->d_raw = __bid128_fma(x.data()->d_raw,
                                    y.data()->d_raw,
                                    z.data()->d_raw,
                                    &flags);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
    return rv;
}
                       // Selecting, converting functions

Decimal32 DecimalUtil::fabs(Decimal32 value)
{
    Decimal32 rv;
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    *rv.data() = fabsd32(value.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER) \
   || defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    // TODO TBD Just flip the sign bit, but beware of endianness
    rv = Decimal32(DecimalUtil::fabs(Decimal64(value)));
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
    return rv;
}
Decimal64 DecimalUtil::fabs(Decimal64 value)
{
    Decimal64 rv;
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    *rv.data() = fabsd64(value.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    decDoubleAbs(rv.data(),
                 value.data(),
                 DecimalImpUtil_DecNumber::getDecNumberContext());
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    rv.data()->d_raw = __bid64_abs(value.data()->d_raw);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
    return rv;
}
Decimal128 DecimalUtil::fabs(Decimal128 value)
{
    Decimal128 rv;
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    *rv.data() = fabsd128(value.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    decQuadAbs(rv.data(),
               value.data(),
               DecimalImpUtil_DecNumber::getDecNumberContext());
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    rv.data()->d_raw = __bid128_abs(value.data()->d_raw);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
    return rv;
}
                        // classification functions

#ifdef BDLDFP_DECIMALPLATFORM_DECNUMBER
static int canonicalizeDecimalValueClassification(int classification)
    // Return a standard mandated constant indicating the kind of floating
    // point value specified by 'classification'.  The behavior is undefined
    // unless 'classification' is a valid classification code for the
    // underlying implementation.  Note that 'classification' is of an
    // implementation defined type, and corresponds to specific underlying
    // library constants.
{
    enum decClass cl = static_cast<decClass>(classification);
    switch (cl) {
    case DEC_CLASS_SNAN:
    case DEC_CLASS_QNAN:          return FP_NAN;                      // RETURN
    case DEC_CLASS_NEG_INF:
    case DEC_CLASS_POS_INF:       return FP_INFINITE;                 // RETURN
    case DEC_CLASS_NEG_ZERO:
    case DEC_CLASS_POS_ZERO:      return FP_ZERO;                     // RETURN
    case DEC_CLASS_NEG_NORMAL:
    case DEC_CLASS_POS_NORMAL:    return FP_NORMAL;                   // RETURN
    case DEC_CLASS_NEG_SUBNORMAL:
    case DEC_CLASS_POS_SUBNORMAL: return FP_SUBNORMAL;                // RETURN
    }
    BSLS_ASSERT(!"Unknown decClass");
    return -1;
}
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
static int canonicalizeDecimalValueClassification(int classification)
    // Return a standard mandated constant indicating the kind of floating
    // point value specified by 'classification'.  The behavior is undefined
    // unless 'classification' is a valid classification code for the
    // underlying implementation.  Note that 'classification' is of an
    // implementation defined type, and corresponds to specific underlying
    // library constants.
{
    enum class_types cl = static_cast<class_types>(classification);
    switch (cl) {
    case signalingNaN:
    case quietNaN:          return FP_NAN;                      // RETURN
    case negativeInfinity:
    case positiveInfinity:  return FP_INFINITE;                 // RETURN
    case negativeZero:
    case positiveZero:      return FP_ZERO;                     // RETURN
    case negativeNormal:
    case positiveNormal:    return FP_NORMAL;                   // RETURN
    case negativeSubnormal:
    case positiveSubnormal: return FP_SUBNORMAL;                // RETURN
    }
    BSLS_ASSERT(!"Unknown decClass");
    return -1;
}
#endif

int DecimalUtil::classify(Decimal32 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return fpclassify(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    enum decClass cl = decSingleClass(x.data());
    return canonicalizeDecimalValueClassification(cl);
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return canonicalizeDecimalValueClassification(
                                               __bid32_class(x.data()->d_raw));
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}
int DecimalUtil::classify(Decimal64 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return fpclassify(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    enum decClass cl = decDoubleClass(x.data());
    return canonicalizeDecimalValueClassification(cl);
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return canonicalizeDecimalValueClassification(
                                               __bid64_class(x.data()->d_raw));
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}
int DecimalUtil::classify(Decimal128 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return fpclassify(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    enum decClass cl = decQuadClass(x.data());
    return canonicalizeDecimalValueClassification(cl);
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return canonicalizeDecimalValueClassification(
                                              __bid128_class(x.data()->d_raw));
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

bool DecimalUtil::isFinite(Decimal32 x)
{
    int cl = classify(x);
    return cl != FP_INFINITE && cl != FP_NAN;
}
bool DecimalUtil::isFinite(Decimal64 x)
{
    int cl = classify(x);
    return cl != FP_INFINITE && cl != FP_NAN;
}
bool DecimalUtil::isFinite(Decimal128 x)
{
    int cl = classify(x);
    return cl != FP_INFINITE && cl != FP_NAN;
}


bool DecimalUtil::isInf(Decimal32 x)
{
    return classify(x) == FP_INFINITE;
}
bool DecimalUtil::isInf(Decimal64 x)
{
    return classify(x) == FP_INFINITE;
}
bool DecimalUtil::isInf(Decimal128 x)
{
    return classify(x) == FP_INFINITE;
}

bool DecimalUtil::isNan(Decimal32 x)
{
    return classify(x) == FP_NAN;
}
bool DecimalUtil::isNan(Decimal64 x)
{
    return classify(x) == FP_NAN;
}
bool DecimalUtil::isNan(Decimal128 x)
{
    return classify(x) == FP_NAN;
}

bool DecimalUtil::isNormal(Decimal32 x)
{
    return classify(x) == FP_NORMAL;
}
bool DecimalUtil::isNormal(Decimal64 x)
{
    return classify(x) == FP_NORMAL;
}
bool DecimalUtil::isNormal(Decimal128 x)
{
    return classify(x) == FP_NORMAL;
}

                           // Comparison functions

bool DecimalUtil::isUnordered(Decimal32 x, Decimal32 y)
{
    return isNan(x) || isNan(y);
}
bool DecimalUtil::isUnordered(Decimal64 x, Decimal64 y)
{
    return isNan(x) || isNan(y);
}
bool DecimalUtil::isUnordered(Decimal128 x, Decimal128 y)
{
    return isNan(x) || isNan(y);
}
                             // Rounding functions

Decimal32 DecimalUtil::ceil(Decimal32 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return ceild32(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal64 xw(x);
    Decimal64 rv;
    decDoubleToIntegralValue(rv.data(),
                             xw.data(),
                             DecimalImpUtil_DecNumber::getDecNumberContext(),
                             DEC_ROUND_CEILING);
    return Decimal32(rv);
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid32_round_integral_positive(x.data()->d_raw, &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal64 DecimalUtil::ceil(Decimal64 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return ceild64(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal64 rv;
    decDoubleToIntegralValue(rv.data(),
                             x.data(),
                             DecimalImpUtil_DecNumber::getDecNumberContext(),
                             DEC_ROUND_CEILING);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid64_round_integral_positive(x.data()->d_raw, &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal128 DecimalUtil::ceil(Decimal128 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return ceild128(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal128 rv;
    decQuadToIntegralValue(rv.data(),
                           x.data(),
                           DecimalImpUtil_DecNumber::getDecNumberContext(),
                           DEC_ROUND_CEILING);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid128_round_integral_positive(x.data()->d_raw, &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal32 DecimalUtil::floor(Decimal32 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return floord32(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal64 xw(x);
    Decimal64 rv;
    decDoubleToIntegralValue(rv.data(),
                             xw.data(),
                             DecimalImpUtil_DecNumber::getDecNumberContext(),
                             DEC_ROUND_FLOOR);
    return Decimal32(rv);
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid32_round_integral_negative(x.data()->d_raw, &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal64 DecimalUtil::floor(Decimal64 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return floord64(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal64 rv;
    decDoubleToIntegralValue(rv.data(),
                             x.data(),
                             DecimalImpUtil_DecNumber::getDecNumberContext(),
                             DEC_ROUND_FLOOR);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid64_round_integral_negative(x.data()->d_raw, &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal128 DecimalUtil::floor(Decimal128 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return floord128(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal128 rv;
    decQuadToIntegralValue(rv.data(),
                           x.data(),
                           DecimalImpUtil_DecNumber::getDecNumberContext(),
                           DEC_ROUND_FLOOR);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid128_round_integral_negative(x.data()->d_raw,
                                                       &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal32 DecimalUtil::round(Decimal32 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return roundd32(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal64 xw(x);
    Decimal64 rv;
    decDoubleToIntegralValue(rv.data(),
                             xw.data(),
                             DecimalImpUtil_DecNumber::getDecNumberContext(),
                             DEC_ROUND_HALF_UP);
    return Decimal32(rv);
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid32_round_integral_nearest_away(x.data()->d_raw,
                                                          &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal64 DecimalUtil::round(Decimal64 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return roundd64(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal64 rv;
    decDoubleToIntegralValue(rv.data(),
                             x.data(),
                             DecimalImpUtil_DecNumber::getDecNumberContext(),
                             DEC_ROUND_HALF_UP);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid64_round_integral_nearest_away(x.data()->d_raw,
                                                          &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal128 DecimalUtil::round(Decimal128 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return roundd128(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal128 rv;
    decQuadToIntegralValue(rv.data(),
                           x.data(),
                           DecimalImpUtil_DecNumber::getDecNumberContext(),
                           DEC_ROUND_HALF_UP);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid128_round_integral_nearest_away(x.data()->d_raw,
                                                           &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal32 DecimalUtil::trunc(Decimal32 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return truncd32(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal64 xw(x);
    Decimal64 rv;
    decDoubleToIntegralValue(rv.data(),
                             xw.data(),
                             DecimalImpUtil_DecNumber::getDecNumberContext(),
                             DEC_ROUND_DOWN);
    return Decimal32(rv);
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid32_round_integral_zero(x.data()->d_raw, &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal64 DecimalUtil::trunc(Decimal64 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return truncd64(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal64 rv;
    decDoubleToIntegralValue(rv.data(),
                             x.data(),
                             DecimalImpUtil_DecNumber::getDecNumberContext(),
                             DEC_ROUND_DOWN);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid64_round_integral_zero(x.data()->d_raw, &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal128 DecimalUtil::trunc(Decimal128 x)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return truncd128(x.value());
#elif defined(BDLDFP_DECIMALPLATFORM_DECNUMBER)
    Decimal128 rv;
    decQuadToIntegralValue(rv.data(),
                           x.data(),
                           DecimalImpUtil_DecNumber::getDecNumberContext(),
                           DEC_ROUND_DOWN);
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    x.data()->d_raw = __bid128_round_integral_zero(x.data()->d_raw, &flags);
    return x;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}


                             // Quantum functions

Decimal64 DecimalUtil::multiplyByPowerOf10(Decimal64 value, Decimal64 exponent)
{
    BSLS_ASSERT_SAFE(
      makeDecimal64(-1999999997, 0) <= exponent);
    BSLS_ASSERT_SAFE(                  exponent <= makeDecimal64(99999999, 0));

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    const int intExponent = __d64_to_long_long(*exponent.data());
    return scalblnd64(*value.data(), intExponent);
#elif defined(BDLDFP_DECIMALPLATFORM_DPD)
    Decimal64 result = value;
    decDoubleScaleB(result.data(),
                    value.data(),
                    exponent.data(),
                    DecimalImpUtil_DecNumber::getDecNumberContext());
    return result;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    value.data()->d_raw =
        __bid64_scalbn(value.data()->d_raw,
                       __bid64_to_int32_int(exponent.data()->d_raw, &flags),
                       &flags);
    return value;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal128 DecimalUtil::multiplyByPowerOf10(Decimal128 value,
                                            Decimal128 exponent)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    const int intExponent = __d128_to_long_long(*exponent.data());
    return scalblnd128(*value.data(), intExponent);
#elif defined(BDLDFP_DECIMALPLATFORM_DPD)
    Decimal128 result = value;
    decQuadScaleB(result.data(),
                  value.data(),
                  exponent.data(),
                  DecimalImpUtil_DecNumber::getDecNumberContext());
    return result;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    value.data()->d_raw =
        __bid128_scalbn(value.data()->d_raw,
                        __bid128_to_int32_int(exponent.data()->d_raw, &flags),
                        &flags);
    return value;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal64 DecimalUtil::quantize(Decimal64 value, Decimal64 exponent)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return quantized64(*value.data(), *exponent.data());
#elif defined(BDLDFP_DECIMALPLATFORM_DPD)
    Decimal64 result = value;
    decDoubleQuantize(result.data(),
                      value.data(),
                      exponent.data(),
                      DecimalImpUtil_DecNumber::getDecNumberContext());
    return result;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    value.data()->d_raw = __bid64_quantize(value.data()->d_raw,
                                           exponent.data()->d_raw,
                                           &flags);
    return value;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

Decimal128 DecimalUtil::quantize(Decimal128 value, Decimal128 exponent)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return quantized128(*value.data(), *exponent.data());
#elif defined(BDLDFP_DECIMALPLATFORM_DPD)
    Decimal128 rv = value;
    decQuadQuantize(rv.data(),
                    value.data(),
                    exponent.data(),
                    DecimalImpUtil_DecNumber::getDecNumberContext());
    return rv;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    _IDEC_flags flags;
    value.data()->d_raw = __bid128_quantize(value.data()->d_raw,
                                            exponent.data()->d_raw,
                                            &flags);
    return value;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

int DecimalUtil::quantum(Decimal64 value)
{
    BSLS_ASSERT(!isInf(value));
    BSLS_ASSERT(!isNan(value));
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    const int d64_bias = 398;
    return __d64_biased_exponent(*value.data()) - d64_bias;
#elif defined(BDLDFP_DECIMALPLATFORM_DPD)
    return decDoubleGetExponent(value.data());
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    BID_UINT64 sign;
    int exponent;
    BID_UINT64 coeff;
    unpack_BID64(&sign, &exponent, &coeff, value.data()->d_raw);
    return exponent - DECIMAL_EXPONENT_BIAS;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

int DecimalUtil::quantum(Decimal128 value)
{
    BSLS_ASSERT(!isInf(value));
    BSLS_ASSERT(!isNan(value));

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    const int d128_bias = 6176;
    return __d128_biased_exponent(*value.data()) - d128_bias;
#elif defined(BDLDFP_DECIMALPLATFORM_DPD)
    return decQuadGetExponent(value.data());
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    BID_UINT64 sign;
    int exponent;
    BID_UINT128 coeff;
    BID_UINT128 raw = value.data()->d_raw;
    BID_SWAP128(raw);
    unpack_BID128_value(&sign, &exponent, &coeff, raw);
    return exponent - DECIMAL_EXPONENT_BIAS_128;
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

bool DecimalUtil::sameQuantum(Decimal64 x, Decimal64 y)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return samequantumd64(*x.data(), *y.data());
#elif defined(BDLDFP_DECIMALPLATFORM_DPD)
    return decDoubleSameQuantum(x.data(), y.data()) == 1;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return __bid64_sameQuantum(x.data()->d_raw, y.data()->d_raw);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

bool DecimalUtil::sameQuantum(Decimal128 x, Decimal128 y)
{
#ifdef BDLDFP_DECIMALPLATFORM_C99_TR
    return samequantumd128(*x.data(), *y.data());
#elif defined(BDLDFP_DECIMALPLATFORM_DPD)
    return decQuadSameQuantum(x.data(), y.data()) == 1;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    return __bid128_sameQuantum(x.data()->d_raw, y.data()->d_raw);
#else
BDLDFP_DISABLE_COMPILE; // Unsupported platform
#endif
}

int DecimalUtil::decompose(int          *sign,
                           unsigned int *significand,
                           int          *exponent,
                           Decimal32     value)
{
    BSLS_ASSERT(sign);
    BSLS_ASSERT(significand);
    BSLS_ASSERT(exponent);

    enum {
        k_SIGN_MASK             = 0x80000000ul,
        k_SPECIAL_ENCODING_MASK = 0x60000000ul,
        k_INFINITY_MASK         = 0x78000000ul,
        k_NAN_MASK              = 0x7c000000ul,
        k_SMALL_COEFF_MASK      = 0x001ffffful,
        k_LARGE_COEFF_MASK      = 0x007ffffful,
        k_LARGE_COEFF_HIGH_BIT  = 0x00800000ul,
        k_EXPONENT_MASK         = 0xff,
        k_EXPONENT_SHIFT_LARGE  = 21,
        k_EXPONENT_SHIFT_SMALL  = 23,
        k_DECIMAL_EXPONENT_BIAS = 101,
    };

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    unsigned int x = value.data()->d_raw;
#else
    unsigned int x = *value.data()->words;
#endif

    unsigned int tmp;
    int cl = classify(value);

    *sign = (x & k_SIGN_MASK) ? -1 : 1;

    if ((x & k_SPECIAL_ENCODING_MASK) == k_SPECIAL_ENCODING_MASK) {
        // special encodings
        if ((x & k_INFINITY_MASK) == k_INFINITY_MASK) {
            // NaN or Infinity
            *exponent = 0;
            *significand = x & 0x7e000000;
            if ((x & k_NAN_MASK) == k_INFINITY_MASK) {
                *significand = x & 0x7c000000;
            }
            return cl;
        }
        // get significand
        *significand = (x & k_SMALL_COEFF_MASK) | k_LARGE_COEFF_HIGH_BIT;

        // get exponent
        tmp = x >> k_EXPONENT_SHIFT_LARGE;
        *exponent = (tmp & k_EXPONENT_MASK) - k_DECIMAL_EXPONENT_BIAS;
        return cl;
    }
    // get exponent
    tmp = x >> k_EXPONENT_SHIFT_SMALL;
    *exponent = (tmp & k_EXPONENT_MASK) - k_DECIMAL_EXPONENT_BIAS;
    // get coefficient
    *significand = (x & k_LARGE_COEFF_MASK);

    return cl;
}

int DecimalUtil::decompose(int                 *sign,
                           bsls::Types::Uint64 *significand,
                           int                 *exponent,
                           Decimal64            value)
{
    BSLS_ASSERT(sign);
    BSLS_ASSERT(significand);
    BSLS_ASSERT(exponent);

    enum {
        k_SIGN_MASK             = 0x8000000000000000ull,
        k_SPECIAL_ENCODING_MASK = 0x6000000000000000ull,
        k_INFINITY_MASK         = 0x7800000000000000ull,
        k_SINFINITY_MASK        = 0xf800000000000000ull,
        k_NAN_MASK              = 0x7c00000000000000ull,
        k_SMALL_COEFF_MASK      = 0x001fffffffffffffull,
        k_LARGE_COEFF_MASK      = 0x0007ffffffffffffull,
        k_LARGE_COEFF_HIGH_BIT  = 0x0020000000000000ull,
        k_EXPONENT_MASK         = 0x3ff,
        k_EXPONENT_SHIFT_LARGE  = 51,
        k_EXPONENT_SHIFT_SMALL  = 53,
        k_DECIMAL_EXPONENT_BIAS = 398,
    };

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    bsls::Types::Uint64 x = value.data()->d_raw;
#else
    bsls::Types::Uint64 x = *value.data()->longs;
#endif

    bsls::Types::Uint64 tmp;
    int cl = classify(value);

    *sign = (x & k_SIGN_MASK) ? -1 : 1;

    if ((x & k_SPECIAL_ENCODING_MASK) == k_SPECIAL_ENCODING_MASK) {
        // special encodings
        if ((x & k_INFINITY_MASK) == k_INFINITY_MASK) {
            *exponent = 0;
            *significand = x & 0x7e00000000000000ull;
            if ((x & k_NAN_MASK) == k_INFINITY_MASK) {
                *significand = x & 0x7c00000000000000ull;
            }
            return cl; // NaN or Infinity
        }
        *significand = (x & k_LARGE_COEFF_MASK) | k_LARGE_COEFF_HIGH_BIT;
        // get exponent
        tmp = x >> k_EXPONENT_SHIFT_LARGE;
        *exponent = static_cast<int>(tmp & k_EXPONENT_MASK)
                    - static_cast<int>(k_DECIMAL_EXPONENT_BIAS);
        return cl;
    }
    // exponent
    tmp = x >> k_EXPONENT_SHIFT_SMALL;
    *exponent = static_cast<int>(tmp & k_EXPONENT_MASK)
                - static_cast<int>(k_DECIMAL_EXPONENT_BIAS);
    // coefficient
    *significand = (x & k_SMALL_COEFF_MASK);

    return cl;
}

int DecimalUtil::decompose(int                 *sign,
                           Uint128             *significand,
                           int                 *exponent,
                           Decimal128           value)
{
    enum {
        k_SIGN_MASK             = 0x8000000000000000ull,
        k_SPECIAL_ENCODING_MASK = 0x6000000000000000ull,
        k_INFINITY_MASK         = 0x7800000000000000ull,
        k_SINFINITY_MASK        = 0xf800000000000000ull,
        k_NAN_MASK              = 0x7c00000000000000ull,
        k_SMALL_COEFF_MASK      = 0x0001ffffffffffffull,
        k_LARGE_COEFF_MASK      = 0x00007fffffffffffull,
        k_LARGE_COEFF_HIGH_BIT  = 0x0020000000000000ull,
        k_EXPONENT_MASK         = 0x3fff,
        k_EXPONENT_SHIFT_LARGE  = 47,
        k_EXPONENT_SHIFT_SMALL  = 49,
        k_DECIMAL_EXPONENT_BIAS = 6176,
    };

    BSLS_ASSERT(sign);
    BSLS_ASSERT(significand);
    BSLS_ASSERT(exponent);

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    #ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
    bsls::Types::Uint64 xH = value.data()->d_raw.w[0];
    bsls::Types::Uint64 xL = value.data()->d_raw.w[1];
    #elif defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
    bsls::Types::Uint64 xL = value.data()->d_raw.w[0];
    bsls::Types::Uint64 xH = value.data()->d_raw.w[1];
    #endif
#else
    #ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
    bsls::Types::Uint64 xH = *value.data()->longs[0];
    bsls::Types::Uint64 xL = *value.data()->longs[1];
    #elif defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
    bsls::Types::Uint64 xL = *value.data()->longs[0];
    bsls::Types::Uint64 xH = *value.data()->longs[1];
    #endif
#endif

    bsls::Types::Uint64 tmp;
    int cl = classify(value);

   *sign = (xH & k_SIGN_MASK) ? -1 : 1;

    if ((xH & k_SPECIAL_ENCODING_MASK) == k_SPECIAL_ENCODING_MASK) {
        // special encodings
        if ((xH & k_INFINITY_MASK) == k_INFINITY_MASK) {
            *exponent = 0;
            significand->setHigh(xH & 0x7e00000000000000ull);
            significand->setLow(0);
            if ((xH & k_NAN_MASK) == k_INFINITY_MASK) {
                significand->setHigh(xH & 0x7c00000000000000ull);
            }
            return cl; // NaN or Infinity
        }

        // The significand is out of the valid range (begins with 2^113) and
        // thus is decomposed as zero.
        return FP_ZERO;
    }

    tmp = xH >> k_EXPONENT_SHIFT_SMALL;
    *exponent = static_cast<int>(tmp & k_EXPONENT_MASK)
                - static_cast<int>(k_DECIMAL_EXPONENT_BIAS);
    // coefficient
    significand->setHigh(xH & k_SMALL_COEFF_MASK);
    significand->setLow(xL);

    return cl;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
