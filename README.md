[![Build Status](https://travis-ci.org/TyRoXx/tyroxx-blog-generator.svg?branch=master)](https://travis-ci.org/TyRoXx/tyroxx-blog-generator)
[![Build status](https://ci.appveyor.com/api/projects/status/9tb4f2fnckm6bx3e/branch/master?svg=true)](https://ci.appveyor.com/project/TyRoXx/tyroxx-blog-generator/branch/master)

# Installation
* get conan with `pip install conan` or from https://www.conan.io/downloads
* install the package: cmake, build-essential
* run `conan install [source] --build=missing` in the build directory
* for QtCreator, run `cmake -G "CodeBlocks - Unix Makefiles" [source]` in the build directory
* for Visual Studio, use the CMake GUI to generate your solution as usual and choose the right build directory
* requires GCC version >= 4.9 or Visual Studio 2015

# How to format the code
* make sure that clang-format 3.7 is installed
  * http://releases.llvm.org/download.html#3.7.1
  * or run `sudo apt-get install clang-format-3.7`
* Visual Studio: build the `clang-format` target
* Linux etc:
    * `make clang-format`
    * or `ninja clang-format` if you are using ninja
