[![Build Status](https://travis-ci.org/TyRoXx/tyroxx-blog-generator.svg?branch=master)](https://travis-ci.org/TyRoXx/tyroxx-blog-generator)
[![Build status](https://ci.appveyor.com/api/projects/status/9tb4f2fnckm6bx3e/branch/master?svg=true)](https://ci.appveyor.com/project/TyRoXx/tyroxx-blog-generator/branch/master)

# Installation
* Get conan with "pip install conan"
* install the package: cmake, build-essential
* run 'conan install [source] --build=missing' in the build directory
* for QtCreator, run 'cmake -G "CodeBlocks - Unix Makefiles" [source]' in the build directory
* for Visual Studio, use the CMake GUI to generate your solution as usual and choose the right build directory
* requires GCC version >= 4.9 or Visual Studio 2015

# How to format the code
* make sure that clang-format 3.7 is installed
* build `clang-format` in Visual Studio or build the `clang-format` target with `make` or `ninja`

# Changelog
See a list of the most important changes that were made

## Version 0
* Multipage generation (currently just two)
* Links between the two pages in some kind of menu
* code snippets have code highlighting now

# Todo
* Add an external file for CSS file (done)
* See todos on the page itself (code formatting and execution)
* More pages!!!
* highlight preprocessor
