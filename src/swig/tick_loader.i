
namespace DxApiImpl {
namespace Python {

class TickLoader {
public:
	TickLoader(DxApi::TickLoader *loader);
	~TickLoader();

	uint32_t registerType(const std::string &type_name);
	uint32_t registerInstrument(const std::string &instrument);

	void send(PyObject *message);
	void flush();
	void close();

    void addListener(DxApi::TickLoader::ErrorListener * listener);
    //void addListener(DxApi::TickLoader::SubscriptionListener *listener);

    void removeListener(DxApi::TickLoader::ErrorListener *listener);
    //void removeListener(DxApi::TickLoader::SubscriptionListener *listener);

    size_t nErrorListeners();
    size_t nSubscriptionListeners();

}; // TickLoader

}
}

namespace DxApi {
namespace TickLoader {

class ErrorListener {
public:
	virtual void onError(const std::string &errorClass, const std::string &errorMsg) = 0;
	virtual ~ErrorListener();
};

/*
class SubscriptionListener {
public:
	virtual void typesAdded(const std::vector<std::string> &types) = 0;
	virtual void typesRemoved(const std::vector<std::string> &types) = 0;
	virtual void allTypesAdded() = 0;
	virtual void allTypesRemoved() = 0;

	virtual void entitiesAdded(const std::vector<InstrumentIdentity> &entities) = 0;
	virtual void entitiesRemoved(const std::vector<InstrumentIdentity> &entities) = 0;
	virtual void allEnititesAdded() = 0;
	virtual void allEnititesRemoved() = 0;

protected:
	~SubscriptionListener();
};
*/

} // TickLoader
} // DxApi