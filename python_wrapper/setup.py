from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize
from setuptools import setup, find_packages
import sys
import os
import platform
import sysconfig

def get_extra_link_args():
    if platform.system() == 'Windows':
        return []
    else:
        return ["-Wl,-rpath=$ORIGIN/lib/."]

requirements = ["PyOpenGL", "glfw", "Cython"] 

version = '0.0.4'

readme = 'Coming Soon ...'

required_libraries = ['cgl', 'glfw3', 'glad', 'freetype']

if platform.system() == 'Windows':
    required_libraries.append('user32')
    required_libraries.append('Gdi32')
    required_libraries.append('Shell32')

# link to gl library on linux
if platform.system() == 'Linux':
    required_libraries.append('GL')

extension = Extension(
    name="cgl",
    sources=["cgl_wrapper.pyx"],
    libraries=required_libraries,
    library_dirs=["lib"],
    define_macros=[("CGL_EXCLUDE_AUDIO", None), ("CGL_EXCLUDE_NETWORKING", None), ("CGL_LOGGING_ENABLED", None)],
    include_dirs=["include", "include/glad/include", "include/glfw/include", "include/freetype/include"],
    extra_link_args=get_extra_link_args(),
)

setup(
    name = 'cgl-py',
    author = 'Jaysmito Mukherjee',
    author_email = 'jaysmito101@gmail.com',
    url = 'https://github.com/Jaysmito101/cgl',
    project_urls = {
        'Documentation': 'https://www.libcgl.tech/',
        'Issue tracker': 'https://github.com/Jaysmito101/cgl/issues',
    },
    version = version,
    packages = find_packages(),
    license = 'MIT',
    description = 'A Python Wrapper for CGL',
    long_description = readme,
    long_description_content_type = 'text/markdown',
    ext_modules=cythonize([extension]),
    include_package_data = True,
    install_requires = requirements,
    python_requires='>=3.6.0',
    package_data={
        "cgl-py": [
            "include/**.h",
            "include/**.hpp",
            "lib/**.lib",
        ]    
    },
    classifiers=[
        'Development Status :: 3 - Alpha',
        'License :: OSI Approved :: MIT License',
        'Intended Audience :: Developers',
        'Natural Language :: English',
        'Operating System :: OS Independent',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Topic :: Office/Business :: Financial',
        'Topic :: Software Development :: Libraries',
        'Topic :: Software Development :: Libraries :: Python Modules',
        'Topic :: Utilities',
        'Typing :: Typed',
    ],
)
