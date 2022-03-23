import unittest
import servertest
import testutils, generators
import time
import dxapi


class TestLoader(servertest.TBServerTest):

    streamKeys = [
        'bars1min', 'tradeBBO', 'l2'
    ]

    def test_LoadFixed(self):
        key = self.streamKeys[0]
        try:
            stream = self.createStreamQQL(key)
            self.assertIsNotNone(stream)
            self.assertEqual(self.streamCount(key), 0)  # check stream is empty

            count = 12345
            loadCount = testutils.loadBars(stream, count)
            self.assertEqual(count, loadCount)
            readCount = self.streamCount(key)
            self.assertEqual(readCount, loadCount)
        finally:
            self.deleteStream(key)

    def test_LoadPolymorphic(self):
        key = self.streamKeys[1]
        try:
            stream = self.createStreamQQL(key)
            self.assertIsNotNone(stream)
            self.assertEqual(self.streamCount(key), 0)  # check stream is empty

            count = 12354
            loadCount = testutils.loadTradeBBO(stream, count)
            self.assertEqual(count * 2, loadCount)
            readCount = self.streamCount(key)
            self.assertEqual(readCount, loadCount)
        finally:
            self.deleteStream(key)

    def test_LoadL2(self):
        key = self.streamKeys[2]
        try:
            stream = self.createStreamQQL(key)
            self.assertIsNotNone(stream)
            self.assertEqual(self.streamCount(key), 0)  # check stream is empty

            count = 12543
            loadCount = testutils.loadL2(stream, count)
            self.assertEqual(count, loadCount)
            readCount = self.streamCount(key)
            self.assertEqual(readCount, loadCount)
        finally:
            self.deleteStream(key)

    def test_registerTypesAndEntities(self):
        key = self.streamKeys[1]
        loader = None
        cursor = None
        try:
            stream = self.createStreamQQL(key)
            self.assertIsNotNone(stream)
            self.assertEqual(self.streamCount(key), 0)  # check stream is empty

            loader = stream.createLoader(dxapi.LoadingOptions())
            self.assertIsNotNone(loader)

            message = dxapi.InstrumentMessage()

            # register types
            bboTypeId = loader.registerType('deltix.timebase.api.messages.BestBidOfferMessage')
            tradeTypeId = loader.registerType('deltix.timebase.api.messages.TradeMessage')

            # register entities
            instrument1 = loader.registerInstrument('AAAA')
            instrument2 = loader.registerInstrument('BBBB')

            message.typeId = bboTypeId

            message.instrumentId = instrument1
            message.timestamp = 0
            loader.send(message)

            message.instrumentId = instrument2
            message.timestamp = 1
            loader.send(message)

            message.typeId = tradeTypeId

            message.instrumentId = instrument1
            message.timestamp = 2
            loader.send(message)

            message.instrumentId = instrument2
            message.timestamp = 3
            loader.send(message)

            loader.close()

            cursor = stream.createCursor(dxapi.SelectionOptions())
            self.assertIsNotNone(cursor)
            self.assertTrue(cursor.next())
            self.assertEqual(cursor.getMessage().typeName, 'deltix.timebase.api.messages.BestBidOfferMessage')
            self.assertEqual(cursor.getMessage().symbol, 'AAAA')

            self.assertTrue(cursor.next())
            self.assertEqual(cursor.getMessage().typeName, 'deltix.timebase.api.messages.BestBidOfferMessage')
            self.assertEqual(cursor.getMessage().symbol, 'BBBB')

            self.assertTrue(cursor.next())
            self.assertEqual(cursor.getMessage().typeName, 'deltix.timebase.api.messages.TradeMessage')
            self.assertEqual(cursor.getMessage().symbol, 'AAAA')

            self.assertTrue(cursor.next())
            self.assertEqual(cursor.getMessage().typeName, 'deltix.timebase.api.messages.TradeMessage')
            self.assertEqual(cursor.getMessage().symbol, 'BBBB')

        finally:
            if loader != None:
                loader.close()
            if cursor != None:
                cursor.close()
            self.deleteStream(key)

    def test_Flush(self):
        key = self.streamKeys[0]
        stream = self.createStreamQQL(key)
        loader = stream.createLoader(dxapi.LoadingOptions())
        try:
            barGenerator = generators.BarGenerator(0, 1000000000, 21, ['EPAM'])

            count = 0
            for i in range(5):
                barGenerator.next()
                loader.send(barGenerator.getMessage())
                time.sleep(1)
                self.assertEqual(count, self.streamCount(key))

            time.sleep(1)
            self.assertEqual(count, self.streamCount(key))

            loader.flush()
            time.sleep(1)

            count += 5
            self.assertEqual(count, self.streamCount(key))

            for i in range(5):
                barGenerator.next()
                loader.send(barGenerator.getMessage())
                loader.flush()
                time.sleep(1)
                count += 1
                self.assertEqual(count, self.streamCount(key))

            for i in range(5):
                barGenerator.next()
                loader.send(barGenerator.getMessage())
                time.sleep(1)
                self.assertEqual(count, self.streamCount(key))

            time.sleep(2)
            self.assertEqual(count, self.streamCount(key))

            loader.flush()
            time.sleep(1)
            count += 5
            self.assertEqual(count, self.streamCount(key))

            for i in range(100):
                loader.flush()
            self.assertEqual(count, self.streamCount(key))
        finally:
            if loader != None:
                loader.close()
            self.deleteStream(key)

    def test_MassFlush(self):
        key = self.streamKeys[0]
        stream = self.createStreamQQL(key)
        loader = stream.createLoader(dxapi.LoadingOptions())
        try:
            count = 100000
            barGenerator = generators.BarGenerator(0, 1000000000, count, ['EPAM'])
            while barGenerator.next():
                loader.send(barGenerator.getMessage())
                loader.flush()
            self.assertEqual(count, self.streamCount(key))
        finally:
            if loader != None:
                loader.close()
            self.deleteStream(key)

    # helpers

    def streamCount(self, key):
        stream = self.db.getStream(key)
        cursor = stream.createCursor(dxapi.SelectionOptions())
        cursor.reset(0)
        try:
            count = 0
            while cursor.next():
                count += 1
            return count
        finally:
            if cursor != None:
                cursor.close()


    def streamCountQQL(self, key):
        cursor = self.db.executeQuery('SELECT count() as count FROM "' + key + '"')
        try:
            if cursor.next():
                return cursor.getMessage().COUNT
            else:
                return 0
        finally:
            if cursor != None:
                cursor.close()

if __name__ == '__main__':
    unittest.main()