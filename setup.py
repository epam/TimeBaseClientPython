from setuptools import setup, find_packages, dist
import configparser

with open('project.properties', 'r') as f:
    config_string = '[version_section]\n' + f.read()
   
config = configparser.ConfigParser()
config.read_string(config_string)

setup(name='timebase-client',
      version=config['version_section']['version'],
      packages=find_packages(),
      package_data={'tbapi': ['project.properties',
            'windows/x64/py36/*.pyd',
            'windows/x64/py37/*.pyd',
            'windows/x64/py38/*.pyd',
            'windows/x64/py39/*.pyd',
            'windows/x64/py310/*.pyd',
            'windows/x64/py36/*.dll',
            'windows/x64/py37/*.dll',
            'windows/x64/py38/*.dll',
            'windows/x64/py39/*.dll',
            'windows/x64/py310/*.dll',
            'linux/x64/py36/*.so',
            'linux/x64/py37/*.so',
            'linux/x64/py38/*.so',
            'linux/x64/py39/*.so',
            'linux/x64/py310/*.so',
            'darwin/x64/py36/*.so',
            'darwin/x64/py37/*.so',
            'darwin/x64/py38/*.so',
            'darwin/x64/py39/*.so',
            'darwin/x64/py310/*.so',
            'darwin/x64/py36/*.dylib',
            'darwin/x64/py37/*.dylib',
            'darwin/x64/py38/*.dylib',
            'darwin/x64/py39/*.dylib',
            'darwin/x64/py310/*.dylib']},
      include_package_data=True,
      zip_safe=False,
      description='Python Timebase client API',
      long_description='Python Timebase client API',
      long_description_content_type='text/markdown',
      keywords='timebase, database',
      #url='',
      author='Epam',
      classifiers=[
          "Operating System :: Microsoft :: Windows",
          "Operating System :: POSIX :: Linux",
          "Operating System :: MacOS",
          "Programming Language :: Python :: 3.6",
          "Programming Language :: Python :: 3.7",
          "Programming Language :: Python :: 3.8",
          "Programming Language :: Python :: 3.9",
          "Programming Language :: Python :: 3.10"
      ],
      python_requires='>=3.6,<3.11',
      platforms=['Windows', 'Linux', 'MacOS']
)
