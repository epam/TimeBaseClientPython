
namespace DxApiImpl {
namespace Python {

enum NextResult {
    OK, END_OF_CURSOR, UNAVAILABLE
};

class TickCursor {
private:
	TickCursor();

public:
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

}; // TickCursor

}
}


