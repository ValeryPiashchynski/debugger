CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
PROJECT(debugger)

set(CMAKE_CXX_STANDARD 17)

set(INCLUDE_DIR ./include)
set(SOURCE_DIR ./src)

include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})
include_directories(/usr/include)

find_library(LIBNOISE liblinenoise.so.0 /usr/lib64/)

link_libraries("${LIBNOISE}")

set(
        SOURCE_FILES
        ${INCLUDE_DIR}/main.h
        ${INCLUDE_DIR}/breakpoint.h
        ${INCLUDE_DIR}/debugger.h
        ${INCLUDE_DIR}/registers.h

        ${SOURCE_DIR}/main.cpp
        ${SOURCE_DIR}/debugger.cpp
        ${SOURCE_DIR}/breakpoint.cpp
)

ADD_EXECUTABLE(debugger ${SOURCE_FILES})

ADD_EXECUTABLE(sample sample/main.cpp sample/main.h)