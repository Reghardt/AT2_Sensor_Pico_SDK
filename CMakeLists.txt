# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# recomended from json library
set(JSON_BuildTests OFF CACHE INTERNAL "")

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
   set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(PICO_SDK_PATH ${USERHOME}/.pico-sdk/sdk/1.5.1)
set(PICO_TOOLCHAIN_PATH ${USERHOME}/.pico-sdk/toolchain/13_2_Rel1)
if(WIN32)
    set(pico-sdk-tools_DIR ${USERHOME}/.pico-sdk/tools/1.5.1)
    include(${pico-sdk-tools_DIR}/pico-sdk-tools-config.cmake)
    include(${pico-sdk-tools_DIR}/pico-sdk-tools-config-version.cmake)
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(pico_sdk_sensor C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_subdirectory(lib/Pi-Pico-SDK-LoRa)
add_subdirectory(lib/NewPing)
add_subdirectory(lib/json)
add_executable(pico_sdk_sensor pico_sdk_sensor.cpp )


pico_set_program_name(pico_sdk_sensor "pico_sdk_sensor")
pico_set_program_version(pico_sdk_sensor "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(pico_sdk_sensor 1)
pico_enable_stdio_usb(pico_sdk_sensor 0)

# Add the standard library to the build
target_link_libraries(pico_sdk_sensor
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(pico_sdk_sensor PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(pico_sdk_sensor 
        hardware_spi
        hardware_i2c
        hardware_watchdog
        LoRa
        NewPing
        nlohmann_json
        )

pico_add_extra_outputs(pico_sdk_sensor)

