#include "message_codec.h"

#include "field_codecs.h"

#include <cstring>
#include <algorithm>
#include <string> 

namespace TbApiImpl {
namespace Python {
    
MessageCodec::MessageCodec(const ClassDescriptors &descriptors, intptr_t num) {
    buildDecoders(descriptors, num);
}

MessageCodec::MessageCodec(PythonTbApiModule *tbapi_module, const ClassDescriptors &descriptors, intptr_t num)
{
    tbapi_module_ = tbapi_module;
    buildDecoders(descriptors, num);
}

MessageCodec::~MessageCodec() {
}

void MessageCodec::decode(PyObject *message, DxApi::DataReader &reader) {
    for (int i = 0; i < field_codecs_.size(); ++i) {
        PythonRefHolder object(field_codecs_[i]->decode(reader));
        PyObject_SetAttr(message, field_codecs_[i]->getKey(), object.getReference());
    }
}

void MessageCodec::encode(PyObject *message, DxApi::DataWriter &writer) {
    for (int i = 0; i < field_codecs_.size(); ++i) {
        if (!PyObject_HasAttr(message, field_codecs_[i]->getKey())) {
            field_codecs_[i]->encode(Py_None, writer);
            continue;
        }

        PythonRefHolder object(PyObject_GetAttr(message, field_codecs_[i]->getKey()));
        field_codecs_[i]->encode(object.getReference(), writer);
    }
}

void MessageCodec::buildDecoders(const ClassDescriptors &descriptors, intptr_t num) {
    if (descriptors.size() <= 0)
        return;

    std::vector<Schema::FieldInfo> fields;
    collectFields(fields, descriptors, num);
    field_codecs_ = buildFieldDecoders(fields, descriptors);
}

void MessageCodec::collectFields(std::vector<Schema::FieldInfo> &fields, const ClassDescriptors &descriptors, intptr_t index) {
    intptr_t parentIndex = descriptors[index].parentIndex;
    if (parentIndex >= 0)
        collectFields(fields, descriptors, parentIndex);

    for (int i = 0; i < descriptors[index].fields.size(); ++i)
        fields.push_back(descriptors[index].fields[i]);
}

std::vector<FieldCodecPtr> MessageCodec::buildFieldDecoders(
    std::vector<Schema::FieldInfo> &fields,
    const ClassDescriptors &descriptors)
{
    std::vector<FieldCodecPtr> field_codecs;

    for (int i = 0; i < fields.size(); ++i) {
        Schema::FieldInfo &field = fields[i];
        FieldCodecPtr field_codec = createFieldCodec(field.dataType, field.name, field.relativeTo, descriptors);

        if (field_codec != nullptr) {
            field_codecs.push_back(field_codec);
            codecs_search_map_[field.name] = field_codec;
        } else {
            THROW_EXCEPTION("Unable to create decoder for field '%s', type '%s', encoding '%s'", 
                field.name.c_str(), field.dataType.typeName.c_str(), field.dataType.encodingName.c_str());
        }
    }

    return field_codecs;
}

FieldCodecPtr MessageCodec::createFieldCodec(
    Schema::DataType &data_type, std::string &field_name, std::string &relative_to, const ClassDescriptors &descriptors)
{
    std::string temptype = data_type.encodingName.empty() ? data_type.typeName : data_type.encodingName;
    std::transform(temptype.begin(), temptype.end(), temptype.begin(), ::tolower);
    std::remove_if(temptype.begin(), temptype.end(), ::isspace);
    const char *cmpr = temptype.c_str();

    if (strcmp("datetime", cmpr) == 0) {
        return FieldCodecPtr(new TimestampFieldCodec(field_name.c_str(), data_type.isNullable));
    }
    else if (!temptype.compare(0, strlen("decimal64"), "decimal64")) {
        return FieldCodecPtr(new Decimal64FieldCodec(field_name.c_str(), data_type.isNullable));
    }
    else if (!temptype.compare(0, strlen("decimal"), "decimal")) {
        return FieldCodecPtr(new FloatFieldCodec(field_name.c_str(),
            findRelativeTo<FloatFieldCodec *>(relative_to),
            63, data_type.isNullable));
    }
    else if (strcmp("ieee32", cmpr) == 0) {
        return FieldCodecPtr(new FloatFieldCodec(field_name.c_str(),
            findRelativeTo<FloatFieldCodec *>(relative_to),
            32, data_type.isNullable));
    }
    else if (strcmp("ieee64", cmpr) == 0) {
        return FieldCodecPtr(new FloatFieldCodec(field_name.c_str(),
            findRelativeTo<FloatFieldCodec *>(relative_to),
            64, data_type.isNullable));
    } else if (temptype.rfind("binary(", 0) == 0) {
        return FieldCodecPtr(new FloatFieldCodec(field_name.c_str(),
            findRelativeTo<FloatFieldCodec *>(relative_to),
            atoi(temptype.substr(strlen("binary(")).c_str()), data_type.isNullable)
        );
    }
    else if (strcmp("int8", cmpr) == 0) {
        return FieldCodecPtr(new IntegerFieldCodec(field_name.c_str(),
            findRelativeTo<IntegerFieldCodec *>(relative_to),
            8, data_type.isNullable));
    }
    else if (strcmp("int16", cmpr) == 0) {
        return FieldCodecPtr(new IntegerFieldCodec(field_name.c_str(),
            findRelativeTo<IntegerFieldCodec *>(relative_to),
            16, data_type.isNullable));
    }
    else if (strcmp("int32", cmpr) == 0) {
        return FieldCodecPtr(new IntegerFieldCodec(field_name.c_str(),
            findRelativeTo<IntegerFieldCodec *>(relative_to),
            32, data_type.isNullable));
    }
    else if (strcmp("int48", cmpr) == 0) {
        return FieldCodecPtr(new IntegerFieldCodec(field_name.c_str(),
            findRelativeTo<IntegerFieldCodec *>(relative_to),
            48, data_type.isNullable));
    }
    else if (strcmp("int64", cmpr) == 0) {
        return FieldCodecPtr(new IntegerFieldCodec(field_name.c_str(),
            findRelativeTo<IntegerFieldCodec *>(relative_to),
            64, data_type.isNullable));
    } 
    else if (temptype.rfind("signed(", 0) == 0) {
        return FieldCodecPtr(new IntegerFieldCodec(field_name.c_str(),
            findRelativeTo<IntegerFieldCodec *>(relative_to),
            atoi(temptype.substr(strlen("signed(")).c_str()), data_type.isNullable)
        );
    }
    else if (strcmp("puint30", cmpr) == 0) {
        return FieldCodecPtr(new IntegerFieldCodec(field_name.c_str(),
            findRelativeTo<IntegerFieldCodec *>(relative_to),
            30, data_type.isNullable));
    }
    else if (strcmp("puint61", cmpr) == 0) {
        return FieldCodecPtr(new IntegerFieldCodec(field_name.c_str(),
            findRelativeTo<IntegerFieldCodec *>(relative_to),
            61, data_type.isNullable));
    } 
    else if (temptype.rfind("unsigned(", 0) == 0) {
        return FieldCodecPtr(new IntegerFieldCodec(field_name.c_str(),
            findRelativeTo<IntegerFieldCodec *>(relative_to),
            atoi(temptype.substr(strlen("unsigned(")).c_str()), data_type.isNullable)
        );
    }
    else if (strcmp("pinterval", cmpr) == 0) {
        return FieldCodecPtr(new IntervalFieldCodec(field_name.c_str(), data_type.isNullable));
    } 
    else if (strcmp("interval", cmpr) == 0) {
        return FieldCodecPtr(new IntervalFieldCodec(field_name.c_str(), data_type.isNullable));
    }
    else if (strcmp("timeofday", cmpr) == 0) {
        return FieldCodecPtr(new TimeOfDayFieldCodec(field_name.c_str(), data_type.isNullable));
    }
    else if (strcmp("binary", cmpr) == 0) {
        return FieldCodecPtr(new BinaryFieldCodec(field_name.c_str(), data_type.isNullable));
    }
    else if (strcmp("boolean", cmpr) == 0) {
        return FieldCodecPtr(new BooleanFieldCodec(field_name.c_str(), data_type.isNullable));
    }
    else if (strcmp("char", cmpr) == 0) {
        return FieldCodecPtr(new CharFieldCodec(field_name.c_str(), data_type.isNullable));
    }
    else if (strcmp("utf8", cmpr) == 0) {
        return FieldCodecPtr(new Utf8FieldCodec(field_name.c_str(), data_type.isNullable));
    }
    else if (strcmp("ascii", cmpr) == 0) {
        return FieldCodecPtr(new AsciiFieldCodec(field_name.c_str(), data_type.isNullable));
    }
    else if (strcmp("enum", cmpr) == 0) {
        int32_t index = findDescriptorByGuid(descriptors, data_type.descriptorGuid);
        if (index == INT32_MIN)
            THROW_EXCEPTION("Unknown enum type: %s.", data_type.descriptorGuid.c_str());

        return FieldCodecPtr(new EnumFieldCodec(field_name.c_str(), descriptors[index], data_type.isNullable));
    }
    else if (strcmp("array", cmpr) == 0) {
        if (data_type.elementType == nullptr)
            THROW("Empty array element type.");

        std::string new_field_name = "";
        std::string new_relative_to = "";

        FieldCodecPtr element_codec(
            createFieldCodec(*data_type.elementType, new_field_name, new_relative_to, descriptors)
        );

        return FieldCodecPtr(new ArrayFieldCodec(field_name.c_str(), element_codec, data_type.isNullable));
    }
    else if (strcmp("object", cmpr) == 0) {
        std::vector<MessageCodecPtr> codecs;
        std::vector<std::string> types;
        for (std::string &type : data_type.types) {
            int32_t index = findDescriptorByGuid(descriptors, type);
            if (index == INT32_MIN)
                THROW_EXCEPTION("Unknown object type: %s.", type.c_str());

            codecs.push_back(MessageCodecPtr(new MessageCodec(tbapi_module_, descriptors, index)));
            types.push_back(descriptors[index].className);
        }

        return FieldCodecPtr(
            new ObjectFieldCodec(field_name.c_str(), tbapi_module_, types, codecs, data_type.isNullable)
        );
    }
    else if (!temptype.compare(0, strlen("alphanumeric("), "alphanumeric(")) {
            return FieldCodecPtr(
                new AlphanumericFieldCodec(
                    field_name.c_str(), 
                    atoi(temptype.substr(strlen("alphanumeric(")).c_str()), 
                    data_type.isNullable
                )
            );
    };

    return nullptr;
}

int32_t MessageCodec::findDescriptorByGuid(const ClassDescriptors &descriptors, const std::string &guid) {
    int32_t index = INT32_MIN;
    for (int i = 0; i < descriptors.size(); ++i) {
        if (descriptors[i].guid == guid) {
            index = i;
            break;
        }
    }

    return index;
}

template<typename CODEC_TYPE>
CODEC_TYPE MessageCodec::findRelativeTo(std::string &relativeTo)
{
    CODEC_TYPE relative_to = NULL;
    auto it = codecs_search_map_.find(relativeTo);
    if (it != codecs_search_map_.end())
        relative_to = dynamic_cast<CODEC_TYPE>(it->second.get());

    return relative_to;
}

}
}
