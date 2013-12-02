# Mac OS X specific configuration
IF(APPLE)
	SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.6")
	SET(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX10.6.sdk/")
	SET(CMAKE_OSX_ARCHITECTURES "i386;x86_64")
ENDIF()

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Tippi)

# Source directories
SET(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
SET(TEST_SOURCE_DIR ${CMAKE_SOURCE_DIR}/test/src)

FILE(GLOB_RECURSE SOURCE
    "${SOURCE_DIR}/*.h"
    "${SOURCE_DIR}/*.cpp"
)

FILE(GLOB_RECURSE TEST_SOURCE
    "${TEST_SOURCE_DIR}/*.h"
    "${TEST_SOURCE_DIR}/*.cpp"
)

# Library configuration
SET(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
SET(LIB_BIN_DIR ${LIB_DIR}/bin)
SET(LIB_INCLUDE_DIR ${LIB_DIR}/include)
SET(LIB_SOURCE_DIR ${LIB_DIR}/src)

INCLUDE(cmake/GTest.cmake)
INCLUDE(cmake/GMock.cmake)

ADD_LIBRARY(gtest ${LIB_GTEST_SOURCE} ${LIB_INCLUDE_DIR})
ADD_LIBRARY(gmock ${LIB_GMOCK_SOURCE} ${LIB_INCLUDE_DIR})

# Include directories
INCLUDE_DIRECTORIES("${LIB_INCLUDE_DIR}" "${SOURCE_DIR}" "${TEST_SOURCE_DIR}")

# Compiler configuration
IF(MSVC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /EHsc /MP /D _CRT_SECURE_NO_DEPRECATE /D _CRT_NONSTDC_NO_DEPRECATE")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3 /MP")
ELSE()
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x")

	# Xcode requires these flags to allow debugging
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0 -fno-inline")

	# Hide some bogus warnings and enable color diagnostics if ccache is used with clang
	IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments -fcolor-diagnostics")
	ENDIF()
ENDIF()

# Tippi executable
ADD_EXECUTABLE(Tippi ${SOURCE})

# Tippi-Test executable
# We need to remove main.cpp to avoid linker errors (duplicate symbol _main)
LIST(REMOVE_ITEM SOURCE "${CMAKE_SOURCE_DIR}/src/main.cpp")
ADD_EXECUTABLE(Tippi-Test ${SOURCE} ${TEST_SOURCE})
TARGET_LINK_LIBRARIES(Tippi-Test gtest gmock)
SET_TARGET_PROPERTIES(Tippi-Test PROPERTIES COMPILE_DEFINITIONS "TESTING")


ADD_CUSTOM_COMMAND(TARGET Tippi-Test POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/test/data" $<TARGET_FILE_DIR:Tippi-Test>/data
)
