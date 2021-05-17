#ifndef DELTIX_API_TICK_LOADER_H_
#define DELTIX_API_TICK_LOADER_H_

#include "Python.h"

#include "native/tickdb/http/xml/tickdb_class_descriptor.h"
#include "python_common.h"

#include <mutex>

namespace DxApiImpl {
namespace Python {

class MessageCodec;

class LoaderErrorListener : public DxApi::TickLoader::ErrorListener {
private:
    ErrorListener * delegateListener;

public:
    LoaderErrorListener(ErrorListener *listener) {
        this->delegateListener = listener;
    }

    ~LoaderErrorListener() {
        delegateListener = 0;
    }
    
    virtual void onError(const std::string &errorClass, const std::string &errorMsg) {
        if (delegateListener != 0) {
            try {
                PythonGILLockHolder lock;
                delegateListener->onError(errorClass, errorMsg);
            } catch (...) {
                std::cout << "An error occured while processing loader error listener." << std::endl;
            }
        }
    }
};

class TickLoader {
public:
    TickLoader(DxApi::TickLoader *loader);
    ~TickLoader();

    uint32_t registerType(const std::string &type_name);
    uint32_t registerInstrument(const std::string &instrument);

    void send(PyObject *message);
    void flush();
    void close();

    void addListener(DxApi::TickLoader::ErrorListener * listener);
    void addListener(DxApi::TickLoader::SubscriptionListener *listener);

    void removeListener(DxApi::TickLoader::ErrorListener *listener);
    void removeListener(DxApi::TickLoader::SubscriptionListener *listener);

    size_t nErrorListeners();
    size_t nSubscriptionListeners();

private:
    DISALLOW_COPY_AND_ASSIGN(TickLoader);

    int32_t findDescriptor(const std::string &name);

    int32_t getTypeId(PyObject *message);
    int32_t getStrTypeId(PyObject *message);
    int32_t getInstrumentId(PyObject *message);
    int32_t getStrInstrumentId(PyObject *message);
    DxApi::TimestampMs getTimestamp(PyObject *message);

    void clearListeners();
    void freeListeners();

    uint32_t next_id_ = 0;
    std::unordered_map<std::string, uint32_t> type_to_id_;
    std::unordered_map<std::string, uint32_t> symbol_to_id_;
    std::vector<std::shared_ptr<MessageCodec>> message_codecs_;

    std::vector<Schema::TickDbClassDescriptor> descriptors_;
    std::unique_ptr<DxApi::TickLoader> loader_;

    PyObject *  TYPE_ID_PROPERTY1 = PyUnicode_FromString("typeId");
    PyObject *  TYPE_NAME_PROPERTY1 = PyUnicode_FromString("typeName");
    PyObject *  INSTRUMENT_ID_PROPERTY1 = PyUnicode_FromString("instrumentId");
    PyObject *  SYMBOL_PROPERTY1 = PyUnicode_FromString("symbol");
    PyObject *  TIMESTAMP_PROPERTY1 = PyUnicode_FromString("timestamp");

    std::unordered_map<DxApi::TickLoader::ErrorListener *, LoaderErrorListener *> errorListeners_;
    std::mutex errorListenerLock_;
    std::vector<LoaderErrorListener *> removedListeners_;

}; // TickLoader

}
}

#endif // DELTIX_API_TICK_LOADER_H_