# Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
# Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

include($ENV{IDF_PATH}/tools/cmake/idf.cmake)

function(smooth_verify_idf_path)
    if (NOT EXISTS "$ENV{IDF_PATH}/Kconfig")
        message(FATAL_ERROR "Environment IDF_PATH must be set to an existing IDF-installation. IDF_PATH: '$ENV{IDF_PATH}'")
    else ()
        message(STATUS "IDF_PATH appears valid.")
    endif ()
endfunction()

set(smooth_req_comps
        esp_wifi
        esp32
        freertos
        esptool_py
        fatfs
        sdmmc
        spi_flash
        nvs_flash
        wear_levelling
        libsodium
        lwip
        json
        mbedtls
        xtensa
        pthread)

function(smooth_link_to_idf target)
    if(${CMAKE_CXX_COMPILER} MATCHES "xtensa")
        foreach(c ${smooth_req_comps})
            target_link_libraries(${target} idf::${c})
        endforeach()
    endif()
endfunction()


function(smooth_setup target)
    if(${CMAKE_CXX_COMPILER} MATCHES "xtensa")
        target_compile_definitions(${target} PUBLIC ESP_PLATFORM)

        idf_build_component(smooth_idf_component)

        # When a project specifies a specific sdkconfig, use it
        if("${project_specific_sdkconfig}" EQUAL "")
            set(smooth_sdkconfig ${CMAKE_CURRENT_LIST_DIR}/sdkconfig)
        else()
            set(smooth_sdkconfig ${project_specific_sdkconfig})
        endif()

        include($ENV{IDF_PATH}/tools/cmake/idf.cmake)

        idf_build_process(esp32
                COMPONENTS ${smooth_req_comps} smooth_idf_component
                SDKCONFIG ${smooth_sdkconfig}
                BUILD_DIR ${CMAKE_BINARY_DIR})

        smooth_link_to_idf(${target})

        set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    else()
        message(STATUS "Smooth: Building for native Linux.")
        target_include_directories(smooth PRIVATE $ENV{IDF_PATH}/components/json/cJSON)

        target_link_libraries(${target} pthread mbedtls mbedx509 mbedcrypto)

        message(STATUS "Compiling for native Linux: Using default values for Smooth-MQTT")
        add_definitions("-DCONFIG_SMOOTH_MAX_MQTT_MESSAGE_SIZE=3500")
        add_definitions("-DCONFIG_SMOOTH_MAX_MQTT_OUTGOING_MESSAGES=10")
        add_definitions("-DCONFIG_SMOOTH_MQTT_LOGGING_LEVEL=0")
    endif()
endfunction()