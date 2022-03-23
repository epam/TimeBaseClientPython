import unittest
import servertest
import dxapi
import sys
import generators

class TestTickDB(servertest.TBServerTest):

    REFCOUNT_THRESHOLD = 1000

    streamKeys = [
        'bars1min', 'tradeBBO', 'l2'
    ]

    nStreams = 0
    
    def setUp(self):
        servertest.TBServerTest.setUp(self)
        
        for key in self.streamKeys:
            stream = self.db.getStream(key)
            if stream != None:
                stream.deleteStream()
                
        self.nStreams = len(self.db.listStreams())
                
        for key in self.streamKeys:
            self.createStreamQQL(key)
        self.assertEqual(len(self.db.listStreams()), self.nStreams + len(self.streamKeys))

    def tearDown(self):
        for key in self.streamKeys:
            stream = self.db.getStream(key)
            if stream != None:
                stream.deleteStream()
        self.assertEqual(len(self.db.listStreams()), self.nStreams)
        servertest.TBServerTest.tearDown(self)

    def test_NoneTrueFalse_refcount(self):
        start_refcounts = self.getNoneTrueFalseRefcount()

        self.writeL2EmptyMessages()
        load_refcounts = self.getNoneTrueFalseRefcount()
        self.checkRefcountsDelta(start_refcounts, load_refcounts)

        self.readL2EmptyMessages()
        read_refcount = self.getNoneTrueFalseRefcount()
        self.checkRefcountsDelta(start_refcounts, read_refcount)

    def printNonTrueFalseRefcounts(self, refcounts):
        print("None count = " + str(refcounts[0]) + ", True count = " + str(refcounts[1]) + ", False count = " + str(refcounts[2]))

    def getNoneTrueFalseRefcount(self):
        refcounts = (sys.getrefcount(None), sys.getrefcount(True), sys.getrefcount(False))
        self.printNonTrueFalseRefcounts(refcounts)
        return refcounts

    def checkRefcountsDelta(self, start, end):
        delta = (start[0] - end[0], start[1] - end[1], start[2] - end[2])
        self.assertLess(abs(delta[0]), self.REFCOUNT_THRESHOLD)
        self.assertLess(abs(delta[1]), self.REFCOUNT_THRESHOLD)
        self.assertLess(abs(delta[2]), self.REFCOUNT_THRESHOLD)

    def writeL2EmptyMessages(self):
        key = self.streamKeys[2]
        stream = self.db.getStream(key)
        self.assertIsNotNone(stream)

        loader = stream.createLoader(dxapi.LoadingOptions())
        self.assertIsNotNone(loader)
        try:
            loaded = 0
            generator = L2EmptyGenerator(0, 1000000000, 1000000, ['MSFT', 'ORCL'], 5)
            print("Start loading " + key)

            while generator.next():
                loader.send(generator.getMessage())
                loaded = loaded + 1
                if loaded % 200000 == 0:
                    print("Loaded " + str(loaded) + " messages")
            loader.close()
        finally:
            loader.close()

    def readL2EmptyMessages(self):
        key = self.streamKeys[2]
        stream = self.db.getStream(key)
        self.assertIsNotNone(stream)

        cursor = stream.createCursor(dxapi.SelectionOptions())
        self.assertIsNotNone(cursor)
        try:
            read = 0
            print('Start reading from ' + key)
            while cursor.next():
                message = cursor.getMessage()
                read = read + 1
                if (read % 1000000 == 0):
                    print('Read ' + str(read) + ' messages')
            cursor.close()
        finally:
            cursor.close()

class L2EmptyGenerator(generators.BaseGenerator):

    typeName = 'deltix.timebase.api.messages.L2Message'
    actionTypeName = 'deltix.timebase.api.messages.Level2Action'
    actions = ['INSERT', 'UPDATE', 'DELETE']
    actionsCount = 0

    def __init__(self, time, timeInterval, count, symbols, actionsCount):
        generators.BaseGenerator.__init__(self, time, timeInterval, count, symbols)
        self.actionsCount = actionsCount

    def next(self):
        if not generators.BaseGenerator.next(self):
            return False

        self.message.typeName = self.typeName
        self.message.currencyCode = 840
        self.message.exchangeId = None
        self.message.isImplied = False
        self.message.isSnapshot = True

        self.message.actions = []
        for i in range(self.actionsCount):
            self.message.actions.append(self.newAction())

        return True

    def newAction(self):
        action = dxapi.InstrumentMessage()
        action.typeName = None
        action.level = None
        action.isAsk = True
        action.action = None
        action.price = None
        action.size = None
        action.numOfOrders = None
        action.quoteId = None

        return action


if __name__ == '__main__':
    unittest.main()