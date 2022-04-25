
namespace DxApi {

%newobject TickDb::createFromUrl(const char *);
%newobject TickDb::createFromUrl(const char *, const char *, const char *);

%feature("autodoc", "The top-level implementation to the methods of the Deltix Tick
Database engine. Instances of this class are created by static method
createFromUrl:

```
db = tbapi.TickDb.createFromUrl('dxtick://localhost:8011')
```

or

```
db = tbapi.TickDb.createFromUrl('dxtick://localhost:8011', 'user', 'password')
```");

class TickDb {
public:
%pythoncode %{

    @staticmethod
    def createFromUrl(url: str, user: str = None, password: str = None) -> "TickDb":
        '''Creates a new database instance with the specified root folder, or URL.

        Args:
            url (str): Connection URL.
            user (str): User.
            password (str): Password.

        Returns:
            TickDb: An un-opened TickDB instance.
        '''
        if user == None:
            return _tbapi.TickDb___createFromUrl(url)
        else:
            return _tbapi.TickDb___createFromUrlWithUser(url, user, password)

    @staticmethod
    @contextmanager
    def openFromUrl(url: str, readonly: bool, user: str = None, password: str = None):
        '''Creates a new database instance with the specified root folder, or URL, and opens it.

        Args:
            url (str): Connection URL.
            readonly (bool): Open data store in read-only mode.
            user (str): User.
            password (str): Password.

        Returns:
            TickDb: An opened TickDB instance.
        '''
        db = TickDb.createFromUrl(url, user, password)
        try:
            db.open(readonly)
            yield db
        finally:
            if db.isOpen():
                db.close()

    def isReadOnly(self) -> bool:
        '''Determines whether the store is open as read-only.'''
        return self.__isReadOnly()

    def isOpen(self) -> bool:
        '''Determines whether the store is open.'''
        return self.__isOpen()

    def open(self, readOnlyMode: bool) -> bool:
        '''Open the data store.

        Args:
            readOnlyMode (bool): Open data store in read-only mode.
        '''
        return self.__open(readOnlyMode)

    def close(self) -> None:
        '''Closes data store.'''
        return self.__close()

    def format(self) -> bool:
        '''Create a new object on disk and format internally.
        The data store is left open for read-write at the end of this method.
        '''
        return self.__format()

    def listStreams(self) -> 'list[TickStream]':
        '''Enumerates existing streams.

        Returns:
            list[TickStream]: An array of existing stream objects.
        '''
        return self.__listStreams()

    def getStream(self, key: str) -> 'TickStream':
        '''Looks up an existing stream by key.

        Args:
            key (str): Identifies the stream.

        Returns:
            TickStream: A stream object, or None if the key was not found.
        '''
        return self.__getStream(key)

    def createStream(self, key: str, options: StreamOptions) -> 'TickStream':
        '''Creates a new stream within the database.

        Args:
            key (str): A required key later used to identify the stream.
            options (StreamOptions): Options for creating the stream.

        Returns:
            TickStream: A new instance of TickStream.
        '''
        return self.__createStream(key, options)

    def createFileStream(self, key: str, dataFile: str) -> 'TickStream':
        '''Creates a new stream mount to the given data file.

        Args:
            key (str): A required key later used to identify the stream.
            dataFile (str): Path to the data file (on server side).

        Returns:
            TickStream: A new instance of TickStream.
        '''
        return self.__createFileStream(key)

    def createCursor(self, stream: 'TickStream', options: SelectionOptions) -> 'TickCursor':
        '''Opens an initially empty cursor for reading data from multiple streams,
        according to the specified options. The messages
        are returned from the cursor strictly ordered by time. Within the same
        exact timestamp, the order of messages is undefined and may vary from
        call to call, i.e. it is non-deterministic.

        The cursor is returned initially empty and must be reset.
        The TickCursor class provides
        methods for dynamically re-configuring the subscription, or jumping to
        a different timestamp.

        Args:
            stream (TickStream): Stream from which data will be selected.
            options (SelectionOptions): Selection options.

        Returns:
            TickCursor: A cursor used to read messages.
        '''
        return self.__createCursor(stream, options)

    @contextmanager
    def tryCursor(self, stream: 'TickStream', options: SelectionOptions) -> 'TickCursor':
        '''contextmanager version of createCursor. Usage:
            ```
            with db.newCursor(stream, options) as cursor:
                while cursor.next():
                    message = cursor.getMessage()
            ```
        '''
        cursor = None
        try:
            cursor = self.__createCursor(stream, options)
            yield cursor
        finally:
            cursor.close()

    def select(self, timestamp: int, streams: 'list[TickStream]', options: SelectionOptions, types: 'list[str]', entities: 'list[str]') -> 'TickCursor':
        '''Opens a cursor for reading data from multiple streams,
        according to the specified options. The messages
        are returned from the cursor strictly ordered by time. Within the same
        exact time stamp, the order of messages is undefined and may vary from
        call to call, i.e. it is non-deterministic.

        Note that the arguments of this method only determine the initial
        configuration of the cursor. The TickCursor clsas provides
        methods for dynamically re-configuring the subscription, or jumping to
        a different timestamp.

        Args:
            timestamp (int): The start timestamp in millis.
            streams (list[TickStream]): Streams from which data will be selected.
            options (SelectionOptions): Selection options.
            types (list[str]): Specified message types to be subscribed. If null, then all types will be subscribed.
            entities (list[str]): Specified entities to be subscribed. If null, then all entities will be subscribed.

        Returns:
            TickCursor: A cursor used to read messages.
        '''
        return self.__select(timestamp, streams, options, types, entities)

    @contextmanager
    def trySelect(self, timestamp: int, streams: 'list[TickStream]', options: SelectionOptions, types: 'list[str]', entities: 'list[str]') -> 'TickCursor':
        '''Contextmanager version of select. Usage:
            ```
            with db.newSelect(timestamp, streams, options, types, entities) as cursor:
                while cursor.next():
                    message = cursor.getMessage()
            ```
        '''
        cursor = None
        try:
            cursor = self.__select(timestamp, streams, options, types, entities)
            yield cursor
        finally:
            cursor.close()

    def createLoader(self, stream: 'TickStream', options: LoadingOptions) -> 'TickLoader':
        '''Creates a channel for loading data. The loader must be closed
        when the loading process is finished.

        Args:
            stream (TickStream): stream for loading data.
            options (SelectionOptions): Loading Options.

        Returns:
            TickLoader: created loader.
        '''
        return self.__createLoader(stream, options)

    @contextmanager
    def tryLoader(self, stream: 'TickStream', options: LoadingOptions) -> 'TickLoader':
        '''Contextmanager version of createLoader. Usage:

            with db.newLoader(stream, options) as loader:
                loader.send(message)
        '''
        loader = None
        try:
            loader = self.__createLoader(stream, options)
            yield loader
        finally:
            loader.close()

    def executeQuery(self, query: str, options: SelectionOptions = None, timestamp: int = JAVA_LONG_MIN_VALUE, entities: 'list[str]' = None, params: 'list[QueryParameter]' = []) -> 'TickCursor':
        '''Execute Query and creates a message source for reading data from it,
        according to the specified options. The messages
        are returned from the cursor strictly ordered by time. Within the same
        exact time stamp, the order of messages is undefined and may vary from
        call to call, i.e. it is non-deterministic.

        Args:
            query (str): Query text element.
            options (SelectionOptions): Selection options.
            timestamp (int): The start timestamp in millis.
            entities (list[str]): Specified entities to be subscribed.
                If null, then all entities will be subscribed.
            params (list[QueryParameter]): The parameter values of the query.

        Returns:
            TickCursor: An iterable message source to read messages.
        '''
        if options == None:
            return self.__executeQuery(query)
        else:
            return self.__executeQueryFull(query, options, timestamp, entities, params);

    @contextmanager
    def tryExecuteQuery(self, query: str, options: SelectionOptions = None, timestamp: int = JAVA_LONG_MIN_VALUE, entities: 'list[str]' = None, params: 'list[QueryParameter]' = []) -> 'TickCursor':
        '''Contextmanager version of executeQuery. Usage:
            ```
            with db.newExecuteQuery('select * from stream') as cursor:
                while cursor.next():
                    message = cursor.getMessage()
            ```
        '''
        cursor = None
        try:
            if options == None:
                cursor = self.__executeQuery(query)
            else:
                cursor = self.__executeQueryFull(query, options, timestamp, entities, params);
            yield cursor
        finally:
            cursor.close()
%}

    %feature("autodoc", "");

    %rename(__createFromUrl) createFromUrl;
    static TickDb *		createFromUrl(const char * url);

    %rename(__createFromUrlWithUser) createFromUrl;
    static TickDb *		createFromUrl(const char * url, const char * username, const char * password);

    %rename(__isReadOnly) isReadOnly;
    bool				isReadOnly() const;

    %rename(__isOpen) isOpen;
    bool				isOpen() const;

    %rename(__open) open;
    bool				open(bool readOnlyMode);

    %rename(__close) close;
    void				close();

    %rename(__format) format;
    bool				format();

    %rename(__listStreams) listStreams;
    std::vector<DxApi::TickStream *> listStreams();

    %rename(__getStream) getStream;
    DxApi::TickStream * getStream(const std::string &key);

    %rename(__createStream) createStream;
    DxApi::TickStream * createStream(const std::string &key, const DxApi::StreamOptions &options);

    %rename(__createFileStream) createFileStream;
    DxApi::TickStream * createFileStream(const std::string &key, const std::string &dataFile);

    %rename(__createCursor) createCursor;
    DxApi::TickCursor * createCursor(const DxApi::TickStream * stream, const DxApi::SelectionOptions &options);

    %rename(__select) select;
    DxApi::TickCursor * select(
		DxApi::TimestampMs time, 
		const std::vector<DxApi::TickStream *> *streams, 
		const DxApi::SelectionOptions &options,
        const std::vector<std::string> *types, 
		const std::vector<std::string> *entities);

    %rename(__createLoader) createLoader;
    DxApi::TickLoader * createLoader(const DxApi::TickStream * stream, const DxApi::LoadingOptions &options);

    %rename(__executeQueryFull) executeQuery;
	DxApi::TickCursor* executeQuery(
		const std::string &qql, 
		const DxApi::SelectionOptions &options, 
		DxApi::TimestampMs time,
        const std::vector<std::string> *instruments, 
		const std::vector<DxApi::QueryParameter> &params);

    ~TickDb();

protected:
    TickDb();
	TickDb(const TickDb&);
    void operator=(const TickDb&);

}; // class TickDb

%extend TickDb {

    %rename(__executeQuery) executeQuery;
	DxApi::TickCursor* executeQuery(const std::string &qql) {
		return $self->executeQuery(qql, std::vector<DxApi::QueryParameter>());
	}

}

%feature("autodoc", "");

} // namespace DxApi
