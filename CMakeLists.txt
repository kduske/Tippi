INCLUDE(cmake/Utils.cmake)

# Mac OS X specific configuration
IF(APPLE)
	SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.6")
	SET(CMAKE_OSX_ARCHITECTURES "i386;x86_64")
ENDIF()

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Tippi)

# Common source files
SET(LIB_COMMON_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
SET(LIB_COMMON_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src)
FILE(GLOB_RECURSE LIB_COMMON_SOURCE
    "${LIB_COMMON_SOURCE_DIR}/*.cpp"
)
FILE(GLOB_RECURSE LIB_COMMON_HEADER
    "${LIB_COMMON_SOURCE_DIR}/*.h"
)

# App source dir
SET(APPS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/apps)

# Test source files
SET(TEST_SOURCE_DIR ${CMAKE_SOURCE_DIR}/test/src)
FILE(GLOB_RECURSE TEST_SOURCE
    "${TEST_SOURCE_DIR}/*.h"
    "${TEST_SOURCE_DIR}/*.cpp"
)

# Library configuration
SET(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
SET(LIB_BIN_DIR ${LIB_DIR}/bin)
SET(LIB_INCLUDE_DIR ${LIB_DIR}/include)
SET(LIB_SOURCE_DIR ${LIB_DIR}/src)

INCLUDE(cmake/GetOpt.cmake)
INCLUDE(cmake/GTest.cmake)
INCLUDE(cmake/GMock.cmake)

ADD_LIBRARY(getopt ${LIB_GETOPT_SOURCE} ${LIB_INCLUDE_DIR})
ADD_LIBRARY(gtest ${LIB_GTEST_SOURCE} ${LIB_INCLUDE_DIR})
ADD_LIBRARY(gmock ${LIB_GMOCK_SOURCE} ${LIB_INCLUDE_DIR})
ADD_LIBRARY(common ${LIB_COMMON_SOURCE} ${LIB_COMMON_HEADER})

# Include directories
INCLUDE_DIRECTORIES("${LIB_INCLUDE_DIR}" "${LIB_COMMON_INCLUDE_DIR}")

# Compiler configuration
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    SET(COMPILER_IS_CLANG TRUE)
ENDIF()
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    SET(COMPILER_IS_GNU TRUE)
ENDIF()
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    SET(COMPILER_IS_MSVC TRUE)
ENDIF()

IF(COMPILER_IS_CLANG)
    MESSAGE(STATUS "Compiler is Clang")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -pedantic -Wno-format -Wno-variadic-macros -Wno-c++11-long-long -Wno-c99-extensions -Wno-padded -Wno-unused-parameter -Wno-global-constructors -Wno-exit-time-destructors -Wno-weak-vtables -Wno-float-equal -Wno-used-but-marked-unused -Wno-format-nonliteral -Wno-missing-noreturn -stdlib=libstdc++")
ELSEIF(COMPILER_IS_GNU)
    MESSAGE(STATUS "Compiler is GNU")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-format -Wno-variadic-macros -Wno-padded -Wno-unused-parameter -Wno-float-equal -Wno-format-nonliteral -Wno-missing-noreturn")
ELSEIF(COMPILER_IS_MSVC)
    MESSAGE(STATUS "Compiler is MSVC")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /EHsc /MP /D _CRT_SECURE_NO_DEPRECATE /D _CRT_NONSTDC_NO_DEPRECATE")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3 /MP")
ELSE()
    MESSAGE(FATAL_ERROR "Unsupported compiler detected.")
ENDIF()

IF(CMAKE_GENERATOR STREQUAL "Xcode")
    # Xcode requires these flags to allow debugging
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0 -fno-inline")

    # Xcode should treat the library header files as system headers
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem ${LIB_INCLUDE_DIR}")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -isystem ${LIB_INCLUDE_DIR}")
ELSE()
    INCLUDE_DIRECTORIES(SYSTEM "${LIB_INCLUDE_DIR}")
ENDIF()

# net2beh executable
ADD_EXECUTABLE(net2beh "${APPS_SOURCE_DIR}/net2beh.cpp")
TARGET_LINK_LIBRARIES(net2beh common getopt)
SET_XCODE_ATTRIBUTES(net2beh)

# net2cl executable
ADD_EXECUTABLE(net2cl "${APPS_SOURCE_DIR}/net2cl.cpp")
TARGET_LINK_LIBRARIES(net2cl common getopt)
SET_XCODE_ATTRIBUTES(net2cl)

ADD_CUSTOM_COMMAND(TARGET net2cl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/test/data" $<TARGET_FILE_DIR:runtest>/data
)

# chksim executable
ADD_EXECUTABLE(chksim "${APPS_SOURCE_DIR}/chksim.cpp")
TARGET_LINK_LIBRARIES(chksim common getopt)
SET_XCODE_ATTRIBUTES(chksim)

# runtest executable
ADD_EXECUTABLE(runtest ${SOURCE} ${TEST_SOURCE})
TARGET_LINK_LIBRARIES(runtest common gtest gmock)
SET_TARGET_PROPERTIES(runtest PROPERTIES COMPILE_DEFINITIONS "TESTING")
SET_XCODE_ATTRIBUTES(runtest)

ADD_CUSTOM_COMMAND(TARGET runtest POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/test/data" $<TARGET_FILE_DIR:runtest>/data
)
