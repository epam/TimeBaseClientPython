import unittest
import servertest
import testutils
import dxapi

class CursorTest(servertest.TestWithStreams):

    def test_ReadSmoke(self):
        for key in self.streamKeys:
            self.assertEqual(testutils.readStream(self.db.getStream(key)), 10000)

    def test_SelectFromTo(self):
        stream = self.db.getStream(self.streamKeys[0])
        options = dxapi.SelectionOptions()
        options._from = 5000
        options.to = 6000
        cursor = stream.createCursor(options)

        self.assertTrue(cursor.next())
        message = cursor.getMessage()
        self.assertEqual(message.timestamp, 5000000000)

        self.assertTrue(cursor.next())
        message = cursor.getMessage()
        self.assertEqual(message.timestamp, 6000000000)

        self.assertFalse(cursor.next())

        cursor.close()


    def test_SelectFromToReverse(self):
        stream = self.db.getStream(self.streamKeys[0])
        options = dxapi.SelectionOptions()
        options._from = 5000
        options.to = 6000
        options.reverse = True
        cursor = stream.createCursor(options)

        self.assertTrue(cursor.next())
        message = cursor.getMessage()
        self.assertEqual(message.timestamp, 6000000000)

        self.assertTrue(cursor.next())
        message = cursor.getMessage()
        self.assertEqual(message.timestamp, 5000000000)

        self.assertFalse(cursor.next())

        cursor.close()

    def test_Reset(self):
        stream = self.db.getStream(self.streamKeys[0])
        cursor = stream.createCursor(dxapi.SelectionOptions())

        cursor.reset(9000)
        self.assertTrue(cursor.next())
        self.assertEqual(cursor.getMessage().timestamp, self.msToNs(9000))

        cursor.reset(5000)
        self.assertTrue(cursor.next())
        self.assertEqual(cursor.getMessage().timestamp, self.msToNs(5000))

        cursor.reset(100000)
        self.checkCursorSymbols(cursor, set(self.entities.keys()))

        cursor.reset(90000, [self.entities['AAPL'], self.entities['IBM']])
        self.checkCursorSymbols(cursor, set(['AAPL', 'IBM']))

        cursor.close()

    def test_SetTimeForNewSubscription(self):
        barStream = self.db.getStream(self.streamKeys[0])
        cursor = self.db.select(0, [barStream], dxapi.SelectionOptions(), None, [])
        
        cursor.addEntities([self.entities['AAPL']])
        messages = self.read(cursor, 10)
        for m in messages:
            print(str(m.timestamp))
            self.assertGreaterEqual(m.timestamp, 0)
            self.assertLessEqual(m.timestamp, 30000000000)
        
        cursor.removeEntities([self.entities['AAPL']])
        cursor.setTimeForNewSubscriptions(100000)
        cursor.addEntities([self.entities['GOOG']])
        messages = self.read(cursor, 10)
        for m in messages:
            print(str(m.timestamp))
            self.assertGreaterEqual(m.timestamp, 100000000000)
            self.assertLessEqual(m.timestamp, 100000000000 + 3 * 10 * 1000000000)
        
        cursor.removeEntities([self.entities['GOOG']])
        cursor.setTimeForNewSubscriptions(50000)
        cursor.addEntities([self.entities['IBM']])
        messages = self.read(cursor, 10)
        for m in messages:
            print(str(m.timestamp))
            self.assertGreaterEqual(m.timestamp, 50000000000)
            self.assertLessEqual(m.timestamp, 50000000000 + 3 * 10 * 1000000000)

    def test_SubscribeTypes(self):
        barStream = self.db.getStream(self.streamKeys[0])
        tradeBBOStream = self.db.getStream(self.streamKeys[1])
        l2Stream = self.db.getStream(self.streamKeys[2])
        cursor = self.db.select(0, [tradeBBOStream, barStream, l2Stream], dxapi.SelectionOptions(), None, None)

        # all types
        typeSet = set(self.types.values())
        self.checkCursorTypes(cursor, typeSet)

        # remove types
        self.removeAll(typeSet, [self.types['bbo'], self.types['bar']])
        cursor.removeTypes([self.types['bbo'], self.types['bar']])
        self.checkCursorTypes(cursor, typeSet)

        # add types
        typeSet.add(self.types['bbo'])
        cursor.addTypes([self.types['bbo']])
        self.checkCursorTypes(cursor, typeSet)

        # 'trade' and 'bars'
        cursor.setTypes([self.types['trade'], self.types['bar']])
        self.checkCursorTypes(cursor, set([self.types['trade'], self.types['bar']]))

        # none types
        cursor.removeTypes([self.types['trade'], self.types['bar']])
        cursor.reset(0)
        self.assertFalse(cursor.next())

        # all types
        cursor.subscribeToAllTypes()
        cursor.reset(0)
        self.checkCursorTypes(cursor, set(self.types.values()))

        cursor.close()

    def test_SubscribeEntities(self):
        tradeBBOStream = self.db.getStream(self.streamKeys[0])
        barStream = self.db.getStream(self.streamKeys[1])
        cursor = self.db.select(0, [tradeBBOStream, barStream], dxapi.SelectionOptions(), None, None)

        # all entities
        entitySet = set(self.entities.keys())
        self.checkCursorSymbols(cursor, entitySet)

        # remove AAPL, GOOG
        self.removeAll(entitySet, ['AAPL', 'GOOG'])
        cursor.removeEntities([self.entities['AAPL'], self.entities['GOOG']])
        self.checkCursorSymbols(cursor, entitySet)

        # add GOOG
        entitySet.add('GOOG')
        cursor.addEntity(self.entities['GOOG'])
        self.checkCursorSymbols(cursor, entitySet)

        # remove IBM
        entitySet.remove('IBM')
        cursor.removeEntity(self.entities['IBM'])
        self.checkCursorSymbols(cursor, entitySet)

        # add AAPL, IBM
        self.addAll(entitySet, ['AAPL', 'IBM'])
        cursor.addEntities([self.entities['AAPL'], self.entities['IBM']])
        self.checkCursorSymbols(cursor, entitySet)

        # clear all
        cursor.clearAllEntities()
        self.assertFalse(cursor.next())

        # subscribe all
        cursor.subscribeToAllEntities()
        cursor.reset(0)
        self.checkCursorSymbols(cursor, set(self.entities.keys()))

        cursor.close()

    def test_SubscribeTypeAndEntities(self):
        barStream = self.db.getStream(self.streamKeys[0])
        tradeBBOStream = self.db.getStream(self.streamKeys[1])
        l2Stream = self.db.getStream(self.streamKeys[2])
        cursor = self.db.select(0,
                                [tradeBBOStream, barStream, l2Stream],
                                dxapi.SelectionOptions(),
                                [self.types['bbo'], self.types['trade']],
                                [self.entities['AAPL'], self.entities['IBM']])

        #
        typeSet = set([self.types['bbo'], self.types['trade']])
        entitySet = set(['AAPL', 'IBM'])
        self.checkCursorTypesAndSymbols(cursor, typeSet, entitySet)

        # remove bbo type and ibm entity
        typeSet.remove(self.types['bbo'])
        entitySet.remove('IBM')
        cursor.remove([self.types['bbo']], [self.entities['IBM']])
        self.checkCursorTypesAndSymbols(cursor, typeSet, entitySet)

        # remove all
        cursor.remove(list(self.types.values()), list(self.entities.values()))
        self.assertFalse(cursor.next())

        # add types and some entities
        cursor.add(list(self.types.values()), [self.entities['GOOG'], self.entities['IBM']])
        cursor.reset(0)
        self.checkCursorTypesAndSymbols(cursor, set(self.types.values()), set(['GOOG', 'IBM']))

        cursor.close()

    def test_SubscribeStreams(self):
        barStream = self.db.getStream(self.streamKeys[0])
        tradeBBOStream = self.db.getStream(self.streamKeys[1])
        l2Stream = self.db.getStream(self.streamKeys[2])

        cursor = self.db.select(0, [tradeBBOStream], dxapi.SelectionOptions(), None, None)

        # trade streams
        typeSet = set([self.types['trade'], self.types['bbo']])
        self.checkCursorTypes(cursor, typeSet)

        # add l2 and bars streams subscription
        self.addAll(typeSet, [self.types['l2'], self.types['bar']])
        cursor.addStreams([l2Stream, barStream])
        cursor.reset(0)     # why we must reset here?
        self.checkCursorTypes(cursor, typeSet)

        # remove tradeBBO and l2
        self.removeAll(typeSet, [self.types['trade'], self.types['bbo'], self.types['l2']])
        cursor.removeStreams([l2Stream, tradeBBOStream])
        cursor.reset(0)  # why we must reset here?
        self.checkCursorTypes(cursor, typeSet)

        # remove all
        cursor.removeAllStreams()
        cursor.reset(0)  # why we must reset here?
        self.assertFalse(cursor.next())

        cursor.close()

    def test_GetCurrentStreamKey(self):
        barStream = self.db.getStream(self.streamKeys[0])
        tradeBBOStream = self.db.getStream(self.streamKeys[1])
        l2Stream = self.db.getStream(self.streamKeys[2])

        cursor = self.db.select(0, [barStream, tradeBBOStream, l2Stream], dxapi.SelectionOptions(), None, None)
        while cursor.next():
            stream = cursor.getCurrentStreamKey()
            typeName = cursor.getMessage().typeName
            
            if "L2Message" in typeName:
                self.assertEqual(stream, "l2")
            elif "TradeMessage" in typeName or "BestBidOfferMessage" in typeName:
                self.assertEqual(stream, "tradeBBO")
            elif "BarMessage" in typeName:
                self.assertEqual(stream, "bars1min")

if __name__ == '__main__':
    unittest.main()