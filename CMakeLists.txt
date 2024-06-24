#THIS FILE IS AUTO GENERATED FROM THE TEMPLATE! DO NOT CHANGE!
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.28)

# specify cross-compilers and tools
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# project settings
project(NUCLEO-F429ZI C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

#Uncomment for hardware floating point
#add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
#add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
#add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)

#Uncomment for software floating point
#add_compile_options(-mfloat-abi=soft)

add_compile_options(-mcpu=cortex-m4 -mthumb -mthumb-interwork)
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0)

# uncomment to mitigate c++17 absolute addresses warnings
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")

# Enable assembler files preprocessing
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Og -g)
endif ()

include_directories(
        Core/Inc
        Drivers/STM32F4xx_HAL_Driver/Inc
        Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
        Drivers/CMSIS/Device/ST/STM32F4xx/Include
        Drivers/CMSIS/Include LWIP/App LWIP/Target

        Middlewares/Third_Party/LwIP/src/include
        Middlewares/Third_Party/LwIP/system

        Drivers/BSP/Components/lan8742
        Middlewares/Third_Party/LwIP/src/include/netif/ppp
        Middlewares/Third_Party/LwIP/src/include/lwip
        Middlewares/Third_Party/LwIP/src/include/lwip/apps
        Middlewares/Third_Party/LwIP/src/include/lwip/priv
        Middlewares/Third_Party/LwIP/src/include/lwip/prot
        Middlewares/Third_Party/LwIP/src/include/netif
        Middlewares/Third_Party/LwIP/src/include/compat/posix
        Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa
        Middlewares/Third_Party/LwIP/src/include/compat/posix/net
        Middlewares/Third_Party/LwIP/src/include/compat/posix/sys
        Middlewares/Third_Party/LwIP/src/include/compat/stdc
        Middlewares/Third_Party/LwIP/system/arch

        Middlewares/Third_Party/rkh/source/fwk/inc
        Middlewares/Third_Party/rkh/source/mempool/inc
        Middlewares/Third_Party/rkh/source/queue/inc
        Middlewares/Third_Party/rkh/source/sm/inc
        Middlewares/Third_Party/rkh/source/sma/inc
        Middlewares/Third_Party/rkh/source/tmr/inc
        Middlewares/Third_Party/rkh/source/trc/inc

        Middlewares/Third_Party/rkh/source/portable/arm-cortex/rkhs/arm_cm4f/stm32

        Drivers/BSP-Nucleo-144

        reactive-framework/blinky
        reactive-framework/blinky/bsp
        reactive-framework/salt/utilities/rtime
        reactive-framework/salt/utilities/mTime
        reactive-framework/salt/utilities/date
        reactive-framework/salt/utilities/epoch

        Middlewares/Third_Party/MQTT-C/include

        reactive-framework/salt
        reactive-framework/salt/configuration
        reactive-framework/salt/bsp

        reactive-framework/salt/state-machines/mqtt-protocol
        reactive-framework/salt/state-machines/connector-manager
        reactive-framework/salt/state-machines/module-manager
        reactive-framework/salt/state-machines/logic

        reactive-framework/salt/peripherals/module-command
        reactive-framework/salt/peripherals/module-power
        reactive-framework/salt/peripherals/serial
        reactive-framework/salt/peripherals/gps
        reactive-framework/salt/peripherals/sim808
        reactive-framework/salt/peripherals/ledPanel
        reactive-framework/salt/peripherals/ihm
        reactive-framework/salt/peripherals/relay-control
        reactive-framework/salt/peripherals/analog-inputs

        reactive-framework/salt/utilities/emaFilter
        reactive-framework/salt/utilities/parser
        reactive-framework/salt/utilities/parser/sim900
        reactive-framework/salt/utilities/parser/salt-command
        reactive-framework/salt/utilities/ssp/src

)


add_definitions(
        -DDEBUG
        -DUSE_HAL_DRIVER
        -DSTM32F429xx
        -DRKH_PLATFORM="__STM32__"
        -DRKH_CONF_FILE="reactive-framework/blinky/rkhcfg.h"
        -DMQTTC_PAL_FILE="reactive-framework/salt/stm32_mqtt_pal.h"
)

file(
        GLOB_RECURSE SOURCES
        "Core/*.*"
        "LWIP/*.*"
        "Drivers/*.*"

        "Middlewares/Third_Party/LwIP/*.*"

        "Middlewares/Third_Party/rkh/source/fwk/src/rkhfwk_bittbl.c"
        "Middlewares/Third_Party/rkh/source/fwk/src/rkhfwk_dynevt.c"
        "Middlewares/Third_Party/rkh/source/fwk/src/rkhfwk_evtpool.c"
        "Middlewares/Third_Party/rkh/source/fwk/src/rkhfwk_pubsub.c"
        "Middlewares/Third_Party/rkh/source/fwk/src/rkhfwk_rdygrp.c"
        "Middlewares/Third_Party/rkh/source/fwk/src/rkhfwk_sched.c"
        "Middlewares/Third_Party/rkh/source/fwk/src/rkhfwk_version.c"
        "Middlewares/Third_Party/rkh/source/mempool/src/rkhmempool.c"
        "Middlewares/Third_Party/rkh/source/queue/src/rkhqueue.c"
        "Middlewares/Third_Party/rkh/source/sm/src/rkhsm.c"
        "Middlewares/Third_Party/rkh/source/sma/src/rkhsma.c"
        "Middlewares/Third_Party/rkh/source/sma/src/rkhsma_prio.c"
        "Middlewares/Third_Party/rkh/source/sma/src/rkhsma_sync.c"
        "Middlewares/Third_Party/rkh/source/tmr/src/rkhtmr.c"
        "Middlewares/Third_Party/rkh/source/trc/src/rkhtrc_filter.c"
        "Middlewares/Third_Party/rkh/source/trc/src/rkhtrc_record.c"
        "Middlewares/Third_Party/rkh/source/trc/src/rkhtrc_stream.c"

        "Middlewares/Third_Party/rkh/source/portable/arm-cortex/rkhs/arm_cm4f/stm32/rkhport.c"


        "Middlewares/Third_Party/MQTT-C/src/mqtt-c.c"
        "Middlewares/Third_Party/MQTT-C/src/mqtt_pal.c"

        "reactive-framework/*.*"
)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F429ZITX_FLASH.ld)

add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-mcpu=cortex-m4 -mthumb -mthumb-interwork)
add_link_options(-T ${LINKER_SCRIPT})

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
