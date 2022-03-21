import unittest
import servertest
import time
import dxapi

class TestStream(servertest.TestWithStreams):

    def test_StreamOptions(self):
        stream = self.db.getStream(self.streamKeys[1])
        self.assertIsNotNone(stream)

        options = stream.options()
        self.assertEqual(options.name(), 'tradeBBO')
        self.assertEqual(options.description(), 'tradeBBO')
        self.assertEqual(options.owner(), None)
        self.assertEqual(options.location(), None)
        self.assertEqual(options.distributionRuleName(), None)
        self.assertIsNotNone(options.metadata())
        self.assertEqual(str(options.scope), 'DURABLE')
        self.assertEqual(options.distributionFactor, 0)
        self.assertEqual(options.duplicatesAllowed, True)
        self.assertEqual(options.highAvailability, False)
        self.assertEqual(options.unique, False)
        self.assertEqual(options.polymorphic, True)
        self.assertEqual(options.periodicity, 'IRREGULAR')

    def test_ListEntities(self):
        stream = self.db.getStream(self.streamKeys[2])
        self.assertIsNotNone(stream)

        streamEntities = stream.listEntities()
        self.compareEntities(streamEntities, set(self.entities.keys()))

    def test_DeleteStream(self):
        keys = set()
        for stream in self.db.listStreams():
            keys.add(stream.key())

        keyToRemove = self.streamKeys[2]
        keys.remove(keyToRemove)
        self.db.getStream(keyToRemove).deleteStream()

        afterRemove = set()
        for stream in self.db.listStreams():
            afterRemove.add(stream.key())

        self.assertEqual(keys, afterRemove)

    def test_TruncatePurgeGetTimeRange(self):
        stream = self.db.getStream(self.streamKeys[0])

        range = stream.getTimeRange()
        self.assertIsNotNone(range)
        self.assertEqual(range[0], 0)
        self.assertEqual(range[1], 9999000)
        messages = self.readMessages(stream, 0, 9999000)
        self.checkSymbols(messages, set(self.entities.keys()))

        stream.truncate(5000000, [self.entities['GOOG'], self.entities['IBM']])
        range = stream.getTimeRange()
        self.assertIsNotNone(range)
        self.assertTrue(0 <= range[0] <= 3000)
        self.assertTrue(9995000 <= range[1] <= 9999000) # consider order of entities

        range = stream.getTimeRange([self.entities['IBM']])
        self.assertIsNotNone(range)
        self.assertTrue(0 <= range[0] <= 3000)
        self.assertTrue(4995000 <= range[1] <= 4999000)  # consider order of entities

        messages = self.readMessages(stream, 0, 4997000)
        self.checkSymbols(messages, set(self.entities.keys()))

        messages = self.readMessages(stream, 5000000, 9999000)
        self.checkSymbols(messages, set(['AAPL']))

        stream.truncate(3000000)
        range = stream.getTimeRange()
        self.assertIsNotNone(range)
        self.assertEqual(range[0], 0)
        self.assertEqual(range[1], 2999000)

        # Todo: purge must be synchronous
        # stream.purge(1000000)
        # range = stream.getTimeRange()
        # self.assertIsNotNone(range)
        # self.assertEqual(range[0], 1000000)
        # self.assertEqual(range[1], 2999000)

    def test_Clear(self):
        stream = self.db.getStream(self.streamKeys[0])

        stream.clear([self.entities['GOOG'], self.entities['IBM']])
        messages = self.readMessages(stream, 0, 9999000)
        self.checkSymbols(messages, set(['AAPL']))

        stream.clear()
        messages = self.readMessages(stream, 0, 9999000)
        self.checkSymbols(messages, set())


if __name__ == '__main__':
    unittest.main()