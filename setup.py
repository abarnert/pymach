from distutils.core import setup, Extension

mach = Extension('mach',
                 sources = ['mach.c']
    )

setup(name = 'mach',
      version = '1.0',
      description = 'Wrap some low-level Mach stuff',
      ext_modules = [mach]
    )
