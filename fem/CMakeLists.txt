cmake_minimum_required(VERSION 2.8)

project(fem)

add_definitions(-std=c++0x)

set(HEADER_FILES
    utils.h
        common.h
    consts.h
    )

set(SOURCE_FILES
       solver1.cpp solver2.cpp solver3.cpp solver4.cpp solver5.cpp solver6.cpp solver7.cpp solver8.cpp solver9.cpp solver10.cpp solver11.cpp consts.cpp)

add_library(fem STATIC ${SOURCE_FILES} ${HEADER_FILES})
