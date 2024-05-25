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
        Drivers/CMSIS/Include

        qpc/include
        qpc/ports/arm-cm/qk/gnu

        qpc/bsp
        qpc/blinky

        qpc/salt
        qpc/salt/peripherals/aes
        qpc/salt/peripherals/anIn

        qpc/salt/utilities/rtime
        qpc/salt/utilities/epoch
        qpc/salt/utilities/date

        qpc/salt/peripherals/gps
        qpc/salt/peripherals/ihm
        qpc/salt/peripherals/ledPanel
        qpc/salt/peripherals/emaFilter
        qpc/salt/peripherals/teloc
        qpc/salt/peripherals/relayControl

        qpc/salt/utilities/serial
        qpc/salt/utilities/parser/saltCmd

        qpc/salt/peripherals/pulseCounter

        qpc/salt/state-machines/logic

        qpc/salt/utilities/parser

        qpc/salt/peripherals/sim808

        qpc/salt/parser/sim900
        qpc/third-party/ssp/src
        qpc/salt/mTime

        qpc/salt/conmgr
        qpc/salt/modmgr
        qpc/salt/modcmd
        qpc/salt/modpwr

        qpc/salt/utilities/publisher

        qpc/salt/state-machines/mqttProt

        qpc/third-party/MQTT-C/include
        qpc/salt/parser/saltCmd

)

add_definitions(-DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx)

file(
        GLOB_RECURSE
        SOURCES
        "Core/*.*"
        "Drivers/*.*"

        "qpc/src/qk/qk.c"
        "qpc/src/qf/*.c"

        "qpc/ports/arm-cm/qk/gnu/qk_port.c"

        "qpc/blinky/blinky.c"

        "qpc/bsp/bsp.c"
        "qpc/bsp/stm32f4xx_nucleo_144.c"


        "qpc/salt/peripherals/aes/aes.c"
        "qpc/salt/utilities/epoch/epoch.c"
        "qpc/salt/utilities/date/date.c"

        "qpc/salt/peripherals/ihm/buzzer.c"
        "qpc/salt/peripherals/ledPanel/ledPanel.c"
        "qpc/salt/peripherals/emaFilter/emaFilter.c"
        "qpc/salt/peripherals/relayControl/relay.c"
        "qpc/salt/peripherals/pulseCounter/pulseCounter.c"

        "qpc/salt/utilities/mTime/mTime.c"


        # "qpc/salt/parser/sim900/sim900parser.c"


        # "qpc/salt/conmgr/conmgr.c"
       # "qpc/salt/modmgr/modmgr.c"
       # "qpc/salt/modcmd/modcmdSim900.c"
       # "qpc/salt/modpwr/modpwr.c"
       # "qpc/salt/mqttProt/mqttProt.c"
       # "qpc/salt/logic/logic.c"

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
