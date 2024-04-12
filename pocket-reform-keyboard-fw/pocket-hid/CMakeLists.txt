cmake_minimum_required(VERSION 3.17)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../tinyusb/hw/bsp/family_support.cmake)

# gets PROJECT name for the example (e.g. <BOARD>-<DIR_NAME>)
family_get_project_name(PROJECT ${CMAKE_CURRENT_LIST_DIR})

project(${PROJECT} C CXX ASM)

# Checks this example is valid for the family and initializes the project
family_initialize_project(${PROJECT} ${CMAKE_CURRENT_LIST_DIR})

add_executable(${PROJECT})

# Example source
target_sources(${PROJECT} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/usb_descriptors.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/oled.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/remote.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/menu.c
        )

# Example include
target_include_directories(${PROJECT} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

# Configure compilation flags and libraries for the example without RTOS.
# See the corresponding function in hw/bsp/FAMILY/family.cmake for details.
family_configure_device_example(${PROJECT} noos)

target_link_libraries(${PROJECT} PRIVATE pico_stdlib pico_multicore tinyusb_device tinyusb_board hardware_pwm hardware_i2c hardware_pio)

target_compile_definitions(${PROJECT} PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

target_compile_definitions(${PROJECT} PUBLIC
	PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
  )
