cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

include(pico_extras_import.cmake)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

add_executable(sysctl
	sysctl.c
)

target_link_libraries(sysctl pico_stdlib hardware_i2c hardware_rtc hardware_sleep)

pico_enable_stdio_usb(sysctl 1)
pico_enable_stdio_uart(sysctl 0)

pico_add_extra_outputs(sysctl)

target_compile_definitions(sysctl PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

target_compile_definitions(sysctl PUBLIC
	PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
  )
