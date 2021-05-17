#ifndef DECIMALNATIVE_H
#define DECIMALNATIVE_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
#define DECIMALNATIVE_MANGLING "C"
#else
#define DECIMALNATIVE_MANGLING
#endif

#ifdef DECIMALNATIVE_EXPORTS
#if defined(_WIN64)
#define DECIMALNATIVE_API(x) extern DECIMALNATIVE_MANGLING __declspec(dllexport) x __fastcall
#elif defined(_WIN32)
#define DECIMALNATIVE_API(x) extern DECIMALNATIVE_MANGLING __declspec(dllexport) x __stdcall
#else
#define DECIMALNATIVE_API(x) extern DECIMALNATIVE_MANGLING x __attribute__ ((visibility("default")))
#endif
#else
#if defined(_WIN64)
#define DECIMALNATIVE_API(x) extern DECIMALNATIVE_MANGLING __declspec(dllimport) x __fastcall
#elif defined(_WIN32)
#define DECIMALNATIVE_API(x) extern DECIMALNATIVE_MANGLING __declspec(dllimport) x __stdcall
#else
#define DECIMALNATIVE_API(x) extern DECIMALNATIVE_MANGLING x __attribute__ ((visibility("default")))
#endif
#endif

typedef struct {
    uint64_t val;
} decimal_native_t;

inline static uint64_t decimal_native_toUnderlying(decimal_native_t _value) {
    return _value.val;
}
inline static decimal_native_t decimal_native_fromUnderlying(uint64_t value) {
    decimal_native_t dn;
    dn.val = value;
    return dn;
}

static const uint64_t DECIMAL_NATIVE_UNDERLYING_POSITIVE_INFINITY = 0x7800000000000000ULL;
static const uint64_t DECIMAL_NATIVE_UNDERLYING_NEGATIVE_INFINITY = 0xF800000000000000ULL;
static const uint64_t DECIMAL_NATIVE_UNDERLYING_NAN = 0x7C00000000000000ULL;
static const uint64_t DECIMAL_NATIVE_UNDERLYING_NULL = 0xFFFFFFFFFFFFFF80ULL;	// = -0x80

static const uint64_t DECIMAL_NATIVE_UNDERLYING_MIN_VALUE = 0xF7FB86F26FC0FFFFULL;
static const uint64_t DECIMAL_NATIVE_UNDERLYING_MAX_VALUE = 0x77FB86F26FC0FFFFULL;

static const uint64_t DECIMAL_NATIVE_UNDERLYING_MIN_POSITIVE_VALUE = 0x0000000000000001ULL;
static const uint64_t DECIMAL_NATIVE_UNDERLYING_MAX_NEGATIVE_VALUE = 0x8000000000000001ULL;

static const uint64_t DECIMAL_NATIVE_UNDERLYING_ZERO = 0x31C0000000000000ULL; // e=0, m=0, sign=0
static const uint64_t DECIMAL_NATIVE_UNDERLYING_ONE = 0x31C0000000000001ULL;
static const uint64_t DECIMAL_NATIVE_UNDERLYING_TWO = 0x31C0000000000002ULL;
static const uint64_t DECIMAL_NATIVE_UNDERLYING_TEN = 0x31C000000000000AULL;
static const uint64_t DECIMAL_NATIVE_UNDERLYING_HUNDRED = 0x31C0000000000064ULL;
static const uint64_t DECIMAL_NATIVE_UNDERLYING_THOUSAND = 0x31C00000000003E8ULL;
static const uint64_t DECIMAL_NATIVE_UNDERLYING_MILLION = 0x31C00000000F4240ULL;

static const uint64_t DECIMAL_NATIVE_UNDERLYING_ONETENTH = 0x31A0000000000000ULL + 1;
static const uint64_t DECIMAL_NATIVE_UNDERLYING_ONEHUNDREDTH = 0x3180000000000000ULL + 1;

DECIMALNATIVE_API(int32_t) decimal_native_version();
DECIMALNATIVE_API(decimal_native_t) decimal_native_fromFixedPoint64(int64_t value, int32_t numberOfDigits);
DECIMALNATIVE_API(decimal_native_t) decimal_native_fromFixedPoint32(int32_t value, int32_t numberOfDigits);
DECIMALNATIVE_API(decimal_native_t) decimal_native_fromFixedPointU32(uint32_t value, int32_t numberOfDigits);
DECIMALNATIVE_API(int64_t) decimal_native_toFixedPoint(decimal_native_t value, int32_t numberOfDigits);
DECIMALNATIVE_API(decimal_native_t) decimal_native_fromInt64(int64_t value);
DECIMALNATIVE_API(int64_t) decimal_native_toInt64(decimal_native_t value);
DECIMALNATIVE_API(decimal_native_t) decimal_native_fromUInt64(uint64_t value);
DECIMALNATIVE_API(uint64_t) decimal_native_toUInt64(decimal_native_t value);
DECIMALNATIVE_API(decimal_native_t) decimal_native_fromInt32(int32_t value);
DECIMALNATIVE_API(int32_t) decimal_native_toInt32(decimal_native_t value);
DECIMALNATIVE_API(decimal_native_t) decimal_native_fromUInt32(uint32_t value);
DECIMALNATIVE_API(uint32_t) decimal_native_toUInt32(decimal_native_t value);
DECIMALNATIVE_API(decimal_native_t) decimal_native_fromInt16(int16_t value);
DECIMALNATIVE_API(int16_t) decimal_native_toInt16(decimal_native_t value);
DECIMALNATIVE_API(decimal_native_t) decimal_native_fromInt8(int8_t value);
DECIMALNATIVE_API(int8_t) decimal_native_toInt8(decimal_native_t value);
DECIMALNATIVE_API(uint8_t) decimal_native_toUInt8(decimal_native_t value);
DECIMALNATIVE_API(decimal_native_t) decimal_native_fromFloat64(double value);
DECIMALNATIVE_API(double) decimal_native_toFloat64(decimal_native_t value);
DECIMALNATIVE_API(decimal_native_t) decimal_native_roundTowardsPositiveInfinity(decimal_native_t a);
DECIMALNATIVE_API(decimal_native_t) decimal_native_roundTowardsNegativeInfinity(decimal_native_t a);
DECIMALNATIVE_API(decimal_native_t) decimal_native_roundTowardsZero(decimal_native_t a);
DECIMALNATIVE_API(decimal_native_t) decimal_native_roundToNearestTiesAwayFromZero(decimal_native_t a);
DECIMALNATIVE_API(uint8_t) decimal_native_isNaN(decimal_native_t a);
DECIMALNATIVE_API(uint8_t) decimal_native_isInfinity(decimal_native_t a);
DECIMALNATIVE_API(uint8_t) decimal_native_isPositiveInfinity(decimal_native_t a);
DECIMALNATIVE_API(uint8_t) decimal_native_isNegativeInfinity(decimal_native_t a);
DECIMALNATIVE_API(uint8_t) decimal_native_isFinite(decimal_native_t a);
DECIMALNATIVE_API(uint8_t) decimal_native_isNormal(decimal_native_t a);
DECIMALNATIVE_API(uint8_t) decimal_native_signBit(decimal_native_t a);
DECIMALNATIVE_API(int32_t) decimal_native_compare(decimal_native_t a, decimal_native_t b);
DECIMALNATIVE_API(uint8_t) decimal_native_isEqual(decimal_native_t a, decimal_native_t b);
DECIMALNATIVE_API(uint8_t) decimal_native_isNotEqual(decimal_native_t a, decimal_native_t b);
DECIMALNATIVE_API(uint8_t) decimal_native_isLess(decimal_native_t a, decimal_native_t b);
DECIMALNATIVE_API(uint8_t) decimal_native_isLessOrEqual(decimal_native_t a, decimal_native_t b);
DECIMALNATIVE_API(uint8_t) decimal_native_isGreater(decimal_native_t a, decimal_native_t b);
DECIMALNATIVE_API(uint8_t) decimal_native_isGreaterOrEqual(decimal_native_t a, decimal_native_t b);
DECIMALNATIVE_API(uint8_t) decimal_native_isZero(decimal_native_t a);
DECIMALNATIVE_API(uint8_t) decimal_native_isNonZero(decimal_native_t a);
DECIMALNATIVE_API(uint8_t) decimal_native_isPositive(decimal_native_t a);
DECIMALNATIVE_API(uint8_t) decimal_native_isNegative(decimal_native_t a);
DECIMALNATIVE_API(uint8_t) decimal_native_isNonPositive(decimal_native_t a);
DECIMALNATIVE_API(uint8_t) decimal_native_isNonNegative(decimal_native_t a);
DECIMALNATIVE_API(decimal_native_t) decimal_native_max2(decimal_native_t a, decimal_native_t b);
DECIMALNATIVE_API(decimal_native_t) decimal_native_max3(decimal_native_t a, decimal_native_t b, decimal_native_t c);
DECIMALNATIVE_API(decimal_native_t) decimal_native_max4(decimal_native_t a, decimal_native_t b, decimal_native_t c, decimal_native_t d);
DECIMALNATIVE_API(decimal_native_t) decimal_native_min2(decimal_native_t a, decimal_native_t b);
DECIMALNATIVE_API(decimal_native_t) decimal_native_min3(decimal_native_t a, decimal_native_t b, decimal_native_t c);
DECIMALNATIVE_API(decimal_native_t) decimal_native_min4(decimal_native_t a, decimal_native_t b, decimal_native_t c, decimal_native_t d);
DECIMALNATIVE_API(decimal_native_t) decimal_native_negate(decimal_native_t a);
DECIMALNATIVE_API(decimal_native_t) decimal_native_abs(decimal_native_t x);
DECIMALNATIVE_API(decimal_native_t) decimal_native_add2(decimal_native_t a, decimal_native_t b);
DECIMALNATIVE_API(decimal_native_t) decimal_native_add3(decimal_native_t a, decimal_native_t b, decimal_native_t c);
DECIMALNATIVE_API(decimal_native_t) decimal_native_add4(decimal_native_t a, decimal_native_t b, decimal_native_t c, decimal_native_t d);
DECIMALNATIVE_API(decimal_native_t) decimal_native_subtract(decimal_native_t a, decimal_native_t b);
DECIMALNATIVE_API(decimal_native_t) decimal_native_multiply2(decimal_native_t a, decimal_native_t b);
DECIMALNATIVE_API(decimal_native_t) decimal_native_multiply3(decimal_native_t a, decimal_native_t b, decimal_native_t c);
DECIMALNATIVE_API(decimal_native_t) decimal_native_multiply4(decimal_native_t a, decimal_native_t b, decimal_native_t c, decimal_native_t d);
DECIMALNATIVE_API(decimal_native_t) decimal_native_multiplyByInt32(decimal_native_t a, int32_t b);
DECIMALNATIVE_API(decimal_native_t) decimal_native_multiplyByInt64(decimal_native_t a, int64_t b);
DECIMALNATIVE_API(decimal_native_t) decimal_native_divide(decimal_native_t a, decimal_native_t b);
DECIMALNATIVE_API(decimal_native_t) decimal_native_divideByInt32(decimal_native_t a, int32_t b);
DECIMALNATIVE_API(decimal_native_t) decimal_native_divideByInt64(decimal_native_t a, int64_t b);
DECIMALNATIVE_API(decimal_native_t) decimal_native_multiplyAndAdd(decimal_native_t a, decimal_native_t b, decimal_native_t c);
DECIMALNATIVE_API(decimal_native_t) decimal_native_scaleByPowerOfTen(decimal_native_t a, int32_t n);
DECIMALNATIVE_API(decimal_native_t) decimal_native_mean2(decimal_native_t a, decimal_native_t b);
DECIMALNATIVE_API(decimal_native_t) decimal_native_nextUp(decimal_native_t a);
DECIMALNATIVE_API(decimal_native_t) decimal_native_nextDown(decimal_native_t a);
DECIMALNATIVE_API(void) decimal_native_toString(decimal_native_t a, char* str);
DECIMALNATIVE_API(decimal_native_t) decimal_native_fromString(const char* str);

#endif
