// bdldfp_decimalformatutil.cpp                                       -*-C++-*-
#include <bdldfp_decimalformatutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdldfp_decimalformatutil_cpp,"$Id$ $CSID$")

#include <bdldfp_decimalutil.h>
#include <bdldfp_uint128.h>

#include <bsls_assert.h>
#include <bsl_cstdio.h>

#include <bsl_iostream.h>

namespace BloombergLP {
namespace bdldfp {

namespace {

template <class DECIMAL>
struct DecimalTraits;

template <>
struct DecimalTraits<Decimal32>
{
    typedef unsigned int Significand;
};

template <>
struct DecimalTraits<Decimal64>
{
    typedef bsls::Types::Uint64 Significand;
};

template <>
struct DecimalTraits<Decimal128>
{
    typedef Uint128 Significand;
};

static
int formatSign(char *buffer,
               int length,
               int sign,
               DecimalFormatUtil::Sign& signOutput)
    // Format the specified 'sign' according to the specified 'signOutput'
    // type and load the result into the specified 'buffer'.  Return the
    // length of the formatted sign.  The behavior is undefined if
    // 'length < 1'.
{
    BSLS_ASSERT(length >= 1);

    int len = 1;
    if (sign < 0) {
        *buffer = '-';
    }
    else {
      switch (signOutput) {
        case DecimalFormatUtil::e_ALWAYS: {
          *buffer = '+';
        } break;
        case DecimalFormatUtil::e_POSITIVE_SPACE: {
          *buffer = ' ';
        } break;
        case DecimalFormatUtil::e_NEGATIVE_ONLY: {
          len = 0;
        } break;
        default: {
          BSLS_ASSERT(!"Unexpected sign value");
        }
      }
    }
    return len;
}

template <class T>
static
T divmod10(T& v)
    // Load the resultant value of division the specified value 'v' by 10 into
    // 'v'.  Return the remainder of the division.
{
    T r =  v % 10;
    v /= 10;
    return r;
}

static
bsls::Types::Uint64 divmod10(Uint128& v)
    // Load the resultant value of division the specified value 'v' by 10 into
    // 'v'.  Return the remainder of the division.
{
    // Set a = v.high(), b = v.low().
    // Let Q = (2^64 * a + b) / 10 and R = (2^64 * a + b) % 10.
    // Set a to Q / 2^64, b to Q % 2^64, and return R.
    bsls::Types::Uint64 a =  v.high();
    bsls::Types::Uint64 b  = v.low();
    bsls::Types::Uint64 r  = 0;

    bsls::Types::Uint64 qa = a / 10;
    bsls::Types::Uint64 ra = a % 10;
    bsls::Types::Uint64 qb = b / 10;
    bsls::Types::Uint64 rb = b % 10;

    v.setHigh(qa);
    v.setLow(qb + 0x1999999999999999ull * ra + (6 * ra + rb) / 10);

    return (6 * ra + rb) % 10;
}

template <class T>
static
bool isZero(const T& v)
    // Return true if the specified 'v' equals 0, and false otherwise.
{
    return v == 0;
}

static
bool isZero(const Uint128& v)
    // Return true if the specified 'v' equals 0, and false otherwise.
{
    return v.high() == 0 && v.low() == 0;
}

template <class SIGNIFICAND>
static
int parseSignificand(int *remainders, SIGNIFICAND significand)
    // Divide the specified 'significand' value by 10 unless it equals 0 and
    // load the result of each division into the specified buffer 'remainders'.
    // Note that the 'remainders' will represent the 'significand' in
    // 'little-endian' format, i.e. the least significand digit will be stored
    // first.  Return the length of resultant buffer.
{
    int *begin = remainders;
    do {
        *remainders++ = divmod10(significand);
    } while (!isZero(significand));

    return remainders - begin;
}

template <class SIGNIFICAND>
int pointPosition(SIGNIFICAND significand, int significandLength, int exponent)
    // Return the point position relatively the most significand digit of a
    // decimal value designated by the specified 'significand',
    // 'significandLength' and 'exponent' values.
{
    return isZero(significand) ? 0 : significandLength + exponent;
}

static
int formatFixed(char      *buffer,
                int        length,
                const int *significand,
                int        significandLength,
                int        pointPos,
                int        precision,
                char       point)
{
    // Format a decimal value designated by the specified 'significand' and
    // 'exponent' values using 'e_FIXED' style and load the output into the
    // specified 'buffer'.  If there is insufficient room in the buffer, its
    // contents will be left in an unspecified state, with the returned
    // value indicating the necessary size.  The behavior is undefined if
    // either 'significand' or 'length' or 'precision' is negative or
    // 'buffer == 0'.

    BSLS_ASSERT(buffer);
    BSLS_ASSERT(length >= 0);
    BSLS_ASSERT(precision >= 0);

    int outputLength = (pointPos > 0 ? pointPos : sizeof('0'))
                       + (precision ? sizeof(point) : 0) + precision;

    if (outputLength <= length) {

        int i = 0;
        int j = significandLength - 1;

        int stop = bsl::min(j + 1, pointPos);
        for (; i < stop; ++i, --j) {
            buffer[i] = '0' + significand[j];
        }

        for (; i < pointPos; ++i) {
            buffer[i] = '0';
        }

        if (0 >= pointPos) {
            buffer[i++] = '0';
        }

        if (precision) {
            buffer[i++] = point;

            stop = bsl::min(i - pointPos, outputLength);
            for (; i < stop; ++i) {
                buffer[i] = '0';
            }

            stop = bsl::min(i + j + 1, outputLength);
            for (; i < stop; ++i, --j) {
                buffer[i] = '0' + significand[j];
            }

            for (; i < outputLength; ++i) {
                buffer[i] = '0';
            }
        }
    }

    return outputLength;
}

template <class SIGNIFICAND>
static
int printScientific(char        *buffer,
                    int          length,
                    SIGNIFICAND  significand,
                    int          exponent,
                    int          precision,
                    char         point)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(length >= 0);
    BSLS_ASSERT(precision >= 0);

    return 0;

#if 0

    int significandLength = decimalLength(significand);
    exponent              = significandLength - 1 + exponent;
    int exponentLength    = decimalLength(exponent);
    int decimalLen        = 1
                            + (precision ? sizeof('.') : 0) + precision
                            + 2
                            + exponentLength;

    if (decimalLen <= length) {

        bsl::fill_n(buffer, decimalLen, '0');

        char *it = buffer;

        if (precision < significandLength - 1) {
            significand /= pow10(significandLength - 1
                                 - precision);
            significandLength = precision + 1;
        }

        it += decimalLen - 1;

        for (char *end = it - exponentLength; it != end; --it) {
            *it = '0' + exponent % 10;
            exponent /= 10;
        }
        *it-- = (exponent >= 0 ? '+' : '-');
        *it-- = 'E';

        if (precision) {
            for (char *end = buffer + 1; it > end; --it) {
                *it = '0' + significand % 10;
                significand /= 10;
            }
            *it-- = point;
        }
        *it = '0' + significand;
    }

    return decimalLen;
#endif
}

Decimal32 round(Decimal32 value, int exponent)
{
    Decimal64 v = value;
    v = DecimalUtil::multiplyByPowerOf10(v, exponent);
    v = DecimalUtil::quantize(v, Decimal64(exponent));
    v = DecimalUtil::multiplyByPowerOf10(v, -exponent);
    return Decimal32(v);
}

template <class DECIMAL>
DECIMAL round(DECIMAL value, int exponent)
{
    value = DecimalUtil::multiplyByPowerOf10(value, exponent);
    value = DecimalUtil::quantize(value, DECIMAL(exponent));
    value = DecimalUtil::multiplyByPowerOf10(value, -exponent);
    return value;
}

template <class DECIMAL>
static
int parseDecimal(int     *buffer,
                 int     *length,
                 int     *sign,
                 int     *pointPos,
                 int      precision,
                 DECIMAL  value)
{
    typedef typename DecimalTraits<DECIMAL>::Significand SIGNIFICAND;

    SIGNIFICAND significand;
    int         exponent;
    int         cls = DecimalUtil::decompose(sign,
                                             &significand,
                                             &exponent,
                                             value);

    *length          = parseSignificand(buffer, significand);
    *pointPos        = pointPosition(significand, *length, exponent);
    int roundingPos  = precision + *pointPos;

    if (roundingPos > 0 && roundingPos < *length) {
        value     = round(value, roundingPos - 1);
        cls       = DecimalUtil::decompose(sign,
                                           &significand,
                                           &exponent,
                                           value);
        *length   = parseSignificand(buffer, significand);
        *pointPos = pointPosition(significand, *length, exponent);
    }
    return cls;
}

template <class DECIMAL>
static
int formatImpl(char                       *buffer,
               int                         length,
               DECIMAL                     value,
               int                         precision,
               DecimalFormatUtil::Style    style,
               DecimalFormatUtil::Sign     signFormat,
               DecimalFormatUtil::Letters  letters,
               char                        point)
{
    BSLS_ASSERT(buffer || length < 0);
    BSLS_ASSERT(precision >= 0);

    const int   k_MAX_SIGNIFICAND_LENGTH = 34;
    int         significand[k_MAX_SIGNIFICAND_LENGTH] = { 0 };
    int         significandLength;
    int         sign;
    int         pointPos;
    int         cls = parseDecimal(&significand[0],
                                   &significandLength,
                                   &sign,
                                   &pointPos,
                                   precision,
                                   value);

    char  signSymbol;
    int   signLength    = formatSign(&signSymbol, 1, sign, signFormat);
    int   bufferLength  = length - signLength;
    int   decimalLength = 0;
    char *it            = buffer;

    it += signLength;

    if (FP_NORMAL == cls || FP_ZERO == cls) {

        switch (style) {
          case DecimalFormatUtil::e_SCIENTIFIC: {
              // decimalLength = printScientific(it,
              //                                 bufferLength,
              //                                 significand,
              //                                 exponent,
              //                                 precision,
              //                                 point);
          } break;
          case DecimalFormatUtil::e_FIXED: {
              decimalLength = formatFixed(it,
                                          bufferLength,
                                          &significand[0],
                                          significandLength,
                                          pointPos,
                                          precision,
                                          point);
          } break;
          default: {
            BSLS_ASSERT(!"Unexpected format style value");
          } break;
        }
    }

    decimalLength += signLength;
    if (decimalLength <= length && signLength) {
        *buffer = signSymbol;
    }

    return decimalLength;
}

}  // close unnamed namespace

int DecimalFormatUtil::format(char       *buffer,
                              int         length,
                              Decimal32   value,
                              int         precision,
                              Style       style,
                              Sign        sign,
                              Letters     letters,
                              char        point)
{
    return formatImpl(buffer,
                      length,
                      value,
                      precision,
                      style,
                      sign,
                      letters,
                      point);
}

int DecimalFormatUtil::format(char       *buffer,
                              int         length,
                              Decimal64   value,
                              int         precision,
                              Style       style,
                              Sign        sign,
                              Letters     letters,
                              char        point)
{
    return 0;
}

int DecimalFormatUtil::format(char       *buffer,
                              int         length,
                              Decimal128  value,
                              int         precision,
                              Style       style,
                              Sign        sign,
                              Letters     letters,
                              char        point)
{
    return formatImpl(buffer,
                      length,
                      value,
                      precision,
                      style,
                      sign,
                      letters,
                      point);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
