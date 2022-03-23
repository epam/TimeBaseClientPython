import unittest
import datetime
import sys, os

#from HtmlTestRunner import HTMLTestRunner
import HTMLTestRunner as HTMLTestRunner

testmodules = [
    'TestTickDB',
    'TestStream',
    'TestLoader',
    'TestCursor',
    'TestPackageHeader',
    'TestQQL',
    'TestMemoryManagement',
    'TestNextIfAvailable',
    'TestMultithreaded'
]

suite = unittest.TestSuite()

for t in testmodules:
    try:
        # If the module defines a suite() function, call it to get the suite.
        mod = __import__(t, globals(), locals(), ['suite'])
        suitefn = getattr(mod, 'suite')
        suite.addTest(suitefn())
    except (ImportError, AttributeError):
        # else, just load all the test cases from the module.
        suite.addTest(unittest.defaultTestLoader.loadTestsFromName(t))

testdir = os.path.dirname(__file__)
if testdir != "":
    testdir = testdir + '/'
reportsDir =  testdir + 'reports'
if not os.path.exists(reportsDir):
    os.makedirs(reportsDir)
reportFile = reportsDir + '/dxapi-test-report-' + datetime.datetime.now().strftime('%d%m%Y-%H%M%S') + '.html'
fileStream = open(reportFile, 'w')
version = "2"
if sys.version.startswith("3.6"):
    version = "3.6"
elif sys.version.startswith("3.7"):
    version = "3.7"
title = 'Dxapi test report (Python version: ' + (version) + ')'
runner = HTMLTestRunner.HTMLTestRunner(stream = fileStream, title = title)
result = runner.run(suite)

print('Tests finished')
print('Errors: ' + str(result.error_count) + ", Failures: " + str(result.failure_count) + ", Tests OK: " + str(result.success_count))
print('See reports in "' + reportFile + '"')

if result.error_count > 0 or result.failure_count > 0:
    sys.exit(-1)
else:
    sys.exit(0)


