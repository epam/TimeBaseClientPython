import unittest
import servertest
import testutils
import os, sys, threading, time
import dxapi

from sys import version_info

testdir = os.path.dirname(__file__)
if testdir != "":
    testdir = testdir + '/'

class TestMultithreaded(servertest.TestWithStreams):

    def test_NextIfAvailableWithLoader(self):
        results = [None] * 1
        reader = threading.Thread(target = self.readStream, args = (results, 0, self.streamKeys[0], ))
        writer = threading.Thread(target = self.loadBarsThread, args = ("bars1min", 3000, ))

        reader.start()
        writer.start()

        writer.join()
        reader.join()

        self.assertEqual(results[0], 13000)

    def test_NextIfAvailable3Cursors(self):
        results = [None] * 3
        reader1 = threading.Thread(target = self.readStream, args = (results, 0, self.streamKeys[0], 1000000000, ))
        reader2 = threading.Thread(target = self.readStream, args = (results, 1, self.streamKeys[1], ))
        reader3 = threading.Thread(target = self.readStream, args = (results, 2, self.streamKeys[2], ))

        reader1.start()
        reader2.start()
        reader3.start()

        reader1.join()
        reader2.join()
        reader3.join()

        self.assertEqual(results[0], 0)
        self.assertEqual(results[1], 10000)
        self.assertEqual(results[2], 10000)

    def test_MassiveLoads(self):
        # switch off for 2.7 python, it works too slow
        if version_info >= (2, 7) and version_info < (2, 8):
            return

        loadCount = 1000000
        timeout = 60

        barsStreamKey = "bars1min.load"
        # l2StreamKey = "l2.load"
        # tradeBBOStreamKey = "tradeBBO.load"

        self.deleteStream(barsStreamKey)
        # self.deleteStream(l2StreamKey)
        # self.deleteStream(tradeBBOStreamKey)

        self.createStream("bars1min", barsStreamKey)
        # self.createStream("l2", l2StreamKey)
        # self.createStream("tradeBBO", tradeBBOStreamKey, True)
        try:
            results = [None] * 3
            reader1 = threading.Thread(target = self.readStream, args = (results, 0, barsStreamKey, 0, timeout, loadCount, ))
            # reader2 = threading.Thread(target = self.readStream, args = (results, 1, l2StreamKey, 0, timeout, loadCount / 5, ))
            # reader3 = threading.Thread(target = self.readStream, args = (results, 2, tradeBBOStreamKey, 0, timeout, loadCount, ))

            writer1 = threading.Thread(target = self.loadBarsThread, args = (barsStreamKey, loadCount, ))
            # writer2 = threading.Thread(target = self.loadL2Thread, args = (l2StreamKey, loadCount / 5, ))
            # writer3 = threading.Thread(target = self.loadTradeBBOThread, args = (tradeBBOStreamKey, loadCount, ))

            writer1.start()
            # writer2.start()
            # writer3.start()

            reader1.start()
            # reader2.start()
            # reader3.start()

            reader1.join()
            # reader2.join()
            # reader3.join()
            writer1.join()
            # writer2.join()
            # writer3.join()

            self.assertEqual(results[0], loadCount)
            # self.assertEqual(results[1], loadCount / 5)
            # self.assertEqual(results[2], loadCount)
        finally:
            # b = 3
            self.deleteStream(barsStreamKey)
            # self.deleteStream(l2StreamKey)
            # self.deleteStream(tradeBBOStreamKey)

    ### helpers
    def createStream(self, fileName, key, polymorphic = False):
        with open(testdir + 'testdata/' + fileName + '.xml', 'r') as schemaFile:
            schema = schemaFile.read()
        options = dxapi.StreamOptions()
        options.name(key)
        options.description(key)
        options.scope = dxapi.StreamScope('DURABLE')
        options.distributionFactor = 1
        options.highAvailability = False
        options.polymorphic = polymorphic
        options.metadata(schema)

        return self.db.createStream(key, options)

    def loadBarsThread(self, key, count):
        testutils.loadBars(self.db.getStream(key), count)

    def loadL2Thread(self, key, count):
        testutils.loadL2(self.db.getStream(key), count)

    def loadTradeBBOThread(self, key, count):
        testutils.loadTradeBBO(self.db.getStream(key), count / 2)

    def readStream(self, results, num, key, startTime = 0, timeout = 30, readUntil = 1000000000):
        stream = self.db.getStream(key)
        options = dxapi.SelectionOptions()
        options.live = True

        cursor = stream.createCursor(options)
        cursor.reset(startTime)

        tStart = time.time()
        messages = 0
        while True:
            if time.time() - tStart > timeout:
                break
            state = cursor.nextIfAvailable()
            if state == dxapi.OK:
                message = cursor.getMessage()
                messages += 1
                if messages >= readUntil:
                    break
            elif state == dxapi.END_OF_CURSOR:
                break
        print("Total read from " + key + ": " + str(messages))
        results[num] = messages

if __name__ == '__main__':
    unittest.main()