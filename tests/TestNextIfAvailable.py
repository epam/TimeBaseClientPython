import unittest
import servertest
import testutils
import copy
import dxapi

class TestNextIfAvailable(servertest.TestWithStreams):

    def test_Smoke(self):
        stream = self.db.getStream(self.streamKeys[0])
        self.assertEqual(self.readStreamIfAvailable(stream), 10000)
        stream = self.db.getStream(self.streamKeys[1])
        self.assertEqual(self.readStreamIfAvailable(stream), 10000)
        stream = self.db.getStream(self.streamKeys[2])
        self.assertEqual(self.readStreamIfAvailable(stream), 10000)

    def test_Multistream(self):
        barStream = self.db.getStream(self.streamKeys[0])
        tradeBBOStream = self.db.getStream(self.streamKeys[1])
        l2Stream = self.db.getStream(self.streamKeys[2])
        cursor = self.db.select(0, [tradeBBOStream, barStream, l2Stream], dxapi.SelectionOptions(), None, None)
        self.assertEqual(self.readCursorIfAvailable(cursor), 30000)

    def test_Reset(self):
        stream = self.db.getStream(self.streamKeys[0])
        cursor = stream.createCursor(dxapi.SelectionOptions())

        cursor.reset(9000)
        self.assertEqual(self.readMessage(cursor).timestamp, self.msToNs(9000))

        cursor.reset(5000)
        self.assertEqual(self.readMessage(cursor).timestamp, self.msToNs(5000))

        cursor.reset(100000)
        self.checkCursorSymbols(cursor, set(self.entities.keys()))

        cursor.reset(90000, [self.entities['AAPL'], self.entities['IBM']])
        self.checkCursorSymbols(cursor, set(['AAPL', 'IBM']))

        cursor.close()

    def test_SelectReverse(self):
        stream = self.db.getStream(self.streamKeys[0])
        options = dxapi.SelectionOptions()
        options.reverse = True
        cursor = stream.createCursor(options)

        timestamp = 10000000000000
        cursor.reset(timestamp)

        while True:
            message = self.readMessage(cursor)
            if message == None:
                break
            if message.timestamp > timestamp:
                self.assertFalse(True)
            timestamp = message.timestamp

        cursor.close()

    ### helpers
    def readMessage(self, cursor):
        while True:
            state = cursor.nextIfAvailable()
            if state == dxapi.OK:
                return cursor.getMessage()
            elif state == dxapi.END_OF_CURSOR:
                return None

    def readMessagesCount(self, cursor, count):
        messages = []
        for i in range(count):
            messages.append(copy.deepcopy(self.readMessage(cursor)))
        return messages

    def readStreamIfAvailable(self, stream):
        cursor = stream.createCursor(dxapi.SelectionOptions())
        try:
            return self.readCursorIfAvailable(cursor)
        finally:
            if cursor != None:
                cursor.close()

    def readCursorIfAvailable(self, cursor):
        readCount = 0
        while True:
            state = cursor.nextIfAvailable()
            if state == dxapi.OK:
                message = cursor.getMessage()
                readCount += 1
                self.printReadingInfo(readCount)
            elif state == dxapi.END_OF_CURSOR:
                break
        print("Read " + str(readCount) + " messages")
        return readCount

    def printReadingInfo(self, count):
        if count % 1000000 == 0:
            print("Read " + str(count) + " messages")

    def checkCursorSymbols(self, cursor, symbols):
        messages = self.readMessagesCount(cursor, 20)
        self.checkSymbols(messages, symbols)

    def checkCursorTypesAndSymbols(self, cursor, types, symbols):
        messages = self.readMessagesCount(cursor, 20)
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

if __name__ == '__main__':
    unittest.main()