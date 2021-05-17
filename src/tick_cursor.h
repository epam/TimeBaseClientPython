#ifndef DELTIX_API_TICK_CURSOR_H_
#define DELTIX_API_TICK_CURSOR_H_

#include "Python.h"

#include "python_common.h"
#include "native/tickdb/http/xml/tickdb_class_descriptor.h"

namespace DxApiImpl {
namespace Python {

class MessageCodec;

enum NextResult {
    OK, END_OF_CURSOR, UNAVAILABLE
};

class TickCursor {
public:
    TickCursor(DxApi::TickCursor *cursor);
    ~TickCursor();

    bool next();
    NextResult nextIfAvailable();
    PyObject * getMessage();

    bool isAtEnd() const;
    bool isClosed() const;
    void close();

    const char * getCurrentStreamKey();

    void reset(DxApi::TimestampMs dt);
    void reset(DxApi::TimestampMs dt, const std::vector<std::string> *entities);

    void subscribeToAllEntities();
    void clearAllEntities();

    void addEntities(const std::vector<std::string> *entities);
    void addEntity(const std::string &entity);

    void removeEntities(const std::vector<std::string> *entities);
    void removeEntity(const std::string &entity);

    void subscribeToAllTypes();

    void addTypes(const std::vector<std::string> *types);
    void removeTypes(const std::vector<std::string> *types);
    void setTypes(const std::vector<std::string> *types);

    void add(const std::vector<std::string> *types, const std::vector<std::string> * entities);
    void remove(const std::vector<std::string> *types, const std::vector<std::string> * entities);

    void addStreams(const std::vector<DxApi::TickStream *> *streams);
    void removeStreams(const std::vector<DxApi::TickStream *> *streams);
    void removeAllStreams();

    void setTimeForNewSubscriptions(DxApi::TimestampMs dt);

private:

    DISALLOW_COPY_AND_ASSIGN(TickCursor);

    void decodeCurrentMessage();
    void decodeHeader(PyObject * message);

    std::unique_ptr<DxApi::TickCursor> cursor_ = nullptr;
    std::shared_ptr<DxApi::InstrumentMessage> instrument_message_ = nullptr;
    std::vector<std::shared_ptr<MessageCodec>> message_decoders_;
    std::vector<PyObject *> message_objects_;

    PythonDxApiModule dxapi_module_;

    PyObject *  TYPE_ID_PROPERTY1 = PyUnicode_FromString("typeId");
    PyObject *  TYPE_NAME_PROPERTY1 = PyUnicode_FromString("typeName");
    PyObject *  INSTRUMENT_ID_PROPERTY1 = PyUnicode_FromString("instrumentId");
    PyObject *  SYMBOL_PROPERTY1 = PyUnicode_FromString("symbol");
    PyObject *  TIMESTAMP_PROPERTY1 = PyUnicode_FromString("timestamp");

}; // TickCursor

} // namespace Python
} // namespace DxApiImpl

#endif //DELTIX_API_TICK_CURSOR_H_