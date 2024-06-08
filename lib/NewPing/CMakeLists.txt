cmake_minimum_required(VERSION 3.13)

project(NewPing)

# Add the LoRa library
add_library(NewPing STATIC
    NewPing.cpp
)

# Inherit the include directories from the Pico SDK
target_link_libraries(NewPing pico_stdlib)

# Include current directory for the library's headers
target_include_directories(NewPing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
