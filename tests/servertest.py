import unittest

import sys, os, inspect, struct
from subprocess import Popen
import socket

import copy
import time

print("Python version: " + str(sys.version))

testdir = os.path.dirname(__file__)
if testdir != "":
    testdir = testdir + '/'
sys.path.append(testdir + "..")

import testutils
import dxapi

class TBServerTest(unittest.TestCase):

    HOST = os.getenv('TIMEBASE_HOST', 'localhost')
    PORT = os.getenv('TIMEBASE_PORT', 8011)
    DELTIX_HOME = os.getenv('DELTIX_HOME', None)

    TIMEBASE_WAIT_TIMEOUT = 10

    @classmethod
    def setUpClass(cls):
        print('TimeBase host: ' + str(cls.HOST))
        print('TimeBase port: ' + str(cls.PORT))
        print('TimeBase home: ' + str(cls.DELTIX_HOME))

        #if not cls.DELTIX_HOME == None:
        #    print('Starting TimeBase...')
        #    socket.setdefaulttimeout(5)
        #    cls.server = Popen([cls.getTimeBaseCommand(), "-port", str(cls.PORT)])
        #    cls.waitUntilStarted(cls.TIMEBASE_WAIT_TIMEOUT)
        #    print('OK')

    @classmethod
    def tearDownClass(cls):
        print('TimeBase host: ' + str(cls.HOST))
        print('TimeBase port: ' + str(cls.PORT))
        print('TimeBase home: ' + str(cls.DELTIX_HOME))
        #if not cls.DELTIX_HOME == None:
        #    try:
        #        print('Shutdown TimeBase...')
        #        s = urllib2.urlopen(cls.httpURL() + "/shutdown").read()
        #        time.sleep(5) # temp fix wait a little for closing server
        #        if (cls.server != None):
        #            cls.server.terminate()
        #        print('OK')
        #    except:
        #        #cls.printException()
        #        print("exception during shutdown")
        #        pass

    def setUp(self):
        url = self.dxtickURL()
        print('Connecting to ' + str(url) + "...")
        self.db = dxapi.TickDb.createFromUrl(url)
        self.db.open(False)
        print('OK')

    def tearDown(self):
        self.db.close()
        
    @classmethod
    def getTimeBaseCommand(cls):
        if cls.isLinux():
            print('Linux OS')
            return testdir + 'timebase'
        else:
            print('Windows OS')
            return testdir + 'timebase.cmd'
    
    @classmethod
    def isLinux(cls):
        if sys.platform.startswith('linux'):
            return True
        else:
            return False

    # helpers

    @classmethod
    def waitUntilStarted(cls, waitTimeout):
        while waitTimeout > 0:
            try:
                time.sleep(1)
                waitTimeout = waitTimeout - 1
                response = urllib2.urlopen(cls.httpURL())
                response.read()
                return
            except:
                pass

    @classmethod
    def dxtickURL(cls):
        return "dxtick://" + cls.HOST + ":" + str(cls.PORT)

    @classmethod
    def httpURL(cls):
        return "http://" + cls.HOST + ":" + str(cls.PORT)

    def createStream(self, key, polymorphic = False):
        self.deleteStream(key)

        with open(testdir + 'testdata/' + key + '.xml', 'r') as schemaFile:
            schema = schemaFile.read()
        options = dxapi.StreamOptions()
        options.polymorphic = polymorphic
        options.metadata(schema)

        return self.db.createStream(key, options)

    def createStreamQQL(self, key):
        self.deleteStream(key)

        with open(testdir + 'testdata/' + key + '.qql', 'r') as qqlFile:
            qql = qqlFile.read()

        cursor = self.db.executeQuery(qql)
        self.assertTrue(cursor.next())
        reportMessage = cursor.getMessage()
        #self.assertEqual(reportMessage.messageText, 'Stream created')

        return self.db.getStream(key)

    def deleteStream(self, key):
        stream = self.db.getStream(key)
        if stream != None:
            stream.deleteStream()


class TestWithStreams(TBServerTest):

    streamKeys = [
        'bars1min', 'tradeBBO', 'l2'
    ]

    entities = {
        'AAPL':'AAPL',
        'GOOG':'GOOG',
        'IBM':'IBM',
    }

    types = {
        'trade':'deltix.timebase.api.messages.TradeMessage',
        'bbo':'deltix.timebase.api.messages.BestBidOfferMessage',
        'bar':'deltix.timebase.api.messages.BarMessage',
        'l2':'deltix.timebase.api.messages.L2Message'
    }
    
    nStreams = 0

    def setUp(self):
        TBServerTest.setUp(self)
        
        for key in self.streamKeys:
            stream = self.db.getStream(key)
            if stream != None:
                stream.deleteStream()
        
        self.nStreams = len(self.db.listStreams())
        for key in self.streamKeys:
            self.createStreamQQL(key)
        self.assertEqual(len(self.db.listStreams()), self.nStreams + len(self.streamKeys))

        stream = self.db.getStream(self.streamKeys[0])
        testutils.loadBars(stream, 10000, 0, 1000000000, list(self.entities.keys()))

        stream = self.db.getStream(self.streamKeys[1])
        testutils.loadTradeBBO(stream, 5000, 0, 1000000000, list(self.entities.keys()))

        stream = self.db.getStream(self.streamKeys[2])
        testutils.loadL2(stream, 10000, 10, 0, 1000000000, list(self.entities.keys()))

    def tearDown(self):
        for key in self.streamKeys:
            stream = self.db.getStream(key)
            if stream != None:
                stream.deleteStream()
        #self.assertEqual(len(self.db.listStreams()), self.nStreams)
        tbStreams = len(self.db.listStreams())
        if tbStreams != self.nStreams:
            print("WARNING: streams in TimeBase " + str(tbStreams) + ", but should be " + str(self.nStreams))
        TBServerTest.tearDown(self)

    # utils

    def read(self, cursor, count):
        messages = []
        for i in range(count):
            self.assertTrue(cursor.next())
            messages.append(copy.deepcopy(cursor.getMessage()))
        return messages

    def readMessages(self, stream, _from, to):
        options = dxapi.SelectionOptions()
        options._from = _from
        options.to = to
        cursor = stream.createCursor(options)
        messages = []
        try:
            while cursor.next():
                messages.append(copy.deepcopy(cursor.getMessage()))
            return messages
        finally:
            cursor.close()

    def readCount(self, cursor):
        readCount = 0
        while cursor.next():
            readCount = readCount + 1
        return readCount

    def checkCursorTypes(self, cursor, types):
        messages = self.read(cursor, 20)
        self.checkTypes(messages, types)

    def checkCursorSymbols(self, cursor, symbols):
        messages = self.read(cursor, 20)
        self.checkSymbols(messages, symbols)

    def checkCursorTypesAndSymbols(self, cursor, types, symbols):
        messages = self.read(cursor, 20)
        self.checkTypes(messages, types)
        self.checkSymbols(messages, symbols)

    def checkTypes(self, messages, types):
        msgSet = set()
        for message in messages:
            msgSet.add(message.typeName)
        self.assertEqual(msgSet, types)

    def checkSymbols(self, messages, symbols):
        smbSet = set()
        for message in messages:
            smbSet.add(message.symbol)
        self.assertEqual(smbSet, symbols)

    def compareEntities(self, entities, symbols):
        symbolsSet = set(entities)
        self.assertEqual(symbolsSet, symbols)

    def removeAll(self, collection, elements):
        for element in elements:
            collection.remove(element)

    def addAll(self, collection, elements):
        for element in elements:
            collection.add(element)

    def msToNs(self, ms):
        return ms * 1000000


if __name__ == '__main__':
    unittest.main()
