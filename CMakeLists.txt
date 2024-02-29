cmake_minimum_required(VERSION 3.17)
project(EasyDraft)

set(CMAKE_CXX_STANDARD 17)

# Add the KZip library
add_library(KZip INTERFACE)
target_include_directories(KZip INTERFACE ${CMAKE_SOURCE_DIR}/KZip)

# Create an executable for your project
add_executable(EasyDraft
        main.cpp
        TemplateParser.cpp
        TemplateParser.hpp
)

# Link the KZip library to your executable
target_link_libraries(EasyDraft PRIVATE KZip)
