import unittest
import servertest
import generators
import time
import dxapi

class TestSpeed(servertest.TBServerTest):

    streamKeys = [
        'bars1min', 'tradeBBO', 'l2'
    ]

    def setUp(self):
        servertest.TBServerTest.setUp(self)
        for key in self.streamKeys:
            self.createStreamQQL(key)
        self.assertEqual(len(self.db.listStreams()), len(self.streamKeys) + 1)

    def tearDown(self):
        for key in self.streamKeys:
            self.deleteStream(key)
        self.assertEqual(len(self.db.listStreams()), 1)
        servertest.TBServerTest.tearDown(self)

    def test_FixedReadWriteSpeed(self):
        key = self.streamKeys[0]
        stream = self.db.getStream(key)
        self.assertIsNotNone(stream)

        loader = stream.createLoader(dxapi.LoadingOptions())
        self.assertIsNotNone(loader)
        try:
            typeId = loader.registerType('deltix.timebase.api.messages.BarMessage')
            loaded = 0
            generator = generators.BarGenerator(0, 1000000000, 2000000, ['MSFT', 'ORCL'])
            print("Start loading " + key)

            startMeasure = time.time()
            while generator.next():
                message = generator.getMessage()
                message.typeId = typeId

                loader.send(message)
                loaded = loaded + 1
                if loaded % 200000 == 0:
                    print("Loaded " + str(loaded) + " messages")
            loader.close()

            timeMeasure = (time.time() - startMeasure)
            print('Total: ' + str(loaded) + ' msgs')
            print('Time: ' + str(timeMeasure) + ' s')
            print('Speed: ' + str(loaded / timeMeasure) + ' msg/s')
        finally:
            loader.close()

        cursor = stream.createCursor(dxapi.SelectionOptions())
        self.assertIsNotNone(cursor)
        try:
            read = 0
            startMeasure = time.time()
            print('Start reading from ' + key)
            while cursor.next():
                cursor.getMessage()

                read = read + 1
                if (read % 1000000 == 0):
                    print('Read ' + str(read) + ' messages')
            cursor.close()

            timeMeasure = (time.time() - startMeasure)
            print('Stream: ' + key)
            print('Total: ' + str(read) + ' messages')
            print('Time: ' + str(timeMeasure) + ' s')
            print('Speed: ' + str(read / timeMeasure) + ' msg/s')
        finally:
            cursor.close()

    def test_PolymorphicReadWriteSpeed(self):
        key = self.streamKeys[1]
        stream = self.db.getStream(key)
        self.assertIsNotNone(stream)

        loader = stream.createLoader(dxapi.LoadingOptions())
        self.assertIsNotNone(loader)
        try:
            tradeId = loader.registerType('deltix.timebase.api.messages.TradeMessage')
            bboId = loader.registerType('deltix.timebase.api.messages.BestBidOfferMessage')
            loaded = 0
            tradeGenerator = generators.TradeGenerator(0, 1000000000, 1000000, ['MSFT', 'ORCL'])
            bboGenerator = generators.BBOGenerator(0, 1000000000, 1000000, ['MSFT', 'ORCL'])
            print("Start loading " + key)

            startMeasure = time.time()
            while tradeGenerator.next() and bboGenerator.next():
                tradeMessage = tradeGenerator.getMessage()
                tradeMessage.typeId = tradeId
                loader.send(tradeMessage)
                loaded = loaded + 1

                bboMessage = bboGenerator.getMessage()
                bboMessage.typeId = bboId
                loader.send(bboMessage)
                loaded = loaded + 1

                if loaded % 200000 == 0:
                    print("Loaded " + str(loaded) + " messages")
            loader.close()

            timeMeasure = (time.time() - startMeasure)
            print('Stream: ' + key)
            print('Total: ' + str(loaded) + ' msgs')
            print('Time: ' + str(timeMeasure) + ' s')
            print('Speed: ' + str(loaded / timeMeasure) + ' msg/s')
        finally:
            loader.close()

        cursor = stream.createCursor(dxapi.SelectionOptions())
        self.assertIsNotNone(cursor)
        try:
            read = 0
            startMeasure = time.time()
            print('Start reading from ' + key)
            while cursor.next():
                cursor.getMessage()

                read = read + 1
                if (read % 1000000 == 0):
                    print('Read ' + str(read) + ' messages')
            cursor.close()

            timeMeasure = (time.time() - startMeasure)
            print('Stream: ' + key)
            print('Total: ' + str(read) + ' messages')
            print('Time: ' + str(timeMeasure) + ' s')
            print('Speed: ' + str(read / timeMeasure) + ' msg/s')
        finally:
            cursor.close()

    def test_L2ReadWriteSpeed(self):
        key = self.streamKeys[2]
        stream = self.db.getStream(key)
        self.assertIsNotNone(stream)

        loader = stream.createLoader(dxapi.LoadingOptions())
        self.assertIsNotNone(loader)
        try:
            typeId = loader.registerType('deltix.timebase.api.messages.L2Message')
            loaded = 0
            generator = generators.L2Generator(0, 1000000000, 2000000, ['MSFT', 'ORCL'], 5)
            print("Start loading " + key)

            startMeasure = time.time()
            while generator.next():
                message = generator.getMessage()
                message.typeId = typeId

                loader.send(message)
                loaded = loaded + 1
                if loaded % 200000 == 0:
                    print("Loaded " + str(loaded) + " messages")
            loader.close()

            timeMeasure = (time.time() - startMeasure)
            print('Total: ' + str(loaded) + ' msgs')
            print('Time: ' + str(timeMeasure) + ' s')
            print('Speed: ' + str(loaded / timeMeasure) + ' msg/s')
        finally:
            loader.close()

        cursor = stream.createCursor(dxapi.SelectionOptions())
        self.assertIsNotNone(cursor)
        try:
            read = 0
            startMeasure = time.time()
            print('Start reading from ' + key)
            while cursor.next():
                cursor.getMessage()

                read = read + 1
                if (read % 1000000 == 0):
                    print('Read ' + str(read) + ' messages')
            cursor.close()

            timeMeasure = (time.time() - startMeasure)
            print('Stream: ' + key)
            print('Total: ' + str(read) + ' messages')
            print('Time: ' + str(timeMeasure) + ' s')
            print('Speed: ' + str(read / timeMeasure) + ' msg/s')
        finally:
            cursor.close()

if __name__ == '__main__':
    unittest.main()