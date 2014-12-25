from distutils.core import setup
from distutils.extension import Extension

import os
path = './'
srcs = [path + i for i in os.listdir(path) if i.endswith('cpp') or i.endswith('hpp')]
inc = ['/usr/include/python2.7','/usr/include/boost', '/usr/include']
lib = ['/usr/lib', '/opt/local/lib']

setup(name = 'mygraph', ext_modules =
      [
          Extension("mygraph",
                    define_macros = [('MAJOR_VERSION', '1'), ('MINOR_VERSION', '0')],
                    include_dirs = inc,
                    library_dirs = lib,
                    libraries=['boost_python-mt', 'python2.7', 'pqxx', 'hdf5', 'hdf5_hl'],
                    sources = srcs)
      ])

