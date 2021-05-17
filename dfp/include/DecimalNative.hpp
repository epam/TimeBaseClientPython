#pragma once
#ifndef DECIMALNATIVE_HPP
#define DECIMALNATIVE_HPP

#include "DecimalNative.h"
#include <iostream>
#include <string>
#include <cstring>
#include <stdexcept>

#define DN_STRINGIFY(x) #x
#define DN_TOSTRING(x) DN_STRINGIFY(x)
#if defined(_MSC_VER)
#define DN_FUNC (std::string("At ") + __FILE__ + "[" + DN_TOSTRING(__LINE__) + "] " + __FUNCSIG__)
#elif defined(__GNUC__)
#define DN_FUNC (std::string("At ") + __FILE__ + "[" + DN_TOSTRING(__LINE__) + "] " + __PRETTY_FUNCTION__)
#else
#define DN_FUNC (std::string("At ") + __FILE__ + "[" + DN_TOSTRING(__LINE__) + "] " + __func__)
#endif
#define DN_NAMEOF(a) #a

inline std::ostream& operator <<(std::ostream& output, decimal_native_t const& a) {
    if (a.val == DECIMAL_NATIVE_UNDERLYING_NULL) {
        output << "null";
    }
    else {
        char str[32];
        decimal_native_toString(a, str);
        output << str;
    }
    return output;
}
inline std::istream& operator >>(std::istream& input, decimal_native_t& a) {
    std::string word;
    input >> word;
    a = word == "null"
        ? decimal_native_fromUnderlying(DECIMAL_NATIVE_UNDERLYING_NULL)
        : decimal_native_fromString(word.c_str());
    return input;
}

namespace deltix {
    namespace dfp {

        template <bool nullCheck = false>
        class Decimal64_t {
        protected:
            decimal_native_t _value;

        public:
            uint64_t toUnderlying() const {
                return _value.val;
            }
            static Decimal64_t fromUnderlying(uint64_t value) {
                decimal_native_t dn;
                dn.val = value;
                return Decimal64_t(dn);
            }
            Decimal64_t() {
            }
            Decimal64_t(Decimal64_t const &b) {
                _value = b._value;
            }
            Decimal64_t& operator =(Decimal64_t const &b) {
                _value = b._value;
                return *this;
            }
            Decimal64_t(decimal_native_t const &b) {
                _value = b;
            }
            Decimal64_t& operator =(decimal_native_t const &b) {
                _value = b;
                return *this;
            }
            explicit operator decimal_native_t() const {
                return _value;
            }
            Decimal64_t operator +(Decimal64_t const& b) const {
                return add(*this, b);
            }
            Decimal64_t& operator +=(Decimal64_t const& b) {
                *this = add(*this, b);
                return *this;
            }
            Decimal64_t& operator++() {
                *this += fromUnderlying(DECIMAL_NATIVE_UNDERLYING_ONE);
                return *this;
            }
            Decimal64_t operator++(int) {
                Decimal64_t ret = *this;
                ++*this;
                return ret;
            }
            Decimal64_t operator -(Decimal64_t const& b) const {
                return subtract(*this, b);
            }
            Decimal64_t& operator -=(Decimal64_t const& b) {
                *this = subtract(*this, b);
                return *this;
            }
            Decimal64_t& operator--() {
                *this -= fromUnderlying(DECIMAL_NATIVE_UNDERLYING_ONE);
                return *this;
            }
            Decimal64_t operator--(int) {
                Decimal64_t ret = *this;
                --*this;
                return ret;
            }
            Decimal64_t operator *(Decimal64_t const& b) const {
                return multiply(*this, b);
            }
            Decimal64_t& operator *=(Decimal64_t const& b) {
                *this = multiply(*this, b);
                return *this;
            }
            Decimal64_t operator /(Decimal64_t const& b) const {
                return divide(*this, b);
            }
            Decimal64_t& operator /=(Decimal64_t const& b) {
                *this = divide(*this, b);
                return *this;
            }
            explicit operator std::string() const {
                char str[32];
                toString(str);
                return std::string(str);
            }
            friend std::ostream& operator <<(std::ostream& output, Decimal64_t const& a) {
                output << a._value;
                return output;
            }
            friend std::istream& operator >>(std::istream& input, Decimal64_t& a) {
                decimal_native_t val;
                input >> val;
                a = Decimal64_t(val);
                return input;
            }
            bool isNull() const {
                return toUnderlying() == DECIMAL_NATIVE_UNDERLYING_NULL;
            }


            static int32_t version() {
                return decimal_native_version();
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t fromFixedPoint(int64_t value, int32_t numberOfDigits) {
                return Decimal64_t(decimal_native_fromFixedPoint64(value, numberOfDigits));
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t fromFixedPoint(int32_t value, int32_t numberOfDigits) {
                return Decimal64_t(decimal_native_fromFixedPoint32(value, numberOfDigits));
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t fromFixedPoint(uint32_t value, int32_t numberOfDigits) {
                return Decimal64_t(decimal_native_fromFixedPointU32(value, numberOfDigits));
            }

//--------------------------------------------------------------------------------------------------------

            int64_t toFixedPoint(int32_t numberOfDigits) const {
                return nullCheck
                    ? toFixedPointChecked(numberOfDigits)
                    : toFixedPointUnchecked(numberOfDigits);
            }

            int64_t toFixedPointChecked(int32_t numberOfDigits) const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return toFixedPointUnchecked(numberOfDigits);
            }

            int64_t toFixedPointUnchecked(int32_t numberOfDigits) const {
                return decimal_native_toFixedPoint(_value, numberOfDigits);
            }

//--------------------------------------------------------------------------------------------------------

            explicit Decimal64_t(int64_t value) {
                _value = decimal_native_fromInt64(value);
            }

//--------------------------------------------------------------------------------------------------------
            explicit operator int64_t() const {
                return toInt64(_value);
            }

            int64_t toInt64() const {
                return nullCheck
                    ? toInt64Checked()
                    : toInt64Unchecked();
            }

            int64_t toInt64Checked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return toInt64Unchecked();
            }

            int64_t toInt64Unchecked() const {
                return decimal_native_toInt64(_value);
            }

//--------------------------------------------------------------------------------------------------------

            explicit Decimal64_t(uint64_t value) {
                _value = decimal_native_fromUInt64(value);
            }

//--------------------------------------------------------------------------------------------------------
            explicit operator uint64_t() const {
                return toUInt64(_value);
            }

            uint64_t toUInt64() const {
                return nullCheck
                    ? toUInt64Checked()
                    : toUInt64Unchecked();
            }

            uint64_t toUInt64Checked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return toUInt64Unchecked();
            }

            uint64_t toUInt64Unchecked() const {
                return decimal_native_toUInt64(_value);
            }

//--------------------------------------------------------------------------------------------------------

            explicit Decimal64_t(int32_t value) {
                _value = decimal_native_fromInt32(value);
            }

//--------------------------------------------------------------------------------------------------------
            explicit operator int32_t() const {
                return toInt32(_value);
            }

            int32_t toInt32() const {
                return nullCheck
                    ? toInt32Checked()
                    : toInt32Unchecked();
            }

            int32_t toInt32Checked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return toInt32Unchecked();
            }

            int32_t toInt32Unchecked() const {
                return decimal_native_toInt32(_value);
            }

//--------------------------------------------------------------------------------------------------------

            explicit Decimal64_t(uint32_t value) {
                _value = decimal_native_fromUInt32(value);
            }

//--------------------------------------------------------------------------------------------------------
            explicit operator uint32_t() const {
                return toUInt32(_value);
            }

            uint32_t toUInt32() const {
                return nullCheck
                    ? toUInt32Checked()
                    : toUInt32Unchecked();
            }

            uint32_t toUInt32Checked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return toUInt32Unchecked();
            }

            uint32_t toUInt32Unchecked() const {
                return decimal_native_toUInt32(_value);
            }

//--------------------------------------------------------------------------------------------------------

            explicit Decimal64_t(int16_t value) {
                _value = decimal_native_fromInt16(value);
            }

//--------------------------------------------------------------------------------------------------------
            explicit operator int16_t() const {
                return toInt16(_value);
            }

            int16_t toInt16() const {
                return nullCheck
                    ? toInt16Checked()
                    : toInt16Unchecked();
            }

            int16_t toInt16Checked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return toInt16Unchecked();
            }

            int16_t toInt16Unchecked() const {
                return decimal_native_toInt16(_value);
            }

//--------------------------------------------------------------------------------------------------------

            explicit Decimal64_t(int8_t value) {
                _value = decimal_native_fromInt8(value);
            }

//--------------------------------------------------------------------------------------------------------
            explicit operator int8_t() const {
                return toInt8(_value);
            }

            int8_t toInt8() const {
                return nullCheck
                    ? toInt8Checked()
                    : toInt8Unchecked();
            }

            int8_t toInt8Checked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return toInt8Unchecked();
            }

            int8_t toInt8Unchecked() const {
                return decimal_native_toInt8(_value);
            }

//--------------------------------------------------------------------------------------------------------
            explicit operator uint8_t() const {
                return toUInt8(_value);
            }

            uint8_t toUInt8() const {
                return nullCheck
                    ? toUInt8Checked()
                    : toUInt8Unchecked();
            }

            uint8_t toUInt8Checked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return toUInt8Unchecked();
            }

            uint8_t toUInt8Unchecked() const {
                return decimal_native_toUInt8(_value);
            }

//--------------------------------------------------------------------------------------------------------

            explicit Decimal64_t(double value) {
                _value = decimal_native_fromFloat64(value);
            }

//--------------------------------------------------------------------------------------------------------
            explicit operator double() const {
                return toFloat64(_value);
            }

            double toFloat64() const {
                return nullCheck
                    ? toFloat64Checked()
                    : toFloat64Unchecked();
            }

            double toFloat64Checked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return toFloat64Unchecked();
            }

            double toFloat64Unchecked() const {
                return decimal_native_toFloat64(_value);
            }

//--------------------------------------------------------------------------------------------------------

            Decimal64_t& roundTowardsPositiveInfinity() {
                return nullCheck
                    ? roundTowardsPositiveInfinityChecked()
                    : roundTowardsPositiveInfinityUnchecked();
            }

            Decimal64_t& roundTowardsPositiveInfinityChecked() {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return roundTowardsPositiveInfinityUnchecked();
            }

            Decimal64_t& roundTowardsPositiveInfinityUnchecked() {
                _value = decimal_native_roundTowardsPositiveInfinity(_value);

                return *this;
            }

//--------------------------------------------------------------------------------------------------------

            Decimal64_t& roundTowardsNegativeInfinity() {
                return nullCheck
                    ? roundTowardsNegativeInfinityChecked()
                    : roundTowardsNegativeInfinityUnchecked();
            }

            Decimal64_t& roundTowardsNegativeInfinityChecked() {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return roundTowardsNegativeInfinityUnchecked();
            }

            Decimal64_t& roundTowardsNegativeInfinityUnchecked() {
                _value = decimal_native_roundTowardsNegativeInfinity(_value);

                return *this;
            }

//--------------------------------------------------------------------------------------------------------

            Decimal64_t& roundTowardsZero() {
                return nullCheck
                    ? roundTowardsZeroChecked()
                    : roundTowardsZeroUnchecked();
            }

            Decimal64_t& roundTowardsZeroChecked() {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return roundTowardsZeroUnchecked();
            }

            Decimal64_t& roundTowardsZeroUnchecked() {
                _value = decimal_native_roundTowardsZero(_value);

                return *this;
            }

//--------------------------------------------------------------------------------------------------------

            Decimal64_t& roundToNearestTiesAwayFromZero() {
                return nullCheck
                    ? roundToNearestTiesAwayFromZeroChecked()
                    : roundToNearestTiesAwayFromZeroUnchecked();
            }

            Decimal64_t& roundToNearestTiesAwayFromZeroChecked() {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return roundToNearestTiesAwayFromZeroUnchecked();
            }

            Decimal64_t& roundToNearestTiesAwayFromZeroUnchecked() {
                _value = decimal_native_roundToNearestTiesAwayFromZero(_value);

                return *this;
            }

//--------------------------------------------------------------------------------------------------------

            bool isNaN() const {
                return nullCheck
                    ? isNaNChecked()
                    : isNaNUnchecked();
            }

            bool isNaNChecked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return isNaNUnchecked();
            }

            bool isNaNUnchecked() const {
                return decimal_native_isNaN(_value) != 0;
            }

//--------------------------------------------------------------------------------------------------------

            bool isInfinity() const {
                return nullCheck
                    ? isInfinityChecked()
                    : isInfinityUnchecked();
            }

            bool isInfinityChecked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return isInfinityUnchecked();
            }

            bool isInfinityUnchecked() const {
                return decimal_native_isInfinity(_value) != 0;
            }

//--------------------------------------------------------------------------------------------------------

            bool isPositiveInfinity() const {
                return nullCheck
                    ? isPositiveInfinityChecked()
                    : isPositiveInfinityUnchecked();
            }

            bool isPositiveInfinityChecked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return isPositiveInfinityUnchecked();
            }

            bool isPositiveInfinityUnchecked() const {
                return decimal_native_isPositiveInfinity(_value) != 0;
            }

//--------------------------------------------------------------------------------------------------------

            bool isNegativeInfinity() const {
                return nullCheck
                    ? isNegativeInfinityChecked()
                    : isNegativeInfinityUnchecked();
            }

            bool isNegativeInfinityChecked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return isNegativeInfinityUnchecked();
            }

            bool isNegativeInfinityUnchecked() const {
                return decimal_native_isNegativeInfinity(_value) != 0;
            }

//--------------------------------------------------------------------------------------------------------

            bool isFinite() const {
                return nullCheck
                    ? isFiniteChecked()
                    : isFiniteUnchecked();
            }

            bool isFiniteChecked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return isFiniteUnchecked();
            }

            bool isFiniteUnchecked() const {
                return decimal_native_isFinite(_value) != 0;
            }

//--------------------------------------------------------------------------------------------------------

            bool isNormal() const {
                return nullCheck
                    ? isNormalChecked()
                    : isNormalUnchecked();
            }

            bool isNormalChecked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return isNormalUnchecked();
            }

            bool isNormalUnchecked() const {
                return decimal_native_isNormal(_value) != 0;
            }

//--------------------------------------------------------------------------------------------------------

            bool signBit() const {
                return nullCheck
                    ? signBitChecked()
                    : signBitUnchecked();
            }

            bool signBitChecked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return signBitUnchecked();
            }

            bool signBitUnchecked() const {
                return decimal_native_signBit(_value) != 0;
            }

//--------------------------------------------------------------------------------------------------------

            int32_t compare(Decimal64_t const& b) const {
                return nullCheck
                    ? compareChecked(b)
                    : compareUnchecked(b);
            }

            int32_t compareChecked(Decimal64_t const& b) const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");

                return compareUnchecked(b);
            }

            int32_t compareUnchecked(Decimal64_t const& b) const {
                return decimal_native_compare(_value, b._value);
            }

//--------------------------------------------------------------------------------------------------------
            bool operator ==(Decimal64_t const &b) const {
                if (toUnderlying() == DECIMAL_NATIVE_UNDERLYING_NULL &&
                    b.toUnderlying() == DECIMAL_NATIVE_UNDERLYING_NULL)
                    return true;

                return decimal_native_isEqual(_value, b._value) != 0;
            }


//--------------------------------------------------------------------------------------------------------
            bool operator !=(Decimal64_t const &b) const {
                if (toUnderlying() == DECIMAL_NATIVE_UNDERLYING_NULL &&
                    b.toUnderlying() == DECIMAL_NATIVE_UNDERLYING_NULL)
                    return false;

                return decimal_native_isNotEqual(_value, b._value) != 0;
            }


//--------------------------------------------------------------------------------------------------------
            bool operator <(Decimal64_t const &b) const {
                return isLess(b);
            }

            bool isLess(Decimal64_t const &b) const {
                return nullCheck
                    ? isLessChecked(b)
                    : isLessUnchecked(b);
            }

            bool isLessChecked(Decimal64_t const &b) const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");

                return isLessUnchecked(b);
            }

            bool isLessUnchecked(Decimal64_t const &b) const {
                return decimal_native_isLess(_value, b._value) != 0;
            }

//--------------------------------------------------------------------------------------------------------
            bool operator <=(Decimal64_t const &b) const {
                return isLessOrEqual(b);
            }

            bool isLessOrEqual(Decimal64_t const &b) const {
                return nullCheck
                    ? isLessOrEqualChecked(b)
                    : isLessOrEqualUnchecked(b);
            }

            bool isLessOrEqualChecked(Decimal64_t const &b) const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");

                return isLessOrEqualUnchecked(b);
            }

            bool isLessOrEqualUnchecked(Decimal64_t const &b) const {
                return decimal_native_isLessOrEqual(_value, b._value) != 0;
            }

//--------------------------------------------------------------------------------------------------------
            bool operator >(Decimal64_t const &b) const {
                return isGreater(b);
            }

            bool isGreater(Decimal64_t const &b) const {
                return nullCheck
                    ? isGreaterChecked(b)
                    : isGreaterUnchecked(b);
            }

            bool isGreaterChecked(Decimal64_t const &b) const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");

                return isGreaterUnchecked(b);
            }

            bool isGreaterUnchecked(Decimal64_t const &b) const {
                return decimal_native_isGreater(_value, b._value) != 0;
            }

//--------------------------------------------------------------------------------------------------------
            bool operator >=(Decimal64_t const &b) const {
                return isGreaterOrEqual(b);
            }

            bool isGreaterOrEqual(Decimal64_t const &b) const {
                return nullCheck
                    ? isGreaterOrEqualChecked(b)
                    : isGreaterOrEqualUnchecked(b);
            }

            bool isGreaterOrEqualChecked(Decimal64_t const &b) const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");

                return isGreaterOrEqualUnchecked(b);
            }

            bool isGreaterOrEqualUnchecked(Decimal64_t const &b) const {
                return decimal_native_isGreaterOrEqual(_value, b._value) != 0;
            }

//--------------------------------------------------------------------------------------------------------

            bool isZero() const {
                return nullCheck
                    ? isZeroChecked()
                    : isZeroUnchecked();
            }

            bool isZeroChecked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return isZeroUnchecked();
            }

            bool isZeroUnchecked() const {
                return decimal_native_isZero(_value) != 0;
            }

//--------------------------------------------------------------------------------------------------------

            bool isNonZero() const {
                return nullCheck
                    ? isNonZeroChecked()
                    : isNonZeroUnchecked();
            }

            bool isNonZeroChecked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return isNonZeroUnchecked();
            }

            bool isNonZeroUnchecked() const {
                return decimal_native_isNonZero(_value) != 0;
            }

//--------------------------------------------------------------------------------------------------------

            bool isPositive() const {
                return nullCheck
                    ? isPositiveChecked()
                    : isPositiveUnchecked();
            }

            bool isPositiveChecked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return isPositiveUnchecked();
            }

            bool isPositiveUnchecked() const {
                return decimal_native_isPositive(_value) != 0;
            }

//--------------------------------------------------------------------------------------------------------

            bool isNegative() const {
                return nullCheck
                    ? isNegativeChecked()
                    : isNegativeUnchecked();
            }

            bool isNegativeChecked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return isNegativeUnchecked();
            }

            bool isNegativeUnchecked() const {
                return decimal_native_isNegative(_value) != 0;
            }

//--------------------------------------------------------------------------------------------------------

            bool isNonPositive() const {
                return nullCheck
                    ? isNonPositiveChecked()
                    : isNonPositiveUnchecked();
            }

            bool isNonPositiveChecked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return isNonPositiveUnchecked();
            }

            bool isNonPositiveUnchecked() const {
                return decimal_native_isNonPositive(_value) != 0;
            }

//--------------------------------------------------------------------------------------------------------

            bool isNonNegative() const {
                return nullCheck
                    ? isNonNegativeChecked()
                    : isNonNegativeUnchecked();
            }

            bool isNonNegativeChecked() const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return isNonNegativeUnchecked();
            }

            bool isNonNegativeUnchecked() const {
                return decimal_native_isNonNegative(_value) != 0;
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t max(Decimal64_t const& a, Decimal64_t const& b) {
                return nullCheck
                    ? maxChecked(a, b)
                    : maxUnchecked(a, b);
            }

            static Decimal64_t maxChecked(Decimal64_t const& a, Decimal64_t const& b) {
                if (a.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(a) + "' is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");

                return maxUnchecked(a, b);
            }

            static Decimal64_t maxUnchecked(Decimal64_t const& a, Decimal64_t const& b) {
                return Decimal64_t(decimal_native_max2(a._value, b._value));
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t max(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c) {
                return nullCheck
                    ? maxChecked(a, b, c)
                    : maxUnchecked(a, b, c);
            }

            static Decimal64_t maxChecked(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c) {
                if (a.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(a) + "' is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");
                if (c.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(c) + "' is null.");

                return maxUnchecked(a, b, c);
            }

            static Decimal64_t maxUnchecked(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c) {
                return Decimal64_t(decimal_native_max3(a._value, b._value, c._value));
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t max(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c, Decimal64_t const& d) {
                return nullCheck
                    ? maxChecked(a, b, c, d)
                    : maxUnchecked(a, b, c, d);
            }

            static Decimal64_t maxChecked(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c, Decimal64_t const& d) {
                if (a.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(a) + "' is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");
                if (c.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(c) + "' is null.");
                if (d.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(d) + "' is null.");

                return maxUnchecked(a, b, c, d);
            }

            static Decimal64_t maxUnchecked(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c, Decimal64_t const& d) {
                return Decimal64_t(decimal_native_max4(a._value, b._value, c._value, d._value));
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t min(Decimal64_t const& a, Decimal64_t const& b) {
                return nullCheck
                    ? minChecked(a, b)
                    : minUnchecked(a, b);
            }

            static Decimal64_t minChecked(Decimal64_t const& a, Decimal64_t const& b) {
                if (a.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(a) + "' is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");

                return minUnchecked(a, b);
            }

            static Decimal64_t minUnchecked(Decimal64_t const& a, Decimal64_t const& b) {
                return Decimal64_t(decimal_native_min2(a._value, b._value));
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t min(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c) {
                return nullCheck
                    ? minChecked(a, b, c)
                    : minUnchecked(a, b, c);
            }

            static Decimal64_t minChecked(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c) {
                if (a.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(a) + "' is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");
                if (c.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(c) + "' is null.");

                return minUnchecked(a, b, c);
            }

            static Decimal64_t minUnchecked(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c) {
                return Decimal64_t(decimal_native_min3(a._value, b._value, c._value));
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t min(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c, Decimal64_t const& d) {
                return nullCheck
                    ? minChecked(a, b, c, d)
                    : minUnchecked(a, b, c, d);
            }

            static Decimal64_t minChecked(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c, Decimal64_t const& d) {
                if (a.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(a) + "' is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");
                if (c.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(c) + "' is null.");
                if (d.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(d) + "' is null.");

                return minUnchecked(a, b, c, d);
            }

            static Decimal64_t minUnchecked(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c, Decimal64_t const& d) {
                return Decimal64_t(decimal_native_min4(a._value, b._value, c._value, d._value));
            }

//--------------------------------------------------------------------------------------------------------

            Decimal64_t& negate() {
                return nullCheck
                    ? negateChecked()
                    : negateUnchecked();
            }

            Decimal64_t& negateChecked() {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return negateUnchecked();
            }

            Decimal64_t& negateUnchecked() {
                _value = decimal_native_negate(_value);

                return *this;
            }

//--------------------------------------------------------------------------------------------------------

            Decimal64_t& abs() {
                return nullCheck
                    ? absChecked()
                    : absUnchecked();
            }

            Decimal64_t& absChecked() {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return absUnchecked();
            }

            Decimal64_t& absUnchecked() {
                _value = decimal_native_abs(_value);

                return *this;
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t add(Decimal64_t const& a, Decimal64_t const& b) {
                return nullCheck
                    ? addChecked(a, b)
                    : addUnchecked(a, b);
            }

            static Decimal64_t addChecked(Decimal64_t const& a, Decimal64_t const& b) {
                if (a.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(a) + "' is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");

                return addUnchecked(a, b);
            }

            static Decimal64_t addUnchecked(Decimal64_t const& a, Decimal64_t const& b) {
                return Decimal64_t(decimal_native_add2(a._value, b._value));
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t add(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c) {
                return nullCheck
                    ? addChecked(a, b, c)
                    : addUnchecked(a, b, c);
            }

            static Decimal64_t addChecked(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c) {
                if (a.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(a) + "' is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");
                if (c.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(c) + "' is null.");

                return addUnchecked(a, b, c);
            }

            static Decimal64_t addUnchecked(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c) {
                return Decimal64_t(decimal_native_add3(a._value, b._value, c._value));
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t add(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c, Decimal64_t const& d) {
                return nullCheck
                    ? addChecked(a, b, c, d)
                    : addUnchecked(a, b, c, d);
            }

            static Decimal64_t addChecked(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c, Decimal64_t const& d) {
                if (a.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(a) + "' is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");
                if (c.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(c) + "' is null.");
                if (d.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(d) + "' is null.");

                return addUnchecked(a, b, c, d);
            }

            static Decimal64_t addUnchecked(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c, Decimal64_t const& d) {
                return Decimal64_t(decimal_native_add4(a._value, b._value, c._value, d._value));
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t subtract(Decimal64_t const& a, Decimal64_t const& b) {
                return nullCheck
                    ? subtractChecked(a, b)
                    : subtractUnchecked(a, b);
            }

            static Decimal64_t subtractChecked(Decimal64_t const& a, Decimal64_t const& b) {
                if (a.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(a) + "' is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");

                return subtractUnchecked(a, b);
            }

            static Decimal64_t subtractUnchecked(Decimal64_t const& a, Decimal64_t const& b) {
                return Decimal64_t(decimal_native_subtract(a._value, b._value));
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t multiply(Decimal64_t const& a, Decimal64_t const& b) {
                return nullCheck
                    ? multiplyChecked(a, b)
                    : multiplyUnchecked(a, b);
            }

            static Decimal64_t multiplyChecked(Decimal64_t const& a, Decimal64_t const& b) {
                if (a.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(a) + "' is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");

                return multiplyUnchecked(a, b);
            }

            static Decimal64_t multiplyUnchecked(Decimal64_t const& a, Decimal64_t const& b) {
                return Decimal64_t(decimal_native_multiply2(a._value, b._value));
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t multiply(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c) {
                return nullCheck
                    ? multiplyChecked(a, b, c)
                    : multiplyUnchecked(a, b, c);
            }

            static Decimal64_t multiplyChecked(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c) {
                if (a.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(a) + "' is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");
                if (c.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(c) + "' is null.");

                return multiplyUnchecked(a, b, c);
            }

            static Decimal64_t multiplyUnchecked(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c) {
                return Decimal64_t(decimal_native_multiply3(a._value, b._value, c._value));
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t multiply(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c, Decimal64_t const& d) {
                return nullCheck
                    ? multiplyChecked(a, b, c, d)
                    : multiplyUnchecked(a, b, c, d);
            }

            static Decimal64_t multiplyChecked(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c, Decimal64_t const& d) {
                if (a.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(a) + "' is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");
                if (c.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(c) + "' is null.");
                if (d.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(d) + "' is null.");

                return multiplyUnchecked(a, b, c, d);
            }

            static Decimal64_t multiplyUnchecked(Decimal64_t const& a, Decimal64_t const& b, Decimal64_t const& c, Decimal64_t const& d) {
                return Decimal64_t(decimal_native_multiply4(a._value, b._value, c._value, d._value));
            }

//--------------------------------------------------------------------------------------------------------
            Decimal64_t operator *(int32_t b) const {
                return multiplyByInt32(_value, b);
            }

            Decimal64_t& operator *=(int32_t b) {
                _value = multiplyByInt32(_value, b);

                return *this;
            }

            Decimal64_t multiplyByInt32(int32_t b) const {
                return nullCheck
                    ? multiplyByInt32Checked(b)
                    : multiplyByInt32Unchecked(b);
            }

            Decimal64_t multiplyByInt32Checked(int32_t b) const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return multiplyByInt32Unchecked(b);
            }

            Decimal64_t multiplyByInt32Unchecked(int32_t b) const {
                return Decimal64_t(decimal_native_multiplyByInt32(_value, b));
            }

//--------------------------------------------------------------------------------------------------------
            Decimal64_t operator *(int64_t b) const {
                return multiplyByInt64(_value, b);
            }

            Decimal64_t& operator *=(int64_t b) {
                _value = multiplyByInt64(_value, b);

                return *this;
            }

            Decimal64_t multiplyByInt64(int64_t b) const {
                return nullCheck
                    ? multiplyByInt64Checked(b)
                    : multiplyByInt64Unchecked(b);
            }

            Decimal64_t multiplyByInt64Checked(int64_t b) const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return multiplyByInt64Unchecked(b);
            }

            Decimal64_t multiplyByInt64Unchecked(int64_t b) const {
                return Decimal64_t(decimal_native_multiplyByInt64(_value, b));
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t divide(Decimal64_t const& a, Decimal64_t const& b) {
                return nullCheck
                    ? divideChecked(a, b)
                    : divideUnchecked(a, b);
            }

            static Decimal64_t divideChecked(Decimal64_t const& a, Decimal64_t const& b) {
                if (a.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(a) + "' is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");

                return divideUnchecked(a, b);
            }

            static Decimal64_t divideUnchecked(Decimal64_t const& a, Decimal64_t const& b) {
                return Decimal64_t(decimal_native_divide(a._value, b._value));
            }

//--------------------------------------------------------------------------------------------------------
            Decimal64_t operator /(int32_t b) const {
                return divideByInt32(_value, b);
            }

            Decimal64_t& operator /=(int32_t b) {
                _value = divideByInt32(_value, b);

                return *this;
            }

            Decimal64_t divideByInt32(int32_t b) const {
                return nullCheck
                    ? divideByInt32Checked(b)
                    : divideByInt32Unchecked(b);
            }

            Decimal64_t divideByInt32Checked(int32_t b) const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return divideByInt32Unchecked(b);
            }

            Decimal64_t divideByInt32Unchecked(int32_t b) const {
                return Decimal64_t(decimal_native_divideByInt32(_value, b));
            }

//--------------------------------------------------------------------------------------------------------
            Decimal64_t operator /(int64_t b) const {
                return divideByInt64(_value, b);
            }

            Decimal64_t& operator /=(int64_t b) {
                _value = divideByInt64(_value, b);

                return *this;
            }

            Decimal64_t divideByInt64(int64_t b) const {
                return nullCheck
                    ? divideByInt64Checked(b)
                    : divideByInt64Unchecked(b);
            }

            Decimal64_t divideByInt64Checked(int64_t b) const {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return divideByInt64Unchecked(b);
            }

            Decimal64_t divideByInt64Unchecked(int64_t b) const {
                return Decimal64_t(decimal_native_divideByInt64(_value, b));
            }

//--------------------------------------------------------------------------------------------------------

            Decimal64_t& multiplyAndAdd(Decimal64_t const& a, Decimal64_t const& b) {
                return nullCheck
                    ? multiplyAndAddChecked(a, b)
                    : multiplyAndAddUnchecked(a, b);
            }

            Decimal64_t& multiplyAndAddChecked(Decimal64_t const& a, Decimal64_t const& b) {
                if (a.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(a) + "' is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return multiplyAndAddUnchecked(a, b);
            }

            Decimal64_t& multiplyAndAddUnchecked(Decimal64_t const& a, Decimal64_t const& b) {
                _value = decimal_native_multiplyAndAdd(a._value, b._value, _value);

                return *this;
            }

//--------------------------------------------------------------------------------------------------------

            Decimal64_t& scaleByPowerOfTen(int32_t n) {
                return nullCheck
                    ? scaleByPowerOfTenChecked(n)
                    : scaleByPowerOfTenUnchecked(n);
            }

            Decimal64_t& scaleByPowerOfTenChecked(int32_t n) {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return scaleByPowerOfTenUnchecked(n);
            }

            Decimal64_t& scaleByPowerOfTenUnchecked(int32_t n) {
                _value = decimal_native_scaleByPowerOfTen(_value, n);

                return *this;
            }

//--------------------------------------------------------------------------------------------------------

            static Decimal64_t mean(Decimal64_t const& a, Decimal64_t const& b) {
                return nullCheck
                    ? meanChecked(a, b)
                    : meanUnchecked(a, b);
            }

            static Decimal64_t meanChecked(Decimal64_t const& a, Decimal64_t const& b) {
                if (a.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(a) + "' is null.");
                if (b.isNull())
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(b) + "' is null.");

                return meanUnchecked(a, b);
            }

            static Decimal64_t meanUnchecked(Decimal64_t const& a, Decimal64_t const& b) {
                return Decimal64_t(decimal_native_mean2(a._value, b._value));
            }

//--------------------------------------------------------------------------------------------------------

            Decimal64_t& nextUp() {
                return nullCheck
                    ? nextUpChecked()
                    : nextUpUnchecked();
            }

            Decimal64_t& nextUpChecked() {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return nextUpUnchecked();
            }

            Decimal64_t& nextUpUnchecked() {
                _value = decimal_native_nextUp(_value);

                return *this;
            }

//--------------------------------------------------------------------------------------------------------

            Decimal64_t& nextDown() {
                return nullCheck
                    ? nextDownChecked()
                    : nextDownUnchecked();
            }

            Decimal64_t& nextDownChecked() {
                if (isNull())
                    throw std::invalid_argument(DN_FUNC + ": This object is null.");

                return nextDownUnchecked();
            }

            Decimal64_t& nextDownUnchecked() {
                _value = decimal_native_nextDown(_value);

                return *this;
            }

//--------------------------------------------------------------------------------------------------------

            void toString(char* dst) const {
                if (dst == nullptr)
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(dst) + "' is nullptr.");
                if (isNull())
                    memcpy(dst, "null", 5 * sizeof(char));
                else
                    decimal_native_toString(_value, dst);
            }

//--------------------------------------------------------------------------------------------------------

            explicit Decimal64_t(const char* str) {
                if (str == nullptr)
                    throw std::invalid_argument(DN_FUNC + ": Argument '" + DN_NAMEOF(str) + "' is nullptr.");
                decimal_native_t _value;
                if (!strncmp(str, "null", 4) && (str[4] == '\0' || strchr(" \f\n\r\t\v", str[4])))
                    _value = decimal_native_fromUnderlying(DECIMAL_NATIVE_UNDERLYING_NULL);
                else
                    _value = decimal_native_fromString(str);
            }

//--------------------------------------------------------------------------------------------------------
        };

        typedef Decimal64_t<> Decimal64;

        static const Decimal64 D64_POSITIVE_INFINITY = Decimal64::fromUnderlying(DECIMAL_NATIVE_UNDERLYING_POSITIVE_INFINITY);
        static const Decimal64 D64_NEGATIVE_INFINITY = Decimal64::fromUnderlying(DECIMAL_NATIVE_UNDERLYING_NEGATIVE_INFINITY);
        static const Decimal64 D64_NAN = Decimal64::fromUnderlying(DECIMAL_NATIVE_UNDERLYING_NAN);
        static const Decimal64 D64_NULL = Decimal64::fromUnderlying(DECIMAL_NATIVE_UNDERLYING_NULL); // = -0x80

        static const Decimal64 D64_MIN_VALUE = Decimal64::fromUnderlying(DECIMAL_NATIVE_UNDERLYING_MIN_VALUE);
        static const Decimal64 D64_MAX_VALUE = Decimal64::fromUnderlying(DECIMAL_NATIVE_UNDERLYING_MAX_VALUE);

        static const Decimal64 D64_MIN_POSITIVE_VALUE = Decimal64::fromUnderlying(DECIMAL_NATIVE_UNDERLYING_MIN_POSITIVE_VALUE);
        static const Decimal64 D64_MAX_NEGATIVE_VALUE = Decimal64::fromUnderlying(DECIMAL_NATIVE_UNDERLYING_MAX_NEGATIVE_VALUE);

        static const Decimal64 D64_ZERO = Decimal64::fromUnderlying(DECIMAL_NATIVE_UNDERLYING_ZERO); // e=0, m=0, sign=0
        static const Decimal64 D64_ONE = Decimal64::fromUnderlying(DECIMAL_NATIVE_UNDERLYING_ONE);
        static const Decimal64 D64_TWO = Decimal64::fromUnderlying(DECIMAL_NATIVE_UNDERLYING_TWO);
        static const Decimal64 D64_TEN = Decimal64::fromUnderlying(DECIMAL_NATIVE_UNDERLYING_TEN);
        static const Decimal64 D64_HUNDRED = Decimal64::fromUnderlying(DECIMAL_NATIVE_UNDERLYING_HUNDRED);
        static const Decimal64 D64_THOUSAND = Decimal64::fromUnderlying(DECIMAL_NATIVE_UNDERLYING_THOUSAND);
        static const Decimal64 D64_MILLION = Decimal64::fromUnderlying(DECIMAL_NATIVE_UNDERLYING_MILLION);

        static const Decimal64 D64_ONETENTH = Decimal64::fromUnderlying(DECIMAL_NATIVE_UNDERLYING_ONETENTH);
        static const Decimal64 D64_ONEHUNDREDTH = Decimal64::fromUnderlying(DECIMAL_NATIVE_UNDERLYING_ONEHUNDREDTH);
    }
}

#endif
