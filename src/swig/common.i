
#include <string>

namespace DxApi {

/*
class InstrumentType {
public:
	enum Enum {
        EQUITY = 0,
        OPTION,
        FUTURE,
        BOND,
        FX,
        INDEX,                  // = 5
        ETF,
        CUSTOM,
        SIMPLE_OPTION,
        EXCHANGE,
        TRADING_SESSION,        // = 10
        STREAM,
        DATA_CONNECTOR,
        EXCHANGE_TRADED_SYNTHETIC,
        SYSTEM,
        CFD,                    // Contract-For-Difference-Synthetic = 15
		UNKNOWN = 0xFF          // unknown/invalid
    };

	InstrumentType(InstrumentType::Enum value);
	InstrumentType(const char *value);
};
%extend InstrumentType {
	uint8_t __int__() {
		return $self->toInt();
	}

	const char *__str__() {
		return $self->toString();
	}
}

struct InstrumentIdentity {
private:
	InstrumentIdentity();
	
public:
    InstrumentIdentity(DxApi::InstrumentType type, const char * symbol);

	bool operator==(const DxApi::InstrumentIdentity &other) const;

    std::string symbol;
    DxApi::InstrumentType type;
};

%extend InstrumentIdentity {
	InstrumentIdentity(DxApi::InstrumentType::Enum type, const char *symbol) {
		return new DxApi::InstrumentIdentity(DxApi::InstrumentType(type), symbol);
	}
}
*/



class StreamScope {
public:
    enum Enum {
        DURABLE,
        EXTERNAL_FILE,
        TRANSIENT,
        RUNTIME
    };

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

class WriteMode {
public:
    enum Enum {
        APPEND,
        REPLACE,
        REWRITE,
        TRUNCATE
    };

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

class TimeUnit {
public:
    enum Enum {
        MILLISECOND = 0,
        SECOND,
        MINUTE,
        HOUR,
        DAY,
        /* Weeks have fixed day order, Monday through Sunday */
        WEEK,
        MONTH,
        /* Quarters are calendar quarters, starting in January, April, July and */
        /* October. This is different from fiscal quarters, which can start in */
        /* any calendar month. */
        QUARTER,
        YEAR,
        UNKNOWN = 0xFF
    };

	TimeUnit(TimeUnit::Enum value);
	TimeUnit(const char *value);
};

%extend TimeUnit {
	uint8_t __int__() {
		return $self->toInt();
	}

	const char *__str__() {
		return $self->toString();
	}
}

class SelectionOptions {
public:
    SelectionOptions();

    int64_t             from;
    int64_t             to;
    bool                useCompression;
    bool                live;
    bool                reverse;
    bool                isBigEndian;
    bool                allowLateOutOfOrder;
    bool                realTimeNotification;
    bool                minLatency;
};

class LoadingOptions {
public:
    WriteMode writeMode;
    bool raw;
    bool minLatency;

    LoadingOptions();
};

class StreamOptions {
public:
    enum {
        MAX_DISTRIBUTION = 0
    };

	//TODO: buffer options
    //BufferOptions			bufferOptions;

    DxApi::StreamScope		scope;
    int32_t					distributionFactor;
    bool					duplicatesAllowed;
    bool					highAvailability;
    bool					unique;
    bool					polymorphic;
    std::string				periodicity;

    bool operator==(const StreamOptions &value) const;

    StreamOptions();
};

%extend StreamOptions {

    const char * name() {
		return $self->name.has_value() ? $self->name.get().c_str() : NULL;
	}

	void name(const char *name) {
		$self->name = std::string(name);
	}

	const char * description() {
		return $self->description.has_value() ? $self->description.get().c_str() : NULL;
	}

	void description(const char *description) { 
		$self->description = std::string(description);
	}

	const char * owner() {
		return $self->owner.has_value() ? $self->owner.get().c_str() : NULL;
	}

	void owner(const char *owner) { 
		$self->owner = std::string(owner);
	}
	
	const char * location() {
		return $self->location.has_value() ? $self->location.get().c_str() : NULL;
	}

	void location(const char *location) { 
		$self->location = std::string(location);
	}

    const char * distributionRuleName() {
		return $self->distributionRuleName.has_value() ? $self->distributionRuleName.get().c_str() : NULL;
	}

	void distributionRuleName(const char *distributionRuleName) { 
		$self->distributionRuleName = std::string(distributionRuleName);
	}

	const char * metadata() {
		return $self->metadata.has_value() ? $self->metadata.get().c_str() : NULL;
	}

	void metadata(const char *metadata) { 
		$self->metadata = std::string(metadata);
	}

};

class Interval {
public:
    Interval(int64_t numUnits = 0, TimeUnit unitType = TimeUnitEnum::MILLISECOND);

    bool isZero() const;
    bool isNegative() const;
    bool isPositive() const;

    int64_t  numUnits;
    DxApi::TimeUnit unitType;
};

struct QueryParameter {
    std::string name;
    std::string type;

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

} // namespace DxApi