CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
set (CMAKE_CXX_STANDARD 17)

set(INCLUDE_DIR ./include)
set(SOURCE_DIR ./src)

include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})
include_directories(/usr/include)

set(
        SOURCE_FILES
        ${INCLUDE_DIR}/main.h
        ${INCLUDE_DIR}/breakpoint.h
        ${INCLUDE_DIR}/debugger.h

        ${SOURCE_DIR}/main.cpp
        ${SOURCE_DIR}/debugger.cpp
        ${SOURCE_DIR}/breakpoint.cpp
)

PROJECT(debugger)

ADD_EXECUTABLE(debugger ${SOURCE_FILES})