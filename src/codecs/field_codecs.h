#ifndef DELTIX_API_DECODERS_FIELD_DECODERS_H_
#define DELTIX_API_DECODERS_FIELD_DECODERS_H_

#include "Python.h"

#include "python_common.h"
#include "schema.h"

#include "message_codec.h"

#include <algorithm>
#include <memory>


#if defined(__APPLE__)
extern "C" double toFloat64(uint64_t value);
extern "C" uint64_t fromFloat64(double value);

double dfp_toDouble(uint64_t value) {
    return toFloat64(value);
}

uint64_t dfp_fromDouble(double value) {
    return fromFloat64(value);
}
#else
extern "C" double decimal_native_toFloat64(uint64_t value);
extern "C" uint64_t decimal_native_fromFloat64(double value);

double dfp_toDouble(uint64_t value) {
    return decimal_native_toFloat64(value);
}

uint64_t dfp_fromDouble(double value) {
    return decimal_native_fromFloat64(value);
}
#endif


namespace TbApiImpl {
namespace Python {

class FieldCodec {
protected:
    FieldCodec(const char *field_name) : field_name_(field_name) {
        key_ = PyUnicode_FromString(field_name);
    };

    virtual ~FieldCodec() {
        Py_XDECREF(key_);
        Py_XDECREF(TYPE_NAME_PROPERTY1);
    }

public:
    virtual PyObject * decode(DxApi::DataReader &reader) = 0;
    virtual void encode(PyObject *field_value, DxApi::DataWriter &writer) = 0;

    const char * getFieldName() {
        return field_name_.c_str();
    }

    PyObject * getKey() {
        return key_;
    }

protected:
    PyObject * key_;
    std::string field_name_;
    PyObject *  TYPE_NAME_PROPERTY1 = PyUnicode_FromString("typeName");

private:
    DISALLOW_COPY_AND_ASSIGN(FieldCodec);
};

template <typename T>
class FieldValueCodec : public FieldCodec {
protected:
    FieldValueCodec(const char *field_name, FieldValueCodec<T> *relative_to)
        : FieldCodec(field_name), relative_to_(relative_to)
    { };

public:
    virtual void appendRelative(T &value) {
        if (relative_to_ != NULL && !relative_to_->isNull())
            value = relative_to_->getValue() + value;
    }

    virtual void substractRelative(T &value) {
        if (relative_to_ != NULL && !relative_to_->isNull())
            value = value - relative_to_->getValue();
    }

    virtual T & getValue() {
        return value_;
    }

    virtual bool isNull() {
        return is_null_;
    }

protected:
    T value_;
    bool is_null_;

    FieldValueCodec<T> *relative_to_;
};

class AlphanumericFieldCodec : public FieldCodec {
public:
    AlphanumericFieldCodec(const char* field_name, int field_size) : FieldCodec(field_name), field_size_(field_size) { };

    inline PyObject * decode(DxApi::DataReader &reader) {
        bool hasField = reader.readAlphanumeric(buffer_, field_size_);
        if (!hasField)
            Py_RETURN_NONE;
        else
            return PyUnicode_FromString(buffer_.c_str());
    }

    inline void encode(PyObject *field_value, DxApi::DataWriter &writer) {
        bool type_mismatch = false;
        bool exists = getStringValue(field_value, buffer_, type_mismatch);
        if (type_mismatch)
            THROW_EXCEPTION("Wrong type of field '%s'. Required: STRING.", field_name_.c_str());
        if (exists) {
            writer.writeAlphanumeric(field_size_, buffer_);
        } else {
            writer.writeAlphanumericNull(field_size_);
        }
    }
    
protected:
    int field_size_;
    std::string buffer_;
};

class TimestampFieldCodec : public FieldCodec {
public:
    TimestampFieldCodec(const char* field_name) : FieldCodec(field_name) { };

    inline PyObject * decode(DxApi::DataReader &reader) {
        int64_t value = reader.readTimestamp();
        if (value != DxApi::TIMESTAMP_NULL)
            return PyLong_FromLongLong(value);
        else
            Py_RETURN_NONE;
    }

    inline void encode(PyObject *field_value, DxApi::DataWriter &writer) {
        int64_t value;
        bool type_mismatch;
        bool exists = getInt64Value(field_value, value, type_mismatch);
        if (type_mismatch)
            THROW_EXCEPTION("Wrong type of field '%s'. Required: INTEGER.", field_name_.c_str());

        if (exists) {
            writer.writeTimestamp(value);
        } else {
            writer.writeTimestamp(DxApi::TIMESTAMP_NULL);
        }
    }
};

class IntegerFieldCodec : public FieldValueCodec<int64_t> {
public:
    IntegerFieldCodec(const char* field_name, FieldValueCodec<int64_t> *relative_to, int field_size) 
        : FieldValueCodec(field_name, relative_to), field_size_(field_size) { };

    inline PyObject * decode(DxApi::DataReader &reader) {
        switch (field_size_) {
        case 8: {
            int8_t result = reader.readInt8();
            if (result != DxApi::Constants::INT8_NULL) {
                value_ = result;
                is_null_ = false;
                appendRelative(value_);
                return PyLong_FromLongLong(value_);
            } else {
                Py_RETURN_NONE;
            }
        }
        break;
        case 16: {
            int16_t result = reader.readInt16();
            if (result != DxApi::Constants::INT16_NULL) {
                value_ = result;
                is_null_ = false;
                appendRelative(value_);
                return PyLong_FromLongLong(value_);
            } else {
                Py_RETURN_NONE;
            }
        }
        break;
        case 30: {
            uint32_t result = reader.readPUInt30();
            if (result != DxApi::UINT61_NULL) {
                value_ = result;
                is_null_ = false;
                appendRelative(value_);
                return PyLong_FromLongLong(value_);
            } else {
                Py_RETURN_NONE;
            }
        }
        break;
        case 32: {
            int32_t result = reader.readInt32();
            if (result != DxApi::Constants::INT32_NULL) {
                value_ = result;
                is_null_ = false;
                appendRelative(value_);
                return PyLong_FromLongLong(value_);
            } else {
                Py_RETURN_NONE;
            }
        }
        break;
        case 48: {
            int64_t result = reader.readInt48();
            if (result != DxApi::INT48_NULL) {
                value_ = result;
                is_null_ = false;
                appendRelative(value_);
                return PyLong_FromLongLong(value_);
            } else {
                Py_RETURN_NONE;
            }
        }
        break;
        case 61: {
            uint64_t result = reader.readPUInt61();
            if (result != DxApi::UINT61_NULL) {
                value_ = result;
                is_null_ = false;
                appendRelative(value_);
                return PyLong_FromLongLong(value_);
            } else {
                Py_RETURN_NONE;
            }
        }
        break;
        case 64: {
            int64_t result = reader.readInt64();
            if (result != DxApi::INT64_NULL) {
                value_ = result;
                is_null_ = false;
                appendRelative(value_);
                return PyLong_FromLongLong(value_);
            } else {
                Py_RETURN_NONE;
            }
        }
        break;
        default:
            THROW_EXCEPTION("Unknow size of integer: %d.", field_size_);
        }
    }

    inline void encode(PyObject *field_value, DxApi::DataWriter &writer) {
        bool type_mismatch;
        bool exists = getInt64Value(field_value, value_, type_mismatch);
        if (type_mismatch)
            THROW_EXCEPTION("Wrong type of field '%s'. Required: INTEGER.", field_name_.c_str());
        if (!exists) {
            is_null_ = true;
            switch (field_size_) {
            case 8: {
                writer.writeInt8(DxApi::Constants::INT8_NULL);
            }
            break;
            case 16: {
                writer.writeInt16(DxApi::Constants::INT16_NULL);
            }
            break;
            case 30: {
                writer.writePUInt30(DxApi::UINT30_NULL);
            }
            break;
            case 32: {
                writer.writeInt32(DxApi::Constants::INT32_NULL);
            }
            break;
            case 48: {
                writer.writeInt48(DxApi::INT48_NULL);
            }
            break;
            case 61: {
                writer.writePUInt61(DxApi::UINT61_NULL);
            }
            break;
            case 64: {
                writer.writeInt64(DxApi::INT64_NULL);
            }
            break;
            default:
                THROW_EXCEPTION("Unknow size of integer: %d.", field_size_);
            }
        } else {
            substractRelative(value_);
            is_null_ = false;
            switch (field_size_)
            {
            case 8: {
                writer.writeInt8((int8_t)value_);
            }
            break;
            case 16: {
                writer.writeInt16((int16_t)value_);
            }
            break;
            case 30: {
                writer.writePUInt30((uint32_t)value_);
            }
            break;
            case 32: {
                writer.writeInt32((int32_t)value_);
            }
            break;
            case 48: {
                writer.writeInt48(value_);
            }
            break;
            case 61: {
                writer.writePUInt61((uint64_t)value_);
            }
            break;
            case 64: {
                writer.writeInt64(value_);
            }
            break;
            default:
                THROW_EXCEPTION("Unknow size of integer: %d.", field_size_);
            }
        }
       
    }

private:
    int field_size_;
};

class Decimal64FieldCodec : public FieldCodec {
public:
    Decimal64FieldCodec(const char* field_name) : FieldCodec(field_name) { };

    inline PyObject * decode(DxApi::DataReader &reader) {
        int64_t result = reader.readInt64();
        if (result != DxApi::INT64_NULL) {
            return PyFloat_FromDouble(decodeDecimal64(result));
        } else {
            Py_RETURN_NONE;
        }
    }
    
    inline void encode(PyObject *field_value, DxApi::DataWriter &writer) {
        double value;
        bool type_mismatch;
        bool exists = getDoubleValue(field_value, value, type_mismatch);
        if (type_mismatch)
            THROW_EXCEPTION("Wrong type of field '%s'. Required: DOUBLE.", field_name_.c_str());

        if (exists) {
            writer.writeInt64(encodeDecimal64(value));
        } else {
            writer.writeInt64(DxApi::INT64_NULL);
        }
    }

    inline double decodeDecimal64(int64_t value) {
        return dfp_toDouble(value);
    }

    inline int64_t encodeDecimal64(double value) {
        return dfp_fromDouble(value);
    }
};

class FloatFieldCodec : public FieldValueCodec<double> {
public:
    FloatFieldCodec(const char* field_name, FieldValueCodec *relative_to, int field_size) 
        : FieldValueCodec(field_name, relative_to), field_size_(field_size) {
    };
    
    inline PyObject * decode(DxApi::DataReader &reader) {
        switch (field_size_) {
        case 32: {
            float result = reader.readFloat32();
            if (result != result) {
                Py_RETURN_NONE;
            } else {
                value_ = result;
                is_null_ = false;
                appendRelative(value_);
                return PyFloat_FromDouble(value_);
            }
        }
        break;
        case 63: {
            double result = reader.readDecimal();
            value_ = result;
            if (result != result) {
                Py_RETURN_NONE;
            } else {
                value_ = result;
                is_null_ = false;
                appendRelative(value_);
                return PyFloat_FromDouble(value_);
            }
        }
        break;
        case 64: {
            double result = reader.readFloat64();
            value_ = result;
            if (result != result) {
                Py_RETURN_NONE;
            } else {
                value_ = result;
                is_null_ = false;
                appendRelative(value_);
                return PyFloat_FromDouble(value_);
            }
        }
        break;
        default:
            THROW_EXCEPTION("Unknow size of float: %d.", field_size_);
        }
    }

    inline void encode(PyObject *field_value, DxApi::DataWriter &writer) {
        bool type_mismatch;
        bool exists = getDoubleValue(field_value, value_, type_mismatch);
        if (type_mismatch)
            THROW_EXCEPTION("Wrong type of field '%s'. Required: FLOAT.", field_name_.c_str());
        if (!exists) {
            is_null_ = true;
            switch (field_size_) {
            case 32: {
                writer.writeFloat32(DxApi::FLOAT32_NULL);
            }
            break;
            case 63: {
                writer.writeDecimal(DxApi::DECIMAL_NULL);
            }
            break;
            case 64: {
                writer.writeFloat64(DxApi::FLOAT64_NULL);
            }
            break;
            default:
                THROW_EXCEPTION("Unknow size of float: %d.", field_size_);
            }
        } else {
            substractRelative(value_);
            is_null_ = false;
            switch (field_size_)
            {
            case 32: {
                writer.writeFloat32((float)value_);
            }
            break;
            case 63: {
                writer.writeDecimal(value_);
            }
            break;
            case 64: {
                writer.writeFloat64(value_);
            }
            break;
            default:
                THROW_EXCEPTION("Unknow size of float: %d.", field_size_);
            }
        }
    }

protected:
    int field_size_;
};

class IntervalFieldCodec : public FieldCodec {
public:
    IntervalFieldCodec(const char* field_name) : FieldCodec(field_name) {};

    inline PyObject * decode(DxApi::DataReader &reader) {
        int32_t result = reader.readInterval();
        if (result != DxApi::Constants::INTERVAL_NULL)
            return PyLong_FromLong(result);
        else
            Py_RETURN_NONE;
    }

    inline void encode(PyObject *field_value, DxApi::DataWriter &writer) {
        int32_t value;
        bool type_mismatch;
        bool exists = getInt32Value(field_value, value, type_mismatch);
        if (type_mismatch)
            THROW_EXCEPTION("Wrong type of field '%s'. Required: INTEGER.", field_name_.c_str());
        if (exists) {
            writer.writeInterval(value);
        } else {
            writer.writeInterval(DxApi::Constants::INTERVAL_NULL);
        }
    }
};

class TimeOfDayFieldCodec : public FieldCodec {
public:
    TimeOfDayFieldCodec(const char* field_name) : FieldCodec(field_name) {};

    inline PyObject * decode(DxApi::DataReader &reader) {
        int32_t result = reader.readTimeOfDay();
        if (result != DxApi::Constants::TIMEOFDAY_NULL) {
            return PyLong_FromLong(result);
        } else {
            Py_RETURN_NONE;
        }
    }

    inline void encode(PyObject *field_value, DxApi::DataWriter &writer) {
        int32_t value;
        bool type_mismatch;
        bool exists = getInt32Value(field_value, value, type_mismatch);
        if (type_mismatch)
            THROW_EXCEPTION("Wrong type of field '%s'. Required: INTEGER.", field_name_.c_str());
        if (exists) {
            writer.writeInterval(value);
        } else {
            writer.writeInterval(DxApi::Constants::TIMEOFDAY_NULL);
        }
    }
};

class BinaryFieldCodec : public FieldCodec {
public:
    BinaryFieldCodec(const char* field_name) : FieldCodec(field_name) {};

    inline PyObject * decode(DxApi::DataReader &reader) {
        bool not_null = reader.readBinary(buffer_);
        if (not_null) {
            return PyBytes_FromStringAndSize(reinterpret_cast<char*>(buffer_.data()), buffer_.size());
        }
        else {
            Py_RETURN_NONE;
        }
    }

    inline void encode(PyObject *field_value, DxApi::DataWriter &writer) {
        if (field_value == Py_None || field_value == NULL) {
            writer.writeBinaryArrayNull();
            return;
        }
        if (PyBytes_Check(field_value)) {
            uint8_t *bytes = (uint8_t *) PyBytes_AsString(field_value);
            size_t size = PyBytes_Size(field_value);
            if (bytes == NULL)
                writer.writeBinaryArrayNull();
            else
                writer.writeBinaryArray(bytes, size);
        }
        else if (PyByteArray_Check(field_value)) {
            uint8_t *bytes = (uint8_t *) PyByteArray_AsString(field_value);
            if (bytes == NULL)
                writer.writeBinaryArrayNull();
            else
                writer.writeBinaryArray(bytes, PyByteArray_Size(field_value));
        }
        else {
            THROW_EXCEPTION("Wrong type of '%s' field. Required: BYTES.", field_name_.c_str());
        }
    }

private:
    std::vector<uint8_t> buffer_;
};

class BooleanFieldCodec : public FieldCodec {
public:
    BooleanFieldCodec(const char* field_name, bool nullable) : FieldCodec(field_name), nullable(nullable) {};

    inline PyObject * decode(DxApi::DataReader &reader) {
        if (nullable) {
            uint8_t result = reader.readNullableBooleanInt8();
            if (result == DxApi::Constants::BOOL_TRUE) {
                Py_RETURN_TRUE;
            } else if (result == DxApi::Constants::BOOL_FALSE) {
                Py_RETURN_FALSE;
            } else {
                Py_RETURN_NONE;
            }
        } else {
            if (reader.readBoolean())
                Py_RETURN_TRUE;
            else
                Py_RETURN_FALSE;
        }
    }

    inline void encode(PyObject *field_value, DxApi::DataWriter &writer) {
        bool ret_value;
        bool exists = getBooleanValue(field_value, ret_value);
        if (exists) {
            writer.writeBoolean(ret_value);
        } else {
            writer.writeNullableBoolean(false, true);
        }
    }

protected:
    bool nullable;
};

class CharFieldCodec : public FieldCodec {
public:
    CharFieldCodec(const char* field_name) : FieldCodec(field_name) { };

    inline PyObject * decode(DxApi::DataReader &reader) {
        const wchar_t ch = reader.readWChar();
        if (ch != DxApi::Constants::CHAR_NULL) {
            return PyUnicode_FromOrdinal(ch); /*PyUnicode_FromWideChar(&ch, 1)*/
        } else {
            Py_RETURN_NONE;
        }
    }

    inline void encode(PyObject *field_value, DxApi::DataWriter &writer) {
        if (field_value == NULL || Py_None == field_value) {
            writer.writeWChar(DxApi::Constants::CHAR_NULL);
        } else {
            if (PyUnicode_Check(field_value)) {
                std::wstring str;
                str.resize(PyUnicode_GetSize(field_value) + 1, 0);
                int len = (int)PyUnicode_AsWideChar(
#if PY_VERSION_HEX < 0x03020000
                    (PyUnicodeObject *)
#endif
                    field_value, &str[0], (Py_ssize_t)str.size());
                if (len > -1)
                {
                    assert(len < (int)str.size());
                    str[len] = 0;
                }
                else str[str.size() - 1] = 0;

                writer.writeWChar(str[0]);
            } else {
                writer.writeWChar(DxApi::Constants::CHAR_NULL);
            }
        }
    }
};

class Utf8FieldCodec : public FieldCodec {
public:
    Utf8FieldCodec(const char* field_name) : FieldCodec(field_name) { };

    inline PyObject * decode(DxApi::DataReader &reader) {
        bool hasField = reader.readUTF8(buffer);
        if (hasField) {
            return PyUnicode_DecodeUTF8(buffer.c_str(), buffer.size(), "ignore");
        } else {
            Py_RETURN_NONE;
        }
    }

    inline void encode(PyObject *field_value, DxApi::DataWriter &writer) {
        if (Py_None == field_value || field_value == NULL) {
            writer.writeUTF8((const char *) NULL, 0);
        } else {
            if (PyUnicode_Check(field_value)) {
                std::wstring str;
                str.resize(PyUnicode_GetSize(field_value) + 1, 0);
                int len = (int)PyUnicode_AsWideChar(
#if PY_VERSION_HEX < 0x03020000
                    (PyUnicodeObject *)
#endif
                    field_value, &str[0], (Py_ssize_t)str.size());
                if (len > -1)
                {
                    assert(len < (int)str.size());
                    str[len] = 0;
                } else 
                    str[str.size() - 1] = 0;

                writer.writeUTF8(str);
            } else {
                writer.writeUTF8((const char *)NULL, 0);
            }
        }
    }

private:
    std::string buffer;
};

class AsciiFieldCodec : public FieldCodec {
public:
    AsciiFieldCodec(const char* field_name) : FieldCodec(field_name) { };

    inline PyObject * decode(DxApi::DataReader &reader) {
        bool hasField = reader.readAscii(buffer);
        if (hasField) {
            return PyUnicode_FromString(buffer.c_str());
        }
        else {
            Py_RETURN_NONE;
        }
    }

    inline void encode(PyObject *field_value, DxApi::DataWriter &writer) {
        if (Py_None == field_value || field_value == NULL) {
            writer.writeAscii((const char *)NULL, 0);
        }
        else {
            if (PyUnicode_Check(field_value)) {
                std::wstring str;
                str.resize(PyUnicode_GetSize(field_value) + 1, 0);
                int len = (int)PyUnicode_AsWideChar(
#if PY_VERSION_HEX < 0x03020000
                    (PyUnicodeObject *)
#endif
                    field_value, &str[0], (Py_ssize_t)str.size());
                if (len > -1)
                {
                    assert(len < (int)str.size());
                    str[len] = 0;
                }
                else
                    str[str.size() - 1] = 0;

                writer.writeAscii(str);
            }
            else {
                writer.writeAscii((const char *)NULL, 0);
            }
        }
    }

private:
    std::string buffer;
};

class EnumFieldCodec : public FieldCodec {
public:
    EnumFieldCodec(const char* field_name, const Schema::TickDbClassDescriptor &descriptor) 
        : FieldCodec(field_name), descriptor_(descriptor) {
    };

    inline PyObject * decode(DxApi::DataReader &reader) {
        switch (descriptor_.enumType) {
        case Schema::FieldTypeEnum::ENUM8: {
            int8_t result = reader.readEnum8();
            if (result != DxApi::Constants::ENUM_NULL) {
                if (result < 0 || result >= descriptor_.enumSymbols.size())
                    THROW_EXCEPTION("Enum value out of bound for '%s' field.", field_name_.c_str());

                return PyUnicode_FromString(descriptor_.enumSymbols[result].c_str());
            } else {
                Py_RETURN_NONE;
            }
        }
        break;
        case Schema::FieldTypeEnum::ENUM16: {
            int16_t result = reader.readEnum16();

            if (result != DxApi::ENUM_NULL) {
                if (result < 0 || result >= descriptor_.enumSymbols.size())
                    THROW_EXCEPTION("Enum value out of bound for '%s' field.", field_name_.c_str());

                return PyUnicode_FromString(descriptor_.enumSymbols[result].c_str());
            } else {
                Py_RETURN_NONE;
            }
        }
        break;
        case Schema::FieldTypeEnum::ENUM32: {
            int32_t result = reader.readEnum32();

            if (result != DxApi::Constants::ENUM_NULL) {
                if (result < 0 || result >= descriptor_.enumSymbols.size())
                    THROW_EXCEPTION("Enum value out of bound for '%s' field.", field_name_.c_str());

                return PyUnicode_FromString(descriptor_.enumSymbols[result].c_str());
            } else {
                Py_RETURN_NONE;
            }
        }
        case Schema::FieldTypeEnum::ENUM64: {
            int64_t result = reader.readEnum64();

            if (result != DxApi::Constants::ENUM_NULL) {
                if (result < 0 || result >= (int64_t) descriptor_.enumSymbols.size())
                    THROW_EXCEPTION("Enum value out of bound for '%s' field.", field_name_.c_str());

                return PyUnicode_FromString(descriptor_.enumSymbols[result].c_str());
            } else {
                Py_RETURN_NONE;
            }
        }
        default:
            THROW_EXCEPTION("Unknow type of enum for '%s' field.", field_name_.c_str());
        }
    }

    inline void encode(PyObject *field_value, DxApi::DataWriter &writer) {
        bool type_mismatch;
        bool exists = getStringValue(field_value, buffer_, type_mismatch);
        if (type_mismatch)
            THROW_EXCEPTION("Wrong type of field '%s'. Required: STRING.", field_name_.c_str());

        if (exists) {
            auto it = descriptor_.symbolToEnumValue.find(buffer_);
            if (it == descriptor_.symbolToEnumValue.end())
                THROW_EXCEPTION("Unknown enum value '%s' of field '%s'", buffer_.c_str(), field_name_.c_str());

            switch (descriptor_.enumType) {
            case Schema::FieldTypeEnum::ENUM8:
                writer.writeEnum8(descriptor_.symbolToEnumValue[buffer_]);
                break;
            case Schema::FieldTypeEnum::ENUM16:
                writer.writeEnum16(descriptor_.symbolToEnumValue[buffer_]);
                break;
            case Schema::FieldTypeEnum::ENUM32:
                writer.writeEnum32(descriptor_.symbolToEnumValue[buffer_]);
                break;
            case Schema::FieldTypeEnum::ENUM64:
                writer.writeEnum64(descriptor_.symbolToEnumValue[buffer_]);
                break;
            default:
                THROW_EXCEPTION("Unknow type of enum for '%s' field.", field_name_.c_str());
            }
        } else {
            switch (descriptor_.enumType) {
            case Schema::FieldTypeEnum::ENUM8:
                writer.writeEnum8(DxApi::Constants::ENUM_NULL);
                break;
            case Schema::FieldTypeEnum::ENUM16:
                writer.writeEnum16(DxApi::Constants::ENUM_NULL);
                break;
            case Schema::FieldTypeEnum::ENUM32:
                writer.writeEnum32(DxApi::Constants::ENUM_NULL);
                break;
            case Schema::FieldTypeEnum::ENUM64:
                writer.writeEnum64(DxApi::Constants::ENUM_NULL);
                break;
            default:
                THROW_EXCEPTION("Unknow type of enum for '%s' field.", field_name_.c_str());
            }
        }
    }

protected:
    std::string buffer_;
    Schema::TickDbClassDescriptor descriptor_;
};

class ArrayFieldCodec : public FieldCodec {
public:
    ArrayFieldCodec(const char* field_name, FieldCodecPtr element_codec)
        : FieldCodec(field_name), element_codec_(element_codec) {
    };

    inline PyObject * decode(DxApi::DataReader &reader) {
        int32_t len = reader.readArrayStart();
        if (len == DxApi::Constants::INT32_NULL)
            Py_RETURN_NONE;

        PyObject *list = PyList_New(len);
        for (int i = 0; i < len; ++i) {
            PyObject *element = element_codec_->decode(reader);
            PyList_SetItem(list, i, element);
        }
        reader.readArrayEnd();

        return list;
    }

    inline void encode(PyObject *field_value, DxApi::DataWriter &writer) {
        if (field_value == NULL) {
            writer.writeArrayNull();
            return;
        }

        if (PyList_Check(field_value)) {
            Py_ssize_t size = PyList_Size(field_value);

            writer.writeArrayStart(size);
            for (int i = 0; i < size; i++) {
                element_codec_->encode(PyList_GetItem(field_value, i), writer);
            }
            writer.writeArrayEnd();
        } else {
            if (field_value == Py_None) {
                writer.writeArrayNull();
            } else {
                THROW_EXCEPTION("Wrong type of field '%s'. Required: ARRAY.", field_name_.c_str());
            }
        }
    }

private:
    FieldCodecPtr element_codec_;
};

class ObjectFieldCodec : public FieldCodec {
public:
    ObjectFieldCodec(
        const char* field_name, PythonTbApiModule *tbapi_module,
        std::vector<std::string> types, std::vector<MessageCodecPtr> codecs)
        : FieldCodec(field_name), tbapi_module_(tbapi_module), types_(types), codecs_(codecs)
    {
        for (int i = 0; i < types.size(); ++i) {
            type_name_to_id_[types[i]] = i;
        }
    };

    inline PyObject * decode(DxApi::DataReader &reader) {
        int32_t type_id = reader.readObjectStart();
        if (type_id == DxApi::Constants::INT32_NULL)
            Py_RETURN_NONE;

        if (type_id < 0 || type_id >= codecs_.size())
            THROW_EXCEPTION("Can't find codec of type id '%d' for field: %s.", type_id, field_name_.c_str());

        if (tbapi_module_ == NULL)
            THROW("DxApi module is not initialized for object codec.");

        PyObject *object = tbapi_module_->newInstrumentMessageObject();
        codecs_[type_id]->decode(object, reader);
        reader.readObjectEnd();

        PythonRefHolder type_name_obj(PyUnicode_FromString(types_[type_id].c_str()));
        PyObject_SetAttrString(object, TYPE_NAME_PROPERTY.c_str(), type_name_obj.getReference());

        return object;
    }

    inline void encode(PyObject *message, DxApi::DataWriter &writer) {
        if (message == NULL || message == Py_None) {
            writer.writeObjectNull();
            return;
        }

        int32_t type_id = getTypeId(message);

        writer.writeObjectStart(type_id);
        codecs_[type_id]->encode(message, writer);
        writer.writeObjectEnd();
    }

    int32_t getTypeId(PyObject *message) {
        if (types_.size() == 1)
            return 0;

        std::string type_name;
        bool exists = getStringValue(message, TYPE_NAME_PROPERTY, TYPE_NAME_PROPERTY1, type_name);
        if (!exists)
            THROW_EXCEPTION("Unkown type of object. Specify '%s' attribute for field '%s'.", 
                TYPE_NAME_PROPERTY.c_str(), field_name_.c_str());

        auto type_id_it = type_name_to_id_.find(type_name);
        if (type_id_it == type_name_to_id_.end())
            THROW_EXCEPTION("Unknown type '%s' of field '%s'.", 
                type_name.c_str(), field_name_.c_str());

        int32_t type_id = type_id_it->second;
        if (type_id < 0 || type_id >= codecs_.size())
            THROW_EXCEPTION("Can't find codec for type '%s' of field '%s'", 
                type_name.c_str(), field_name_.c_str());

        return type_id;
    }

private:
    PythonTbApiModule *tbapi_module_;
    std::vector<std::string> types_;
    std::vector<MessageCodecPtr> codecs_;
    std::unordered_map<std::string, int32_t> type_name_to_id_;
};

}
}

#endif //DELTIX_API_DECODERS_FIELD_DECODERS_H_