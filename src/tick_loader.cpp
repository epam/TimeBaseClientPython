#include "tick_loader.h"

#include "python_common.h"
#include "codecs/message_codec.h"

#include <thread>
#include <chrono>

namespace DxApiImpl {
namespace Python {

TickLoader::TickLoader(DxApi::TickLoader *loader) {
    loader_ = std::unique_ptr<DxApi::TickLoader>(loader);

    const DxApi::Nullable<std::string> &metadata = loader->stream()->metadata();
    if (!metadata.has_value())
        THROW("Empty stream schema.");

    descriptors_ = Schema::TickDbClassDescriptor::parseDescriptors(metadata.get(), true);
}

TickLoader::~TickLoader() {
    Py_DECREF(TYPE_ID_PROPERTY1);
    Py_DECREF(TYPE_NAME_PROPERTY1);
    Py_DECREF(INSTRUMENT_ID_PROPERTY1);
    Py_DECREF(SYMBOL_PROPERTY1);
    Py_DECREF(TIMESTAMP_PROPERTY1);

    clearListeners();
    freeListeners();

    if (loader_ == nullptr)
        return;

    if (!loader_->isClosed())
        loader_->close();
}

void TickLoader::clearListeners() {
    MutexHolder mutexHolder(&errorListenerLock_);

    for (auto it = errorListeners_.begin(); it != errorListeners_.end(); ++it) {
        LoaderErrorListener *errorListener = it->second;
        if (errorListener != 0) {
            loader_->removeListener(errorListener);
            removedListeners_.push_back(errorListener);
        }
    }
    errorListeners_.clear();
}

void TickLoader::freeListeners() {
    MutexHolder mutexHolder(&errorListenerLock_);

    for (auto l : removedListeners_) {
        delete l;
    }
    removedListeners_.clear();
}

uint32_t TickLoader::registerType(const std::string &type_name) {
    auto it = type_to_id_.find(type_name);
    if (it == type_to_id_.end()) {
        int32_t descriptor_id = findDescriptor(type_name);
        if (descriptor_id == INT32_MIN)
            THROW_EXCEPTION("Type '%s' not found in stream schema.", type_name.c_str());

        std::shared_ptr<MessageCodec> new_message_codec = 
            std::shared_ptr<MessageCodec>(new MessageCodec(descriptors_, descriptor_id));

        while (message_codecs_.size() <= next_id_)
            message_codecs_.push_back(NULL);
        message_codecs_[next_id_] = new_message_codec;
        type_to_id_[type_name] = next_id_;
        loader_->registerMessageType(next_id_, type_name);

        return next_id_++;
    }
        
    return it->second;
}

uint32_t TickLoader::registerInstrument(const std::string &instrument) {
    return loader_->getInstrumentId(instrument);
}

void TickLoader::send(PyObject *message) {
    int32_t type_id = getTypeId(message);
    int32_t instrument_id = getInstrumentId(message);
    DxApi::TimestampMs timestamp = getTimestamp(message);

    DxApi::DataWriter &writer = loader_->beginMessage(type_id, instrument_id, timestamp);
    message_codecs_[type_id]->encode(message, writer);
    loader_->send();
}

void TickLoader::flush() {
    loader_->flush();
}

void TickLoader::close() {
    clearListeners();

    Py_BEGIN_ALLOW_THREADS;
    try {
        loader_->close();
    }
    catch (...) {
        std::cout << "Error occured while closing loader" << std::endl;
    }
    Py_END_ALLOW_THREADS;
}

void TickLoader::addListener(DxApi::TickLoader::ErrorListener * listener) {
    MutexHolder mutexHolder(&errorListenerLock_);

    LoaderErrorListener * loaderListener = new LoaderErrorListener(listener);
    errorListeners_[listener] = loaderListener;
    loader_->addListener(loaderListener);
}

void TickLoader::addListener(DxApi::TickLoader::SubscriptionListener *listener) {
    loader_->addListener(listener);
}

void TickLoader::removeListener(DxApi::TickLoader::ErrorListener *listener) {
    MutexHolder mutexHolder(&errorListenerLock_);

    LoaderErrorListener * loaderListener = errorListeners_[listener];
    if (loaderListener != 0) {
        loader_->removeListener(loaderListener);
        errorListeners_.erase(listener);
        removedListeners_.push_back(loaderListener);
    }
}

void TickLoader::removeListener(DxApi::TickLoader::SubscriptionListener *listener) {
    loader_->removeListener(listener);
}

size_t TickLoader::nErrorListeners() {
    return loader_->nErrorListeners();
}

size_t TickLoader::nSubscriptionListeners() {
    return loader_->nSubscriptionListeners();
}

int32_t TickLoader::getTypeId(PyObject *message) {
    int32_t type_id;
    bool exists = getInt32Value(message, TYPE_ID_PROPERTY, TYPE_ID_PROPERTY1, type_id);
    if (!exists) {
        type_id = getStrTypeId(message);
        if (type_id == INT32_MIN)
            THROW_EXCEPTION("Unkown type of message. Specify '%s' attribute.", TYPE_NAME_PROPERTY.c_str());
    }

    return type_id;
}

int32_t TickLoader::getStrTypeId(PyObject *message) {
    std::string type_name;
    bool exists = getStringValue(message, TYPE_NAME_PROPERTY, TYPE_NAME_PROPERTY1, type_name);
    if (!exists)
        return INT32_MIN;

    return registerType(type_name);
}

int32_t TickLoader::getInstrumentId(PyObject *message) {
    int32_t instrument_id;
    bool exists = getInt32Value(message, INSTRUMENT_ID_PROPERTY, INSTRUMENT_ID_PROPERTY1, instrument_id);
    if (!exists) {
        instrument_id = getStrInstrumentId(message);
        if (instrument_id == INT32_MIN)
            THROW_EXCEPTION("Unknown instrument id. Specify '%s' property.", INSTRUMENT_ID_PROPERTY.c_str());
    }

    return instrument_id;
}

int32_t TickLoader::getStrInstrumentId(PyObject *message) {
    std::string symbol;
    bool exists = getStringValue(message, SYMBOL_PROPERTY, SYMBOL_PROPERTY1, symbol);
    if (!exists)
        return INT32_MIN;

    if (symbol.empty())
        THROW_EXCEPTION("Symbol is empty. Specify '%s' attribute for message.", SYMBOL_PROPERTY.c_str());

    auto it = symbol_to_id_.find(symbol);
    if (it != symbol_to_id_.end())
        return it->second;

    int32_t id = registerInstrument(symbol);
    symbol_to_id_[symbol] = id;
    return id;
}

DxApi::TimestampMs TickLoader::getTimestamp(PyObject *message) {
    int64_t ret_value;
    bool ok = getInt64Value(message, TIMESTAMP_PROPERTY, TIMESTAMP_PROPERTY1, ret_value);
    if (!ok)
        return DxApi::TIMESTAMP_UNKNOWN;

    return ret_value;
}

int32_t TickLoader::findDescriptor(const std::string &name) {
    for (int i = 0; i < descriptors_.size(); ++i) {
        if (descriptors_[i].className == name) {
            return i;
        }
    }

    return INT32_MIN;
}

} // namespace Python
} // namespace DxApiImpl