%pythoncode %{
from contextlib import contextmanager

JAVA_LONG_MIN_VALUE = -9223372036854775808
JAVA_LONG_MAX_VALUE = 9223372036854775807

class InstrumentMessage(object):
    def __str__(self):
        return str(vars(self))
%}

#include <string>

namespace DxApi {

%feature("autodoc", "Determines the scope of a stream's durability, if any.

Example:
    ```
    scope = tbapi.StreamScope('TRANSIENT')
    ```

Possible values:
    ```
    DURABLE,
    EXTERNAL_FILE,
    TRANSIENT,
    RUNTIME
    ```");
class StreamScope {
public:
    enum Enum {
        DURABLE,
        EXTERNAL_FILE,
        TRANSIENT,
        RUNTIME
    };

    %feature("autodoc", "");
	StreamScope(StreamScope::Enum value);
	StreamScope(const char *value);
};

%extend StreamScope {
	uint8_t __int__() {
		return $self->toInt();
	}

	const char *__str__() {
		return $self->toString();
	}
}

%feature("autodoc", "APPEND: Adds only new data into a stream without truncations.
REPLACE: Adds data into a stream and removes previous data older that first message time
    [truncate(first message time + 1)].
REWRITE: Default. Adds data into a stream and removes previous data by truncating using first message time.
    [truncate(first message time)].
TRUNCATE: Stream truncated every time when loader writes a messages earlier than last message time.

Example:
    ```
    mode = tbapi.StreamScope('TRUNCATE')
    ```

Possible values:
    ```
    APPEND,
    REPLACE,
    REWRITE,
    TRUNCATE
    ```");
class WriteMode {
public:
    enum Enum {
        APPEND,
        REPLACE,
        REWRITE,
        TRUNCATE
    };

    %feature("autodoc", "");
	WriteMode(WriteMode::Enum value);
	WriteMode(const char *value);
};

%extend WriteMode {
	uint8_t __int__() {
		return $self->toInt();
	}

	const char *__str__() {
		return $self->toString();
	}
}

%feature("autodoc", "Options for selecting data from a stream.

Example:
    ```
    so = tbapi.SelectionOptions()
    so._from = 0
    so.to = 100000
    so.useCompression = False
    ...
    ```");
class SelectionOptions {
public:

    %feature("autodoc", "");
    SelectionOptions();

    %feature("autodoc", "startTime (int): Start timestamp in millis.");
    int64_t             from;

    %feature("autodoc", "endTime (int): End timestamp in millis.");
    int64_t             to;

    %feature("autodoc", "useCompression (bool): Use compression.");
    bool                useCompression;

    %feature("autodoc", "live (bool): Instead of returning false from next () at the end of the stream,
    wait for live data to be added.");
    bool                live;

    %feature("autodoc", "reverse (bool): Specify cursor direction.");
    bool                reverse;

    %feature("autodoc", "allowLateOutOfOrder (bool): Output out-of-order late messages.
    Timebase consumers receive historical messages they requested strictly ordered by their time.
    For scenarios when new messages arrive in the middle of consumer's session (So called 'live' mode)
    it is possible that newly arrived message has a timestamp in already consumer time region.
    In this cases this flag allows consumer to receive these 'late' messages even if they out of order
    with respect to current baseline time.

    NOTE: Late Messages that are timestamped prior to consumer's select time or last reset time
    will not be delivered even with this flag enabled.");
    bool                allowLateOutOfOrder;


    %feature("autodoc", "realTimeNotification (bool): Enabled/Disables sending system messages when cursor switches from historical to realtime mode.");
    bool                realTimeNotification;

    %feature("autodoc", "minLatency (bool): try to receive messages ASAP, with minimal buffering.
    Can potentially reduce max throughput and increase CPU use / network bandwidth usage.");
    bool                minLatency;

};

%feature("autodoc", "Options for loading data into a stream.

Example:
    ```
    lo = tbapi.LoadingOptions()
    lo.writeMode = tbapi.WriteMode('TRUNCATE')
    so.space = 'myspace'
    ...
    ```");
class LoadingOptions {
public:

    %feature("autodoc", "writeMode (WriteMode): see WriteMode class description.");
    WriteMode writeMode;

    %feature("autodoc", "minLatency (bool): try to send messages ASAP, with minimal buffering.
    Can potentially reduce max throughput and increase CPU use / network bandwidth usage.");
    bool minLatency;

    %feature("autodoc", "space (str): Data Partition. Contains unique number of instruments or Time Ranges.");
    std::string space;

    %feature("autodoc", "");
    LoadingOptions();
};

%feature("autodoc", "Stream definition attributes.

Example:
    ```
    so = tbapi.StreamOptions()
    so.name = key
    so.description = key
    so.scope = tbapi.StreamScope('DURABLE')
    so.distributionFactor = 1
    so.highAvailability = False
    so.polymorphic = False
    so.metadata = schema

    db.createStream(key, options)
    ```");
class StreamOptions {
public:
%pythoncode %{
    def name(self, name: str = None) -> None:
        '''Optional user-readable name.'''
        if name == None:
            return self.__getName()
        else:
            self.__setName(name)
            return name

    def description(self, description: str = None) -> None:
        '''Optional multi-line description.'''
        if description == None:
            return self.__getDescription()
        else:
            self.__setDescription(description)
            return description

    def owner(self, owner: str = None) -> None:
        '''Optional owner of stream.
        During stream creation it will be set
        equals to authenticated user name.
        '''
        if owner == None:
            return self.__getOwner()
        else:
            self.__setOwner(owner)
            return owner

    def location(self, location: str = None) -> None:
        '''Location of the stream (by default null). When defined this attribute provides alternative stream location (rather than default location under QuantServerHome)'''
        if location == None:
            return self.__getLocation()
        else:
            self.__setLocation(location)
            return location

    def distributionRuleName(self, distributionRuleName: str = None) -> None:
        '''Class name of the distribution rule'''
        if distributionRuleName == None:
            return self.__getDistributionRuleName()
        else:
            self.__setDistributionRuleName(distributionRuleName)
            return distributionRuleName

    def metadata(self, metadata: str = None) -> None:
        '''Stream metadata in XML format. To build metadata programatically, use tbapi.SchemaDef class.'''
        if metadata == None:
            return self.__getMetadata()
        else:
            self.__setMetadata(metadata)
            return metadata
%}

    %feature("autodoc", "scope (StreamScope): Determines persistent properties of a stream.");
    DxApi::StreamScope		scope;

    %feature("autodoc", "distributionFactor (int): The number of M-files into which to distribute the
    data. Supply MAX_DISTRIBUTION to keep a separate file for each instrument (default).");
    int32_t					distributionFactor;

    %feature("autodoc", "duplicatesAllowed (bool): Indicates that loader will ignore binary similar messages(for 'unique' streams only).");
    bool					duplicatesAllowed;

    %feature("autodoc", "highAvailability (bool): High availability durable streams are cached on startup.");
    bool					highAvailability;

    %feature("autodoc", "unique (bool): Unique streams maintain in-memory cache of resent messages.
    This concept assumes that stream messages will have some field(s) marked as primary key.
    Primary key may be a simple field (e.g. symbol) or composite (e.g. symbol and portfolio ID).
    For each key TimeBase runtime maintains a copy of the last message received for this key (cache).
    Each new consumer will receive a snapshot of current cache at the beginning of live data subscription.");
    bool					unique;

    %feature("autodoc", "polymorphic (bool).");
    bool					polymorphic;

    %feature("autodoc", "periodicity (bool): Stream periodicity, if known.");
    std::string				periodicity;

    %feature("autodoc", "");
    bool operator==(const StreamOptions &value) const;

    StreamOptions();
};

%extend StreamOptions {

    const char * __getName() {
		return $self->name.has_value() ? $self->name.get().c_str() : NULL;
	}

	void __setName(const char *name) {
		$self->name = std::string(name);
	}

	const char * __getDescription() {
		return $self->description.has_value() ? $self->description.get().c_str() : NULL;
	}

	void __setDescription(const char *description) {
		$self->description = std::string(description);
	}

	const char * __getOwner() {
		return $self->owner.has_value() ? $self->owner.get().c_str() : NULL;
	}

	void __setOwner(const char *owner) {
		$self->owner = std::string(owner);
	}

	const char * __getLocation() {
		return $self->location.has_value() ? $self->location.get().c_str() : NULL;
	}

	void __setLocation(const char *location) {
		$self->location = std::string(location);
	}

    const char * __getDistributionRuleName() {
		return $self->distributionRuleName.has_value() ? $self->distributionRuleName.get().c_str() : NULL;
	}

	void __setDistributionRuleName(const char *distributionRuleName) {
		$self->distributionRuleName = std::string(distributionRuleName);
	}

	const char * __getMetadata() {
		return $self->metadata.has_value() ? $self->metadata.get().c_str() : NULL;
	}

	void __setMetadata(const char *metadata) {
		$self->metadata = std::string(metadata);
	}

};

%feature("autodoc", "Input parameter definition for a prepared statement.");
struct QueryParameter {
    std::string name;
    std::string type;

    %feature("autodoc", "");
    QueryParameter(const std::string &name_, const std::string &type_, const std::string value_)
        : name(name_), type(type_), value(value_) {}

    QueryParameter(const std::string &name_, const std::string &type_)
        : name(name_), type(type_) {}
};


%extend QueryParameter {

	QueryParameter(const std::string &name_, const std::string &type_, const char *value) {
		DxApi::QueryParameter *qp = new DxApi::QueryParameter(name_, type_);
		qp->value = std::string(value);
		return qp;
	}

    const char * value() {
		return $self->value.has_value() ? $self->value.get().c_str() : NULL;
	}

	void value(const char *value) {
		$self->value = std::string(value);
	}
}

%feature("autodoc", "");
} // namespace DxApi
