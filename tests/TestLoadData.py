import unittest
import servertest
import generators
import tbapi

class TestLoadData(servertest.TBServerTest):

    streamKeys = [
        'bars1min', 'tradeBBO', 'l2'
    ]

    def test_LoadVarchar(self):
        key = self.streamKeys[1]
        try:
            stream = self.createStream(key, True)
            self.assertIsNotNone(stream)

            with stream.tryLoader(tbapi.LoadingOptions()) as loader:
                loadCount = 0
                tradeGenerator = generators.TradeGenerator(0, 1000000000, 10, ['MSFT', 'ORCL'])
                while tradeGenerator.next():
                    tradeMessage = tradeGenerator.getMessage()

                    tradeMessage.condition = "Hello " + str(loadCount)

                    loader.send(tradeMessage)
                    loadCount = loadCount + 1
                print("Total loaded " + str(loadCount) + " messages")

            with stream.trySelect(0, tbapi.SelectionOptions(), None, None) as cursor:
                readCount = 0
                while cursor.next():
                    message = cursor.getMessage()
                    self.assertEqual("Hello " + str(readCount), message.condition)
                    readCount = readCount + 1
        finally:
            self.deleteStream(key)

if __name__ == '__main__':
    unittest.main()
