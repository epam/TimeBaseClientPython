%pythoncode
%{

from contextlib import contextmanager

@contextmanager
def open_TickDb(url, readonly):
    db = TickDb.createFromUrl(url)
    try:
        db.open(readonly)
        yield db
    finally:
        if db.isOpen():
            db.close()


@contextmanager
def open_TickCursor(stream, ts_from=0, options=SelectionOptions(), types=None, entities=None):
    cursor = None
    try:
        cursor = stream.select(ts_from, options, types, entities)
        yield cursor
    finally:
        cursor.close()



from collections import defaultdict

__JAVA_LONG_MAX_VALUE = 922337203685477580

def stream_to_dict(db, stream, fields=None, ts_from=0, ts_to=__JAVA_LONG_MAX_VALUE):
    if ts_to > __JAVA_LONG_MAX_VALUE:
        ts_to = __JAVA_LONG_MAX_VALUE
    if not db.isOpen():
        raise Exception('Database is not opened.')
    options = SelectionOptions()
    options.to = ts_to
    messages = []
    table = defaultdict(list)
    with open_TickCursor(stream, ts_from, options) as cursor:
        counter = 0
        while cursor.next():
            message = vars(cursor.getMessage())
            messages.append(message)
            if fields is None:
                def to_write(x):
                    return True
            else:
                def to_write(x):
                    return x in fields
            for key in table.keys():
                if key in message:
                    table[key].append(message[key])
                    del message[key]
                else:
                    table[key].append(None)
            for key in message:
                if to_write(key):
                    table[key] = [None] * counter
                    table[key].append(message[key])
            counter += 1
    return table



import xml.etree.ElementTree as ETree
import re


class ClassType:
    ENUM_CLASS = 'enumClass'
    RECORD_CLASS = 'recordClass'


class FieldType:
    NONSTATIC = 'nonStaticDataField'
    STATIC = 'staticDataField'


class SchemaDef:
    def __init__(self, types, all_types):
        self.types = types
        self.all_types = all_types


class ClassDescDef:
    def __init__(self, name):
        self.name = name


class TypeDef(ClassDescDef):
    def __init__(self, name, fields):
        ClassDescDef.__init__(self, name)
        self.fields = fields


class EnumDef(ClassDescDef):
    def __init__(self, name, values):
        ClassDescDef.__init__(self, name)
        self.values = values


class FieldDef:
    def __init__(self, name, data_type, nullable, field_type):
        self.name = name
        self.data_type = data_type
        self.nullable = nullable
        self.field_type = field_type


class SchemaParser:

    @staticmethod
    def parse_Schema(stream):
        xml = stream.metadata()
        tree = ETree.fromstring(xml)
        return SchemaParser.parse_classSet(tree)

    @staticmethod
    def parse_value(value, prefix):
        return value.find('prefix:symbol', prefix).text

    @staticmethod
    def parse_field(field, prefix):
        name = field.find('prefix:name', prefix)
        field_type = SchemaParser.rm_brackets(list(field.attrib.values())[0])
        if field_type != FieldType.STATIC and field_type != FieldType.NONSTATIC:
            raise Exception('Unknown field type {}'.format(field_type))
        data_type = field.find('prefix:type', prefix)
        nullable = bool(data_type.find('prefix:nullable', prefix).text)
        data_type = list(data_type.attrib.values())[0]
        return FieldDef(name.text, data_type, nullable, field_type)

    @staticmethod
    def parse_classDescriptor(class_desc, prefix):
        name = class_desc.find('prefix:name', prefix)
        type_ = list(class_desc.attrib.values())[0]
        if type_ == ClassType.RECORD_CLASS:
            fields = [SchemaParser.parse_field(field, prefix) for field in class_desc.findall('prefix:field', prefix)]
            return TypeDef(name.text, fields)
        elif type_ == ClassType.ENUM_CLASS:
            values = [SchemaParser.parse_value(value, prefix) for value in class_desc.findall('prefix:value', prefix)]
            return EnumDef(name.text, values)
        else:
            raise Exception('Unknown class descriptor type {}'.format(type_))

    @staticmethod
    def parse_classSet(class_set):
        types = []
        all_types = []
        prefix = {}
        key = class_set.tag
        prefix['prefix'] = re.search(r'(?<={).*(?=})', key).group(0)
        for class_desc in class_set.findall('prefix:classDescriptor', prefix):
            class_desc = SchemaParser.parse_classDescriptor(class_desc, prefix)
            all_types.append(class_desc)
            if isinstance(class_desc, TypeDef):
                types.append(class_desc)
        return SchemaDef(types, all_types)

    @staticmethod
    def rm_brackets(obj):
        if isinstance(obj, str):
            return re.sub('^{.*}', '', obj)
        elif isinstance(obj, dict):
            return {(SchemaParser.rm_brackets(key), value) for (key, value) in obj.items()}
        else:
            raise Exception('rm_brackets for {} is not implemented'.format(type(obj)))


def __getSchema(self):
    return SchemaParser.parse_Schema(self)


TickStream.getSchema = __getSchema
%}