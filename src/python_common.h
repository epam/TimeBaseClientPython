#ifndef DELTIX_API_PYTHON_COMMON_H_
#define DELTIX_API_PYTHON_COMMON_H_

#include "Python.h"

#include "common.h"

#include <iostream>

namespace DxApiImpl {
namespace Python {

const std::string MODULE_NAME = "dxapi";
const std::string MESSAGE_OBJECT_CLASS_NAME = "InstrumentMessage";

const std::string TYPE_ID_PROPERTY = "typeId";
const std::string TYPE_NAME_PROPERTY = "typeName";
const std::string INSTRUMENT_ID_PROPERTY = "instrumentId";
const std::string INSTRUMENT_TYPE_PROPERTY = "instrumentType";
const std::string SYMBOL_PROPERTY = "symbol";
const std::string TIMESTAMP_PROPERTY = "timestamp";


//functions return false, if field_value is NULL, Py_None or type_mismatch is true
//type_mismatch is true, when type of field_value not corresponds to return value of function
bool getStringValue(PyObject *field_value, std::string &ret_value, bool &type_mismatch);
bool getInt32Value(PyObject *field_value, int32_t &ret_value, bool &type_mismatch);
bool getInt64Value(PyObject *field_value, int64_t &ret_value, bool &type_mismatch);
bool getDoubleValue(PyObject *field_value, double &ret_value, bool &type_mismatch);
bool getBooleanValue(PyObject *field_value, bool &ret_value);

bool getStringValue(PyObject *message, const std::string &field_name, PyObject *field_key, std::string &ret_value);
bool getInt32Value(PyObject *message, const std::string &field_name, PyObject *field_key, int32_t &ret_value);
bool getInt64Value(PyObject *message, const std::string &field_name, PyObject *field_key, int64_t &ret_value);
//bool getDoubleValue(PyObject *message, const std::string &field_name, PyObject *field_key, double &ret_value);
//bool getBooleanValue(PyObject *message, const std::string &field_name, PyObject *field_key, bool &ret_value);

//todo: should be singletone
class PythonDxApiModule {
public:
    PythonDxApiModule() {
        dxapi_module_ = PyImport_ImportModule(MODULE_NAME.c_str());
        if (dxapi_module_ == NULL)
            THROW_EXCEPTION("Module '%32s' is not loaded.", MODULE_NAME.c_str());

        module_dict_ = PyModule_GetDict(dxapi_module_);

        instrument_message_class_ = PyDict_GetItemString(module_dict_, MESSAGE_OBJECT_CLASS_NAME.c_str());
        if (instrument_message_class_ == NULL)
            THROW_EXCEPTION("Class '%32s' not found in module '%32s'.", MESSAGE_OBJECT_CLASS_NAME.c_str(), MODULE_NAME.c_str());
    }

    ~PythonDxApiModule() {
        Py_XDECREF(dxapi_module_);
    }

    PyObject * newInstrumentMessageObject() {
        if (instrument_message_class_ == NULL)
            THROW_EXCEPTION("Class '%32s' not found in module '%32s'.", MESSAGE_OBJECT_CLASS_NAME.c_str(), MODULE_NAME.c_str());

        PyObject *message_object = PyObject_CallObject(instrument_message_class_, NULL);
        return message_object;
    }

private:
    PyObject *dxapi_module_ = NULL;
    PyObject *module_dict_ = NULL;
    PyObject *instrument_message_class_ = NULL;
};

//RAII for new reference of PyObject *
class PythonRefHolder {
public:
    PythonRefHolder(PyObject *object) {
        object_ = object;
    }

    ~PythonRefHolder() {
        Py_XDECREF(object_);
    }

    PyObject * getReference() {
        return object_;
    }

private:
    PyObject *object_;
};

class PythonGILLockHolder {
public:
    PythonGILLockHolder() {
        state_ = PyGILState_Ensure();
    }

    ~PythonGILLockHolder() {
        PyGILState_Release(state_);
    }
    
private:
    PyGILState_STATE state_;
};

}
}

#endif //DELTIX_API_PYTHON_COMMON_H_
