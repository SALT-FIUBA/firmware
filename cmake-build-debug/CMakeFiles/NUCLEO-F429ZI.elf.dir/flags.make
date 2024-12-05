# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

# compile ASM with /usr/local/bin/arm-none-eabi-gcc
# compile C with /usr/local/bin/arm-none-eabi-gcc
ASM_DEFINES = -DDEBUG -DRKH_CONF_FILE=\"salt/rkhcfg.h\" -DRKH_PLATFORM=\"__STM32__\" -DSTM32F429xx -DUSE_HAL_DRIVER

ASM_INCLUDES = -I/Users/nando/NUCLEO-F429ZI/Core/Inc -I/Users/nando/NUCLEO-F429ZI/Drivers/STM32F4xx_HAL_Driver/Inc -I/Users/nando/NUCLEO-F429ZI/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I/Users/nando/NUCLEO-F429ZI/Drivers/CMSIS/Device/ST/STM32F4xx/Include -I/Users/nando/NUCLEO-F429ZI/Drivers/CMSIS/Include -I/Users/nando/NUCLEO-F429ZI/bsp -I/Users/nando/NUCLEO-F429ZI/blinky -I/Users/nando/NUCLEO-F429ZI/third-party/rkh/source/fwk/inc -I/Users/nando/NUCLEO-F429ZI/third-party/rkh/source/mempool/inc -I/Users/nando/NUCLEO-F429ZI/third-party/rkh/source/queue/inc -I/Users/nando/NUCLEO-F429ZI/third-party/rkh/source/sm/inc -I/Users/nando/NUCLEO-F429ZI/third-party/rkh/source/sma/inc -I/Users/nando/NUCLEO-F429ZI/third-party/rkh/source/tmr/inc -I/Users/nando/NUCLEO-F429ZI/third-party/rkh/source/trc/inc -I/Users/nando/NUCLEO-F429ZI/third-party/rkh/source/portable/arm-cortex/rkhs/arm_cm4f/stm32 -I/Users/nando/NUCLEO-F429ZI/mTime -I/Users/nando/NUCLEO-F429ZI/third-party/jWrite/jWrite -I/Users/nando/NUCLEO-F429ZI/third-party/MQTT-C/include -I/Users/nando/NUCLEO-F429ZI/third-party/stm32-sapi/inc -I/Users/nando/NUCLEO-F429ZI/salt/utilities/rtime -I/Users/nando/NUCLEO-F429ZI/salt/utilities/mTime -I/Users/nando/NUCLEO-F429ZI/salt/utilities/date -I/Users/nando/NUCLEO-F429ZI/salt/utilities/epoch -I/Users/nando/NUCLEO-F429ZI/salt -I/Users/nando/NUCLEO-F429ZI/salt/configuration -I/Users/nando/NUCLEO-F429ZI/salt/bsp -I/Users/nando/NUCLEO-F429ZI/salt/state-machines/mqtt-protocol -I/Users/nando/NUCLEO-F429ZI/salt/state-machines/connector-manager -I/Users/nando/NUCLEO-F429ZI/salt/state-machines/module-manager -I/Users/nando/NUCLEO-F429ZI/salt/state-machines/logic -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/module-command -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/module-power -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/serial -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/gps -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/sim808 -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/ledPanel -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/ihm -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/relay-control -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/analog-inputs -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/pulse-counter -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/teloc -I/Users/nando/NUCLEO-F429ZI/salt/utilities/emaFilter -I/Users/nando/NUCLEO-F429ZI/salt/utilities/parser -I/Users/nando/NUCLEO-F429ZI/salt/utilities/parser/sim900 -I/Users/nando/NUCLEO-F429ZI/salt/utilities/parser/salt-command -I/Users/nando/NUCLEO-F429ZI/salt/utilities/ssp/src -I/Users/nando/NUCLEO-F429ZI/salt/utilities/aes -I/Users/nando/NUCLEO-F429ZI/salt/utilities/publisher -I/Users/nando/NUCLEO-F429ZI/utilities -I/Users/nando/NUCLEO-F429ZI/blinkySysTick

ASM_FLAGS = -g -mcpu=cortex-m4 -mthumb -mthumb-interwork -ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -x assembler-with-cpp -Og -g

C_DEFINES = -DDEBUG -DRKH_CONF_FILE=\"salt/rkhcfg.h\" -DRKH_PLATFORM=\"__STM32__\" -DSTM32F429xx -DUSE_HAL_DRIVER

C_INCLUDES = -I/Users/nando/NUCLEO-F429ZI/Core/Inc -I/Users/nando/NUCLEO-F429ZI/Drivers/STM32F4xx_HAL_Driver/Inc -I/Users/nando/NUCLEO-F429ZI/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I/Users/nando/NUCLEO-F429ZI/Drivers/CMSIS/Device/ST/STM32F4xx/Include -I/Users/nando/NUCLEO-F429ZI/Drivers/CMSIS/Include -I/Users/nando/NUCLEO-F429ZI/bsp -I/Users/nando/NUCLEO-F429ZI/blinky -I/Users/nando/NUCLEO-F429ZI/third-party/rkh/source/fwk/inc -I/Users/nando/NUCLEO-F429ZI/third-party/rkh/source/mempool/inc -I/Users/nando/NUCLEO-F429ZI/third-party/rkh/source/queue/inc -I/Users/nando/NUCLEO-F429ZI/third-party/rkh/source/sm/inc -I/Users/nando/NUCLEO-F429ZI/third-party/rkh/source/sma/inc -I/Users/nando/NUCLEO-F429ZI/third-party/rkh/source/tmr/inc -I/Users/nando/NUCLEO-F429ZI/third-party/rkh/source/trc/inc -I/Users/nando/NUCLEO-F429ZI/third-party/rkh/source/portable/arm-cortex/rkhs/arm_cm4f/stm32 -I/Users/nando/NUCLEO-F429ZI/mTime -I/Users/nando/NUCLEO-F429ZI/third-party/jWrite/jWrite -I/Users/nando/NUCLEO-F429ZI/third-party/MQTT-C/include -I/Users/nando/NUCLEO-F429ZI/third-party/stm32-sapi/inc -I/Users/nando/NUCLEO-F429ZI/salt/utilities/rtime -I/Users/nando/NUCLEO-F429ZI/salt/utilities/mTime -I/Users/nando/NUCLEO-F429ZI/salt/utilities/date -I/Users/nando/NUCLEO-F429ZI/salt/utilities/epoch -I/Users/nando/NUCLEO-F429ZI/salt -I/Users/nando/NUCLEO-F429ZI/salt/configuration -I/Users/nando/NUCLEO-F429ZI/salt/bsp -I/Users/nando/NUCLEO-F429ZI/salt/state-machines/mqtt-protocol -I/Users/nando/NUCLEO-F429ZI/salt/state-machines/connector-manager -I/Users/nando/NUCLEO-F429ZI/salt/state-machines/module-manager -I/Users/nando/NUCLEO-F429ZI/salt/state-machines/logic -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/module-command -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/module-power -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/serial -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/gps -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/sim808 -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/ledPanel -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/ihm -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/relay-control -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/analog-inputs -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/pulse-counter -I/Users/nando/NUCLEO-F429ZI/salt/peripherals/teloc -I/Users/nando/NUCLEO-F429ZI/salt/utilities/emaFilter -I/Users/nando/NUCLEO-F429ZI/salt/utilities/parser -I/Users/nando/NUCLEO-F429ZI/salt/utilities/parser/sim900 -I/Users/nando/NUCLEO-F429ZI/salt/utilities/parser/salt-command -I/Users/nando/NUCLEO-F429ZI/salt/utilities/ssp/src -I/Users/nando/NUCLEO-F429ZI/salt/utilities/aes -I/Users/nando/NUCLEO-F429ZI/salt/utilities/publisher -I/Users/nando/NUCLEO-F429ZI/utilities -I/Users/nando/NUCLEO-F429ZI/blinkySysTick

C_FLAGS = -g -std=gnu11 -fdiagnostics-color=always -mcpu=cortex-m4 -mthumb -mthumb-interwork -ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -Og -g

