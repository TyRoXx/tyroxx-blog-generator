cmake_minimum_required(VERSION 2.8)
project(tyroxx_blog)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

if(UNIX)
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		add_definitions("-std=c++1y")
	else()
		execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
		if(GCC_VERSION VERSION_GREATER 4.7)
			add_definitions(-std=c++1y)
		else()
			add_definitions(-std=c++0x)
		endif()
	endif()
	add_definitions("-Wall -Wextra -Wconversion -Wvla -Werror -Wundef")
	add_definitions("-Wno-missing-field-initializers")

	option(FILESERVER_PEDANTIC "pass -pedantic to the compiler (may generate useless warnings)" ON)
	if(FILESERVER_PEDANTIC)
		add_definitions("-pedantic")
	endif()
endif()

if(MSVC)
	#make Visual C++ 2013 build in parallel
	add_definitions("/MP")

	#we want more warnings
	add_definitions("/W4")

	#disable "'std::_Copy_n': Function call with parameters that may be unsafe"
	add_definitions("/wd4996")

	#workaround for Boost 1.55 Context error LNK2026
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	set(CONAN_SYSTEM_INCLUDES ON)
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup()
else()
	if(MSVC)
		set(Boost_USE_STATIC_LIBS ON) 
	endif()
	find_package(Boost REQUIRED COMPONENTS system filesystem date_time regex coroutine context thread chrono program_options)
	if(UNIX)
		add_definitions("-DBOOST_TEST_DYN_LINK")
	endif()
	include_directories(SYSTEM ${Boost_INCLUDE_DIR})
	link_directories(${Boost_LIBRARY_DIR})

	set(SILICIUM_INCLUDE_DIR "" CACHE PATH "")
	include_directories(SYSTEM ${SILICIUM_INCLUDE_DIR})

	set(VENTURA_INCLUDE_DIR "" CACHE PATH "")
	include_directories(SYSTEM ${VENTURA_INCLUDE_DIR})
endif()

if(WIN32)
	#Boost.Asio wants this for no reason
	add_definitions("-D_WIN32_WINDOWS")

	add_definitions("-DBOOST_ASIO_HAS_IOCP")
endif()

include_directories(.)

add_subdirectory("tests")
add_subdirectory("html_generator")
add_subdirectory("system_test")

file(GLOB snippets "snippets/*.*")
set(formatted ${formatted} ${snippets})

find_program(CLANG_FORMAT NAMES clang-format clang-format-3.7 PATHS "C:/Program Files/LLVM/bin")
add_custom_target(clang-format COMMAND ${CLANG_FORMAT} -i ${formatted} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
