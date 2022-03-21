import unittest
import servertest
import dxapi
import sys

class TestQQL(servertest.TestWithStreams):

    def test_ExecuteQuery(self):
        cursor = self.db.executeQuery("select * from tradeBBO")
        count = self.readCount(cursor)
        self.assertEqual(count, 10000)
        cursor.close()

    def test_ExecuteQueryWithOptions(self):
        options = dxapi.SelectionOptions()
        options._from = 5000
        options.to = 6000
        cursor = self.db.executeQuery("select * from bars1min", options, [])

        self.assertTrue(cursor.next())
        message = cursor.getMessage()
        self.assertEqual(message.timestamp, 5000000000)
        self.assertTrue(cursor.next())
        message = cursor.getMessage()
        self.assertEqual(message.timestamp, 6000000000)
        self.assertFalse(cursor.next())

        cursor.close()

    def test_ExecuteQueryWithInstruments(self):
        cursor = self.db.executeQuery("select * from l2", dxapi.SelectionOptions(), 10000, [self.entities['IBM']], [])
        self.checkCursorSymbols(cursor, set(['IBM']))
        cursor.close()

    def test_ExecuteQueryException(self):
        try:
            self.db.executeQuery("select * from ?")
            self.assertTrue(False)
        except:
            print("Expected error")


if __name__ == '__main__':
    unittest.main()