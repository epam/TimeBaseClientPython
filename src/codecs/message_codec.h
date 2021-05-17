#ifndef DELTIX_API_MESSAGE_CODEC_H_
#define DELTIX_API_MESSAGE_CODEC_H_

#include "Python.h"

#include "schema.h"
#include "data_reader.h"
#include "data_writer.h"

#include <algorithm>
#include <memory>

namespace DxApiImpl {
namespace Python {

class FieldCodec;
class PythonDxApiModule;

typedef std::vector<Schema::TickDbClassDescriptor> ClassDescriptors;
typedef std::shared_ptr<FieldCodec> FieldCodecPtr;

class MessageCodec {
public:
    MessageCodec(const ClassDescriptors &descriptors, intptr_t num);
    MessageCodec(PythonDxApiModule *dxapi_module, const ClassDescriptors &descriptors, intptr_t num);
    ~MessageCodec();

    void decode(PyObject *message, DxApi::DataReader &reader);
    void encode(PyObject *message, DxApi::DataWriter &writer);

private:
    void buildDecoders(const ClassDescriptors &descriptors, intptr_t num);

    void collectFields(std::vector<Schema::FieldInfo> &fields, 
        const ClassDescriptors &descriptors,
        intptr_t index);

    std::vector<FieldCodecPtr> buildFieldDecoders(
        std::vector<Schema::FieldInfo> &fields,
        const ClassDescriptors &descriptors);

    FieldCodecPtr createFieldCodec(
        Schema::DataType &data_type, std::string &field_name, std::string &relative_to,
        const ClassDescriptors &descriptors);

    int32_t findDescriptorByGuid(const ClassDescriptors &descriptors, const std::string &guid);

    template<typename CODEC_TYPE>
    CODEC_TYPE findRelativeTo(std::string &relativeTo);

private:
    std::vector<FieldCodecPtr> field_codecs_;
    std::unordered_map<std::string, FieldCodecPtr> codecs_search_map_;
    PythonDxApiModule *dxapi_module_ = NULL;

};

typedef std::shared_ptr<MessageCodec> MessageCodecPtr;

}
}

#endif //DELTIX_API_MESSAGE_CODEC_H_