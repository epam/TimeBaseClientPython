import unittest
import servertest
import generators
import testutils
import time
import tbapi

class TestEntities(servertest.TBServerTest):

    def test_LoadManyEntities(self):
        stream = self.createStream('bars1min', False)
        loader = stream.createLoader(tbapi.LoadingOptions())
        try:
            loadCount = 0
            generator = generators.BarGenerator(0, 0, 35000, ['AAPL'])
            symbol = 'SYM_'
            while generator.next():
                tradeMessage = generator.getMessage()
                tradeMessage.symbol = symbol + str(loadCount)
                loader.send(tradeMessage)
                loadCount = loadCount + 1
                if loadCount % 5000 == 0:
                    print("Loaded " + str(loadCount) + " messages")
                
            print("Total loaded " + str(loadCount) + " messages")
        finally:
            if loader != None:
                loader.close()
                
        cursor = stream.select(0, tbapi.SelectionOptions(), None, None)
        count = 0
        while cursor.next():
            count += 1
            if count % 5000 == 0:
                print("Read " + str(count) + " messages")
        print("Read " + str(count) + " messages")
        self.assertEqual(count, 35000)

if __name__ == '__main__':
    unittest.main()
