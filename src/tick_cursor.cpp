#include "tick_cursor.h"

#include "python_common.h"
#include "codecs/message_codec.h"

namespace DxApiImpl {
namespace Python {

enum CursorState {
    STARTED,
    READING,
    END
};
    
TickCursor::TickCursor(DxApi::TickCursor *cursor) {
    cursor_ = std::unique_ptr<DxApi::TickCursor>(cursor);
}

TickCursor::~TickCursor() {
    Py_DECREF(TYPE_ID_PROPERTY1);
    Py_DECREF(TYPE_NAME_PROPERTY1);
    Py_DECREF(INSTRUMENT_ID_PROPERTY1);
    Py_DECREF(SYMBOL_PROPERTY1);
    Py_DECREF(TIMESTAMP_PROPERTY1);

    if (cursor_ == nullptr)
        return;

    if (!cursor_->isClosed())
        cursor_->close();
    for (int i = 0; i < message_objects_.size(); ++i) {
        if (message_objects_[i] != NULL) {
            Py_DECREF(message_objects_[i]);
        }
    }
}

const char * TickCursor::getCurrentStreamKey() {
    if (instrument_message_ == nullptr)
        return NULL;

    const std::string * stream = cursor_->getMessageStreamKey(instrument_message_->streamId);
    if (stream != NULL) {
        return stream->c_str();
    } else {
        return NULL;
    }
}

bool TickCursor::next() {
    if (cursor_ == nullptr)
        THROW("Cursor is null.");

    if (cursor_->isClosed())
        THROW("Cursor is closed.");

    if (cursor_->isAtEnd())
        return false;

    if (instrument_message_ == nullptr)
        instrument_message_ = std::shared_ptr<DxApi::InstrumentMessage>(new DxApi::InstrumentMessage());

    bool has_next = cursor_->next(instrument_message_.get());
    if (has_next)
        decodeCurrentMessage();

    return has_next;
}

NextResult TickCursor::nextIfAvailable() {
    if (cursor_ == nullptr)
        THROW("Cursor is null.");

    if (cursor_->isClosed())
        THROW("Cursor is closed.");

    if (cursor_->isAtEnd())
        return NextResult::END_OF_CURSOR;

    if (instrument_message_ == nullptr)
        instrument_message_ = std::shared_ptr<DxApi::InstrumentMessage>(new DxApi::InstrumentMessage());

    bool has_next = cursor_->nextIfAvailable(instrument_message_.get());
    if (has_next) {
        decodeCurrentMessage();
        return NextResult::OK;
    }

    return instrument_message_->cursorState >= CursorState::END ?
        NextResult::END_OF_CURSOR :
        NextResult::UNAVAILABLE;
}

PyObject * TickCursor::getMessage() {
    if (instrument_message_ == nullptr)
        Py_RETURN_NONE;

    if (instrument_message_->typeId >= message_objects_.size())
        Py_RETURN_NONE;

    PyObject *message_object = message_objects_[instrument_message_->typeId];
    Py_INCREF(message_object);
    return message_object;
}

void TickCursor::decodeCurrentMessage() {
    uint32_t type_id = instrument_message_->typeId;
    while (message_decoders_.size() <= type_id)
        message_decoders_.push_back(nullptr);

    while (message_objects_.size() <= type_id)
        message_objects_.push_back(NULL);

    std::shared_ptr<MessageCodec> message_decoder = message_decoders_[type_id];
    if (message_decoder == nullptr) {
        const std::string *schema = cursor_->getMessageSchema(type_id);

        std::vector<Schema::TickDbClassDescriptor> descriptors =
            Schema::TickDbClassDescriptor::parseDescriptors(*schema, true);

        message_decoder = std::shared_ptr<MessageCodec>(new MessageCodec(&dxapi_module_, descriptors, 0));
        message_decoders_[type_id] = message_decoder;
    }

    PyObject *message_object = message_objects_[type_id];
    if (message_object == NULL) {
        message_object = dxapi_module_.newInstrumentMessageObject();
        //message_object = PyDict_New();
        if (message_object == NULL)
            THROW_EXCEPTION("Can't create object of class '%32s'", MESSAGE_OBJECT_CLASS_NAME.c_str());

        message_objects_[type_id] = message_object;
    }

    decodeHeader(message_object);
    message_decoder->decode(message_object, cursor_->getReader());
}

void TickCursor::decodeHeader(PyObject * message_object) {
    //decode timestamp
    PythonRefHolder ts_obj(PyLong_FromLongLong(instrument_message_->timestamp));
    PyObject_SetAttr(message_object, TIMESTAMP_PROPERTY1, ts_obj.getReference());

    //decode symbol
    const std::string *symbol_string = cursor_->getInstrument(instrument_message_->entityId);
    if (symbol_string == NULL) {
        PyObject_SetAttr(message_object, SYMBOL_PROPERTY1, Py_None);
    } else {
        PythonRefHolder symbol_obj(PyUnicode_FromString(symbol_string->c_str()));
        PyObject_SetAttr(message_object, SYMBOL_PROPERTY1, symbol_obj.getReference());
    }

    //decode type id
    PythonRefHolder type_id_obj(PyLong_FromLong(instrument_message_->typeId));
    PyObject_SetAttr(message_object, TYPE_ID_PROPERTY1, type_id_obj.getReference());

    //decode type name
    const std::string *type_name_string = cursor_->getMessageTypeName(instrument_message_->typeId);
    if (type_name_string == NULL) {
        PyObject_SetAttr(message_object, TYPE_NAME_PROPERTY1, Py_None);
    } else {
        PythonRefHolder type_name_obj(PyUnicode_FromString(type_name_string->c_str()));
        PyObject_SetAttr(message_object, TYPE_NAME_PROPERTY1, type_name_obj.getReference());
    }
}

bool TickCursor::isAtEnd() const {
    return cursor_->isAtEnd();
}

bool TickCursor::isClosed() const {
    return cursor_->isClosed();
}

void TickCursor::close() {
    cursor_->close();
}

void TickCursor::reset(DxApi::TimestampMs dt) {
    cursor_->reset(dt);
}

void TickCursor::reset(DxApi::TimestampMs dt, const std::vector<std::string> *entities) {
    if (entities == NULL)
        return;

    cursor_->reset(dt, *entities);
}

void TickCursor::subscribeToAllEntities() {
    cursor_->subscribeToAllEntities();
}

void TickCursor::clearAllEntities() {
    cursor_->clearAllEntities();
}

void TickCursor::addEntities(const std::vector<std::string> *entities) {
    if (entities == NULL)
        return;

    cursor_->addEntities(*entities);
}

void TickCursor::addEntity(const std::string &entity) {
    cursor_->addEntity(entity);
}

void TickCursor::removeEntities(const std::vector<std::string> *entities) {
    if (entities == NULL)
        return;

    cursor_->removeEntities(*entities);
}

void TickCursor::removeEntity(const std::string &entity) {
    cursor_->removeEntity(entity);
}

void TickCursor::subscribeToAllTypes() {
    cursor_->subscribeToAllTypes();
}

void TickCursor::addTypes(const std::vector<std::string> *types) {
    if (types == NULL)
        return;

    cursor_->addTypes(*types);
}

void TickCursor::removeTypes(const std::vector<std::string> *types) {
    if (types == NULL)
        return;

    cursor_->removeTypes(*types);
}

void TickCursor::setTypes(const std::vector<std::string> *types) {
    if (types == NULL)
        return;

    cursor_->setTypes(*types);
}

void TickCursor::add(const std::vector<std::string> *types, const std::vector<std::string> * entities) {
    if (entities == NULL || types == NULL)
        return;

    cursor_->add(*entities, *types);
}

void TickCursor::remove(const std::vector<std::string> *types, const std::vector<std::string> * entities) {
    if (entities == NULL || types == NULL)
        return;

    cursor_->remove(*entities, *types);
}

void TickCursor::addStreams(const std::vector<DxApi::TickStream *> *streams) {
    if (streams == NULL)
        return;

    cursor_->addStreams(*streams);
}

void TickCursor::removeStreams(const std::vector<DxApi::TickStream *> *streams) {
    if (streams == NULL)
        return;

    cursor_->removeStreams(*streams);
}

void TickCursor::removeAllStreams() {
    cursor_->removeAllStreams();
}

void TickCursor::setTimeForNewSubscriptions(DxApi::TimestampMs dt) {
    cursor_->setTimeForNewSubscriptions(dt);
}

}
}