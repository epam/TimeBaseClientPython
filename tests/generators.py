import dxapi
import random

class BaseGenerator:
    
    def __init__(self, time, timeInterval, count, symbols):
        self.message = dxapi.InstrumentMessage()

        self.count = count
        self.currentTime = time
        self.timeInterval = timeInterval
        self.symbols = symbols
        if symbols != None and len(symbols) > 0:
            self.curSymbol = 0

    def isAtEnd(self):
        return self.count == 0

    def next(self):
        if self.isAtEnd():
            return False

        self.message.timestamp = self.nextTimestamp()
        self.message.symbol = self.nextSymbol()
        self.message.instrumentType = 'EQUITY'
        self.count = self.count - 1

        return True

    def getMessage(self):
        return self.message

    def nextTimestamp(self):
        result = self.currentTime
        self.currentTime = self.currentTime + self.timeInterval
        return result

    def nextSymbol(self):
        if self.curSymbol == -1:
            return None

        symbol = self.symbols[self.curSymbol]
        self.curSymbol = self.curSymbol + 1
        if self.curSymbol >= len(self.symbols):
            self.curSymbol = 0

        return symbol

class TradeGenerator(BaseGenerator):

    typeName = 'deltix.timebase.api.messages.TradeMessage'
    sides = ['BUY', 'SELL']

    def next(self):
        if not BaseGenerator.next(self):
            return False

        self.message.typeName = self.typeName
        self.message.currencyCode = 840
        self.message.exchangeId = 'NYSE'
        self.message.price = random.uniform(0.0, 1.0) * 100
        self.message.size = random.randint(0, 1000)
        self.message.netPriceChange = random.uniform(0.0, 1.0) * 100
        self.message.aggressorSide = self.sides[random.randint(0, 1)]
        self.message.eventType = 'TRADE'

        return True

class BBOGenerator(BaseGenerator):

    typeName = 'deltix.timebase.api.messages.BestBidOfferMessage'

    def next(self):
        if not BaseGenerator.next(self):
            return False

        self.message.typeName = self.typeName
        self.message.currencyCode = 840
        if self.count % 2 == 0:
            self.message.bidPrice = random.uniform(0.0, 1.0) * 100
            self.message.bidSize = random.randint(0, 1000)
            self.message.bidExchangeId = 'NYSE'
        else:
            self.message.offerPrice = random.uniform(0.0, 1.0) * 100
            self.message.offerSize = random.randint(0, 1000)
            self.message.offerExchangeId = 'NYSE'

        return True

class BarGenerator(BaseGenerator):

    typeName = 'deltix.timebase.api.messages.BarMessage'

    def next(self):
        if not BaseGenerator.next(self):
            return False

        self.message.typeName = self.typeName
        self.message.currencyCode = 840
        self.message.high = random.uniform(0.0, 100.0)
        self.message.open = self.message.high - random.uniform(0.0, 10.0)
        self.message.close = self.message.high - random.uniform(0.0, 10.0)
        self.message.low = min(self.message.open, self.message.close) + random.uniform(0.0, 10.0)
        self.message.volume = self.count

        return True

class L2Generator(BaseGenerator):

    typeName = 'deltix.timebase.api.messages.L2Message'
    actionTypeName = 'deltix.timebase.api.messages.Level2Action'
    actions = ['INSERT', 'UPDATE', 'DELETE']
    actionsCount = 0

    def __init__(self, time, timeInterval, count, symbols, actionsCount):
        BaseGenerator.__init__(self, time, timeInterval, count, symbols)
        self.actionsCount = actionsCount

    def next(self):
        if not BaseGenerator.next(self):
            return False

        self.message.typeName = self.typeName
        self.message.currencyCode = 840
        self.message.exchangeId = 'NYSE'
        self.message.isImplied = False
        self.message.isSnapshot = False

        self.message.actions = []
        for i in range(self.actionsCount):
            self.message.actions.append(self.newAction())

        return True

    def newAction(self):
        action = dxapi.InstrumentMessage()
        action.typeName = self.actionTypeName
        action.level = random.randint(0, 100)
        action.isAsk = True
        action.action = self.actions[random.randint(0, 2)]
        action.price = random.uniform(0.0, 1.0) * 100
        action.size = random.uniform(0.0, 1.0) * 100
        action.numOfOrders = random.randint(0, 100)
        action.quoteId = b'quoteId'

        return action

class UniversalGenerator(BaseGenerator):
    typeName = 'deltix.timebase.api.messages.universal.PackageHeader'
    entryTypeName = 'deltix.timebase.api.messages.universal.L2EntryNew'
    entriesCount = 0
    currentPrice = 0
    
    def __init__(self, time, timeInterval, count, symbols, entriesCount):
        BaseGenerator.__init__(self, time, timeInterval, count, symbols)
        self.entriesCount = entriesCount

    def next(self):
        if not BaseGenerator.next(self):
            return False

        self.message.typeName = self.typeName
        self.message.currencyCode = 999
        self.message.exchangeId = 'GDAX'
        self.message.packageType = 'VENDOR_SNAPSHOT'

        self.message.entries = []
        for i in range(self.entriesCount):
            self.message.entries.append(self.newEntry(i))

        return True

    def newEntry(self, level):
        self.currentPrice += 1.1
        entry = dxapi.InstrumentMessage()
        entry.typeName = self.entryTypeName
        entry.level = level
        entry.side = 'BID'
        entry.price = self.currentPrice
        entry.size = self.currentPrice
        return entry
        
