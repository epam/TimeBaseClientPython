import dxapi
import generators

def loadTradeBBO(stream, count, startTime = 0, timeInterval = 1000000000, symbols = ['MSFT', 'ORCL']):
    loader = stream.createLoader(dxapi.LoadingOptions())
    try:
        loadCount = 0
        tradeGenerator = generators.TradeGenerator(startTime, timeInterval, count, symbols)
        bboGenerator = generators.BBOGenerator(startTime, timeInterval, count, symbols)
        while tradeGenerator.next() and bboGenerator.next():
            tradeMessage = tradeGenerator.getMessage()
            loader.send(tradeMessage)
            loadCount = loadCount + 1
            #printLoadingInfo(loadCount, tradeMessage)

            bboMessage = bboGenerator.getMessage()
            loader.send(bboMessage)
            loadCount = loadCount + 1
            #printLoadingInfo(loadCount, bboMessage)

        print("Total loaded " + str(loadCount) + " messages")
        return loadCount
    finally:
        if loader != None:
            loader.close()

def loadBars(stream, count, startTime = 0, timeInterval = 1000000000, symbols = ['MSFT', 'ORCL']):
    loader = stream.createLoader(dxapi.LoadingOptions())
    try:
        loadCount = 0
        barGenerator = generators.BarGenerator(startTime, timeInterval, count, symbols)
        while barGenerator.next():
            message = barGenerator.getMessage()
            loader.send(message)
            loadCount = loadCount + 1
            #printLoadingInfo(loadCount, message)

        print("Total loaded " + str(loadCount) + " messages")
        return loadCount
    finally:
        if loader != None:
            loader.close()

def loadL2(stream, count, actionsCount = 5, startTime = 0, timeInterval = 1000000000, symbols = ['MSFT', 'ORCL']):
    loader = stream.createLoader(dxapi.LoadingOptions())
    try:
        loadCount = 0
        barGenerator = generators.L2Generator(startTime, timeInterval, count, symbols, actionsCount)
        while barGenerator.next():
            message = barGenerator.getMessage()
            loader.send(message)
            loadCount = loadCount + 1
            #printLoadingInfo(loadCount, message)

        print("Total loaded " + str(loadCount) + " messages")
        return loadCount
    finally:
        if loader != None:
            loader.close()

def loadUniversal(stream, count, entriesCount = 5, startTime = 0, timeInterval = 1000000000, symbols = ['BTCUSD']):
    loader = stream.createLoader(dxapi.LoadingOptions())
    try:
        loadCount = 0
        universalGenerator = generators.UniversalGenerator(startTime, timeInterval, count, symbols, entriesCount)
        while universalGenerator.next():
            message = universalGenerator.getMessage()
            loader.send(message)
            loadCount = loadCount + 1
            #printLoadingInfo(loadCount, message)

        print("Total loaded " + str(loadCount) + " messages")
        return loadCount
    finally:
        if loader != None:
            loader.close()

def readStream(stream):
    cursor = stream.createCursor(dxapi.SelectionOptions())
    try:
        readCount = 0
        while cursor.next():
            readCount = readCount + 1
            printReadingInfo(readCount, cursor.getMessage())

        print("Read " + str(readCount) + " messages")
        return readCount
    finally:
        if cursor != None:
            cursor.close()

def printLoadingInfo(count, message):
    if count < 10:
        print("Load message: " + str(message))
    elif count == 10:
        print('...')
    if count % 100000 == 0:
        print("Loaded " + str(count) + " messages")

def printReadingInfo(count, message):
    if count < 10:
        print("Read message: " + str(message))
    elif count == 10:
        print('...')
    if count % 1000000 == 0:
        print("Read " + str(count) + " messages")
