cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Ver.2)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
        Core/my_library/my_device/key/key.c
        Core/my_library/my_device/key/key.h
        Core/my_library/my_device/key/key_driver.h
        Core/my_library/my_device/key/key_driver.c
        Core/my_library/my_function/timer.c
        Core/my_library/my_function/timer.h
        Core/my_library/my_device/E18_D80NK/E18_D80NK.c
        Core/my_library/my_device/E18_D80NK/E18_D80NK.h
        Core/my_library/my_device/Buzzer/Buzzer.c
        Core/my_library/my_device/Buzzer/Buzzer.h
        Core/my_library/my_device/TM1650/TM1650.c
        Core/my_library/my_device/TM1650/TM1650.h
        Core/my_library/my_device/Battery/Battery.c
        Core/my_library/my_device/Battery/Battery.h
        Core/my_library/device_state.c
        Core/my_library/device_state.h
        Core/my_library/my_function/delay.c
        Core/my_library/my_function/delay.h
        Core/my_library/my_driver/soft_i2c.c
        Core/my_library/my_driver/soft_i2c.h)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
        Core/my_library
        Core/my_library/my_device/key
        Core/my_library/my_device/E18_D80NK
        Core/my_library/my_device/Buzzer
        Core/my_library/my_device/TM1650
        Core/my_library/my_device/Battery
        Core/my_library/my_function
        Core/my_library/my_driver

)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
