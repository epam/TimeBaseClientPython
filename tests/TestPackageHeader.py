import unittest
import servertest
import testutils
import dxapi

class TestPackageHeader(servertest.TBServerTest):

    streamKey = 'universal'
    
    def test_Decimal64(self):
        try:
            stream = self.createStreamQQL(self.streamKey)
            self.assertIsNotNone(stream)
            self.assertEqual(self.streamCount(self.streamKey), 0)  # check stream is empty
            
            count = 12345
            loadCount = testutils.loadUniversal(stream, count)
            self.assertEqual(count, loadCount)
            
            readCount = 0
            currentPrice = 0
            cursor = stream.select(0, dxapi.SelectionOptions(), None, None)
            while cursor.next():
                readCount += 1
                message = cursor.getMessage()
                self.assertEqual(len(message.entries), 5)
                for entry in message.entries:
                    currentPrice += 1.1
                    self.assertAlmostEqual(entry.price, currentPrice)
                    self.assertAlmostEqual(entry.size, currentPrice)
            self.assertEqual(readCount, loadCount)
        finally:
            self.deleteStream(self.streamKey)

    # helpers

    def streamCount(self, key):
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