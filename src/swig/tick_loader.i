
namespace DxApiImpl {
namespace Python {

%feature("autodoc", "Object which consumes messages.

Create loader from TickDb:
    options = dxapi.LoadingOptions()
    stream = tickdb.createLoader(stream, options)

Create loader from TickStream:
    options = dxapi.LoadingOptions()
    stream = stream.createLoader(options)");

class TickLoader {
public:
	TickLoader(DxApi::TickLoader *loader);
	~TickLoader();

%pythoncode %{

    def send(self, message: InstrumentMessage) -> None:
        '''This method is invoked to send a message to the object.

        Args:
            message (InstrumentMessage): A temporary buffer with the message.
                By convention, the message is only valid for the duration of this call.
        '''
        return self.__send(message)

    def flush(self) -> None:
        '''Flushes all buffered messages by sending them to server.
        Note that calling 'send' method not guaranty that all messages will be delivered and stored to server.
        '''
        return self.__flush()

    def close(self) -> None:
        '''Flushes and closes the loader'''
        return self.__close()

    def addListener(self, listener: 'ErrorListener') -> None:
        '''Register error listener. All writing data errors will be delivered to the listener.

        Args:
            listener (ErrorListener): error listener to register.
        '''
        return self.__addListener(listener)

    def removeListener(self, listener: 'ErrorListener') -> None:
        '''Unsubscribe registered error listener.

        Args:
            listener (ErrorListener): error listener to unsubscribe.
        '''
        return self.__removeListener(listener)

    def nErrorListeners(self) -> int:
        '''Returns number of registered error listeners'''
        return self.__nErrorListeners()

    def registerType(self, type: str) -> int:
        '''Register type of sending message to get type id. For performance reasons,
        you could specify type id instead of type name, for example:

        ```
        message = dxapi.InstrumentMessage()
        message.typeId = loader.registerType("deltix.timebase.api.messages.universal.PackageHeader")
        // as alternative, you could write:
        //   message.typeName = "deltix.timebase.api.messages.universal.PackageHeader"
        loader.send(message)
        ```

        Args:
            type (str): name of type to register.

        Returns:
            int: id of registered type.
        '''
        return self.__registerType(type)

    def registerInstrument(self, symbol: str) -> int:
        '''Register instrument of sending message to get instrument id. For performance reasons,
        you could specify instrument id instead of symbol and instrument type, for example:

        ```
        message = dxapi.InstrumentMessage()
        message.instrumentId = loader.registerInstrument('AAPL')
        // as alternative, you could write:
        //    message.symbol = 'AAPL'
        loader.send(message)
        ```

        Args:
            symbol (str): instrument ticker.

        Returns:
            int: id of registered instrument.
        '''
        return self.__registerInstrument(symbol)

%}

    %feature("autodoc", "");

    %rename(__registerType) registerType;
	uint32_t registerType(const std::string &type_name);

	%rename(__registerInstrument) registerInstrument;
	uint32_t registerInstrument(const std::string &instrument);

    %rename(__send) send;
	void send(PyObject *message);

	%rename(__flush) flush;
	void flush();

	%rename(__close) close;
	void close();

    %rename(__addListener) addListener;
    void addListener(DxApi::TickLoader::ErrorListener * listener);

    %rename(__removeListener) removeListener;
    void removeListener(DxApi::TickLoader::ErrorListener *listener);

    %rename(__nErrorListeners) nErrorListeners;
    size_t nErrorListeners();

}; // TickLoader

%feature("autodoc", "");

}
}

namespace DxApi {
namespace TickLoader {

class ErrorListener {
public:

%pythoncode %{
    def onError(self, errorClass: str, errorMsg: str) -> None:
        return self.__onError()
%}

    %rename(__onError) onError;
	virtual void onError(const std::string &errorClass, const std::string &errorMsg) = 0;
	virtual ~ErrorListener();
};

%feature("autodoc", "");

} // TickLoader
} // DxApi
