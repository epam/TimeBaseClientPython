
namespace DxApi {

%feature("autodoc", "The stream is a time series of messages for a number of
financial instruments ('entities'). Messages can be price bars, trade ticks,
bid/offer ticks, or any of the many more built-in and user-defined types.
In the simplest case, a database will have a single stream of data.
Multiple streams can be used to represent data of different frequencies, or completely
different factors. For instance, separate streams can represent
1-minute price bars and ticks for the same set of entities. Or,
you can have price bars and volatility bars in separate streams.

Get stream:
    ```
    stream = tickdb.getStream('stream_key')
    ```

List stream:
    ```
    streams = tickdb.listStreams()
    ```");

class TickStream {
public:

%pythoncode %{
    def key(self) -> str:
        '''Returns the key, which uniquely identifies the stream within its database.'''
        return self.__key()

    def name(self) -> str:
        '''Returns a user-readable short name.'''
        return self.__name()

    def distributionFactor(self) -> int:
        '''Returns the target number of files to be used for storing data.'''
        return self.__distributionFactor()

    def description(self) -> str:
        '''Returns a user-readable multi-line description.'''
        return self.__description()

    def owner(self) -> str:
        '''Returns stream owner.'''
        return self.__owner()

    def location(self) -> str:
        '''Returns stream location.'''
        return self.__location()

    def metadata(self) -> str:
        '''Returns stream schema (in xml format).'''
        return self.__metadata()

    def scope(self) -> StreamScope:
        '''Returns stream schema (in xml format).'''
        return self.__scope()

    def highAvailability(self) -> bool:
        '''Returns stream memory caching parameter. High availability durable streams are cached on startup.'''
        return self.__highAvailability()

    def unique(self) -> bool:
        '''Unique streams maintain in-memory cache of resent messages.
        This concept assumes that stream messages will have some field(s) marked as primary key.
        Primary key may be a simple field (e.g. symbol) or composite (e.g. symbol and portfolio ID).
        For each key TimeBase runtime maintains a copy of the last message received for this key (cache).
        Each new consumer will receive a snapshot of current cache at the beginning of live data subscription.
        '''
        return self.__unique()

    def polymorphic(self) -> bool:
        '''Returns whether the stream is configured as polymorphic.'''
        return self.__polymorphic()

    def periodicity(self) -> str:
        '''Returns Stream periodicity, if known.'''
        return self.__periodicity()

    def options(self) -> StreamOptions:
        '''Returns stream options object.'''
        return self.__options()

    def describe(self) -> str:
        '''Returns stream DDL description.'''
        return self.__describe()

    def setSchema(self, options: StreamOptions) -> bool:
        '''Changes stream schema.

        Args:
            options (StreamOptions): Stream options, that contains new schema xml.

        Returns
            bool: True, if schema was changed successfully.
        '''
        return self.__setSchema(options)

    def listEntities(self) -> 'list[str]':
        '''Return an inclusive range of times for which the specified entities
        have data in the database.

        Returns:
            list[str]: selected entities.
        '''
        return self.__listEntities()

    def truncate(self, timestamp: int, entities: 'list[str]' = None) -> bool:
        '''Truncates stream data for the given entities from given time

        Args:
            timestamp (int): Timestamp in millis. If time less than stream start time, then all stream data will be deleted.
            entities (list[str]): A list of entities. If None, all stream entities will be used.

        Returns:
            bool: true, if stream was truncated successfully.
        '''
        if entities == None:
            return self.__truncate(timestamp)
        else:
            return self.__truncateEntities(timestamp, entities)

    def clear(self, entities: 'list[str]' = None) -> bool:
        '''Clear stream data for the given entities.

        Args:
            entities (list[str]): A list of entities. If None, all stream entities will be used.
        '''
        if entities == None:
            return self.__clear()
        else:
            return self.__clearEntities(entities)

    def purge(self, timestamp: int) -> bool:
        '''Deletes stream data that is older than a specified time

        Args:
            timestamp (int):Purge time in milliseconds.

        Returns:
            bool: true, if stream was purged successfully.
        '''
        return self.__purge(timestamp)

    def deleteStream(self) -> bool:
        '''Deletes this stream

        Returns:
            bool: true, if stream was deleted successfully.
        '''
        return self.__deleteStream()

    def abortBackgroundProcess(self) -> bool:
        '''Aborts active background process if any exists'''
        return self.__abortBackgroundProcess()

    def select(self, timestamp: int, options: SelectionOptions, types: 'list[str]', entities: 'list[str]') -> 'TickCursor':
        '''Opens a cursor for reading data from this stream, according to the
        specified options. The messages
        are returned from the cursor strictly ordered by time. Within the same
        exact time stamp, the order of messages is undefined and may vary from
        call to call, i.e. it is non-deterministic.

        Note that the arguments of this method only determine the initial
        configuration of the cursor. The TickCursor interface provides
        methods for dynamically re-configuring the subscription, or jumping to
        a different timestamp.

        Args:
            timestamp (int): The start timestamp in millis.
            options (SelectionOptions): Selection options.
            types (list[str]): Specified message types to be subscribed. If null, then all types will be subscribed.
            entities (list[str]): Specified entities to be subscribed. If null, then all entities will be subscribed.

        Returns:
            TickCursor: A cursor used to read messages.
        '''
        return self.__select(timestamp, options, types, entities)

    @contextmanager
    def trySelect(self, timestamp: int, options: SelectionOptions, types: 'list[str]', entities: 'list[str]') -> 'TickCursor':
        '''Contextmanager version of select. Usage:
            ```
            with stream.newSelect(timestamp, options, types, entities) as cursor:
                while cursor.next():
                    message = cursor.getMessage()
            ```
        '''
        cursor = None
        try:
            cursor = self.__select(timestamp, options, types, entities)
            yield cursor
        finally:
            cursor.close()

    def createCursor(self, options: SelectionOptions) -> 'TickCursor':
        '''Creates a cursor for reading data from this stream, according to the
        specified options, but initially with a fully restricted filter.
        The user must call TickCursor.reset at least once, in order to
        begin retrieving data. This method is equivalent to (but is
        slightly more optimal than) calling createCursor(options)

        Args:
            options (SelectionOptions): Selection Options.

        Returns:
            A cursor used to read messages. Never null.
        '''
        return self.__createCursor(options)

    @contextmanager
    def tryCursor(self, options: SelectionOptions) -> 'TickCursor':
        '''contextmanager version of createCursor. Usage:
            ```
            with stream.newCursor(options) as cursor:
                while cursor.next():
                    message = cursor.getMessage()
            ```
        '''
        cursor = None
        try:
            cursor = self.__createCursor(options)
            yield cursor
        finally:
            cursor.close()

    def createLoader(self, options: LoadingOptions) -> 'TickLoader':
        '''Creates a channel for loading data. The loader must be closed
        when the loading process is finished.

        Args:
            options (SelectionOptions): Loading Options.

        Returns:
            TickLoader: created loader.
        '''
        return self.__createLoader(options)

    @contextmanager
    def tryLoader(self, options: LoadingOptions) -> 'TickLoader':
        '''Contextmanager version of createLoader. Usage:
            ```
            with stream.newLoader(options) as loader:
                loader.send(message)
            ```
        '''
        loader = None
        try:
            loader = self.__createLoader(options)
            yield loader
        finally:
            loader.close()

    def listSpaces(self) -> 'list[str]':
        '''Returns all created "spaces" for the stream.
        Default space returns as "" (empty string).
        If backing stream does not support spaces None will be returned.
        '''
        return self.__listSpaces()

    def renameSpace(self, newName: str, oldName: str) -> None:
        '''Rename existing space.

        Args:
            nameName (str): space to rename.
            oldName (str): new space name.
        '''
        return self.__renameSpace(newName, oldName)

    def deleteSpaces(self, spaces: 'list[str]') -> None:
        '''Removed given 'spaces' permanently.

        Args:
            spaces (list[str]): list of spaces names to delete.
        '''
        return self.__deleteSpaces(spaces)

    def getTimeRange(self, entities: 'list[str]' = None) -> 'list[int]':
        '''Return an inclusive range of times for which the specified entities
        have data in the database.

        Args:
            entities (list[str]): A list of entities. If empty, return for all.
        '''
        if entities == None:
            return self.__getTimeRange()
        else:
            return self.__getTimeRangeEntities(entities)

    def getSpaceTimeRange(self, space: str) -> 'list[int]':
        '''An array consisting of two long timestamps (from and to) or None if no data was found.

        Args:
            space (str): space name.
        '''
        return self.__getSpaceTimeRange(space)
%}

    %feature("autodoc", "");

    %rename(__key) key;
    const std::string&						key() const;

    %rename(__distributionFactor) distributionFactor;
    int32_t									distributionFactor() const;

    %rename(__name) name;
    const DxApi::Nullable<std::string>&		name() const;

    %rename(__description) description;
    const DxApi::Nullable<std::string>&		description() const;

    %rename(__owner) owner;
    const DxApi::Nullable<std::string>&		owner() const;

    %rename(__location) location;
    const DxApi::Nullable<std::string>&		location() const;

    %rename(__metadata) metadata;
    const DxApi::Nullable<std::string>&		metadata() const;

    %rename(__scope) scope;
    DxApi::StreamScope  					scope() const;

    %rename(__highAvailability) highAvailability;
    bool									highAvailability() const;

    %rename(__unique) unique;
    bool									unique() const;

    %rename(__polymorphic) polymorphic;
    bool									polymorphic() const;

    %rename(__periodicity) periodicity;
    const std::string&						periodicity() const;

    %rename(__options) options;
    const DxApi::StreamOptions&				options() const;

    %rename(__describe) describe;
    const std::string                       describe() const;

    %rename(__setSchema) setSchema;
    bool									setSchema(const DxApi::StreamOptions & options);

	%rename(__listEntities) listEntities;
    std::vector<std::string>				listEntities();

    %rename(__truncate) truncate;
    bool truncate(TimestampMs millisecondTime) const;

    %rename(__truncateEntities) truncate;
	bool truncate(TimestampMs millisecondTime, const std::vector<std::string> * const entities) const;

	%rename(__clear) clear;
	bool clear() const;

	%rename(__clearEntities) clear;
    bool clear(const std::vector<std::string> * const entities) const;

    %rename(__purge) purge;
    bool purge(TimestampMs millisecondTime) const;

    %rename(__deleteStream) deleteStream;
    bool deleteStream();

    %rename(__abortBackgroundProcess) abortBackgroundProcess;
    bool abortBackgroundProcess() const;

    %rename(__select) select;
	DxApi::TickCursor * select(DxApi::TimestampMs millisecondTime, const DxApi::SelectionOptions &options, 
		const std::vector<std::string> * types, const std::vector<std::string> * entities) const;

    %rename(__createCursor) createCursor;
    DxApi::TickCursor * createCursor(const DxApi::SelectionOptions &options) const;

    %rename(__createLoader) createLoader;
    DxApi::TickLoader * createLoader(const DxApi::LoadingOptions &options) const;

    %rename(__listSpaces) listSpaces;
	std::vector<std::string> listSpaces() const;

	%rename(__renameSpace) renameSpace;
    void renameSpace(const std::string &newName, const std::string &oldName) const;

    %rename(__deleteSpaces) deleteSpaces;
    void deleteSpaces(const std::vector<std::string>& spaces) const;

    static void operator delete(void* ptr, size_t sz); //not supported by swig, will be skipped

    ~TickStream();

protected:
    TickStream();
    TickStream(const TickStream&);
    void operator=(const TickStream&);
}; // class TickStream

%extend TickStream {

	%rename(__getTimeRange) getTimeRange;
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

    %rename(__getTimeRangeEntities) getTimeRange;
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

    %rename(__getSpaceTimeRange) getTimeRange;
	std::vector<long long> getTimeRange(const std::string &space) {
		std::vector<long long> result(2);

		//todo: refactor it
		// used array for clang linux copiler
		TimestampMs range[2];
		bool not_null = $self->getTimeRange(range, space);

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

%feature("autodoc", "");

} // namespace DxApi
