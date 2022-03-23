import os
import unittest
import servertest
import dxapi

testdir = os.path.dirname(__file__)
if testdir != "":
    testdir = testdir + '/'

class TestTickDB(servertest.TBServerTest):

    streamKeys = [
        'bars1min', 'tradeBBO', 'l2'
    ]

    def test_isOpen(self):
        self.assertTrue(self.db.isOpen())

    def test_isReadOnly(self):
        self.assertFalse(self.db.isReadOnly())

    #def test_createStream(self):
    #    key = self.streamKeys[1]
    #    try:
    #        with open(testdir + 'testdata/' + key + '.xml', 'r') as schemaFile:
    #            schema = schemaFile.read()

    #        options = dxapi.StreamOptions()
    #        options.name(key)
    #        options.description(key)
    #        options.scope = dxapi.StreamScope('DURABLE')
    #        options.distributionFactor = 9
    #        options.highAvailability = False
    #        options.polymorphic = False
    #        options.metadata(schema)

    #        self.db.createStream(key, options)

    #        stream = self.db.getStream(key)
    #        self.assertIsNotNone(stream)
    #        self.assertEqual(stream.key(), key)
    #        self.assertEqual(stream.name(), key)
    #        self.assertEqual(stream.distributionFactor(), 0)
    #        self.assertEqual(stream.description(), key)
    #        self.assertEqual(stream.highAvailability(), False)
    #        self.assertEqual(stream.polymorphic(), False)
    #        self.assertEqual(stream.periodicity(), 'IRREGULAR')
    #        self.assertIsNone(stream.location())
    #        self.assertIsNotNone(stream.metadata())
    #        self.assertEqual(str(stream.scope()), 'DURABLE')
    #        self.assertEqual(stream.unique(), False)
    #    finally:
    #        self.deleteStream(key)

    def test_createStreamQQL(self):
        key = self.streamKeys[1]
        try:
            self.createStreamQQL(key)

            stream = self.db.getStream(key)
            self.assertIsNotNone(stream)
            self.assertEqual(stream.key(), key)
            self.assertEqual(stream.name(), key)
            self.assertEqual(stream.distributionFactor(), 0)
            self.assertEqual(stream.description(), key)
            self.assertEqual(stream.highAvailability(), False)
            self.assertEqual(stream.polymorphic(), True)
            self.assertEqual(stream.periodicity(), 'IRREGULAR')
            self.assertIsNone(stream.location())
            self.assertIsNotNone(stream.metadata())
            self.assertEqual(str(stream.scope()), 'DURABLE')
            self.assertEqual(stream.unique(), False)
        finally:
            self.deleteStream(key)

    def test_listStreams(self):
        try:
            self.createStreamsQQL()
            
            keySet = set()
            streams = self.db.listStreams()
            for stream in streams:
                keySet.add(stream.key())
                
            for key in self.streamKeys:
                self.assertEqual(True, key in keySet)
        finally:
            self.deleteStreams()

    def test_removeStream(self):
        key = 'l2'
        try:
            self.createStreamQQL(key)
            stream = self.db.getStream(key)
            self.assertIsNotNone(stream)

            stream.deleteStream()
            stream = self.db.getStream(key)
            self.assertIsNone(stream)
        finally:
            self.deleteStream(key)

    # helpers
    def createStreams(self):
        for key in self.streamKeys:
            self.createStream(key)
            
    def createStreamsQQL(self):
        for key in self.streamKeys:
            self.createStreamQQL(key)

    def deleteStreams(self):
        for key in self.streamKeys:
            self.deleteStream(key)

if __name__ == '__main__':
    unittest.main()