
namespace DxApiImpl {
namespace Python {

enum NextResult {
    OK, END_OF_CURSOR, UNAVAILABLE
};

%feature("autodoc", "A cursor (also known as iterator, or result set) for reading data from a
stream. This class provides methods for dynamically reconfiguring the feed,
as well as method reset for essentially re-opening the cursor on a completely different timestamp.

To get a cursor, use select method from TickDb or TickStream objects,
or call executeQuery to open cursor to QQL result set.

Also cursor can be created with createCursor method, but it will be not initialized cursor,
so cursor should be configured with types, entities and read time calling reset:
    ```
    options = dxapi.SelectionOptions()
    cursor = tickdb.createCursor(stream, options)
    cursor.subscribeToAllEntities()
    cursor.subscribeToAllTypes()
    cursor.reset(timestamp)
    ```");
class TickCursor {
private:
	TickCursor();

public:
	~TickCursor();

%pythoncode %{
    def next(self) -> bool:
        '''Moves cursor on to the next message. This method blocks until the next message becomes available,
        or until the cursor is determined to be at the end of the sequence.
        This method is illegal to call if isAtEnd() returns true.

        Returns:
            bool: false if at the end of the cursor.
        '''
        return self.__next()

    def getMessage(self) -> 'InstrumentMessage':
        '''Returns an InstrumentMessage object cursor points at.'''
        return self.__getMessage()

    def isAtEnd(self) -> bool:
        '''Returns true if the last call to next() returned false. Returns false if next() has not been called yet.
        This method is legal to call any number of times at any point in the cursor's lifecycle.
        '''
        return self.__isAtEnd()

    def nextIfAvailable(self) -> int:
        '''Moves cursor on to the next message, but this method NOT blocks until the next message becomes available.

        Returns:
            NextResult: OK (0) if new message is available,
                END_OF_CURSOR (1) if cursor was closed,
                otherwise, UNAVAILABLE (2)
        '''
        return self.__nextIfAvailable()

    def isClosed(self) -> bool:
        '''Returns true, if cursor was closed'''
        return self.__isClosed()

    def close(self) -> None:
        '''Close the cursor'''
        return self.__close()

    def getCurrentStreamKey(self) -> str:
        '''Return the key of the stream that is the source of the current message.'''
        return self.__getCurrentStreamKey()

    def reset(self, timestamp: int, entities: 'list[str]' = None) -> None:
        '''Reposition the message source to a new point in time, while
        preserving current subscription.

        Args:
            timestamp (int): The new position in time in millis.
            entities ('list[str]'): list of entities to reset
        '''
        if entities == None:
            return self.__reset(timestamp)
        else:
            return self.__resetEntities(timestamp, entities)

    def subscribeToAllEntities(self) -> None:
        '''Subscribe to all available entities.'''
        return self.__subscribeToAllEntities()

    def clearAllEntities(self) -> None:
        '''Switch to selective subscription mode (if necessary) and clear the list.'''
        return self.__clearAllEntities()

    def addEntity(self, entity: str) -> None:
        '''Add the specified entity to subscription. The type and symbol are copied
        from the incoming object, if necessary, so the argument can be re-used
        after the call.

        Special note about options:
        The following fragment will subscribe to specific option contract "DAV   100417P00085000":
            cursor.addEntity('DAV   100417P00085000');

        While the following will subscribe to option root (and you will get all instruments with this root):
            cursor.addEntity("DAV   ");
        '''
        return self.__addEntity(entity)

    def addEntities(self, entities: 'list[str]') -> None:
        '''Bulk add the specified entities to subscription. The type and symbol are copied
        from the incoming objects, if necessary, so the arguments can be re-used
        after the call.
        '''
        return self.__addEntities(entities)

    def removeEntities(self, entities: 'list[str]') -> None:
        '''Remove the specified entities from subscription. The type and symbol are copied
        from the incoming objects, if necessary, so the arguments can be re-used
        after the call.
        '''
        return self.__removeEntities(entities)

    def removeEntity(self, entity: str) -> None:
        '''Remove the specified entity from subscription. The type and symbol are copied
        from the incoming object, if necessary, so the argument can be re-used
        after the call.
        '''
        return self.__removeEntity(entity)

    def subscribeToAllTypes(self) -> None:
        '''Subscribe to all available types (no filtering).'''
        return self.__subscribeToAllTypes()

    def addTypes(self, types: 'list[str]') -> None:
        '''Add the specified type names to subscription.'''
        return self.__addTypes(types)

    def removeTypes(self, types: 'list[str]') -> None:
        '''Remove the specified types from subscription.'''
        return self.__removeTypes(types)

    def setTypes(self, types: 'list[str]') -> None:
        '''Subscribe to specified types.'''
        return self.__setTypes(types)

    def add(self, types: 'list[str]', entities: 'list[str]') -> None:
        '''Add the specified entities and types to subscription. The type and symbol are copied
        from the incoming object, if necessary, so the argument can be re-used
        after the call.

        Args:
            types (list[str]): not-null array of type names to subscribe.
            entities (list[str]): not-null array of instruments to subscribe.
        '''
        return self.__add(types, entities)

    def remove(self, types: 'list[str]', entities: 'list[str]') -> None:
        '''Remove the specified entities and types from subscription. The type and symbol are copied
        from the incoming objects, if necessary, so the arguments can be re-used
        after the call.

        Args:
            types (list[str]): not-null array of type names to unsubscribe.
            entities (list[str]): not-null array of instruments to unsubscribe.
        '''
        return self.__remove(types, entities)

    def addStreams(self, streams: 'list[TickStream]') -> None:
        '''Add streams to subscription. Current time and filter is used to query
        data from new sources.

        Args:
            streams ('list[TickStream]'): Streams to add.
        '''
        return self.__addStreams(streams)

    def removeStreams(self, streams: 'list[TickStream]') -> None:
        '''Remove streams from subscription.

        Args:
            streams (list[TickStream]): Streams to remove.
        '''
        return self.__removeStreams(streams)

    def removeAllStreams(self) -> None:
        '''Remove all streams from subscription.'''
        return self.__removeAllStreams()

    def setTimeForNewSubscriptions(self, timestamp: int) -> None:
        '''This method affects subsequent "add subscription" methods,
        such as, for instance, addEntity(). New subscriptions start at
        the specified time.

        Args:
            timestamp (int): The time to use.
        '''
        return self.__setTimeForNewSubscriptions(timestamp)
%}

    %feature("autodoc", "");

    %rename(__next) next;
	bool next();

	%rename(__nextIfAvailable) nextIfAvailable;
	NextResult nextIfAvailable();

	%rename(__getMessage) getMessage;
	PyObject * getMessage();

    %rename(__isAtEnd) isAtEnd;
	bool isAtEnd() const;

	%rename(__isClosed) isClosed;
	bool isClosed() const;

	%rename(__close) close;
	void close();

    %rename(__getCurrentStreamKey) getCurrentStreamKey;
	const char * getCurrentStreamKey();

    %rename(__reset) reset;
	void reset(DxApi::TimestampMs dt);

	%rename(__resetEntities) reset;
	void reset(DxApi::TimestampMs dt, const std::vector<std::string> *entities);

    %rename(__subscribeToAllEntities) subscribeToAllEntities;
	void subscribeToAllEntities();

	%rename(__clearAllEntities) clearAllEntities;
	void clearAllEntities();

    %rename(__addEntities) addEntities;
	void addEntities(const std::vector<std::string> *entities);

	%rename(__addEntity) addEntity;
	void addEntity(const std::string &entity);

    %rename(__removeEntities) removeEntities;
	void removeEntities(const std::vector<std::string> *entities);

	%rename(__removeEntity) removeEntity;
	void removeEntity(const std::string &entity);

    %rename(__subscribeToAllTypes) subscribeToAllTypes;
	void subscribeToAllTypes();

    %rename(__addTypes) addTypes;
	void addTypes(const std::vector<std::string> *types);

	%rename(__removeTypes) removeTypes;
	void removeTypes(const std::vector<std::string> *types);

	%rename(__setTypes) setTypes;
	void setTypes(const std::vector<std::string> *types);

    %rename(__add) add;
	void add(const std::vector<std::string> *types, const std::vector<std::string> * entities);

	%rename(__remove) remove;
	void remove(const std::vector<std::string> *types, const std::vector<std::string> * entities);

    %rename(__addStreams) addStreams;
	void addStreams(const std::vector<DxApi::TickStream *> *streams);

	%rename(__removeStreams) removeStreams;
	void removeStreams(const std::vector<DxApi::TickStream *> *streams);

	%rename(__removeAllStreams) removeAllStreams;
	void removeAllStreams();

    %rename(__setTimeForNewSubscriptions) setTimeForNewSubscriptions;
	void setTimeForNewSubscriptions(DxApi::TimestampMs dt);

}; // TickCursor

%feature("autodoc", "");

}
}


