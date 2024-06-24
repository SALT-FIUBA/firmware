# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# compile ASM with /usr/local/bin/arm-none-eabi-gcc
# compile C with /usr/local/bin/arm-none-eabi-gcc
ASM_DEFINES = -DDEBUG -DMQTTC_PAL_FILE=\"reactive-framework/salt/stm32_mqtt_pal.h\" -DRKH_CONF_FILE=\"reactive-framework/blinky/rkhcfg.h\" -DRKH_PLATFORM=\"__STM32__\" -DSTM32F429xx -DUSE_HAL_DRIVER

ASM_INCLUDES = -I/Users/nando/NUCLEO-F429ZI/Core/Inc -I/Users/nando/NUCLEO-F429ZI/Drivers/STM32F4xx_HAL_Driver/Inc -I/Users/nando/NUCLEO-F429ZI/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I/Users/nando/NUCLEO-F429ZI/Drivers/CMSIS/Device/ST/STM32F4xx/Include -I/Users/nando/NUCLEO-F429ZI/Drivers/CMSIS/Include -I/Users/nando/NUCLEO-F429ZI/LWIP/App -I/Users/nando/NUCLEO-F429ZI/LWIP/Target -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/system -I/Users/nando/NUCLEO-F429ZI/Drivers/BSP/Components/lan8742 -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/netif/ppp -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/lwip -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/lwip/apps -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/lwip/priv -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/lwip/prot -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/netif -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/compat/posix -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/compat/stdc -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/system/arch -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/rkh/source/fwk/inc -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/rkh/source/mempool/inc -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/rkh/source/queue/inc -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/rkh/source/sm/inc -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/rkh/source/sma/inc -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/rkh/source/tmr/inc -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/rkh/source/trc/inc -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/rkh/source/portable/arm-cortex/rkhs/arm_cm4f/stm32 -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/jWrite/jWrite -I/Users/nando/NUCLEO-F429ZI/Drivers/BSP-Nucleo-144 -I/Users/nando/NUCLEO-F429ZI/reactive-framework/blinky -I/Users/nando/NUCLEO-F429ZI/reactive-framework/blinky/bsp -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/rtime -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/mTime -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/date -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/epoch -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/MQTT-C/include -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/configuration -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/bsp -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/state-machines/mqtt-protocol -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/state-machines/connector-manager -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/state-machines/module-manager -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/state-machines/logic -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/module-command -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/module-power -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/serial -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/gps -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/sim808 -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/ledPanel -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/ihm -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/relay-control -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/analog-inputs -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/pulse-counter -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/teloc -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/emaFilter -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/parser -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/parser/sim900 -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/parser/salt-command -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/ssp/src -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/aes -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/publisher

ASM_FLAGS = -g -mcpu=cortex-m4 -mthumb -mthumb-interwork -ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -x assembler-with-cpp -Og -g

C_DEFINES = -DDEBUG -DMQTTC_PAL_FILE=\"reactive-framework/salt/stm32_mqtt_pal.h\" -DRKH_CONF_FILE=\"reactive-framework/blinky/rkhcfg.h\" -DRKH_PLATFORM=\"__STM32__\" -DSTM32F429xx -DUSE_HAL_DRIVER

C_INCLUDES = -I/Users/nando/NUCLEO-F429ZI/Core/Inc -I/Users/nando/NUCLEO-F429ZI/Drivers/STM32F4xx_HAL_Driver/Inc -I/Users/nando/NUCLEO-F429ZI/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I/Users/nando/NUCLEO-F429ZI/Drivers/CMSIS/Device/ST/STM32F4xx/Include -I/Users/nando/NUCLEO-F429ZI/Drivers/CMSIS/Include -I/Users/nando/NUCLEO-F429ZI/LWIP/App -I/Users/nando/NUCLEO-F429ZI/LWIP/Target -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/system -I/Users/nando/NUCLEO-F429ZI/Drivers/BSP/Components/lan8742 -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/netif/ppp -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/lwip -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/lwip/apps -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/lwip/priv -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/lwip/prot -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/netif -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/compat/posix -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/src/include/compat/stdc -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/LwIP/system/arch -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/rkh/source/fwk/inc -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/rkh/source/mempool/inc -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/rkh/source/queue/inc -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/rkh/source/sm/inc -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/rkh/source/sma/inc -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/rkh/source/tmr/inc -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/rkh/source/trc/inc -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/rkh/source/portable/arm-cortex/rkhs/arm_cm4f/stm32 -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/jWrite/jWrite -I/Users/nando/NUCLEO-F429ZI/Drivers/BSP-Nucleo-144 -I/Users/nando/NUCLEO-F429ZI/reactive-framework/blinky -I/Users/nando/NUCLEO-F429ZI/reactive-framework/blinky/bsp -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/rtime -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/mTime -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/date -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/epoch -I/Users/nando/NUCLEO-F429ZI/Middlewares/Third_Party/MQTT-C/include -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/configuration -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/bsp -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/state-machines/mqtt-protocol -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/state-machines/connector-manager -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/state-machines/module-manager -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/state-machines/logic -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/module-command -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/module-power -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/serial -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/gps -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/sim808 -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/ledPanel -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/ihm -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/relay-control -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/analog-inputs -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/pulse-counter -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/peripherals/teloc -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/emaFilter -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/parser -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/parser/sim900 -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/parser/salt-command -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/ssp/src -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/aes -I/Users/nando/NUCLEO-F429ZI/reactive-framework/salt/utilities/publisher

C_FLAGS = -g -std=gnu11 -fdiagnostics-color=always -mcpu=cortex-m4 -mthumb -mthumb-interwork -ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -Og -g

