cmake_minimum_required(VERSION 3.27)
project(EasyDraft)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(EasyDraft
        main.cpp
        TemplateParser.cpp
        TemplateParser.h)
