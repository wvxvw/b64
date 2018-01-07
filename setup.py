#!/usr/bin/env python3.6
# -*- coding: utf-8 -*-

from setuptools import setup, Extension
from Cython.Distutils import build_ext


setup(
    install_requires=['cython'],
    packages=['b64'],
    zip_safe=False,
    name='b64',
    version='0.0.1',
    description='Base64 encoder and decoder for reading and riting files',
    author='Oleg Sivokon',
    author_email='olegs@traiana.com',
    url='TBD',
    license='PROPRIETARY',
    cmdclass={"build_ext": build_ext},
    scripts=['bin/b64'],
    ext_modules=[
        Extension(
            'b64.wrapped',
            ['b64/lib/base64.c', 'b64/wrapped.pyx'],
            libraries=[],
            include_dirs=[]
        )
    ]
)
