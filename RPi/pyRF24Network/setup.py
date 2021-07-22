#!/usr/bin/env python
import os
from setuptools import setup, Extension
from sys import version_info

if version_info >= (3,):
    BOOST_LIB = "boost_python3"
else:
    BOOST_LIB = "boost_python"

# NOTE current repo directory structure requires the use of
# `python3 setup.py build` and `python3 setup.py install`
# where `pip3 install ./pyRF24` copies pyRF24 directory to
# `tmp` folder that doesn't have the needed `../Makefile.inc`
# NOTE can't access "../Makefile.inc" from working dir because
# it's relative. Brute force absolute path dynamically.
git_dir = os.path.split(os.path.abspath(os.getcwd()))[0]
git_dir = os.path.split(git_dir)[0]  # remove the "RPi" dir from working path

# get LIB_VERSION from library.properties file for Arduino IDE
version = "1.0"
with open(os.path.join(git_dir, "library.properties"), "r") as f:
    for line in f.read().splitlines():
        if line.startswith("version"):
            version = line.split("=")[1]


long_description = """
.. warning:: This python wrapper for the RF24Network C++ library was not intended
    for distribution on pypi.org. If you're reading this, then this package
    is likely unauthorized or unofficial.
"""


setup(
    name="RF24Network",
    version=version,
    license_files=os.path.join(git_dir, "LICENSE"),
    long_description=long_description,
    long_description_content_type="text/x-rst",
    classifiers=[
        "Programming Language :: Python :: 2",
        "Programming Language :: Python :: 3",
        "Programming Language :: C++",
        "License :: OSI Approved :: GNU General Public License v2 (GPLv2)",
    ],
    ext_modules=[
        Extension(
            "RF24Network",
            sources=["pyRF24Network.cpp"],
            libraries=["rf24", "rf24network", BOOST_LIB],
        )
    ],
)
