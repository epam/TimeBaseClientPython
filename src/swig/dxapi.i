%define MODULEIMPORT
"
from sys import version_info as _swig_python_version_info
from sys import platform as _swig_python_platform

platform = 'windows'
if _swig_python_platform.startswith('linux'):
    platform = 'linux'
elif _swig_python_platform.startswith('darwin'):
    platform = 'darwin'

if _swig_python_version_info >= (2, 7) and _swig_python_version_info < (2, 8):
    subdir = 'py27'
elif _swig_python_version_info >= (3, 6) and _swig_python_version_info < (3, 7):
    subdir = 'py36'
elif _swig_python_version_info >= (3, 7) and _swig_python_version_info < (3, 8):
    subdir = 'py37'
elif _swig_python_version_info >= (3, 8) and _swig_python_version_info < (3, 9):
    subdir = 'py38'
else:
    raise Exception('Version of python (' + str(_swig_python_version_info) + ') is not supported')

if _swig_python_version_info >= (3, 7, 0):
    def swig_import_helper():
        import importlib
        mname = '.'.join((__name__, platform, 'x64', subdir, '_dxapi')).lstrip('.')
        try:
            return importlib.import_module(mname)
        except ImportError:
            return importlib.import_module('_dxapi')
    _dxapi = swig_import_helper()
    del swig_import_helper
elif _swig_python_version_info >= (2, 6, 0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            directory = '/'.join((dirname(__file__), platform, 'x64', subdir))
            fp, pathname, description = imp.find_module('_dxapi', [directory])
        except ImportError:
            import _dxapi
            return _dxapi
        try:
            _mod = imp.load_module('_dxapi', fp, pathname, description)
        finally:
            if fp is not None:
                fp.close()
        return _mod
    _dxapi = swig_import_helper()
    del swig_import_helper
else:
    import _dxapi
del _swig_python_version_info
del _swig_python_platform
"
%enddef


%module(moduleimport=MODULEIMPORT, directors="1") dxapi

%include "stdint.i"
%include "std_string.i"
%include "std_vector.i"
%include "cwstring.i"

%begin %{
//#define SWIG_PYTHON_INTERPRETER_NO_DEBUG
%}

%{
#define SWIG_FILE_WITH_INIT

#include "dxapi.h"

#include "python_common.h"
#include "tick_cursor.h"
#include "tick_loader.h"

typedef int64_t TimestampMs;
typedef int64_t TimestampNs;

%}

%feature("director") DxApi::TickLoader::ErrorListener;
//%feature("director") DxApi::TickLoader::SubscriptionListener;

%exception {
   try {
		$action
   } catch (const std::runtime_error &e) {
		PyErr_SetString(PyExc_Exception, const_cast<char*>(e.what()));
		goto fail;
   } catch (const std::runtime_error *e) {
		PyErr_SetString(PyExc_Exception, const_cast<char*>(e->what()));
		goto fail;
   } catch (const std::exception &e) {
		PyErr_SetString(PyExc_Exception, const_cast<char*>(e.what()));
		goto fail;
   } catch (const std::exception *e) {
		PyErr_SetString(PyExc_Exception, const_cast<char*>(e->what()));
		goto fail;
   } catch (...) {
		PyErr_SetString(PyExc_RuntimeError, "Unknow runtime error");
		goto fail;
   }
}

%pythoncode %{
class InstrumentMessage(object):
    def __str__(self):
        return str(vars(self))
%}

typedef int64_t TimestampMs;
typedef int64_t TimestampNs;

///--------

%typemap(in) const std::vector<std::string> * {

	/* %typemap(in) const std::vector<std::string> * */
	if ($input == NULL) {
		$1 = NULL;
	} else if (PyList_Check($input)) {
		Py_ssize_t size = PyList_Size($input);
		$1 = new std::vector<std::string>();
		for (int i = 0; i < size; i++) {
			std::string str;
			bool type_mismatch = false;
			DxApiImpl::Python::getStringValue(PyList_GetItem($input,i), str, type_mismatch);
			$1->push_back(str);
			if (type_mismatch) {
				delete $1;
				PyErr_SetString(PyExc_TypeError, "list must contain strings");
				return NULL;
			}
		}
	} else {
		if ($input == Py_None) {
			$1 = NULL;
		} else {
			PyErr_SetString(PyExc_TypeError, "not a list");
			return NULL;
		}
	}
}

%typemap(typecheck) const std::vector<std::string> * {
	$1 = PyList_Check($input) ? 1 : 0;
}

%typemap(freearg) const std::vector<std::string> * {

	/* %typemap(freearg) const std::vector<std::string> * */
    if ($1 != NULL) 
		delete $1;
}

///--------

%typemap(in) const std::vector<std::string> * const  {

	/* %typemap(in) const std::vector<std::string> * const */
	if ($input == NULL) {
		$1 = NULL;
	} else if (PyList_Check($input)) {
		Py_ssize_t size = PyList_Size($input);
		$1 = new std::vector<std::string>();
		for (int i = 0; i < size; i++) {
			std::string str;
			bool type_mismatch = false;
			DxApiImpl::Python::getStringValue(PyList_GetItem($input,i), str, type_mismatch);
			$1->push_back(str);
			if (type_mismatch) {
				delete $1;
				PyErr_SetString(PyExc_TypeError, "list must contain strings");
				return NULL;
			}
		}
	} else {
		if ($input == Py_None) {
			$1 = NULL;
		} else {
			PyErr_SetString(PyExc_TypeError, "not a list");
			return NULL;
		}
	}
}

%typemap(typecheck) const std::vector<std::string> * const {
	$1 = PyList_Check($input) ? 1 : 0;
}

%typemap(freearg) const std::vector<std::string> * const {

	/* %typemap(freearg) const std::vector<std::string> * const */
    if ($1 != NULL) 
		delete $1;
}

///--------

%typemap(directorin) const std::vector<std::string> & {

	/* %typemap(directorin) const std::vector<std::string> & */
	$input = PyList_New($1.size());
	if ($input!= NULL) {
		for (int i = 0; i < $1.size(); ++i) {
			PyList_SetItem($input,
				i,
				PyString_FromString($1.at(i).c_str())
			);
		}
	}
}

//%typemap(directorin) const std::string & {

	/* %typemap(directorin) const std::string & */
//	$input = PyUnicode_FromString($1.c_str());
//}

///--------

%typemap(in) const std::vector<DxApi::TickStream *> * {

	/* %typemap(in) const std::vector<DxApi::TickStream *> * */
	if ($input == NULL) {
		$1 = NULL;
	} else if (PyList_Check($input)) {
		Py_ssize_t size = PyList_Size($input);

		$1 = new std::vector<DxApi::TickStream *>();
		for (int i = 0; i < size; i++) {
			PyObject *my_value = PyList_GetItem($input,i);
			if (my_value != Py_None) {
				void *argp1_my = 0 ;
				int res_my = SWIG_ConvertPtr(my_value, &argp1_my, SWIGTYPE_p_DxApi__TickStream,  0  | 0);
				if (SWIG_IsOK(res_my)) { 
					DxApi::TickStream *tmp_stream = reinterpret_cast< DxApi::TickStream * >(argp1_my);
					$1->push_back(tmp_stream);
				} else {
					delete $1;
					PyErr_SetString(PyExc_TypeError, "list must contain TickStream");
					return NULL;
				}
			}
		}
	} else {
		if ($input == Py_None) {
			$1 = NULL;
		} else {
			PyErr_SetString(PyExc_TypeError,"not a list");
			return NULL;
		}
	}
}

%typemap(typecheck) const std::vector<DxApi::TickStream *> * {
	$1 = PyList_Check($input) ? 1 : 0;
}

%typemap(freearg) const std::vector<DxApi::TickStream *> * {

	/* %typemap(freearg) const std::vector<DxApi::TickStream *> * */
	if ($1 != NULL) 
		delete $1;
}

%typemap(in) const std::vector<DxApi::QueryParameter> & {

	/* %typemap(in) const std::vector<DxApi::QueryParameter> & */
	if ($input == NULL) {
		PyErr_SetString(PyExc_TypeError, "list of Query Parameters is null");
		return NULL;
	} else if (PyList_Check($input)) {
		Py_ssize_t size = PyList_Size($input);

		$1 = new std::vector<DxApi::QueryParameter>();
		for (int i = 0; i < size; i++) {
			PyObject *my_value = PyList_GetItem($input,i);
			if (my_value != Py_None) {
				void *argp1_my = 0 ;
				int res_my = SWIG_ConvertPtr(my_value, &argp1_my, SWIGTYPE_p_DxApi__QueryParameter,  0  | 0);
				if (SWIG_IsOK(res_my)) { 
					DxApi::QueryParameter *tmp_id = reinterpret_cast< DxApi::QueryParameter * >(argp1_my);
					$1->push_back(*tmp_id);
				} else {
					delete $1;
					PyErr_SetString(PyExc_TypeError, "list must contain QueryParameter");
					return NULL;
				}
			}
		}
	} else {
		PyErr_SetString(PyExc_TypeError, "not a list");
		return NULL;
	}
}

%typemap(typecheck) const std::vector<DxApi::QueryParameter> & {
	$1 = PyList_Check($input) ? 1 : 0;
}

%typemap(freearg) const std::vector<DxApi::QueryParameter> & {

	/* %typemap(freearg) const std::vector<DxApi::QueryParameter> & */
	if ($1 != NULL) 
		delete $1;
}

%typemap(out) const DxApi::Nullable<std::string> & {
	
	/* %typemap(out) const DxApi::Nullable<std::string> & */
	if ($1->has_value()) {
		$result = PyString_FromString($1->get().c_str());
	} else {
		Py_XINCREF($result);
		$result = Py_None;
	}
}

%typemap(out) DxApi::TickCursor * {

	/* %typemap(out) DxApi::TickCursor * */
	$result = SWIG_NewPointerObj(
		SWIG_as_voidptr(new DxApiImpl::Python::TickCursor($1)), 
		SWIGTYPE_p_DxApiImpl__Python__TickCursor, 
		SWIG_POINTER_OWN | 0 
	);
}

%typemap(out) DxApi::TickLoader * {

	/* %typemap(out) DxApi::TickLoader * */
	$result = SWIG_NewPointerObj(
		SWIG_as_voidptr(new DxApiImpl::Python::TickLoader($1)), 
		SWIGTYPE_p_DxApiImpl__Python__TickLoader, 
		SWIG_POINTER_OWN | 0 
	);
}

%typemap(out) std::vector<DxApi::TickStream *> {

	/* %typemap(out) std::vector<DxApi::TickStream *> */ 
	$result = PyList_New($1.size());
	if ($result != NULL) {
		for (int i = 0; i < $1.size(); ++i) {
			PyList_SetItem($result,
				i,
				SWIG_NewPointerObj(SWIG_as_voidptr($1.at(i)), SWIGTYPE_p_DxApi__TickStream, SWIG_POINTER_OWN |  0 )
			);
		}
	}
}

%typemap(out) std::vector<DxApi::TimestampMs> {

	/* %typemap(out) std::vector<DxApi::TimestampMs> */
	$result = PyList_New($1.size());
	if ($result != NULL) {
		for (int i = 0; i < $1.size(); ++i) {
			PyList_SetItem($result,
				i,
				PyLong_FromLongLong($1.at(i))
			);
		}
	}
}

%typemap(out) std::vector<std::string> {

	/* %typemap(out) std::vector<std::string> */
	$result = PyList_New($1.size());
	if ($result != NULL) {
		for (int i = 0; i < $1.size(); ++i) {
			PyList_SetItem($result,
				i,
				PyString_FromString($1.at(i).c_str())
			);
		}
	}
}


%include "common.i"

%include "tick_db.i"
%include "tick_stream.i"

%include "tick_cursor.i"
%include "tick_loader.i"

%include "tick_utils.i"
