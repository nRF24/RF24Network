#!/usr/bin/env python

from distutils.core import setup, Extension

module_RF24Network = Extension('RF24Network',
            libraries = ['rf24network', 'boost_python'],
            sources = ['pyRF24Network.cpp'])

setup(name='RF24Network',
    version='1.0',
    ext_modules=[module_RF24Network]
      )
