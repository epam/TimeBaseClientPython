
namespace DxApi {

class TickStream {
public:
    const std::string&						key() const;
    int32_t									distributionFactor() const;
    const DxApi::Nullable<std::string>&		name() const;
    const DxApi::Nullable<std::string>&		description() const;
    const DxApi::Nullable<std::string>&		owner() const;
    const DxApi::Nullable<std::string>&		location() const;
    const DxApi::Nullable<std::string>&		metadata() const;
    DxApi::StreamScope  					scope() const;
    bool									highAvailability() const;
    bool									unique() const;
    bool									polymorphic() const;
    const std::string&						periodicity() const;
    const DxApi::StreamOptions&				options() const;

    bool									setSchema(const DxApi::StreamOptions & options);
	
    std::vector<std::string>				listEntities();

    bool truncate(TimestampMs millisecondTime) const;
	bool truncate(TimestampMs millisecondTime, const std::vector<std::string> * const entities) const;
	bool clear() const;
    bool clear(const std::vector<std::string> * const entities) const;
    bool purge(TimestampMs millisecondTime) const;

    bool deleteStream();
    bool abortBackgroundProcess() const;

	DxApi::TickCursor * select(DxApi::TimestampMs millisecondTime, const DxApi::SelectionOptions &options, 
		const std::vector<std::string> * types, const std::vector<std::string> * entities) const;
    DxApi::TickCursor * createCursor(const DxApi::SelectionOptions &options) const;
    DxApi::TickLoader * createLoader(const DxApi::LoadingOptions &options) const;

    static void operator delete(void* ptr, size_t sz); //not supported by swig, will be skipped

    ~TickStream();

protected:
    TickStream();
    TickStream(const TickStream&);
    void operator=(const TickStream&);
}; // class TickStream

%extend TickStream {
	
	std::vector<long long> getTimeRange() {
		std::vector<long long> result(2);

		//todo: refactor it
		// used array for clang linux copiler
		TimestampMs range[2];
		bool not_null = $self->getTimeRange(range, NULL);

		if (not_null) {
			result[0] = range[0];
			result[1] = range[1];
			return result;
		} else {
			result[0] = DxApi::TIMESTAMP_UNKNOWN;
			result[1] = DxApi::TIMESTAMP_UNKNOWN;
			return result;
		}
	}

	std::vector<long long> getTimeRange(const std::vector<std::string> * const entities) {
		std::vector<long long> result(2);
		
		//todo: refactor it
		// used array for clang linux copiler
		TimestampMs range[2];
		bool not_null = $self->getTimeRange(range, entities);

		if (not_null) {
			result[0] = range[0];
			result[1] = range[1];
			return result;
		} else {
			result[0] = DxApi::TIMESTAMP_UNKNOWN;
			result[1] = DxApi::TIMESTAMP_UNKNOWN;
			return result;
		}
	}
}


} // namespace DxApi