
namespace DxApi {

%newobject TickDb::createFromUrl(const char *);
%newobject TickDb::createFromUrl(const char *, const char *, const char *);

class TickDb {
public:
    static TickDb *		createFromUrl(const char * url);
    static TickDb *		createFromUrl(const char * url, const char * username, const char * password);

    bool				isReadOnly() const;
    bool				isOpen() const;

    bool				open(bool readOnlyMode);
    void				close();
    bool				format();

    std::vector<DxApi::TickStream *> listStreams();
    DxApi::TickStream * getStream(const std::string &key);
    DxApi::TickStream * createStream(const std::string &key, const std::string &name, const std::string &description, int distributionFactor);
    DxApi::TickStream * createStream(const std::string &key, const DxApi::StreamOptions &options);
    DxApi::TickStream * createFileStream(const std::string &key, const std::string &dataFile);

    DxApi::TickCursor * createCursor(const DxApi::TickStream * stream, const DxApi::SelectionOptions &options);
    DxApi::TickCursor * select(
		DxApi::TimestampMs time, 
		const std::vector<DxApi::TickStream *> *streams, 
		const DxApi::SelectionOptions &options,
        const std::vector<std::string> *types, 
		const std::vector<std::string> *entities);
    DxApi::TickLoader * createLoader(const DxApi::TickStream * stream, const DxApi::LoadingOptions &options);

	DxApi::TickCursor* executeQuery(
		const std::string &qql, 
		const DxApi::SelectionOptions &options, 
		DxApi::TimestampMs time,
        const std::vector<std::string> *instruments, 
		const std::vector<DxApi::QueryParameter> &params);

    DxApi::TickCursor* executeQuery(
		const std::string &qql, 
		const DxApi::SelectionOptions &options, 
		DxApi::TimestampMs time, 
		const std::vector<DxApi::QueryParameter> &params);

    DxApi::TickCursor* executeQuery(
		const std::string &qql, 
		const DxApi::SelectionOptions &options, 
		const std::vector<DxApi::QueryParameter> &params);

    DxApi::TickCursor* executeQuery(
		const std::string &qql, 
		const std::vector<DxApi::QueryParameter> &params);

    ~TickDb();

protected:
    TickDb();
	TickDb(const TickDb&);
    void operator=(const TickDb&);

}; // class TickDb

%extend TickDb {

	DxApi::TickCursor* executeQuery(const std::string &qql) {
		return $self->executeQuery(qql, std::vector<DxApi::QueryParameter>());
	}

}


} // namespace DxApi