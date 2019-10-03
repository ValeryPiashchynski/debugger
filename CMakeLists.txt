CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
set (CMAKE_CXX_STANDARD 17)
PROJECT(debugger)

ADD_EXECUTABLE(debugger src/main.cpp include/main.h src/debugger.cpp include/debugger.h src/breakpoint.cpp include/breakpoint.h)