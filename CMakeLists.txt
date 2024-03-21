# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly

cmake_minimum_required(VERSION 3.5)

# Initialize EXTRA_COMPONENT_DIRS as an empty list
set(EXTRA_COMPONENT_DIRS)

# Append values to EXTRA_COMPONENT_DIRS list
list(APPEND EXTRA_COMPONENT_DIRS
    ../components/esp-serial-flasher/
    $ENV{IDF_PATH}/examples/common_components/protocol_examples_common
)


include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(OTA_UPDATE_FPGA_USING_ESP32)

# There are issues with ESP-IDF 4.4 and -Wunused-parameter
if ("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER "4.4")
    idf_component_get_property(flasher esp-serial-flasher COMPONENT_LIB)

    target_compile_options(${flasher}
    PRIVATE
        -Wunused-parameter
        -Wshadow
    )
endif()
