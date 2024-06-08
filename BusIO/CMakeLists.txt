# Adafruit Bus IO Library
# https://github.com/adafruit/Adafruit_BusIO
# MIT License

# cmake_minimum_required(VERSION 3.5)

# idf_component_register(SRCS "Adafruit_I2CDevice.cpp" "Adafruit_BusIO_Register.cpp" "Adafruit_SPIDevice.cpp" 
#                        INCLUDE_DIRS "."
#                        REQUIRES arduino)

# project(Adafruit_BusIO)

cmake_minimum_required(VERSION 3.5)

project(Adafruit_BusIO)

add_library(Adafruit_BusIO STATIC
    Adafruit_I2CDevice.cpp
    Adafruit_BusIO_Register.cpp
    # Adafruit_SPIDevice.cpp
)

target_include_directories(Adafruit_BusIO PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
