# tbapi

## StreamScope

```python
class StreamScope(_object)
```

> Determines the scope of a stream's durability, if any.
> 
> **Example**:
> 
>   ```
>   scope = tbapi.StreamScope('TRANSIENT')
>   ```
>   
>   Possible values:
>   ```
>   DURABLE,
>   EXTERNAL_FILE,
>   TRANSIENT,
>   RUNTIME
>   ```

## WriteMode

```python
class WriteMode(_object)
```

> APPEND: Adds only new data into a stream without truncations.
> REPLACE: Adds data into a stream and removes previous data older that first message time
> [truncate(first message time + 1)].
> REWRITE: Default. Adds data into a stream and removes previous data by truncating using first message time.
> [truncate(first message time)].
> TRUNCATE: Stream truncated every time when loader writes a messages earlier than last message time.
> 
> **Example**:
> 
>   ```
>   mode = tbapi.StreamScope('TRUNCATE')
>   ```
>   
>   Possible values:
>   ```
>   APPEND,
>   REPLACE,
>   REWRITE,
>   TRUNCATE
>   ```

## SelectionOptions

```python
class SelectionOptions(_object)
```

> Options for selecting data from a stream.
> 
> **Example**:
> 
>   ```
>   so = tbapi.SelectionOptions()
>   so._from = 0
>   so.to = 100000
>   so.useCompression = False
>   ...
>   ```

## LoadingOptions

```python
class LoadingOptions(_object)
```

> Options for loading data into a stream.
> 
> **Example**:
> 
>   ```
>   lo = tbapi.LoadingOptions()
>   lo.writeMode = tbapi.WriteMode('TRUNCATE')
>   so.space = 'myspace'
>   ...
>   ```

## StreamOptions

```python
class StreamOptions(_object)
```

> Stream definition attributes.
> 
> **Example**:
> 
>   ```
>   so = tbapi.StreamOptions()
>   so.name = key
>   so.description = key
>   so.scope = tbapi.StreamScope('DURABLE')
>   so.distributionFactor = 1
>   so.highAvailability = False
>   so.polymorphic = False
>   so.metadata = schema
>   
>   db.createStream(key, options)
>   ```

#### name

```python
def name(name: str = None) -> None
```

> Optional user-readable name.

#### description

```python
def description(description: str = None) -> None
```

> Optional multi-line description.

#### owner

```python
def owner(owner: str = None) -> None
```

> Optional owner of stream.
> During stream creation it will be set
> equals to authenticated user name.

#### location

```python
def location(location: str = None) -> None
```

> Location of the stream (by default null). When defined this attribute provides alternative stream location (rather than default location under QuantServerHome)

#### distributionRuleName

```python
def distributionRuleName(distributionRuleName: str = None) -> None
```

> Class name of the distribution rule

#### metadata

```python
def metadata(metadata: str = None) -> None
```

> Stream metadata in XML format. To build metadata programatically, use tbapi.SchemaDef class.

## QueryParameter

```python
class QueryParameter(_object)
```

> Input parameter definition for a prepared statement.

## TickDb

```python
class TickDb(_object)
```

> The top-level implementation to the methods of the Deltix Tick
> Database engine. Instances of this class are created by static method
> createFromUrl:
> 
> ```
> db = tbapi.TickDb.createFromUrl('dxtick://localhost:8011')
> ```
> 
> or
> 
> ```
> db = tbapi.TickDb.createFromUrl('dxtick://localhost:8011', 'user', 'password')
> ```

#### createFromUrl

```python
@staticmethod
def createFromUrl(url: str, user: str = None, password: str = None) -> "TickDb"
```

> Creates a new database instance with the specified root folder, or URL.
> 
> **Arguments**:
> 
> - `url` _str_ - Connection URL.
> - `user` _str_ - User.
> - `password` _str_ - Password.
>   
> 
> **Returns**:
> 
> - `TickDb` - An un-opened TickDB instance.

#### openFromUrl

```python
@staticmethod
@contextmanager
def openFromUrl(url: str, readonly: bool, user: str = None, password: str = None)
```

> Creates a new database instance with the specified root folder, or URL, and opens it.
> 
> **Arguments**:
> 
> - `url` _str_ - Connection URL.
> - `readonly` _bool_ - Open data store in read-only mode.
> - `user` _str_ - User.
> - `password` _str_ - Password.
>   
> 
> **Returns**:
> 
> - `TickDb` - An opened TickDB instance.

#### isReadOnly

```python
def isReadOnly() -> bool
```

> Determines whether the store is open as read-only.

#### isOpen

```python
def isOpen() -> bool
```

> Determines whether the store is open.

#### open

```python
def open(readOnlyMode: bool) -> bool
```

> Open the data store.
> 
> **Arguments**:
> 
> - `readOnlyMode` _bool_ - Open data store in read-only mode.

#### close

```python
def close() -> None
```

> Closes data store.

#### format

```python
def format() -> bool
```

> Create a new object on disk and format internally.
> The data store is left open for read-write at the end of this method.

#### listStreams

```python
def listStreams() -> 'list[TickStream]'
```

> Enumerates existing streams.
> 
> **Returns**:
> 
> - `list[TickStream]` - An array of existing stream objects.

#### getStream

```python
def getStream(key: str) -> 'TickStream'
```

> Looks up an existing stream by key.
> 
> **Arguments**:
> 
> - `key` _str_ - Identifies the stream.
>   
> 
> **Returns**:
> 
> - `TickStream` - A stream object, or None if the key was not found.

#### createStream

```python
def createStream(key: str, options: StreamOptions) -> 'TickStream'
```

> Creates a new stream within the database.
> 
> **Arguments**:
> 
> - `key` _str_ - A required key later used to identify the stream.
> - `options` _StreamOptions_ - Options for creating the stream.
>   
> 
> **Returns**:
> 
> - `TickStream` - A new instance of TickStream.

#### createFileStream

```python
def createFileStream(key: str, dataFile: str) -> 'TickStream'
```

> Creates a new stream mount to the given data file.
> 
> **Arguments**:
> 
> - `key` _str_ - A required key later used to identify the stream.
> - `dataFile` _str_ - Path to the data file (on server side).
>   
> 
> **Returns**:
> 
> - `TickStream` - A new instance of TickStream.

#### createCursor

```python
def createCursor(stream: 'TickStream', options: SelectionOptions) -> 'TickCursor'
```

> Opens an initially empty cursor for reading data from multiple streams,
> according to the specified options. The messages
> are returned from the cursor strictly ordered by time. Within the same
> exact timestamp, the order of messages is undefined and may vary from
> call to call, i.e. it is non-deterministic.
> 
> The cursor is returned initially empty and must be reset.
> The TickCursor class provides
> methods for dynamically re-configuring the subscription, or jumping to
> a different timestamp.
> 
> **Arguments**:
> 
> - `stream` _TickStream_ - Stream from which data will be selected.
> - `options` _SelectionOptions_ - Selection options.
>   
> 
> **Returns**:
> 
> - `TickCursor` - A cursor used to read messages.

#### tryCursor

```python
@contextmanager
def tryCursor(stream: 'TickStream', options: SelectionOptions) -> 'TickCursor'
```

> contextmanager version of createCursor. Usage:
> ```
> with db.newCursor(stream, options) as cursor:
>     while cursor.next():
>         message = cursor.getMessage()
> ```

#### select

```python
def select(timestamp: int, streams: 'list[TickStream]', options: SelectionOptions, types: 'list[str]', entities: 'list[str]') -> 'TickCursor'
```

> Opens a cursor for reading data from multiple streams,
> according to the specified options. The messages
> are returned from the cursor strictly ordered by time. Within the same
> exact time stamp, the order of messages is undefined and may vary from
> call to call, i.e. it is non-deterministic.
> 
> Note that the arguments of this method only determine the initial
> configuration of the cursor. The TickCursor clsas provides
> methods for dynamically re-configuring the subscription, or jumping to
> a different timestamp.
> 
> **Arguments**:
> 
> - `timestamp` _int_ - The start timestamp in millis.
> - `streams` _list[TickStream]_ - Streams from which data will be selected.
> - `options` _SelectionOptions_ - Selection options.
> - `types` _list[str]_ - Specified message types to be subscribed. If null, then all types will be subscribed.
> - `entities` _list[str]_ - Specified entities to be subscribed. If null, then all entities will be subscribed.
>   
> 
> **Returns**:
> 
> - `TickCursor` - A cursor used to read messages.

#### trySelect

```python
@contextmanager
def trySelect(timestamp: int, streams: 'list[TickStream]', options: SelectionOptions, types: 'list[str]', entities: 'list[str]') -> 'TickCursor'
```

> Contextmanager version of select. Usage:
> ```
> with db.newSelect(timestamp, streams, options, types, entities) as cursor:
>     while cursor.next():
>         message = cursor.getMessage()
> ```

#### createLoader

```python
def createLoader(stream: 'TickStream', options: LoadingOptions) -> 'TickLoader'
```

> Creates a channel for loading data. The loader must be closed
> when the loading process is finished.
> 
> **Arguments**:
> 
> - `stream` _TickStream_ - stream for loading data.
> - `options` _SelectionOptions_ - Loading Options.
>   
> 
> **Returns**:
> 
> - `TickLoader` - created loader.

#### tryLoader

```python
@contextmanager
def tryLoader(stream: 'TickStream', options: LoadingOptions) -> 'TickLoader'
```

> Contextmanager version of createLoader. Usage:
> 
> with db.newLoader(stream, options) as loader:
>     loader.send(message)

#### executeQuery

```python
def executeQuery(query: str, options: SelectionOptions = None, timestamp: int = JAVA_LONG_MIN_VALUE, entities: 'list[str]' = None, params: 'list[QueryParameter]' = []) -> 'TickCursor'
```

> Execute Query and creates a message source for reading data from it,
> according to the specified options. The messages
> are returned from the cursor strictly ordered by time. Within the same
> exact time stamp, the order of messages is undefined and may vary from
> call to call, i.e. it is non-deterministic.
> 
> **Arguments**:
> 
> - `query` _str_ - Query text element.
> - `options` _SelectionOptions_ - Selection options.
> - `timestamp` _int_ - The start timestamp in millis.
> - `entities` _list[str]_ - Specified entities to be subscribed.
>   If null, then all entities will be subscribed.
> - `params` _list[QueryParameter]_ - The parameter values of the query.
>   
> 
> **Returns**:
> 
> - `TickCursor` - An iterable message source to read messages.

#### tryExecuteQuery

```python
@contextmanager
def tryExecuteQuery(query: str, options: SelectionOptions = None, timestamp: int = JAVA_LONG_MIN_VALUE, entities: 'list[str]' = None, params: 'list[QueryParameter]' = []) -> 'TickCursor'
```

> Contextmanager version of executeQuery. Usage:
> ```
> with db.newExecuteQuery('select * from stream') as cursor:
>     while cursor.next():
>         message = cursor.getMessage()
> ```

## TickStream

```python
class TickStream(_object)
```

> The stream is a time series of messages for a number of
> financial instruments ('entities'). Messages can be price bars, trade ticks,
> bid/offer ticks, or any of the many more built-in and user-defined types.
> In the simplest case, a database will have a single stream of data.
> Multiple streams can be used to represent data of different frequencies, or completely
> different factors. For instance, separate streams can represent
> 1-minute price bars and ticks for the same set of entities. Or,
> you can have price bars and volatility bars in separate streams.
> 
> Get stream:
>     ```
>     stream = tickdb.getStream('stream_key')
>     ```
> 
> List stream:
>     ```
>     streams = tickdb.listStreams()
>     ```

#### key

```python
def key() -> str
```

> Returns the key, which uniquely identifies the stream within its database.

#### name

```python
def name() -> str
```

> Returns a user-readable short name.

#### distributionFactor

```python
def distributionFactor() -> int
```

> Returns the target number of files to be used for storing data.

#### description

```python
def description() -> str
```

> Returns a user-readable multi-line description.

#### owner

```python
def owner() -> str
```

> Returns stream owner.

#### location

```python
def location() -> str
```

> Returns stream location.

#### metadata

```python
def metadata() -> str
```

> Returns stream schema (in xml format).

#### scope

```python
def scope() -> StreamScope
```

> Returns stream schema (in xml format).

#### highAvailability

```python
def highAvailability() -> bool
```

> Returns stream memory caching parameter. High availability durable streams are cached on startup.

#### unique

```python
def unique() -> bool
```

> Unique streams maintain in-memory cache of resent messages.
> This concept assumes that stream messages will have some field(s) marked as primary key.
> Primary key may be a simple field (e.g. symbol) or composite (e.g. symbol and portfolio ID).
> For each key TimeBase runtime maintains a copy of the last message received for this key (cache).
> Each new consumer will receive a snapshot of current cache at the beginning of live data subscription.

#### polymorphic

```python
def polymorphic() -> bool
```

> Returns whether the stream is configured as polymorphic.

#### periodicity

```python
def periodicity() -> str
```

> Returns Stream periodicity, if known.

#### options

```python
def options() -> StreamOptions
```

> Returns stream options object.

#### describe

```python
def describe() -> str
```

> Returns stream DDL description.

#### setSchema

```python
def setSchema(options: StreamOptions) -> bool
```

> Changes stream schema.
> 
> **Arguments**:
> 
> - `options` _StreamOptions_ - Stream options, that contains new schema xml.
>   
>   Returns
> - `bool` - True, if schema was changed successfully.

#### listEntities

```python
def listEntities() -> 'list[str]'
```

> Return an inclusive range of times for which the specified entities
> have data in the database.
> 
> **Returns**:
> 
> - `list[str]` - selected entities.

#### truncate

```python
def truncate(timestamp: int, entities: 'list[str]' = None) -> bool
```

> Truncates stream data for the given entities from given time
> 
> **Arguments**:
> 
> - `timestamp` _int_ - Timestamp in millis. If time less than stream start time, then all stream data will be deleted.
> - `entities` _list[str]_ - A list of entities. If None, all stream entities will be used.
>   
> 
> **Returns**:
> 
> - `bool` - true, if stream was truncated successfully.

#### clear

```python
def clear(entities: 'list[str]' = None) -> bool
```

> Clear stream data for the given entities.
> 
> **Arguments**:
> 
> - `entities` _list[str]_ - A list of entities. If None, all stream entities will be used.

#### purge

```python
def purge(timestamp: int) -> bool
```

> Deletes stream data that is older than a specified time
> 
> **Arguments**:
> 
>   timestamp (int):Purge time in milliseconds.
>   
> 
> **Returns**:
> 
> - `bool` - true, if stream was purged successfully.

#### deleteStream

```python
def deleteStream() -> bool
```

> Deletes this stream
> 
> **Returns**:
> 
> - `bool` - true, if stream was deleted successfully.

#### abortBackgroundProcess

```python
def abortBackgroundProcess() -> bool
```

> Aborts active background process if any exists

#### select

```python
def select(timestamp: int, options: SelectionOptions, types: 'list[str]', entities: 'list[str]') -> 'TickCursor'
```

> Opens a cursor for reading data from this stream, according to the
> specified options. The messages
> are returned from the cursor strictly ordered by time. Within the same
> exact time stamp, the order of messages is undefined and may vary from
> call to call, i.e. it is non-deterministic.
> 
> Note that the arguments of this method only determine the initial
> configuration of the cursor. The TickCursor interface provides
> methods for dynamically re-configuring the subscription, or jumping to
> a different timestamp.
> 
> **Arguments**:
> 
> - `timestamp` _int_ - The start timestamp in millis.
> - `options` _SelectionOptions_ - Selection options.
> - `types` _list[str]_ - Specified message types to be subscribed. If null, then all types will be subscribed.
> - `entities` _list[str]_ - Specified entities to be subscribed. If null, then all entities will be subscribed.
>   
> 
> **Returns**:
> 
> - `TickCursor` - A cursor used to read messages.

#### trySelect

```python
@contextmanager
def trySelect(timestamp: int, options: SelectionOptions, types: 'list[str]', entities: 'list[str]') -> 'TickCursor'
```

> Contextmanager version of select. Usage:
> ```
> with stream.newSelect(timestamp, options, types, entities) as cursor:
>     while cursor.next():
>         message = cursor.getMessage()
> ```

#### createCursor

```python
def createCursor(options: SelectionOptions) -> 'TickCursor'
```

> Creates a cursor for reading data from this stream, according to the
> specified options, but initially with a fully restricted filter.
> The user must call TickCursor.reset at least once, in order to
> begin retrieving data. This method is equivalent to (but is
> slightly more optimal than) calling createCursor(options)
> 
> **Arguments**:
> 
> - `options` _SelectionOptions_ - Selection Options.
>   
> 
> **Returns**:
> 
>   A cursor used to read messages. Never null.

#### tryCursor

```python
@contextmanager
def tryCursor(options: SelectionOptions) -> 'TickCursor'
```

> contextmanager version of createCursor. Usage:
> ```
> with stream.newCursor(options) as cursor:
>     while cursor.next():
>         message = cursor.getMessage()
> ```

#### createLoader

```python
def createLoader(options: LoadingOptions) -> 'TickLoader'
```

> Creates a channel for loading data. The loader must be closed
> when the loading process is finished.
> 
> **Arguments**:
> 
> - `options` _SelectionOptions_ - Loading Options.
>   
> 
> **Returns**:
> 
> - `TickLoader` - created loader.

#### tryLoader

```python
@contextmanager
def tryLoader(options: LoadingOptions) -> 'TickLoader'
```

> Contextmanager version of createLoader. Usage:
> ```
> with stream.newLoader(options) as loader:
>     loader.send(message)
> ```

#### listSpaces

```python
def listSpaces() -> 'list[str]'
```

> Returns all created "spaces" for the stream.
> Default space returns as "" (empty string).
> If backing stream does not support spaces None will be returned.

#### renameSpace

```python
def renameSpace(newName: str, oldName: str) -> None
```

> Rename existing space.
> 
> **Arguments**:
> 
> - `nameName` _str_ - space to rename.
> - `oldName` _str_ - new space name.

#### deleteSpaces

```python
def deleteSpaces(spaces: 'list[str]') -> None
```

> Removed given 'spaces' permanently.
> 
> **Arguments**:
> 
> - `spaces` _list[str]_ - list of spaces names to delete.

#### getTimeRange

```python
def getTimeRange(entities: 'list[str]' = None) -> 'list[int]'
```

> Return an inclusive range of times for which the specified entities
> have data in the database.
> 
> **Arguments**:
> 
> - `entities` _list[str]_ - A list of entities. If empty, return for all.

#### getSpaceTimeRange

```python
def getSpaceTimeRange(space: str) -> 'list[int]'
```

> An array consisting of two long timestamps (from and to) or None if no data was found.
> 
> **Arguments**:
> 
> - `space` _str_ - space name.

## TickCursor

```python
class TickCursor(_object)
```

> A cursor (also known as iterator, or result set) for reading data from a
> stream. This class provides methods for dynamically reconfiguring the feed,
> as well as method reset for essentially re-opening the cursor on a completely different timestamp.
> 
> To get a cursor, use select method from TickDb or TickStream objects,
> or call executeQuery to open cursor to QQL result set.
> 
> Also cursor can be created with createCursor method, but it will be not initialized cursor,
> so cursor should be configured with types, entities and read time calling reset:
>     ```
>     options = tbapi.SelectionOptions()
>     cursor = tickdb.createCursor(stream, options)
>     cursor.subscribeToAllEntities()
>     cursor.subscribeToAllTypes()
>     cursor.reset(timestamp)
>     ```

#### next

```python
def next() -> bool
```

> Moves cursor on to the next message. This method blocks until the next message becomes available,
> or until the cursor is determined to be at the end of the sequence.
> This method is illegal to call if isAtEnd() returns true.
> 
> **Returns**:
> 
> - `bool` - false if at the end of the cursor.

#### getMessage

```python
def getMessage() -> 'InstrumentMessage'
```

> Returns an InstrumentMessage object cursor points at.

#### isAtEnd

```python
def isAtEnd() -> bool
```

> Returns true if the last call to next() returned false. Returns false if next() has not been called yet.
> This method is legal to call any number of times at any point in the cursor's lifecycle.

#### nextIfAvailable

```python
def nextIfAvailable() -> int
```

> Moves cursor on to the next message, but this method NOT blocks until the next message becomes available.
> 
> **Returns**:
> 
> - `NextResult` - OK (0) if new message is available,
>   END_OF_CURSOR (1) if cursor was closed,
>   otherwise, UNAVAILABLE (2)

#### isClosed

```python
def isClosed() -> bool
```

> Returns true, if cursor was closed

#### close

```python
def close() -> None
```

> Close the cursor

#### getCurrentStreamKey

```python
def getCurrentStreamKey() -> str
```

> Return the key of the stream that is the source of the current message.

#### reset

```python
def reset(timestamp: int, entities: 'list[str]' = None) -> None
```

> Reposition the message source to a new point in time, while
> preserving current subscription.
> 
> **Arguments**:
> 
> - `timestamp` _int_ - The new position in time in millis.
> - `entities` _'list[str]'_ - list of entities to reset

#### subscribeToAllEntities

```python
def subscribeToAllEntities() -> None
```

> Subscribe to all available entities.

#### clearAllEntities

```python
def clearAllEntities() -> None
```

> Switch to selective subscription mode (if necessary) and clear the list.

#### addEntity

```python
def addEntity(entity: str) -> None
```

> Add the specified entity to subscription. The type and symbol are copied
> from the incoming object, if necessary, so the argument can be re-used
> after the call.
> 
> Special note about options:
> The following fragment will subscribe to specific option contract "DAV   100417P00085000":
>     cursor.addEntity('DAV   100417P00085000');
> 
> While the following will subscribe to option root (and you will get all instruments with this root):
>     cursor.addEntity("DAV   ");

#### addEntities

```python
def addEntities(entities: 'list[str]') -> None
```

> Bulk add the specified entities to subscription. The type and symbol are copied
> from the incoming objects, if necessary, so the arguments can be re-used
> after the call.

#### removeEntities

```python
def removeEntities(entities: 'list[str]') -> None
```

> Remove the specified entities from subscription. The type and symbol are copied
> from the incoming objects, if necessary, so the arguments can be re-used
> after the call.

#### removeEntity

```python
def removeEntity(entity: str) -> None
```

> Remove the specified entity from subscription. The type and symbol are copied
> from the incoming object, if necessary, so the argument can be re-used
> after the call.

#### subscribeToAllTypes

```python
def subscribeToAllTypes() -> None
```

> Subscribe to all available types (no filtering).

#### addTypes

```python
def addTypes(types: 'list[str]') -> None
```

> Add the specified type names to subscription.

#### removeTypes

```python
def removeTypes(types: 'list[str]') -> None
```

> Remove the specified types from subscription.

#### setTypes

```python
def setTypes(types: 'list[str]') -> None
```

> Subscribe to specified types.

#### add

```python
def add(types: 'list[str]', entities: 'list[str]') -> None
```

> Add the specified entities and types to subscription. The type and symbol are copied
> from the incoming object, if necessary, so the argument can be re-used
> after the call.
> 
> **Arguments**:
> 
> - `types` _list[str]_ - not-null array of type names to subscribe.
> - `entities` _list[str]_ - not-null array of instruments to subscribe.

#### remove

```python
def remove(types: 'list[str]', entities: 'list[str]') -> None
```

> Remove the specified entities and types from subscription. The type and symbol are copied
> from the incoming objects, if necessary, so the arguments can be re-used
> after the call.
> 
> **Arguments**:
> 
> - `types` _list[str]_ - not-null array of type names to unsubscribe.
> - `entities` _list[str]_ - not-null array of instruments to unsubscribe.

#### addStreams

```python
def addStreams(streams: 'list[TickStream]') -> None
```

> Add streams to subscription. Current time and filter is used to query
> data from new sources.
> 
> **Arguments**:
> 
> - `streams` _'list[TickStream]'_ - Streams to add.

#### removeStreams

```python
def removeStreams(streams: 'list[TickStream]') -> None
```

> Remove streams from subscription.
> 
> **Arguments**:
> 
> - `streams` _list[TickStream]_ - Streams to remove.

#### removeAllStreams

```python
def removeAllStreams() -> None
```

> Remove all streams from subscription.

#### setTimeForNewSubscriptions

```python
def setTimeForNewSubscriptions(timestamp: int) -> None
```

> This method affects subsequent "add subscription" methods,
> such as, for instance, addEntity(). New subscriptions start at
> the specified time.
> 
> **Arguments**:
> 
> - `timestamp` _int_ - The time to use.

## TickLoader

```python
class TickLoader(_object)
```

> Object which consumes messages.
> 
> Create loader from TickDb:
>     options = tbapi.LoadingOptions()
>     stream = tickdb.createLoader(stream, options)
> 
> Create loader from TickStream:
>     options = tbapi.LoadingOptions()
>     stream = stream.createLoader(options)

#### send

```python
def send(message: InstrumentMessage) -> None
```

> This method is invoked to send a message to the object.
> 
> **Arguments**:
> 
> - `message` _InstrumentMessage_ - A temporary buffer with the message.
>   By convention, the message is only valid for the duration of this call.

#### flush

```python
def flush() -> None
```

> Flushes all buffered messages by sending them to server.
> Note that calling 'send' method not guaranty that all messages will be delivered and stored to server.

#### close

```python
def close() -> None
```

> Flushes and closes the loader

#### addListener

```python
def addListener(listener: 'ErrorListener') -> None
```

> Register error listener. All writing data errors will be delivered to the listener.
> 
> **Arguments**:
> 
> - `listener` _ErrorListener_ - error listener to register.

#### removeListener

```python
def removeListener(listener: 'ErrorListener') -> None
```

> Unsubscribe registered error listener.
> 
> **Arguments**:
> 
> - `listener` _ErrorListener_ - error listener to unsubscribe.

#### nErrorListeners

```python
def nErrorListeners() -> int
```

> Returns number of registered error listeners

#### registerType

```python
def registerType(type: str) -> int
```

> Register type of sending message to get type id. For performance reasons,
> you could specify type id instead of type name, for example:
> 
> 
> ```
> message = tbapi.InstrumentMessage()
> message.typeId = loader.registerType("deltix.timebase.api.messages.universal.PackageHeader")
> // as alternative, you could write:
> //   message.typeName = "deltix.timebase.api.messages.universal.PackageHeader"
> loader.send(message)
> ```
> 
> **Arguments**:
> 
> - `type` _str_ - name of type to register.
>   
> 
> **Returns**:
> 
> - `int` - id of registered type.

#### registerInstrument

```python
def registerInstrument(symbol: str) -> int
```

> Register instrument of sending message to get instrument id. For performance reasons,
> you could specify instrument id instead of symbol and instrument type, for example:
> 
> 
> ```
> message = tbapi.InstrumentMessage()
> message.instrumentId = loader.registerInstrument('AAPL')
> // as alternative, you could write:
> //    message.symbol = 'AAPL'
> loader.send(message)
> ```
> 
> **Arguments**:
> 
> - `symbol` _str_ - instrument ticker.
>   
> 
> **Returns**:
> 
> - `int` - id of registered instrument.

