import unittest
import servertest
import generators
import time
import random
import tbapi

class TestLoadData(servertest.TBServerTest):

    streamKeys = [
        'bars1min', 'tradeBBO', 'l2'
    ]

    def isclose(self, a, b, rel_tol=1e-09, abs_tol=0.0):
        return abs(a-b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol)

    def test_LoadNulls(self):
        key = 'alltypes'
        try:
            stream = self.createStreamQQL(key)
            self.assertIsNotNone(stream)

            with stream.tryLoader(tbapi.LoadingOptions()) as loader:
                message = tbapi.InstrumentMessage()
                message.symbol = "AAA"
                message.instrumentType = 'EQUITY'
                message.typeName = 'deltix.qsrv.test.messages.AllSimpleTypesMessage'
                loadCount = 0
                for i in range(10):
                    message.asciiTextField = 'asciiText'
                    message.asciiTextNullableField = None if i % 2 == 0 else message.asciiTextField
                    message.binaryField = bytearray([10,20,30,40,50,60,70])
                    message.binaryNullableField = None if i % 2 == 0 else message.binaryField
                    message.boolField = True
                    message.boolNullableField = None if i % 2 == 0 else message.boolField
                    message.byteField = 42
                    message.byteNullableField = None if i % 2 == 0 else message.byteField
                    message.decimalField = 42.42
                    message.decimalNullableField = None if i % 2 == 0 else message.decimalField
                    message.doubleField = 43.43
                    message.doubleNullableField = None if i % 2 == 0 else message.doubleField
                    message.enumField = 'THREE'
                    message.enumNullableField = None if i % 2 == 0 else message.enumField
                    message.floatField = 44.44
                    message.floatNullableField = None if i % 2 == 0 else message.floatField
                    message.intField = 1234 + i
                    message.intNullableField = None if i % 2 == 0 else message.intField
                    message.longField = 12345 + i
                    message.longNullableField = None if i % 2 == 0 else message.longField
                    message.shortField = 123 + i
                    message.shortNullableField = None if i % 2 == 0 else message.shortField
                    message.textAlphaNumericField = 'ABC;D543'
                    message.textAlphaNumericNullableField = None if i % 2 == 0 else message.textAlphaNumericField
                    message.textField = 'sfsgfttjdfsrg45634g@#Df'
                    message.textNullableField = None if i % 2 == 0 else message.textField
                    message.timeOfDayField = 2
                    message.timeOfDayNullableField = None if i % 2 == 0 else message.timeOfDayField
                    message.timestampField = 123456
                    message.timestampNullableField = None if i % 2 == 0 else message.timestampField

                    loader.send(message)
                    loadCount += 1

            with stream.trySelect(0, tbapi.SelectionOptions(), None, None) as cursor:
                readCount = 0
                while cursor.next():
                    message = cursor.getMessage()

                    self.assertEquals(message.asciiTextField, 'asciiText')
                    self.assertEquals(message.binaryField, bytearray([10,20,30,40,50,60,70]))
                    self.assertEquals(message.boolField, True)
                    self.assertEquals(message.byteField, 42)
                    self.assertTrue(self.isclose(message.decimalField, 42.42, 0.0001))
                    self.assertTrue(self.isclose(message.doubleField, 43.43, 0.0001))
                    self.assertTrue(self.isclose(message.floatField, 44.44, 0.0001))
                    self.assertEquals(message.enumField, 'THREE')
                    self.assertEquals(message.intField, 1234 + readCount)
                    self.assertEquals(message.longField, 12345 + readCount)
                    self.assertEquals(message.shortField, 123 + readCount)
                    self.assertEquals(message.textAlphaNumericField, 'ABC;D543')
                    self.assertEquals(message.textField, 'sfsgfttjdfsrg45634g@#Df')
                    self.assertEquals(message.timeOfDayField, 2)
                    self.assertEquals(message.timestampField, 123456)

                    if readCount % 2 == 0:
                        self.assertIsNone(message.binaryNullableField)
                        self.assertIsNone(message.boolNullableField)
                        self.assertIsNone(message.byteNullableField)
                        self.assertIsNone(message.decimalNullableField)
                        self.assertIsNone(message.doubleNullableField)
                        self.assertIsNone(message.enumNullableField)
                        self.assertIsNone(message.floatNullableField)
                        self.assertIsNone(message.intNullableField)
                        self.assertIsNone(message.longNullableField)
                        self.assertIsNone(message.shortNullableField)
                        self.assertIsNone(message.asciiTextNullableField)
                        self.assertIsNone(message.textAlphaNumericNullableField)
                        self.assertIsNone(message.textNullableField)
                        self.assertIsNone(message.timeOfDayNullableField)
                        self.assertIsNone(message.timestampNullableField)
                    else:
                        self.assertEquals(message.asciiTextNullableField, 'asciiText')
                        self.assertEquals(message.binaryNullableField, bytearray([10,20,30,40,50,60,70]))
                        self.assertEquals(message.boolNullableField, True)
                        self.assertEquals(message.byteNullableField, 42)
                        self.assertTrue(self.isclose(message.decimalNullableField, 42.42, 0.0001))
                        self.assertTrue(self.isclose(message.doubleNullableField, 43.43, 0.0001))
                        self.assertTrue(self.isclose(message.floatNullableField, 44.44, 0.0001))
                        self.assertEquals(message.enumNullableField, 'THREE')
                        self.assertEquals(message.intNullableField, 1234 + readCount)
                        self.assertEquals(message.longNullableField, 12345 + readCount)
                        self.assertEquals(message.shortNullableField, 123 + readCount)
                        self.assertEquals(message.textAlphaNumericNullableField, 'ABC;D543')
                        self.assertEquals(message.textNullableField, 'sfsgfttjdfsrg45634g@#Df')
                        self.assertEquals(message.timeOfDayNullableField, 2)
                        self.assertEquals(message.timestampNullableField, 123456)

                    readCount = readCount + 1
        finally:
            self.deleteStream(key)

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
