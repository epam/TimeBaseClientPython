#include "python_common.h"

namespace DxApiImpl {
namespace Python {

bool getStringValue(PyObject *field_value, std::string &ret_value, bool &type_mismatch) {
    type_mismatch = false;
    if (field_value == Py_None || field_value == NULL)
        return false;

#if PY_MAJOR_VERSION >= 3      
    if (!PyUnicode_Check(field_value)) {
#else  
    if (!PyString_Check(field_value)) {
#endif
        type_mismatch = true;
        return false;
    }

#if PY_MAJOR_VERSION >= 3
    PyObject* str_object = PyUnicode_AsUTF8String(field_value);  // TODO: check UTF8?
    ret_value = PyBytes_AsString(str_object);
    Py_XDECREF(str_object);
#else
    ret_value = PyString_AsString(field_value);  // TODO: check UTF8?
#endif

    return true;
}

bool getInt32Value(PyObject *field_value, int32_t &ret_value, bool &type_mismatch) {
    type_mismatch = false;
    if (field_value == Py_None || field_value == NULL)
        return false;

    ret_value = PyLong_AsLong(field_value);
    if (ret_value == -1 && PyErr_Occurred()) {
        type_mismatch = true;
        return false;
    }

    return true;
}

bool getInt64Value(PyObject *field_value, int64_t &ret_value, bool &type_mismatch) {
    type_mismatch = false;
    if (field_value == Py_None || field_value == NULL)
        return false;

    ret_value = PyLong_AsLongLong(field_value);
    if (ret_value == -1 && PyErr_Occurred()) {
        type_mismatch = true;
        return false;
    }

    return true;
}

bool getDoubleValue(PyObject *field_value, double &ret_value, bool &type_mismatch) {
    type_mismatch = false;
    if (field_value == Py_None || field_value == NULL)
        return false;

    ret_value = PyFloat_AsDouble(field_value);
    if (ret_value == -1.0 && PyErr_Occurred()) {
        type_mismatch = true;
        return false;
    }

    return true;
}


bool getBooleanValue(PyObject *field_value, bool &ret_value) {
    if (field_value == Py_None || field_value == NULL)
        return false;

    if (!PyBool_Check(field_value))
        return false;

    ret_value = PyObject_Not(field_value) ? false : true;

    return true;
}

bool getStringValue(PyObject *message, const std::string &field_name, PyObject *field_key, std::string &ret_value) {
    if (message == NULL)
        return false;

    if (!PyObject_HasAttr(message, field_key))
        return false;

    PythonRefHolder object(PyObject_GetAttr(message, field_key));
    if (object.getReference() == Py_None)
        return false;

    bool type_mismatch = false;
    bool not_null = getStringValue(object.getReference(), ret_value, type_mismatch);
    if (type_mismatch)
        THROW_EXCEPTION("Wrong type of field '%s'. Required: STRING.", field_name.c_str());

    return not_null;
}

bool getInt32Value(PyObject *message, const std::string &field_name, PyObject *field_key, int32_t &ret_value) {
    if (message == NULL)
        return false;

    if (!PyObject_HasAttr(message, field_key))
        return false;

    PythonRefHolder object(PyObject_GetAttr(message, field_key));
    if (object.getReference() == Py_None)
        return false;

    bool type_mismatch = false;
    bool not_null = getInt32Value(object.getReference(), ret_value, type_mismatch);
    if (type_mismatch)
        THROW_EXCEPTION("Wrong type of field '%s'. Required: INTEGER.", field_name.c_str());

    return not_null;
}

bool getInt64Value(PyObject *message, const std::string &field_name, PyObject *field_key, int64_t &ret_value) {
    if (message == NULL)
        return false;

    if (!PyObject_HasAttr(message, field_key))
        return false;

    PythonRefHolder object(PyObject_GetAttr(message, field_key));
    if (object.getReference() == Py_None)
        return false;

    bool type_mismatch = false;
    bool not_null = getInt64Value(object.getReference(), ret_value, type_mismatch);
    if (type_mismatch)
        THROW_EXCEPTION("Wrong type of field '%s'. Required: INTEGER.", field_name.c_str());

    return not_null;
}

bool getDoubleValue(PyObject *message, const std::string &field_name, PyObject *field_key, double &ret_value) {
    if (message == NULL)
        return false;

    if (!PyObject_HasAttr(message, field_key))
        return false;

    PythonRefHolder object(PyObject_GetAttr(message, field_key));
    if (object.getReference() == Py_None)
        return false;

    bool type_mismatch = false;
    bool not_null = getDoubleValue(object.getReference(), ret_value, type_mismatch);
    if (type_mismatch)
        THROW_EXCEPTION("Wrong type of field '%s'. Required: FLOAT.", field_name.c_str());

    return not_null;
}

bool getBooleanValue(PyObject *message, const std::string &field_name, PyObject *field_key, bool &ret_value) {
    if (message == NULL)
        return false;

    if (!PyObject_HasAttr(message, field_key))
        return false;

    PythonRefHolder object(PyObject_GetAttr(message, field_key));
    if (object.getReference() == Py_None)
        return false;

    return getBooleanValue(object.getReference(), ret_value);
}

}
}