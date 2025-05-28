# firmware

## stm32 logic state machine - SALT ENABLED

## stm32 huart port
```json
» ./STM32_Programmer_CLI -c port=ttyACM0 br=115200 console
      -------------------------------------------------------------------
                        STM32CubeProgrammer v2.17.0                  
      -------------------------------------------------------------------

Serial Port ttyACM0 is successfully opened.
 Port configuration: parity = even, baudrate = 115200, data-bit = 8,
                     stop-bit = 1,0, flow-control = off

Entering console mode :
Press F to send a file
Press E to exit
Press W to write mode


 %%%%%%%%%%%%%%%%% 
Waiting for network interface...
Waiting for link...
Link up - IP: 192.168.1.78
logic | logic_ctor 
logic | init 
logic | entry_hasler 
logic | entry_disable 
tcp-conmgr | TCP Connected
rv < 0 
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
logic | exit_hasler 
logic | entry_external 

 tcp-mqttprot | publish 
logic | logic_getData 
logic | logic_getPublishPeriod 
Published successful 
logic | exit_external 
logic | entry_gps 

 tcp-mqttprot | publish 
logic | logic_getData 
logic | logic_getPublishPeriod 
Published successful 
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
logic | exit_gps 
logic | entry_missing 

 tcp-mqttprot | publish 
logic | logic_getData 
logic | logic_getPublishPeriod 
Published successful 

 tcp-mqttprot | publish 
logic | logic_getData 
logic | logic_getPublishPeriod 
Published successful 
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c

 tcp-mqttprot | publish 
logic | logic_getData 
logic | logic_getPublishPeriod 
Published successful 


logic | exit_disable 
logic | effect_enabled 
logic | entry_enable 
logic | guard_velDisp 
logic | guard_velRight 
logic | guard_velRightVel 
logic | entry_preventiveStop 

 tcp-mqttprot | publish 
logic | logic_getData 
logic | logic_getPublishPeriod 
Published successful 
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
logic | guard_cmdEnable 
logic | exit_preventiveStop 
logic | guard_velDisp 
logic | guard_velRight 
logic | guard_velRightVel 
logic | entry_controlBlink 
logic | entry_controlBlinkEnable 

 tcp-mqttprot | publish 
logic | logic_getData 
logic | logic_getPublishPeriod 
Published successful 
logic | exit_controlBlinkEnable 
logic | entry_controlBlinkDisable 

 tcp-mqttprot | publish 
logic | logic_getData 
logic | logic_getPublishPeriod 
Published successful 
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c

 tcp-mqttprot | publish 
logic | logic_getData 
logic | logic_getPublishPeriod 
Published successful 
logic | guard_nextBlinkBrake 
logic | exit_controlBlinkDisable 
logic | entry_controlBlinkEnable 

 tcp-mqttprot | publish 
logic | logic_getData 
logic | logic_getPublishPeriod 
Published successful 
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
logic | exit_controlBlinkEnable 
logic | entry_controlBlinkDisable 

 tcp-mqttprot | publish 
logic | logic_getData 
logic | logic_getPublishPeriod 
Published successful 

 tcp-mqttprot | publish 
logic | logic_getData 
logic | logic_getPublishPeriod 
Published successful 
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
logic | guard_nextBlinkBrake 
logic | exit_controlBlinkDisable 
logic | entry_controlBlinkEnable 
tcp-conmgr | Connection closed
```

## mosquitto broker
```json
» mosquitto -c mosquitto.conf -v
1748465450: mosquitto version 2.0.11 starting
1748465450: Config loaded from mosquitto.conf.
1748465450: Opening ipv4 listen socket on port 1883.
1748465450: Opening ipv6 listen socket on port 1883.
1748465450: mosquitto version 2.0.11 running


1748465463: New connection from 192.168.1.78:52432 on port 1883.
1748465463: New client connected from 192.168.1.78:52432 as stm32_client (p2, c1, k400).
1748465463: No will message specified.
1748465463: Sending CONNACK to stm32_client (0, 0)
1748465463: Received SUBSCRIBE from stm32_client
1748465463: 	/stm32/config (QoS 2)
1748465463: stm32_client 2 /stm32/config
1748465463: Sending SUBACK to stm32_client
1748465473: Received PUBLISH from stm32_client (d0, q1, r0, m60968, '/stm32/data', ... (260 bytes))
1748465473: Sending PUBACK to stm32_client (m60968, rc0)
1748465473: Received PUBLISH from stm32_client (d0, q1, r0, m30484, '/stm32/data', ... (260 bytes))
1748465473: Sending PUBACK to stm32_client (m30484, rc0)
1748465478: New connection from 192.168.1.81:59410 on port 1883.
1748465478: New client connected from 192.168.1.81:59410 as auto-3ED8F7F6-CDB9-3B86-2EDB-04D75CB956E2 (p2, c1, k60).
1748465478: No will message specified.
1748465478: Sending CONNACK to auto-3ED8F7F6-CDB9-3B86-2EDB-04D75CB956E2 (0, 0)
1748465478: Received SUBSCRIBE from auto-3ED8F7F6-CDB9-3B86-2EDB-04D75CB956E2
1748465478: 	/stm32/data (QoS 0)
1748465478: auto-3ED8F7F6-CDB9-3B86-2EDB-04D75CB956E2 0 /stm32/data
1748465478: Sending SUBACK to auto-3ED8F7F6-CDB9-3B86-2EDB-04D75CB956E2
1748465483: Received PUBLISH from stm32_client (d0, q1, r0, m15242, '/stm32/data', ... (260 bytes))
1748465483: Sending PUBLISH to auto-3ED8F7F6-CDB9-3B86-2EDB-04D75CB956E2 (d0, q0, r0, m0, '/stm32/data', ... (260 bytes))
1748465483: Sending PUBACK to stm32_client (m15242, rc0)
1748465483: Received PUBLISH from stm32_client (d0, q1, r0, m7621, '/stm32/data', ... (260 bytes))
1748465483: Sending PUBLISH to auto-3ED8F7F6-CDB9-3B86-2EDB-04D75CB956E2 (d0, q0, r0, m0, '/stm32/data', ... (260 bytes))
1748465483: Sending PUBACK to stm32_client (m7621, rc0)
1748465493: Received PUBLISH from stm32_client (d0, q1, r0, m47842, '/stm32/data', ... (260 bytes))
1748465493: Sending PUBLISH to auto-3ED8F7F6-CDB9-3B86-2EDB-04D75CB956E2 (d0, q0, r0, m0, '/stm32/data', ... (260 bytes))
1748465493: Sending PUBACK to stm32_client (m47842, rc0)
1748465493: Received PUBLISH from stm32_client (d0, q1, r0, m23921, '/stm32/data', ... (259 bytes))
1748465493: Sending PUBLISH to auto-3ED8F7F6-CDB9-3B86-2EDB-04D75CB956E2 (d0, q0, r0, m0, '/stm32/data', ... (259 bytes))
1748465493: Sending PUBACK to stm32_client (m23921, rc0)
1748465503: Received PUBLISH from stm32_client (d0, q1, r0, m39608, '/stm32/data', ... (259 bytes))
1748465503: Sending PUBLISH to auto-3ED8F7F6-CDB9-3B86-2EDB-04D75CB956E2 (d0, q0, r0, m0, '/stm32/data', ... (259 bytes))
1748465503: Sending PUBACK to stm32_client (m39608, rc0)
1748465503: Received PUBLISH from stm32_client (d0, q1, r0, m19804, '/stm32/data', ... (259 bytes))
1748465503: Sending PUBLISH to auto-3ED8F7F6-CDB9-3B86-2EDB-04D75CB956E2 (d0, q0, r0, m0, '/stm32/data', ... (259 bytes))
1748465503: Sending PUBACK to stm32_client (m19804, rc0)
1748465513: Received PUBLISH from stm32_client (d0, q1, r0, m9902, '/stm32/data', ... (259 bytes))
1748465513: Sending PUBLISH to auto-3ED8F7F6-CDB9-3B86-2EDB-04D75CB956E2 (d0, q0, r0, m0, '/stm32/data', ... (259 bytes))
1748465513: Sending PUBACK to stm32_client (m9902, rc0)
1748465513: Received PUBLISH from stm32_client (d0, q1, r0, m4951, '/stm32/data', ... (259 bytes))
1748465513: Sending PUBLISH to auto-3ED8F7F6-CDB9-3B86-2EDB-04D75CB956E2 (d0, q0, r0, m0, '/stm32/data', ... (259 bytes))
1748465513: Sending PUBACK to stm32_client (m4951, rc0)
1748465523: Received PUBLISH from stm32_client (d0, q1, r0, m48555, '/stm32/data', ... (259 bytes))
1748465523: Sending PUBLISH to auto-3ED8F7F6-CDB9-3B86-2EDB-04D75CB956E2 (d0, q0, r0, m0, '/stm32/data', ... (259 bytes))
1748465523: Sending PUBACK to stm32_client (m48555, rc0)
1748465523: Received PUBLISH from stm32_client (d0, q1, r0, m60117, '/stm32/data', ... (259 bytes))
1748465523: Sending PUBLISH to auto-3ED8F7F6-CDB9-3B86-2EDB-04D75CB956E2 (d0, q0, r0, m0, '/stm32/data', ... (259 bytes))
1748465523: Sending PUBACK to stm32_client (m60117, rc0)
1748465524: Received DISCONNECT from auto-3ED8F7F6-CDB9-3B86-2EDB-04D75CB956E2
1748465524: Client auto-3ED8F7F6-CDB9-3B86-2EDB-04D75CB956E2 disconnected.
^C1748465526: mosquitto version 2.0.11 terminating
```

## mosquitto sub
```json
» mosquitto_sub -h 192.168.1.81 -p 1883 -t "/stm32/data"
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":true,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":true,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":true,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":true,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":true,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":true,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":true,"publish_period":5}
```

## stm32 logic state machine - SALT DISABLED

## stm32 huart port
```json
 %%%%%%%%%%%%%%%%%
Waiting for network interface...
Waiting for link...
Link up - IP: 192.168.1.78
logic | logic_ctor
logic | init
logic | entry_hasler
logic | entry_disable
tcp-conmgr | TCP Connected
rv < 0
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c

logic | exit_hasler
logic | entry_external

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful
logic | exit_external
logic | entry_gps

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
logic | exit_gps
logic | entry_missing

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful

tcp-mqttprot | publish
logic | logic_getData
logic | logic_getPublishPeriod
Published successful
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
tcp-conmgr | Connection closed

```

## mosquitto broker
```json
1748465124: New connection from 192.168.1.78:52432 on port 1883.
1748465124: New client connected from 192.168.1.78:52432 as stm32_client (p2, c1, k400).
1748465124: No will message specified.
1748465124: Sending CONNACK to stm32_client (0, 0)
1748465124: Received SUBSCRIBE from stm32_client
1748465124: 	/stm32/config (QoS 2)
1748465124: stm32_client 2 /stm32/config
1748465124: Sending SUBACK to stm32_client
1748465134: Received PUBLISH from stm32_client (d0, q1, r0, m60968, '/stm32/data', ... (260 bytes))
1748465134: Sending PUBACK to stm32_client (m60968, rc0)
1748465134: Received PUBLISH from stm32_client (d0, q1, r0, m30484, '/stm32/data', ... (260 bytes))
1748465134: Sending PUBACK to stm32_client (m30484, rc0)
1748465144: Received PUBLISH from stm32_client (d0, q1, r0, m15242, '/stm32/data', ... (260 bytes))
1748465144: Sending PUBACK to stm32_client (m15242, rc0)
1748465144: Received PUBLISH from stm32_client (d0, q1, r0, m7621, '/stm32/data', ... (260 bytes))
1748465144: Sending PUBACK to stm32_client (m7621, rc0)
1748465153: New connection from 192.168.1.81:55864 on port 1883.
1748465153: New client connected from 192.168.1.81:55864 as auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 (p2, c1, k60).
1748465153: No will message specified.
1748465153: Sending CONNACK to auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 (0, 0)
1748465153: Received SUBSCRIBE from auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2
1748465153: 	/stm32/data (QoS 0)
1748465153: auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 0 /stm32/data
1748465153: Sending SUBACK to auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2
1748465154: Received PUBLISH from stm32_client (d0, q1, r0, m47842, '/stm32/data', ... (260 bytes))
1748465154: Sending PUBLISH to auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 (d0, q0, r0, m0, '/stm32/data', ... (260 bytes))
1748465154: Sending PUBACK to stm32_client (m47842, rc0)
1748465154: Received PUBLISH from stm32_client (d0, q1, r0, m23921, '/stm32/data', ... (260 bytes))
1748465154: Sending PUBLISH to auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 (d0, q0, r0, m0, '/stm32/data', ... (260 bytes))
1748465154: Sending PUBACK to stm32_client (m23921, rc0)
1748465164: Received PUBLISH from stm32_client (d0, q1, r0, m39608, '/stm32/data', ... (260 bytes))
1748465164: Sending PUBLISH to auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 (d0, q0, r0, m0, '/stm32/data', ... (260 bytes))
1748465164: Sending PUBACK to stm32_client (m39608, rc0)
1748465164: Received PUBLISH from stm32_client (d0, q1, r0, m19804, '/stm32/data', ... (260 bytes))
1748465164: Sending PUBLISH to auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 (d0, q0, r0, m0, '/stm32/data', ... (260 bytes))
1748465164: Sending PUBACK to stm32_client (m19804, rc0)
1748465174: Received PUBLISH from stm32_client (d0, q1, r0, m9902, '/stm32/data', ... (260 bytes))
1748465174: Sending PUBLISH to auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 (d0, q0, r0, m0, '/stm32/data', ... (260 bytes))
1748465174: Sending PUBACK to stm32_client (m9902, rc0)
1748465174: Received PUBLISH from stm32_client (d0, q1, r0, m4951, '/stm32/data', ... (260 bytes))
1748465174: Sending PUBLISH to auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 (d0, q0, r0, m0, '/stm32/data', ... (260 bytes))
1748465174: Sending PUBACK to stm32_client (m4951, rc0)
1748465184: Received PUBLISH from stm32_client (d0, q1, r0, m48555, '/stm32/data', ... (260 bytes))
1748465184: Sending PUBLISH to auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 (d0, q0, r0, m0, '/stm32/data', ... (260 bytes))
1748465184: Sending PUBACK to stm32_client (m48555, rc0)
1748465184: Received PUBLISH from stm32_client (d0, q1, r0, m60117, '/stm32/data', ... (260 bytes))
1748465184: Sending PUBLISH to auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 (d0, q0, r0, m0, '/stm32/data', ... (260 bytes))
1748465184: Sending PUBACK to stm32_client (m60117, rc0)
1748465194: Received PUBLISH from stm32_client (d0, q1, r0, m49514, '/stm32/data', ... (260 bytes))
1748465194: Sending PUBLISH to auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 (d0, q0, r0, m0, '/stm32/data', ... (260 bytes))
1748465194: Sending PUBACK to stm32_client (m49514, rc0)
1748465194: Received PUBLISH from stm32_client (d0, q1, r0, m24757, '/stm32/data', ... (260 bytes))
1748465194: Sending PUBLISH to auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 (d0, q0, r0, m0, '/stm32/data', ... (260 bytes))
1748465194: Sending PUBACK to stm32_client (m24757, rc0)
1748465204: Received PUBLISH from stm32_client (d0, q1, r0, m33882, '/stm32/data', ... (260 bytes))
1748465204: Sending PUBLISH to auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 (d0, q0, r0, m0, '/stm32/data', ... (260 bytes))
1748465204: Sending PUBACK to stm32_client (m33882, rc0)
1748465204: Received PUBLISH from stm32_client (d0, q1, r0, m16941, '/stm32/data', ... (260 bytes))
1748465204: Sending PUBLISH to auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 (d0, q0, r0, m0, '/stm32/data', ... (260 bytes))
1748465204: Sending PUBACK to stm32_client (m16941, rc0)
1748465213: Received PINGREQ from auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2
1748465213: Sending PINGRESP to auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2
1748465214: Received PUBLISH from stm32_client (d0, q1, r0, m38166, '/stm32/data', ... (260 bytes))
1748465214: Sending PUBLISH to auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 (d0, q0, r0, m0, '/stm32/data', ... (260 bytes))
1748465214: Sending PUBACK to stm32_client (m38166, rc0)
1748465214: Received PUBLISH from stm32_client (d0, q1, r0, m19083, '/stm32/data', ... (260 bytes))
1748465214: Sending PUBLISH to auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 (d0, q0, r0, m0, '/stm32/data', ... (260 bytes))
1748465214: Sending PUBACK to stm32_client (m19083, rc0)
1748465216: Received DISCONNECT from auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2
1748465216: Client auto-49FADEF3-CF4E-A5D7-1805-1415A0C630E2 disconnected.
^C1748465218: mosquitto version 2.0.11 terminating
```

## mosquitto sub
```json
» mosquitto_sub -h 192.168.1.81 -p 1883 -t "/stm32/data"
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"imei":"ConMgr_imei()","cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":30000,"time_blink_enable":5000,"time_blink_disable":10000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
```


## stm32 subscribe test 2 

### stm32 huart port
```json
 » ./STM32_Programmer_CLI -c port=ttyACM0 br=115200 console
      -------------------------------------------------------------------
                        STM32CubeProgrammer v2.17.0                  
      -------------------------------------------------------------------

Serial Port ttyACM0 is successfully opened.
 Port configuration: parity = even, baudrate = 115200, data-bit = 8,
                     stop-bit = 1,0, flow-control = off

Entering console mode :
Press F to send a file
Press E to exit
Press W to write mode


 %%%%%%%%%%%%%%%%% 
Waiting for network interface...
Waiting for link...
Link up - IP: 192.168.1.78

 tcp-mqttprot | configClient 

 tcp-conmgr | tcp-conmgr | init 
logic | entry_disable 

 tcp-conmgr | tcp_connect_attempt 

 tcp-conmgr | tcp_connect_callback 
tcp-conmgr | TCP Connected

 tcp-mqttprot | downcastNetConnectedEvt 

 tcp-mqttprot | connected 
__mqttc_recv error: 0 MQTT_ERROR: Buffer too small. 
recv_buffer.curr: 0 
recv_buffer.curr_sz: 0 
mqttc_recv_ret: 1 
mqttc_pal_recvall 
rv: -2147483631 
rv < 0 

 tcp-mqttprot | brokerConnect 
mqttc_init 1 MQTT_OK 
mqttc_connect 1 MQTT_OK 
mqttc_subscribe 1 MQTT_OK 
__mqttc_recv error: 1 MQTT_OK 
recv_buffer.curr: 0 
recv_buffer.curr_sz: 1024 
mqttc_recv_ret: 1 
mqttc_pal_recvall 
rv: 0 
__mqttc_send error: 1 MQTT_OK 
__mqttc_send control packet type: 1 
MQTT_CONTROL_CONNECT 
MQTT_CONTROL_PINGREQ 
__mqttc_send control packet type: 8 
MQTT_CONTROL_PINGREQ 

 tcp-mqttprot | entry Awaiting Ack 

 tcp-conmgr | tcp_recv_callback 
Received 4 bytes, total in buffer: 4 
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
tcp-mqttprot | Processing received MQTT data
1668554762 

_mqttc_recv error: 1 MQTT_OK 
recv_buffer.curr: 0 
recv_buffer.curr_sz: 1024 
mqttc_recv_ret: 1 
mqttc_pal_recvall 
rv: 4 
header control type: 2 
mqttc_recv_ret: 1 
mqttc_pal_recvall 
rv: 0 
__mqttc_send error: 1 MQTT_OK 
tcp-mqttprot | MQTT sync successful

 tcp-conmgr | tcp_recv_callback 
Received 5 bytes, total in buffer: 5 
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
tcp-mqttprot | Processing received MQTT data
1668554762 

_mqttc_recv error: 1 MQTT_OK 
recv_buffer.curr: 32 
recv_buffer.curr_sz: 1024 
mqttc_recv_ret: 1 
mqttc_pal_recvall 
rv: 5 
header control type: 9 
mqttc_recv_ret: 1 
mqttc_pal_recvall 
rv: 0 
__mqttc_send error: 1 MQTT_OK 
tcp-mqttprot | MQTT sync successful



 tcp-conmgr | tcp_recv_callback 
Received 30 bytes, total in buffer: 30 
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
tcp-mqttprot | Processing received MQTT data
1668554762 

_mqttc_recv error: 1 MQTT_OK 
recv_buffer.curr: 144 
recv_buffer.curr_sz: 1024 
mqttc_recv_ret: 1 
mqttc_pal_recvall 
rv: 30 
header control type: 3 
case MQTT CONTROL PUBLISH 
/* call publish callback */ 
on mqtt callback called
Received publish('/stm32/config'): test message2
mqttc_recv_ret: 1 
mqttc_pal_recvall 
rv: 0 
__mqttc_send error: 1 MQTT_OK 
tcp-mqttprot | MQTT sync successful



 tcp-conmgr | tcp_recv_callback 
tcp-conmgr | Connection closed

 tcp-conmgr | tcp_connect_attempt 

 tcp-conmgr | tcp_connect_callback 
tcp-conmgr | TCP Connected

 tcp-mqttprot | downcastNetConnectedEvt 

 tcp-mqttprot | connected 
__mqttc_recv error: 1 MQTT_OK 
recv_buffer.curr: 48 
recv_buffer.curr_sz: 1024 
mqttc_recv_ret: 1 
mqttc_pal_recvall 
rv: 0 
__mqttc_send error: 1 MQTT_OK 

 tcp-mqttprot | brokerConnect 
mqttc_init 1 MQTT_OK 
mqttc_connect 1 MQTT_OK 
mqttc_subscribe 1 MQTT_OK 
__mqttc_recv error: 1 MQTT_OK 
recv_buffer.curr: 48 
recv_buffer.curr_sz: 1024 
mqttc_recv_ret: 1 
mqttc_pal_recvall 
rv: 0 
__mqttc_send error: 1 MQTT_OK 
__mqttc_send control packet type: 1 
MQTT_CONTROL_CONNECT 
MQTT_CONTROL_PINGREQ 
__mqttc_send control packet type: 8 
MQTT_CONTROL_PINGREQ 

 tcp-mqttprot | entry Awaiting Ack 

 tcp-conmgr | tcp_recv_callback 
Received 4 bytes, total in buffer: 4 
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
tcp-mqttprot | Processing received MQTT data
1668554762 

_mqttc_recv error: 1 MQTT_OK 
recv_buffer.curr: 48 
recv_buffer.curr_sz: 1024 
mqttc_recv_ret: 1 
mqttc_pal_recvall 
rv: 4 
header control type: 2 
mqttc_recv_ret: 1 
mqttc_pal_recvall 
rv: 0 
__mqttc_send error: 1 MQTT_OK 
tcp-mqttprot | MQTT sync successful

 tcp-conmgr | tcp_recv_callback 
Received 5 bytes, total in buffer: 5 
Assertion "pbuf_free: p->ref > 0" failed at line 753 in /home/nando-toshiba/Desktop/salt/firmware/Middlewares/Third_Party/LwIP/src/core/pbuf.c
tcp-mqttprot | Processing received MQTT data
1668554762 

_mqttc_recv error: 1 MQTT_OK 
recv_buffer.curr: 32 
recv_buffer.curr_sz: 1024 
mqttc_recv_ret: 1 
mqttc_pal_recvall 
rv: 5 
header control type: 9 
mqttc_recv_ret: 1 
mqttc_pal_recvall 
rv: 0 
__mqttc_send error: 1 MQTT_OK 
tcp-mqttprot | MQTT sync successful

```
### mosquitto pub utility

```json
» mosquitto_pub -h 192.168.1.81 -p 1883 -t "/stm32/config" -m "test message2"

```

### mosquitto broker

```json
» mosquitto -c mosquitto.conf -v
1747839856: mosquitto version 2.0.11 starting
1747839856: Config loaded from mosquitto.conf.
1747839856: Opening ipv4 listen socket on port 1883.
1747839856: Opening ipv6 listen socket on port 1883.
1747839856: mosquitto version 2.0.11 running


1747839886: New connection from 192.168.1.78:52432 on port 1883.
1747839886: New client connected from 192.168.1.78:52432 as stm32_client (p2, c1, k400).
1747839886: No will message specified.
1747839886: Sending CONNACK to stm32_client (0, 0)
1747839886: Received SUBSCRIBE from stm32_client
1747839886: 	/stm32/config (QoS 2)
1747839886: stm32_client 2 /stm32/config
1747839886: Sending SUBACK to stm32_client
1747839900: New connection from 192.168.1.81:40514 on port 1883.
1747839900: New client connected from 192.168.1.81:40514 as auto-35EA5844-D5BF-5D4A-8502-13E07C7E9F69 (p2, c1, k60).
1747839900: No will message specified.
1747839900: Sending CONNACK to auto-35EA5844-D5BF-5D4A-8502-13E07C7E9F69 (0, 0)
1747839900: Received PUBLISH from auto-35EA5844-D5BF-5D4A-8502-13E07C7E9F69 (d0, q0, r0, m0, '/stm32/config', ... (13 bytes))
1747839900: Sending PUBLISH to stm32_client (d0, q0, r0, m0, '/stm32/config', ... (13 bytes))
1747839900: Received DISCONNECT from auto-35EA5844-D5BF-5D4A-8502-13E07C7E9F69
1747839900: Client auto-35EA5844-D5BF-5D4A-8502-13E07C7E9F69 disconnected.
1747840492: Client stm32_client has exceeded timeout, disconnecting.
1747840492: New connection from 192.168.1.78:52433 on port 1883.
1747840492: New client connected from 192.168.1.78:52433 as stm32_client (p2, c1, k400).
1747840492: No will message specified.
1747840492: Sending CONNACK to stm32_client (0, 0)
1747840492: Received SUBSCRIBE from stm32_client
1747840492: 	/stm32/config (QoS 2)
1747840492: stm32_client 2 /stm32/config
1747840492: Sending SUBACK to stm32_client
```


## stm32 subscribe test 1 

### stm32 huart port 
```json
» ./STM32_Programmer_CLI -c port=ttyACM0 br=115200 console
      -------------------------------------------------------------------
                        STM32CubeProgrammer v2.17.0                  
      -------------------------------------------------------------------

Serial Port ttyACM0 is successfully opened.
 Port configuration: parity = even, baudrate = 115200, data-bit = 8,
                     stop-bit = 1,0, flow-control = off

 %%%%%%%%%%%%%%%%% 
Waiting for network interface...
Waiting for link...
Link up - IP: 172.20.10.10
logic | entry_disable 
mqttc_subscribe 1 MQTT_OK 
__mqttc_send error: 1 MQTT_OK 
__mqttc_send control packet type: 1 
MQTT_CONTROL_CONNECT 
MQTT_CONTROL_PINGREQ 
__mqttc_send control packet type: 8 
MQTT_CONTROL_PINGREQ 
Received 4 bytes 
Received 5 bytes 
Received 29 bytes 
```

### mosquitto pub utility 
```json
» mosquitto_pub -h 172.20.10.8 -p 1883 -t "/stm32/config" -m "test message"
```

### mosquitto broker 
```json
1746571818: New connection from 172.20.10.10:52432 on port 1883.
1746571818: New client connected from 172.20.10.10:52432 as stm32_client (p2, c1, k400).
1746571818: No will message specified.
1746571818: Sending CONNACK to stm32_client (0, 0)
1746571818: Received SUBSCRIBE from stm32_client
1746571818: 	/stm32/config (QoS 2)
1746571818: stm32_client 2 /stm32/config
1746571818: Sending SUBACK to stm32_client

1746571888: New connection from 172.20.10.8:46208 on port 1883.
1746571888: New client connected from 172.20.10.8:46208 as auto-29E9429D-1E7C-BCBD-DC9F-999E457F7EA4 (p2, c1, k60).
1746571888: No will message specified.
1746571888: Sending CONNACK to auto-29E9429D-1E7C-BCBD-DC9F-999E457F7EA4 (0, 0)
1746571888: Received PUBLISH from auto-29E9429D-1E7C-BCBD-DC9F-999E457F7EA4 (d0, q0, r0, m0, '/stm32/config', ... (12 bytes))
1746571888: Sending PUBLISH to stm32_client (d0, q0, r0, m0, '/stm32/config', ... (12 bytes))
1746571888: Received DISCONNECT from auto-29E9429D-1E7C-BCBD-DC9F-999E457F7EA4
1746571888: Client auto-29E9429D-1E7C-BCBD-DC9F-999E457F7EA4 disconnected.
```


## stm32 lwip tcp-conMgr + tcp-mqttProt + logic state machines working

### susbcribe to topic "/stm32/data" from stm32_client

#### mosquitto cli
```json 
» mosquitto_sub -h 192.168.1.81 -p 1883 -t "/stm32/data"

{"cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":3000,"time_blink_enable":500,"time_blink_disable":1000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":3000,"time_blink_enable":500,"time_blink_disable":1000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":3000,"time_blink_enable":500,"time_blink_disable":1000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":3000,"time_blink_enable":500,"time_blink_disable":1000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":3000,"time_blink_enable":500,"time_blink_disable":1000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":3000,"time_blink_enable":500,"time_blink_disable":1000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
{"cmd_timeout":60000,"vel_ct_on":30,"vel_ct_off":25,"vel_fe_on":36,"time_fe_hold":3000,"time_blink_enable":500,"time_blink_disable":1000,"period_blink":5,"cmd":"automatic","vel":-1,"vel_source":null,"al_mode":false,"publish_period":5}
```

### subscribe to topic "/stm32/config" from mosquitto_sub

#### mosquitto pub
```json 
» mosquitto_pub -h 192.168.1.81 -p 1883 -t "/stm32/config" -m "test message"
» mosquitto_pub -h 192.168.1.81 -p 1883 -t "/stm32/config" -m "test message"
```

#### mosquitto broker 

```json
» mosquitto -c mosquitto.conf -v
1745417494: mosquitto version 2.0.11 starting
1745417494: Config loaded from mosquitto.conf.
1745417494: Opening ipv4 listen socket on port 1883.
1745417494: Opening ipv6 listen socket on port 1883.
1745417494: mosquitto version 2.0.11 running

1745417519: New connection from 192.168.1.78:52432 on port 1883.
1745417519: New client connected from 192.168.1.78:52432 as stm32_client (p2, c1, k400).
1745417519: No will message specified.
1745417519: Sending CONNACK to stm32_client (0, 0)
1745417519: Received SUBSCRIBE from stm32_client
1745417519: 	/stm32/config (QoS 2)
1745417519: stm32_client 2 /stm32/config
1745417519: Sending SUBACK to stm32_client

1745417531: New connection from 192.168.1.81:43772 on port 1883.
1745417531: New client connected from 192.168.1.81:43772 as auto-4FFECE7C-6EA8-2D56-BFCF-6F9A93613D8C (p2, c1, k60).
1745417531: No will message specified.
1745417531: Sending CONNACK to auto-4FFECE7C-6EA8-2D56-BFCF-6F9A93613D8C (0, 0)
1745417531: Received PUBLISH from auto-4FFECE7C-6EA8-2D56-BFCF-6F9A93613D8C (d0, q0, r0, m0, '/stm32/config', ... (12 bytes))
1745417531: Sending PUBLISH to stm32_client (d0, q0, r0, m0, '/stm32/config', ... (12 bytes))
1745417531: Received DISCONNECT from auto-4FFECE7C-6EA8-2D56-BFCF-6F9A93613D8C
1745417531: Client auto-4FFECE7C-6EA8-2D56-BFCF-6F9A93613D8C disconnected.

1745417540: New connection from 192.168.1.81:56424 on port 1883.
1745417540: New client connected from 192.168.1.81:56424 as auto-DF34D743-EAFB-E35F-D11E-94DDA5BD3C3D (p2, c1, k60).
1745417540: No will message specified.
1745417540: Sending CONNACK to auto-DF34D743-EAFB-E35F-D11E-94DDA5BD3C3D (0, 0)
1745417540: Received PUBLISH from auto-DF34D743-EAFB-E35F-D11E-94DDA5BD3C3D (d0, q0, r0, m0, '/stm32/config', ... (12 bytes))
1745417540: Sending PUBLISH to stm32_client (d0, q0, r0, m0, '/stm32/config', ... (12 bytes))
1745417540: Received DISCONNECT from auto-DF34D743-EAFB-E35F-D11E-94DDA5BD3C3D
1745417540: Client auto-DF34D743-EAFB-E35F-D11E-94DDA5BD3C3D disconnected.
```

### stm32 huart connection

```json 
» ./STM32_Programmer_CLI -c port=ttyACM0 br=115200 console
      -------------------------------------------------------------------
                        STM32CubeProgrammer v2.17.0                  
      -------------------------------------------------------------------

Serial Port ttyACM0 is successfully opened.
 Port configuration: parity = even, baudrate = 115200, data-bit = 8,
                     stop-bit = 1,0, flow-control = off

Entering console mode :
Press F to send a file
Press E to exit
Press W to write mode


 %%%%%%%%%%%%%%%%% 
Waiting for network interface...
Waiting for link...
Link up - IP: 192.168.1.78
logic | entry_disable 
mqttc_subscribe 1 MQTT_OK 
__mqttc_send error: 1 MQTT_OK 
__mqttc_send control packet type: 1 
MQTT_CONTROL_CONNECT 
MQTT_CONTROL_PINGREQ 
__mqttc_send control packet type: 8 
MQTT_CONTROL_PINGREQ 
Received 4 bytes 
Received 5 bytes 

Received 29 bytes 
Received 29 bytes 
```


## stm32 lwip tcp MqttProt Client SM with an undesired CONNECT after 25 seconds (without SyncRegion state machine)

### tcp-conmgr.c

quit evSend and evRecv regarding TcpConMgr_sending and TcpConMgr_receiving sub-state machines interaction 
```c 
RKH_CREATE_BASIC_STATE(TcpConMgr_connected, socketConnected, NULL, &TcpConMgr_active, NULL);
RKH_CREATE_TRANS_TABLE(TcpConMgr_connected)
                // TODO  RKH_TRREG(evSend, NULL, send_data, &TcpConMgr_sending),
                // TODO  RKH_TRREG(evRecv, NULL, read_data, &TcpConMgr_receiving),
                RKH_TRREG(evClosed, NULL, NULL, &TcpConMgr_connecting),
                RKH_TRREG(evDisconnected, NULL, socketClosed, &TcpConMgr_connecting),
RKH_END_TRANS_TABLE

static void socketClosed(TcpConMgr *const me) {

    printf("\n tcp-conmgr | socketClosed \n");
    printf("tcp-conmgr | Current state: %s \n", get_state_name_conmgr_sm(tcpConMgr->sm.state));

    bsp_netStatus(DisconnectedSt);

    RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T, &e_NetDisconnected), me);
}
```

### tcp-mqttProt.c

quit mqttc_sync invocation in enAwaitingAck and publish one or two more messages than before (5 to 6/7 messages).

```c 
static void
enAwaitingAck(TCP_MQTTProt *const me, RKH_EVT_T *pe)
{
    printf("\n tcp-mqttprot | entry Awaiting Ack \n");
    printf("tcp-mqttprot | Current state: %s \n \n", get_state_name_mqtt_sm(tcpMqttProt->sm.state));

    printf("tcp-mqttprot | mqttc_sync -> _mqttc_send -> mqttc_pal_sendall");

    if (me->mqttc_client.error == MQTT_OK) {

        printf("tcp-mqttprot | entry Awaiting Ack MQTT_OK \n");
        RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T , &evConnAcceptedObj), me);

    } else {

        printf("tcp-mqttprot | entry Awaiting Ack not MQTT_OK \n");
        RKH_SMA_POST_FIFO(tcpMqttProt, RKH_UPCAST(RKH_EVT_T , &evConnRefusedObj), me);
    }
}
```

```json 
1745154566: New connection from 192.168.1.78:52432 on port 1883.
1745154571: New client connected from 192.168.1.78:52432 as stm32_client (p2, c0, k60).
1745154571: No will message specified.
1745154571: Sending CONNACK to stm32_client (0, 0)
1745154571: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1745154576: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1745154581: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1745154586: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1745154591: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1745154596: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1745154601: Bad client stm32_client sending multiple CONNECT messages.
1745154601: Client stm32_client disconnected due to protocol error.
1745154601: New connection from 192.168.1.78:52433 on port 1883.
1745154606: New client connected from 192.168.1.78:52433 as stm32_client (p2, c0, k60).
1745154606: No will message specified.
1745154606: Sending CONNACK to stm32_client (0, 0)
1745154606: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1745154611: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1745154616: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1745154622: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1745154627: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1745154632: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1745154637: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1745154642: Bad client stm32_client sending multiple CONNECT messages.
1745154642: Client stm32_client disconnected due to protocol error.
```

## stm32 lwip tcp MqttProt Client SM with PUBLISH working and an undesired CONNECT after 25 seconds (without SyncRegion state machine) 

1. Invoke RKH tick handler in SysTick_Handler function (stm32f4xx_it.c). based on `feature/rkh-blinky-systick`.

```c 
/* USER CODE BEGIN PV */
extern volatile tick_t tickCounter;

/* USER CODE END PV */

void SysTick_Handler(void)
{
    /* USER CODE BEGIN SysTick_IRQn 0 */
    /* USER CODE END SysTick_IRQn 0 */

    /* USER CODE BEGIN SysTick_IRQn 1 */
    HAL_IncTick();
    tickCounter++;

        RKH_TIM_TICK(NULL);

        /* USER CODE END SysTick_IRQn 1 */
}
```

2. In `rkhcfg.h` modify frequency from `100u` to `1000u`

```c 
/**
 *  Specify the frequency of the framework tick interrupt (number of ticks
 *  in one second). It's the rate at which the rkh_tmr_tick() function is
 *  invoked. This configuration constant is not used by RKH, it is just a
 *  value to allow an application to deal with time when using timer
 *  services, converting ticks to time. See RKH_TICK_RATE_MS constant.
 */
#define RKH_CFG_FWK_TICK_RATE_HZ            1000u
```

Explanation based on STM32CubeMx configuration (check `void SystemClock_Config(void)` in `main.c`)

+ Oscillator Configuration

HSE with Bypass: The high-speed external oscillator (HSE) is used in bypass mode (RCC_HSE_BYPASS), meaning an external clock signal is provided instead of a crystal. For the STM32 Nucleo-F429ZI, this is typically an 8 MHz clock from the ST-Link debugger (common for Nucleo boards).
PLL Configuration:
PLLM = 4: Divides the HSE input clock by 4. So, 8 MhZ / 4 = 2 MHz
PLLN = 168: Multiplies the PLL input by 168. So, 2 MHz * 168 = 336 MHz
PLLP = RCC_PLLP_DIV2: Divides the PLL output by 2 for SYSCLK. So, 336 MHz / 2 = 168 MHz
PLLQ = 7: Used for USB, SDIO, etc., but not relevant for SYSCLK.

Result: The PLL generates a SYSCLK of 168 MHz.

+   Clock Distribution:

SYSCLK: Set to the PLL output, so 168 MHz.
HCLK (AHB bus): RCC_SYSCLK_DIV1 means HCLK = SYSCLK = 168 MHz.
PCLK1 (APB1 bus): RCC_HCLK_DIV4 means PCLK1 = 168MHz / 4 = 42 MHz
PCLK2 (APB2 bus): RCC_HCLK_DIV2 means PCLK2 = 168 MHz / 2 = 84 MHz
Flash Latency: Set to 5 wait states, which is appropriate for 168 MHz operation at 3.3V (per the STM32F429 datasheet).

+ SystemCoreClock:

+ After this configuration, the SystemCoreClock variable (defined by CMSIS or STM32 HAL) should be set to 168,000,000 Hz (168 MHz). You can confirm this by checking the value of SystemCoreClock in your code or debugger.
Recommended Tick Rate
As discussed previously, a tick rate of 1000 Hz (1 ms per tick) is recommended for the STM32 Nucleo-F429ZI with the RKH framework, especially for your Blinker application, which uses timer intervals like RKH_TIME_MS(1000) or RKH_TIME_MS(20000). This provides sufficient timing resolution and is standard for real-time embedded systems. However, since your provided configuration uses RKH_CFG_FWK_TICK_RATE_HZ = 100u (100 Hz, 10 ms per tick), we’ll calculate for both options and ensure compatibility with your 168 MHz system clock.

```json

    1000 Hz (1 ms per tick)

    Tick Rate: 1000 Hz means one tick every 1 ms.
 
    SysTick Reload Value:
      
      Reload Value = System Clock Frequency / Tick Rate - 1 = 168,000,000 / 1000 - 1 = 168,000 -1 = 167,999
```

**RKH Configuration:**

```c 
#define RKH_CFG_FWK_TICK_RATE_HZ 1000u
#define RKH_TICK_RATE_MS (1000u / RKH_CFG_FWK_TICK_RATE_HZ) // 1 ms
```

**Why 1000 Hz is Preferred**
  
+ Timing Precision: 1 ms resolution (1000 Hz) allows more precise control for timers, especially for shorter intervals (e.g., 100 ms or 500 ms) that you might add later. Your current Blinker intervals (1 s or 20 s) work with either 100 Hz or 1000 Hz, but 1000 Hz is more flexible.
+ Standard Practice: Most STM32 applications and RTOS frameworks (e.g., FreeRTOS) use 1000 Hz for millisecond granularity.
+ Low Overhead: At 168 MHz, a 1000 Hz interrupt (every 168,000 cycles) is a negligible load on the CPU, especially since rkh_tmr_tick is lightweight.
  

### stm32 mqttc_client

```json
 » ./STM32_Programmer_CLI -c port=ttyACM0 br=115200 console
-------------------------------------------------------------------
STM32CubeProgrammer v2.17.0
-------------------------------------------------------------------

Serial Port ttyACM0 is successfully opened.
Port configuration: parity = even, baudrate = 115200, data-bit = 8,
stop-bit = 1,0, flow-control = off

Waiting for network interface...
Waiting for link...
Link up - IP: 192.168.1.78
tcp-mqttprot | configClient 
tcp-mqttprot | Current state: idle 
 
tcp-mqttprot | TCP_MQTTProt_ctor 
tcp-mqttprot | Current state: idle 
 
tcp-conmgr | tcp-conmgr | init 
tcp-conmgr | Current state: inactive 
tcp-mqttprot | init  
tcp-mqttprot | Current state: idle 
 
tickConfig 
tcp-conmgr | socketOpen 
tcp-conmgr | Current state: inactive 
tcp-conmgr | tcp_connect_attempt 
tcp-conmgr | Current state: inactive 
tcp-conmgr | tcp_connect_callback 
tcp-conmgr | TCP Connected
tcp-conmgr | socketConnected 
tcp-conmgr | Current state: connecting 
tcp-mqttprot | dispatch 
tcp-mqttprot | Current state: idle 
 
tcp-mqttprot | downcastNetConnectedEvt 
tcp-mqttprot | Current state: idle 
 
tcp-mqttprot | remote_ip addr: 1359063232 
tcp-mqttprot | connected 
tcp-mqttprot | Current state: idle 
 
tcp-mqttprot | isLocked 
tcp-mqttprot | Current state: idle 
 
tcp-mqttprot | brokerConnect 
tcp-mqttprot | Current state: idle 
 tcp-mqttprop | mqttc_init error: 1 MQTT_OK 
tcp-mqttprop | mqttc_connect error: 1 MQTT_OK 
 
tcp-mqttprot | isConnectOk 1 MQTT_OK 
tcp-mqttprot | Current state: try connect 
 
tcp-mqttprot | entry Awaiting Ack 
tcp-mqttprot | Current state: try connect 
 
tcp-mqttprot | mqttc_sync -> _mqttc_send -> mqttc_pal_sendallmqttc_pal_recvall | tpcb state: 4 
mqttc_pal_send_all | tpcb state: 4 
 
tcp-mqttprot | entry Awaiting Ack MQTT_OK 
tcp-mqttprot | dispatch 
tcp-mqttprot | Current state: awaiting ack 
 
tcp-mqttprot | exit Awaiting Ack 
tcp-mqttprot | Current state: awaiting ack 
 

tcp-mqttprot | entry Wait To Publish 
tcp-mqttprot | Current state: awaiting ack 
tcp-mqttprot | publishTime: 5 seconds 
tcp-mqttprot | publishTmr started 
tcp-conmgr | Sent 26 bytes
tcp-conmgr | tcp_recv_callback 
tcp-conmgr | read_data 
tcp-conmgr | Current state: connected 
tcp-conmgr | socketConnected 
tcp-conmgr | Current state: connected 
tcp-mqttprot | dispatch 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | dispatch 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | isLocked 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | exit Wait To Publish 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | publish 
tcp-mqttprot | Current state: wait to publish 
mqttc_pal_recvall | tpcb state: 4 
mqttc_pal_send_all | tpcb state: 4 
 
Published: Hello from STM32! to stm32/data

 tcp-mqttprot | entry Wait To Publish 
tcp-mqttprot | Current state: wait to publish 
tcp-mqttprot | publishTime: 5 seconds 
tcp-mqttprot | publishTmr started 
tcp-conmgr | Sent 31 bytes
tcp-mqttprot | dispatch 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | isLocked 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | exit Wait To Publish 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | publish 
tcp-mqttprot | Current state: wait to publish 
mqttc_pal_recvall | tpcb state: 4 
mqttc_pal_send_all | tpcb state: 4 
 
Published: Hello from STM32! to stm32/data

 tcp-mqttprot | entry Wait To Publish 
tcp-mqttprot | Current state: wait to publish 
tcp-mqttprot | publishTime: 5 seconds 
tcp-mqttprot | publishTmr started 
tcp-conmgr | Sent 31 bytes
tcp-mqttprot | dispatch 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | isLocked 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | exit Wait To Publish 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | publish 
tcp-mqttprot | Current state: wait to publish 
mqttc_pal_recvall | tpcb state: 4 
mqttc_pal_send_all | tpcb state: 4 
 
Published: Hello from STM32! to stm32/data

 tcp-mqttprot | entry Wait To Publish 
tcp-mqttprot | Current state: wait to publish 
tcp-mqttprot | publishTime: 5 seconds 
tcp-mqttprot | publishTmr started 
tcp-conmgr | Sent 31 bytes
tcp-mqttprot | dispatch 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | isLocked 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | exit Wait To Publish 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | publish 
tcp-mqttprot | Current state: wait to publish 
mqttc_pal_recvall | tpcb state: 4 
mqttc_pal_send_all | tpcb state: 4 
 
Published: Hello from STM32! to stm32/data

 tcp-mqttprot | entry Wait To Publish 
tcp-mqttprot | Current state: wait to publish 
tcp-mqttprot | publishTime: 5 seconds 
tcp-mqttprot | publishTmr started 
tcp-conmgr | Sent 31 bytes
tcp-mqttprot | dispatch 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | isLocked 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | exit Wait To Publish 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | publish 
tcp-mqttprot | Current state: wait to publish 
mqttc_pal_recvall | tpcb state: 4 
mqttc_pal_send_all | tpcb state: 4 
 
Published: Hello from STM32! to stm32/data

 tcp-mqttprot | entry Wait To Publish 
tcp-mqttprot | Current state: wait to publish 
tcp-mqttprot | publishTime: 5 seconds 
tcp-mqttprot | publishTmr started 
tcp-conmgr | Sent 31 bytes
tcp-mqttprot | dispatch 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | isLocked 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | exit Wait To Publish 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | publish 
tcp-mqttprot | Current state: wait to publish 
mqttc_pal_recvall | tpcb state: 4 
mqttc_pal_send_all | tpcb state: 4 
 
mqttc_pal_send_all | tpcb state: 4 
 
Published: Hello from STM32! to stm32/data

 tcp-mqttprot | entry Wait To Publish 
tcp-mqttprot | Current state: wait to publish 
tcp-mqttprot | publishTime: 5 seconds 
tcp-mqttprot | publishTmr started 
tcp-conmgr | Sent 26 bytes
tcp-conmgr | tcp_recv_callback 
tcp-conmgr | Connection closed
tcp-mqttprot | dispatch 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | isLocked 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | exit Wait To Publish 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | publish 
tcp-mqttprot | Current state: wait to publish 

Published: Hello from STM32! to stm32/data

 tcp-mqttprot | entry Wait To Publish 
tcp-mqttprot | Current state: wait to publish 
tcp-mqttprot | publishTime: 5 seconds 
tcp-mqttprot | publishTmr started 
tcp-mqttprot | dispatch 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | isLocked 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | exit Wait To Publish 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | publish 
tcp-mqttprot | Current state: wait to publish 
mqttc_pal_recvall | tpcb state: 0 
Publish failed: -1

 tcp-mqttprot | entry Wait To Publish 
tcp-mqttprot | Current state: wait to publish 
tcp-mqttprot | publishTime: 5 seconds 
tcp-mqttprot | publishTmr started 
tcp-mqttprot | dispatch 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | isLocked 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | exit Wait To Publish 
tcp-mqttprot | Current state: wait to publish 
 
tcp-mqttprot | publish 
tcp-mqttprot | Current state: wait to publish 
Publish failed: -1
```

### mosquitto broker


```json
» mosquitto -c mosquitto.conf -v
1744847883: mosquitto version 2.0.11 starting
1744847883: Config loaded from mosquitto.conf.
1744847883: Opening ipv4 listen socket on port 1883.
1744847883: Opening ipv6 listen socket on port 1883.
1744847883: mosquitto version 2.0.11 running

1744874958: New connection from 192.168.1.78:52432 on port 1883.
1744874958: New client connected from 192.168.1.78:52432 as stm32_client (p2, c0, k9999).
1744874958: No will message specified.
1744874958: Sending CONNACK to stm32_client (0, 0)
1744874963: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1744874968: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1744874973: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1744874978: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1744874983: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (17 bytes))
1744874988: Bad client stm32_client sending multiple CONNECT messages.
1744874988: Client stm32_client disconnected due to protocol error.
```

### wireshark captures

```json
No.     Time           Source                Destination           Protocol Length Info
      8 1.167083811    192.168.1.78          192.168.1.81          TCP      60     52432 → 1883 [SYN] Seq=0 Win=2144 Len=0 MSS=536

Frame 8: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface enp8s0, id 0
Ethernet II, Src: STMicroe_00:00:00 (00:80:e1:00:00:00), Dst: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c)
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 0, Len: 0

No.     Time           Source                Destination           Protocol Length Info
      9 1.167120351    192.168.1.81          192.168.1.78          TCP      58     1883 → 52432 [SYN, ACK] Seq=0 Ack=1 Win=64240 Len=0 MSS=1460

Frame 9: 58 bytes on wire (464 bits), 58 bytes captured (464 bits) on interface enp8s0, id 0
Ethernet II, Src: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c), Dst: STMicroe_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 0, Ack: 1, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     10 1.173093223    192.168.1.78          192.168.1.81          TCP      60     52432 → 1883 [ACK] Seq=1 Ack=1 Win=2144 Len=0

Frame 10: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface enp8s0, id 0
Ethernet II, Src: STMicroe_00:00:00 (00:80:e1:00:00:00), Dst: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c)
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 1, Ack: 1, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     11 1.251629531    192.168.1.78          192.168.1.81          MQTT     80     Connect Command

Frame 11: 80 bytes on wire (640 bits), 80 bytes captured (640 bits) on interface enp8s0, id 0
Ethernet II, Src: STMicroe_00:00:00 (00:80:e1:00:00:00), Dst: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c)
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 1, Ack: 1, Len: 26
MQ Telemetry Transport Protocol, Connect Command

No.     Time           Source                Destination           Protocol Length Info
     12 1.251662640    192.168.1.81          192.168.1.78          TCP      54     1883 → 52432 [ACK] Seq=1 Ack=27 Win=64214 Len=0

Frame 12: 54 bytes on wire (432 bits), 54 bytes captured (432 bits) on interface enp8s0, id 0
Ethernet II, Src: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c), Dst: STMicroe_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 1, Ack: 27, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     13 1.251735712    192.168.1.81          192.168.1.78          MQTT     58     Connect Ack

Frame 13: 58 bytes on wire (464 bits), 58 bytes captured (464 bits) on interface enp8s0, id 0
Ethernet II, Src: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c), Dst: STMicroe_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 1, Ack: 27, Len: 4
MQ Telemetry Transport Protocol, Connect Ack

No.     Time           Source                Destination           Protocol Length Info
     14 1.426076513    192.168.1.78          192.168.1.81          TCP      60     52432 → 1883 [ACK] Seq=27 Ack=5 Win=2140 Len=0

Frame 14: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface enp8s0, id 0
Ethernet II, Src: STMicroe_00:00:00 (00:80:e1:00:00:00), Dst: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c)
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 27, Ack: 5, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     16 6.326615736    192.168.1.78          192.168.1.81          MQTT     85     Publish Message [stm32/data]

Frame 16: 85 bytes on wire (680 bits), 85 bytes captured (680 bits) on interface enp8s0, id 0
Ethernet II, Src: STMicroe_00:00:00 (00:80:e1:00:00:00), Dst: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c)
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 27, Ack: 5, Len: 31
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
     17 6.366946733    192.168.1.81          192.168.1.78          TCP      54     1883 → 52432 [ACK] Seq=5 Ack=58 Win=64183 Len=0

Frame 17: 54 bytes on wire (432 bits), 54 bytes captured (432 bits) on interface enp8s0, id 0
Ethernet II, Src: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c), Dst: STMicroe_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 58, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     20 11.384619643   192.168.1.78          192.168.1.81          MQTT     85     Publish Message [stm32/data]

Frame 20: 85 bytes on wire (680 bits), 85 bytes captured (680 bits) on interface enp8s0, id 0
Ethernet II, Src: STMicroe_00:00:00 (00:80:e1:00:00:00), Dst: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c)
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 58, Ack: 5, Len: 31
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
     21 11.384644256   192.168.1.81          192.168.1.78          TCP      54     1883 → 52432 [ACK] Seq=5 Ack=89 Win=64152 Len=0

Frame 21: 54 bytes on wire (432 bits), 54 bytes captured (432 bits) on interface enp8s0, id 0
Ethernet II, Src: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c), Dst: STMicroe_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 89, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     22 16.441608622   192.168.1.78          192.168.1.81          MQTT     85     Publish Message [stm32/data]

Frame 22: 85 bytes on wire (680 bits), 85 bytes captured (680 bits) on interface enp8s0, id 0
Ethernet II, Src: STMicroe_00:00:00 (00:80:e1:00:00:00), Dst: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c)
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 89, Ack: 5, Len: 31
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
     23 16.441662039   192.168.1.81          192.168.1.78          TCP      54     1883 → 52432 [ACK] Seq=5 Ack=120 Win=64121 Len=0

Frame 23: 54 bytes on wire (432 bits), 54 bytes captured (432 bits) on interface enp8s0, id 0
Ethernet II, Src: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c), Dst: STMicroe_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 120, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     27 21.498626972   192.168.1.78          192.168.1.81          MQTT     85     Publish Message [stm32/data]

Frame 27: 85 bytes on wire (680 bits), 85 bytes captured (680 bits) on interface enp8s0, id 0
Ethernet II, Src: STMicroe_00:00:00 (00:80:e1:00:00:00), Dst: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c)
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 120, Ack: 5, Len: 31
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
     28 21.498651040   192.168.1.81          192.168.1.78          TCP      54     1883 → 52432 [ACK] Seq=5 Ack=151 Win=64090 Len=0

Frame 28: 54 bytes on wire (432 bits), 54 bytes captured (432 bits) on interface enp8s0, id 0
Ethernet II, Src: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c), Dst: STMicroe_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 151, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     29 26.555635114   192.168.1.78          192.168.1.81          MQTT     85     Publish Message [stm32/data]

Frame 29: 85 bytes on wire (680 bits), 85 bytes captured (680 bits) on interface enp8s0, id 0
Ethernet II, Src: STMicroe_00:00:00 (00:80:e1:00:00:00), Dst: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c)
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 151, Ack: 5, Len: 31
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
     30 26.555662811   192.168.1.81          192.168.1.78          TCP      54     1883 → 52432 [ACK] Seq=5 Ack=182 Win=64059 Len=0

Frame 30: 54 bytes on wire (432 bits), 54 bytes captured (432 bits) on interface enp8s0, id 0
Ethernet II, Src: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c), Dst: STMicroe_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 182, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     34 31.612632462   192.168.1.78          192.168.1.81          MQTT     80     Connect Command

Frame 34: 80 bytes on wire (640 bits), 80 bytes captured (640 bits) on interface enp8s0, id 0
Ethernet II, Src: STMicroe_00:00:00 (00:80:e1:00:00:00), Dst: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c)
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 182, Ack: 5, Len: 26
MQ Telemetry Transport Protocol, Connect Command

No.     Time           Source                Destination           Protocol Length Info
     35 31.612658672   192.168.1.81          192.168.1.78          TCP      54     1883 → 52432 [ACK] Seq=5 Ack=208 Win=64059 Len=0

Frame 35: 54 bytes on wire (432 bits), 54 bytes captured (432 bits) on interface enp8s0, id 0
Ethernet II, Src: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c), Dst: STMicroe_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 208, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     36 31.612725587   192.168.1.81          192.168.1.78          TCP      54     1883 → 52432 [FIN, ACK] Seq=5 Ack=208 Win=64059 Len=0

Frame 36: 54 bytes on wire (432 bits), 54 bytes captured (432 bits) on interface enp8s0, id 0
Ethernet II, Src: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c), Dst: STMicroe_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 208, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     37 31.638397331   192.168.1.78          192.168.1.81          MQTT     85     Publish Message [stm32/data]

Frame 37: 85 bytes on wire (680 bits), 85 bytes captured (680 bits) on interface enp8s0, id 0
Ethernet II, Src: STMicroe_00:00:00 (00:80:e1:00:00:00), Dst: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c)
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 208, Ack: 5, Len: 31
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
     38 31.638436083   192.168.1.81          192.168.1.78          TCP      54     1883 → 52432 [RST] Seq=5 Win=0 Len=0

Frame 38: 54 bytes on wire (432 bits), 54 bytes captured (432 bits) on interface enp8s0, id 0
Ethernet II, Src: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c), Dst: STMicroe_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     39 31.644441675   192.168.1.78          192.168.1.81          TCP      60     52432 → 1883 [RST, ACK] Seq=239 Ack=6 Win=24584 Len=0

Frame 39: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface enp8s0, id 0
Ethernet II, Src: STMicroe_00:00:00 (00:80:e1:00:00:00), Dst: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c)
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 239, Ack: 6, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     40 31.644478489   192.168.1.78          192.168.1.81          TCP      60     [TCP Window Update] 52432 → 1883 [ACK] Seq=239 Ack=6 Win=2139 Len=0

Frame 40: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface enp8s0, id 0
Ethernet II, Src: STMicroe_00:00:00 (00:80:e1:00:00:00), Dst: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c)
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 239, Ack: 6, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     41 31.644489071   192.168.1.81          192.168.1.78          TCP      54     1883 → 52432 [RST] Seq=6 Win=0 Len=0

Frame 41: 54 bytes on wire (432 bits), 54 bytes captured (432 bits) on interface enp8s0, id 0
Ethernet II, Src: QuantaCo_2e:71:6c (2c:60:0c:2e:71:6c), Dst: STMicroe_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 6, Len: 0
```

## stm32 lwip tcp MqttProt Client state machine (without SyncRegion state machine)

### stm32 mqttc mqttc_client
```json 
 » ./STM32_Programmer_CLI -c port=ttyACM0 br=115200 console
      -------------------------------------------------------------------
                        STM32CubeProgrammer v2.17.0                  
      -------------------------------------------------------------------

Serial Port ttyACM0 is successfully opened.
 Port configuration: parity = even, baudrate = 115200, data-bit = 8,
                     stop-bit = 1,0, flow-control = off

main | SIZEOF_EP3STO: 1024 
main | SIZEOF_EP3_BLOCK: 72 
Waiting for network interface...
Waiting for link...
Link up - IP: 192.168.1.78
Dispatching event 65533 to SMA 0x801c3e8
tcp-conmgr | init 
Dispatching event 65533 to SMA 0x801c728
Dispatching event 0 to SMA 0x801c3e8
socketOpen 
tcp_connect_attempt 
p_connect_callback 
TCP Connected
Dispatching event 24 to SMA 0x801c370
socketConnected 
Dispatching event 31 to SMA 0x801c728
Polling
Current state: connected 
tcp-conmgr | pre alloc TcpSendEvt 
tcp-conmgr | post alloc TcpSendEvt 
test data: Hello, TCP ! 
 
data size: 15 
evt->buf: Hello, TCP ! 
 
evt->size: 15 
evt->evt.e: 25 
tcp-conmgr | Posting TcpSendEvt 
tcp-conmgr | After post TcpSendEvt 
Dispatching event 25 to SMA 0x801c318
send_data 
Current state: connected 
evt->buf: Hello, TCP ! 
 
evt size: 15 
evet e: 25 
tpcb != NULL: yes 
socketConnected 
Dispatching event 9 to SMA 0x801c468
flush_data 
Dispatching event 31 to SMA 0x801c728
Sent 15 bytes
TCP error: -14
Dispatching event 27 to SMA 0x801c318
Dispatching event 33 to SMA 0x801c318
```

### mosquitto broker
```json
» mosquitto -c mosquitto.conf -v
1743478430: mosquitto version 2.0.11 starting
1743478430: Config loaded from mosquitto.conf.
1743478430: Opening ipv4 listen socket on port 1883.
1743478430: Opening ipv6 listen socket on port 1883.
1743478430: mosquitto version 2.0.11 running

1743478450: New connection from 192.168.1.78:52432 on port 1883.
1743478464: Client <unknown> disconnected due to protocol error.
```

## stm32 lwip tcp ConMgr state machine

### TcpConMgr_connected <-> TcpConMgr_receiving interaction

```c 
static err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {

    printf("tcp_recv_callback \n");
    TcpConMgr *me = (TcpConMgr *)arg;

    if (p != NULL) {

        if (me->recv_len + p->tot_len <= sizeof(me->recv_buffer)) {

            memcpy(me->recv_buffer + me->recv_len, p->payload, p->tot_len);
            me->recv_len += p->tot_len;
            
            TcpReceiveEvt * evt = RKH_ALLOC_EVT(TcpReceiveEvt, evRecv, me);
            if (evt == NULL) {
                return ERR_OK;
            }

            size_t bytes_to_read = (me->recv_len < RECV_BUFF_SIZE) ? me->recv_len : RECV_BUFF_SIZE;
            memcpy(evt->buf, me->recv_buffer + me->recv_index, bytes_to_read);
            evt->size = bytes_to_read;
            me->recv_index += bytes_to_read;

            if (me->recv_index >= me->recv_len) {
                me->recv_len = 0;
                me->recv_index = 0;
            } else {
                me->recv_len -= bytes_to_read;
            }

            RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, evt), me);

        } else {
            ... 
        }
    } else {
        ...
    }

    return ERR_OK;
}

static void read_data(TcpConMgr *const me, RKH_EVT_T *pe) {
    
    TcpReceiveEvt * evt = RKH_DOWNCAST(TcpReceiveEvt, pe);

    me->recv_len = evt->size;

    if (me->recv_len > 0) {
        
        RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Ok), me);
    }
}
```

#### tcp-conmgr.h
```c 
typedef struct TcpReceiveEvt TcpReceiveEvt;
struct TcpReceiveEvt
{
    RKH_EVT_T evt;
    unsigned char buf[RECV_BUFF_SIZE];
    ruint size;
};
```

#### main.c

```c 
#define SIZEOF_EP2STO 1024  // Total size in bytes (e.g., 16 events of 8 bytes each)
#define SIZEOF_EP2_BLOCK sizeof(TcpReceiveEvt)  // Block size matches the event
static rui8_t evPool2Sto[SIZEOF_EP2STO];
```

#### host side 

```json
» nc -l 192.168.1.81 1883

hey there
Hello, TCP !
hey hey
Hello, TCP !
Hello, TCP !
```

#### mqttc_client side - stm32

```json 
» ./STM32_Programmer_CLI -c port=ttyACM0 br=115200 console
      -------------------------------------------------------------------
                        STM32CubeProgrammer v2.17.0                  
      -------------------------------------------------------------------

Serial Port ttyACM0 is successfully opened.
 Port configuration: parity = even, baudrate = 115200, data-bit = 8,
                     stop-bit = 1,0, flow-control = off

                     main | SIZEOF_EP3STO: 1024
main | SIZEOF_EP3_BLOCK: 72
Waiting for network interface...
Waiting for link...
Link up - IP: 192.168.1.78
Dispatching event 65533 to SMA 0x8016d40
tcp-conmgr | init
Dispatching event 0 to SMA 0x8016d40
socketOpen
tcp_connect_attempt
connect_callback
TCP Connected
Dispatching event 24 to SMA 0x8016cc8
socketConnected
tcp_recv_callback
Received 1 bytes
tcp-conmgr | pre malloc failed TcpReceivedEvt
tcp-conmgr | post malloc failed TcpReceivedEvt
tcp-conmgr | Posting TcpReceivedEvt
tcp-conmgr | After post TcpReceivedEvt
Dispatching event 28 to SMA 0x8016c70
read_data
Current state: connected
evt->buf:

evt size: 1
evet e: 28
socketConnected
Dispatching event 9 to SMA 0x8016d78
tcp_recv_callback
Received 10 bytes
tcp-conmgr | pre malloc failed TcpReceivedEvt
tcp-conmgr | post malloc failed TcpReceivedEvt
tcp-conmgr | Posting TcpReceivedEvt
tcp-conmgr | After post TcpReceivedEvt
Dispatching event 28 to SMA 0x8016c70
read_data
Current state: connected
evt->buf:
hey there

evt size: 11
evet e: 28
socketConnected
Dispatching event 9 to SMA 0x8016d78


Polling
Current state: connected
tcp-conmgr | pre alloc TcpSendEvt
tcp-conmgr | post alloc TcpSendEvt
test data: Hello, TCP !

data size: 15
evt->buf: Hello, TCP !

evt->size: 15
evt->evt.e: 25
tcp-conmgr | Posting TcpSendEvt
tcp-conmgr | After post TcpSendEvt
Dispatching event 25 to SMA 0x8016c70
send_data
Current state: connected
evt->buf: Hello, TCP !

evt size: 15
evet e: 25
tpcb != NULL: yes
socketConnected
Dispatching event 9 to SMA 0x8016dc0
flush_data
Sent 15 bytes
Dispatching event 27 to SMA 0x8016c70
tcp_recv_callback
Received 8 bytes
tcp-conmgr | pre malloc failed TcpReceivedEvt
tcp-conmgr | post malloc failed TcpReceivedEvt
tcp-conmgr | Posting TcpReceivedEvt
tcp-conmgr | After post TcpReceivedEvt
Dispatching event 28 to SMA 0x8016c70
read_data
Current state: connected
evt->buf:
hey there
hey hey
```


### TcpConMgr_connected <-> TcpConMgr_sending interaction 

based on framework parameters defined in rkhcfg.h, altered SEND_BUFF_SIZE, RECV_BUFF_SIZE and pool object-like macros 
in main.c to achieve the post a TcpSendEvt from tcp_poll_callback to send_data 

```c 
static err_t tcp_poll_callback(void *arg, struct tcp_pcb *tpcb) {

    ....
    
    RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, evt), me);

    return ERR_OK;
}

static void send_data(TcpConMgr *const me, RKH_EVT_T *pe) {
    
    TcpSendEvt * evt = RKH_DOWNCAST(TcpSendEvt, pe);
    
    if (me->tpcb != NULL) {

        err_t err = tcp_write(me->tpcb, evt->buf, evt->size, TCP_WRITE_FLAG_COPY);

        if (err == ERR_OK) {
            tcp_output(me->tpcb);
            RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Ok), me);
        } else {
            printf("tcp_write failed: %d\n", err);
            RKH_SMA_POST_FIFO(tcpConMgr, RKH_UPCAST(RKH_EVT_T, &e_Error), me);
        }
    }
}
```


#### rkhcfg.h

```c 
/* --- Configuration options related to framework ------------------------- */

#define RKH_CFG_FWK_MAX_SMA             6u

#define RKH_CFG_FWK_DYN_EVT_EN          RKH_ENABLED

#define RKH_CFG_FWK_MAX_EVT_POOL        3u

#define RKH_CFG_FWK_SIZEOF_EVT          16u

#define RKH_CFG_FWK_MAX_SIGNALS         48u

#define RKH_CFG_FWK_SIZEOF_EVT_SIZE     16u
```

#### tcp-conmgr.h 


```c 
#define SEND_BUFF_SIZE      64
#define RECV_BUFF_SIZE      64


/* .......................... Event definition ............................ */
typedef struct TcpSendEvt TcpSendEvt;
struct TcpSendEvt
{
    RKH_EVT_T evt;
    unsigned char buf[SEND_BUFF_SIZE];
    ruint size;
};

typedef struct TcpReceiveEvt TcpReceiveEvt;
struct TcpReceiveEvt
{
    RKH_EVT_T evt;
    unsigned char buf[RECV_BUFF_SIZE];
    ruint size;
};
```

#### main.c

```c 
#define SIZEOF_EP3STO 1024  // Total size in bytes (e.g., 16 events of 8 bytes each)
#define SIZEOF_EP3_BLOCK sizeof(TcpSendEvt)  // Block size matches the event
static rui8_t evPool3Sto[SIZEOF_EP3STO];
```



#### host side 
```json
» nc -l 192.168.1.81 1883


Hello, TCP !Hello, TCP !Hello, TCP !Hello, TCP !Hello, TCP !Hello, TCP !^C
```

#### mqttc_client side

```json
main | SIZEOF_EP3STO: 1024 
main | SIZEOF_EP3_BLOCK: 72 
Waiting for network interface...
Waiting for link...
Link up - IP: 192.168.1.78
Dispatching event 65533 to SMA 0x8016b9c
tcp-conmgr | init 
Dispatching event 0 to SMA 0x8016b9c
socketOpen 
tcp_connect_attempt 
_connect_callback 
TCP Connected
Dispatching event 24 to SMA 0x8016b24
socketConnected 
tcp_recv_callback 
Received 1 bytes
Dispatching event 28 to SMA 0x8016acc
read_data 
socketConnected 
Dispatching event 9 to SMA 0x8016bd4
Polling
Current state: connected 
tcp-conmgr | pre alloc TcpSendEvt 
tcp-conmgr | post alloc TcpSendEvt 
test data: Hello, TCP ! 
data size: 13 
evt->buf: Hello, TCP ! 
evt->size: 13 
evt->evt.e: 25 
tcp-conmgr | Posting TcpSendEvt 
tcp-conmgr | After post TcpSendEvt 
main | SIZEOF_EP3STO: 1024 
main | SIZEOF_EP3_BLOCK: 72 
Waiting for network interface...
Waiting for link...
Link up - IP: 192.168.1.78
Dispatching event 65533 to SMA 0x8016b9c
tcp-conmgr | init 
Dispatching event 0 to SMA 0x8016b9c
socketOpen 
tcp_connect_attempt 
cp_connect_callback 
TCP Connected
Dispatching event 24 to SMA 0x8016b24
socketConnected 
tcp_recv_callback 
Received 2 bytes
Dispatching event 28 to SMA 0x8016acc
read_data 
socketConnected 
Dispatching event 9 to SMA 0x8016bd4
Polling
Current state: connected 
tcp-conmgr | pre alloc TcpSendEvt 
tcp-conmgr | post alloc TcpSendEvt 
test data: Hello, TCP ! 
data size: 13 
evt->buf: Hello, TCP ! 
evt->size: 13 
evt->evt.e: 25 
tcp-conmgr | Posting TcpSendEvt 
tcp-conmgr | After post TcpSendEvt 
Dispatching event 25 to SMA 0x8016acc
send_data 
Current state: connected 
evt->buf: Hello, TCP ! 
evt size: 13 
evet e: 25 
tpcb != NULL: yes 
socketConnected 
Dispatching event 9 to SMA 0x8016c1c
flush_data 
Sent 13 bytes
Dispatching event 27 to SMA 0x8016acc
Polling
Current state: connected 
tcp-conmgr | pre alloc TcpSendEvt 
tcp-conmgr | post alloc TcpSendEvt 
test data: Hello, TCP ! 
data size: 13 
evt->buf: Hello, TCP ! 
evt->size: 13 
evt->evt.e: 25 
tcp-conmgr | Posting TcpSendEvt 
tcp-conmgr | After post TcpSendEvt 
Dispatching event 25 to SMA 0x8016acc
send_data 
Current state: connected 
evt->buf: Hello, TCP ! 
evt size: 13 
evet e: 25 
tpcb != NULL: yes 
socketConnected 
Dispatching event 9 to SMA 0x8016c1c
flush_data 
Sent 13 bytes
Dispatching event 27 to SMA 0x8016acc
Polling
Current state: connected 
tcp-conmgr | pre alloc TcpSendEvt 
tcp-conmgr | post alloc TcpSendEvt 
test data: Hello, TCP ! 
data size: 13 
evt->buf: Hello, TCP ! 
evt->size: 13 
evt->evt.e: 25 
tcp-conmgr | Posting TcpSendEvt 
tcp-conmgr | After post TcpSendEvt 
Dispatching event 25 to SMA 0x8016acc
send_data 
Current state: connected 
evt->buf: Hello, TCP ! 
evt size: 13 
evet e: 25 
tpcb != NULL: yes 
socketConnected 
Dispatching event 9 to SMA 0x8016c1c
flush_data 
Sent 13 bytes
Dispatching event 27 to SMA 0x8016acc
Polling
Current state: connected 
tcp-conmgr | pre alloc TcpSendEvt 
tcp-conmgr | post alloc TcpSendEvt 
test data: Hello, TCP ! 
data size: 13 
evt->buf: Hello, TCP ! 
evt->size: 13 
evt->evt.e: 25 
tcp-conmgr | Posting TcpSendEvt 
tcp-conmgr | After post TcpSendEvt 
Dispatching event 25 to SMA 0x8016acc
send_data 
Current state: connected 
evt->buf: Hello, TCP ! 
evt size: 13 
evet e: 25 
tpcb != NULL: yes 
socketConnected 
Dispatching event 9 to SMA 0x8016c1c
flush_data 
Sent 13 bytes
Dispatching event 27 to SMA 0x8016acc
Polling
Current state: connected 
tcp-conmgr | pre alloc TcpSendEvt 
tcp-conmgr | post alloc TcpSendEvt 
test data: Hello, TCP ! 
data size: 13 
evt->buf: Hello, TCP ! 
evt->size: 13 
evt->evt.e: 25 
tcp-conmgr | Posting TcpSendEvt 
tcp-conmgr | After post TcpSendEvt 
Dispatching event 25 to SMA 0x8016acc
send_data 
Current state: connected 
evt->buf: Hello, TCP ! 
evt size: 13 
evet e: 25 
tpcb != NULL: yes 
socketConnected 
Dispatching event 9 to SMA 0x8016c1c
flush_data 
Sent 13 bytes
Dispatching event 27 to SMA 0x8016acc
Polling
Current state: connected 
tcp-conmgr | pre alloc TcpSendEvt 
tcp-conmgr | post alloc TcpSendEvt 
test data: Hello, TCP ! 
data size: 13 
evt->buf: Hello, TCP ! 
evt->size: 13 
evt->evt.e: 25 
tcp-conmgr | Posting TcpSendEvt 
tcp-conmgr | After post TcpSendEvt 
Dispatching event 25 to SMA 0x8016acc
send_data 
Current state: connected 
evt->buf: Hello, TCP ! 
evt size: 13 
evet e: 25 
tpcb != NULL: yes 
socketConnected 
Dispatching event 9 to SMA 0x8016c1c
flush_data 
Sent 13 bytes
Dispatching event 27 to SMA 0x8016acc
tcp_recv_callback 
Connection closed
Dispatching event 33 to SMA 0x8016acc
```


### test 1

#### host side

```json 
» nc -l 192.168.1.81 1883

hey there
haloo
```

### mqttc_client side

```json
 » ./STM32_Programmer_CLI -c port=ttyACM0 br=115200 console
      -------------------------------------------------------------------
                        STM32CubeProgrammer v2.17.0                  
      -------------------------------------------------------------------

Waiting for network interface...
Waiting for link...
Link up - IP: 192.168.1.78
init 
open 
tcp_connect_attempt 
socketOpen 
tcp_connect_attempt 
tcp_connect_callback 
TCP Connected
tcp_recv_callback 
Received 1 bytes

tcp_recv_callback 
Received 10 bytes
tcp_recv_callback 
Received 6 bytes
tcp_recv_callback 
Connection closed



```

## changes & notes

1. remove defer functions

2. remove tcp_connect_attempt as effect function when evOpen is triggered in TcpConMgr_inactive

```c
# old
                RKH_TRREG(evOpen, NULL, open, &TcpConMgr_active),

# new
                RKH_TRREG(evOpen, NULL, NULL, &TcpConMgr_active),
```

3. remove tcp_connect_attempt as effecto function when evTimeout is triggered in TcpConMgr_connecting

```c
# old
                RKH_TRREG(evTimeout, NULL, tcp_connect_attempt, &TcpConMgr_connecting),

# new
                RKH_TRREG(evTimeout, NULL, NULL, &TcpConMgr_connecting),
```

**at this point, conMgr with tcp mqttc_client and pbuf is working as expected**

4. events posted between conMgr and mqttProt

+ e_Sent: from conMgr to mqttProt in sendOk function

```c 
static RKH_ROM_STATIC_EVENT(e_Sent,     evSent);

RKH_CREATE_BASIC_STATE(ConMgr_waitOk, NULL, NULL, &ConMgr_sending, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_waitOk)
    RKH_TRREG(evOk, NULL,  sendOk, &ConMgr_sendingFinal),    
RKH_END_TRANS_TABLE

static void
sendOk(ConMgr *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

    me->retryCount = 0;
    RKH_SMA_POST_FIFO(mqttProt, &e_Sent, conMgr);
}
```


+ e_Sendfail: from conMgr to mqttProt in sendFail function

```c 
static RKH_ROM_STATIC_EVENT(e_SendFail, evSendFail);

RKH_CREATE_BASIC_STATE(ConMgr_inactive, NULL, NULL, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_inactive)
    RKH_TRINT(evSend, NULL, sendFail),
    ....
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(ConMgr_active, NULL, NULL, RKH_ROOT, 
                             &ConMgr_initialize, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_active)
    RKH_TRINT(evSend, NULL, sendFail),
    ....
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(ConMgr_sending, NULL, NULL, 
                             &ConMgr_connected, &ConMgr_waitPrompt, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_sending)
	RKH_TRREG(evError, NULL, sendFail, &ConMgr_idle),
    RKH_TRREG(evNoResponse, NULL, sendFail, &ConMgr_idle),
    ....
RKH_END_TRANS_TABLE



static void
sendFail(ConMgr *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

    RKH_SMA_POST_FIFO(mqttProt, &e_SendFail, conMgr);
	ModCmd_init();
}
```

+ e_NetConnected: from conMgr to mqttProt in socketConnected entry function of ConMgr_connected 

```c 
static RKH_ROM_STATIC_EVENT(e_NetConnected, evNetConnected);

RKH_CREATE_COMP_REGION_STATE(ConMgr_connected, 
                             socketConnected, socketDisconnected, 
                             &ConMgr_connecting, &ConMgr_idle, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);

static void
socketConnected(ConMgr *const me)
{
    (void)me;

    me->retryCount = 0;
    RKH_SMA_POST_FIFO(mqttProt, &e_NetConnected, conMgr);
    bsp_netStatus(ConnectedSt);
}
```


+ e_NetDisconnected: from conMgr to mqttProt in socketConnected exit function of ConMgr_connected

```c 

static RKH_ROM_STATIC_EVENT(e_NetDisconnected, evNetDisconnected);

RKH_CREATE_COMP_REGION_STATE(ConMgr_connected, 
                             socketConnected, socketDisconnected, 
                             &ConMgr_connecting, &ConMgr_idle, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);

static void
socketDisconnected(ConMgr *const me)
{
    (void)me;

    RKH_SMA_POST_FIFO(mqttProt, &e_NetDisconnected, conMgr);
    bsp_netStatus(DisconnectedSt);
}
```


+ e_Received: 

```c 
typedef struct ReceivedEvt ReceivedEvt;
struct ReceivedEvt
{
    RKH_EVT_T evt;
    unsigned char buf[RECV_BUFF_SIZE];
    ruint size;
};


ReceivedEvt e_Received;

static void 
readData(ConMgr *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

    RKH_SET_STATIC_EVENT(RKH_UPCAST(RKH_EVT_T, &e_Received), evReceived);
    ModCmd_readData();
}

static void
recvOk(ConMgr *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

    me->retryCount = 0;
    RKH_SMA_POST_FIFO(mqttProt, RKH_UPCAST(RKH_EVT_T, &e_Received), conMgr);
}

RKH_CREATE_BASIC_STATE(ConMgr_receiving, NULL, NULL, &ConMgr_connected, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_receiving)
    RKH_TRREG(evOk, NULL,  recvOk, &ConMgr_idle),
	....
RKH_END_TRANS_TABLE
```


+ e_RecvFail: 

```c 
RKH_TRREG(evOk, NULL,  recvOk, &ConMgr_idle),


static void
recvFail(ConMgr *const me, RKH_EVT_T *pe)
{
    (void)pe;
    (void)me;

    RKH_SMA_POST_FIFO(mqttProt, &e_RecvFail, conMgr);
	ModCmd_init();
}


RKH_CREATE_BASIC_STATE(ConMgr_inactive, NULL, NULL, RKH_ROOT, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_inactive)
    RKH_TRINT(evRecv, NULL, recvFail),
    ....
RKH_END_TRANS_TABLE

RKH_CREATE_COMP_REGION_STATE(ConMgr_active, NULL, NULL, RKH_ROOT, 
                             &ConMgr_initialize, NULL,
                             RKH_NO_HISTORY, NULL, NULL, NULL, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_active)
    RKH_TRINT(evRecv, NULL, recvFail),
    ...
RKH_END_TRANS_TABLE

RKH_CREATE_BASIC_STATE(ConMgr_receiving, NULL, NULL, &ConMgr_connected, NULL);
RKH_CREATE_TRANS_TABLE(ConMgr_receiving)
	RKH_TRREG(evError, NULL, recvFail, &ConMgr_idle),
	RKH_TRREG(evNoResponse, NULL, recvFail, &ConMgr_idle),
	...
RKH_END_TRANS_TABLE
```


5. based on the original conMgr state machine, I changed the events posted internally and externally

```c 

// Internal Usage

static RKH_STATIC_EVENT(e_tout, evTimeout);
static RKH_ROM_STATIC_EVENT(e_Open, evOpen);
static RKH_ROM_STATIC_EVENT(e_Close, evClose);

static RKH_ROM_STATIC_EVENT(e_Connected, evConnected);
static RKH_ROM_STATIC_EVENT(e_Disconnected, evDisconnected);


// External Usage. events used to post to mqttProt state machine

static RKH_ROM_STATIC_EVENT(e_NetConnected, evNetConnected);
static RKH_ROM_STATIC_EVENT(e_NetDisconnected, evNetDisconnected);

static RKH_ROM_STATIC_EVENT(e_Sent, evSent);
static RKH_ROM_STATIC_EVENT(e_Received, evReceived);

static RKH_ROM_STATIC_EVENT(e_SendFail, evSendFail);
static RKH_ROM_STATIC_EVENT(e_RecvFail, evRecvFail);
```

6. based on the original conMgr state machine, the use of RKH_SMA_POST_FIFO is
used to communicate with another state machine














































## simple publisher

### mosquitto broker

```json 
» mosquitto -c mosquitto.conf -v
1740848426: mosquitto version 2.0.11 starting
1740848426: Config loaded from mosquitto.conf.
1740848426: Opening ipv4 listen socket on port 1883.
1740848426: Opening ipv6 listen socket on port 1883.
1740848426: mosquitto version 2.0.11 running
1740848434: New connection from 192.168.1.78:52432 on port 1883.
1740848434: New mqttc_client connected from 192.168.1.78:52432 as stm32_client (p2, c0, k30).
1740848434: No will message specified.
1740848434: Sending CONNACK to stm32_client (0, 0)
1740848437: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'topic', ... (7 bytes))
1740848442: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'topic', ... (7 bytes))
1740848447: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'topic', ... (7 bytes))
1740848452: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'topic', ... (7 bytes))
1740848457: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'topic', ... (7 bytes))
1740848462: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'topic', ... (7 bytes))
1740848467: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'topic', ... (7 bytes))
1740848472: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'topic', ... (7 bytes))
1740848477: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'topic', ... (7 bytes))
1740848482: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'topic', ... (7 bytes))
1740848487: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'topic', ... (7 bytes))
1740848492: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'topic', ... (7 bytes))
1740848497: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'topic', ... (7 bytes))
1740848502: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'topic', ... (7 bytes))
```


### stm32 mqttc_client

```json 
» ./STM32_Programmer_CLI -c port=ttyACM0 br=115200 console
      -------------------------------------------------------------------
                        STM32CubeProgrammer v2.17.0                  
      -------------------------------------------------------------------

Serial Port ttyACM0 is successfully opened.
 Port configuration: parity = even, baudrate = 115200, data-bit = 8,
                     stop-bit = 1,0, flow-control = off

Entering console mode :
Press F to send a file
Press E to exit
Press W to write mode

Waiting for network interface...
Waiting for link...
TCP connection initiated
MQTT mqttc_client initialized
tcp_connected: 1 
mqtt_connected: 0 
MQTT CONNECT sent
TCP Connected

Sent 26 bytes
Received 4 bytes:
Published message
Sent 16 bytes
Published message
Sent 16 bytes
Published message
Sent 16 bytes
Published message
Sent 16 bytes
Published message
Sent 16 bytes
Published message
Sent 16 bytes
Published message
Sent 16 bytes
Published message
Sent 16 bytes
Published message
Sent 16 bytes
Published message
Sent 16 bytes
Connection closed
```


### wireshark captures

```json 
 » cat stm32-mqtt-tcp-logs.txt 
No.     Time           Source                Destination           Protocol Length Info
     27 5.638745516    192.168.1.78          192.168.1.81          TCP      62     52432 → 1883 [SYN] Seq=0 Win=2144 Len=0 MSS=536

Frame 27: 62 bytes on wire (496 bits), 62 bytes captured (496 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 0, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     28 5.638765207    192.168.1.81          192.168.1.78          TCP      56     [TCP ACKed unseen segment] 1883 → 52432 [ACK] Seq=3106950861 Ack=251 Win=64070 Len=0

Frame 28: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 3106950861, Ack: 251, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     47 6.645043181    192.168.1.78          192.168.1.81          TCP      62     52432 → 1883 [RST, ACK] Seq=251 Ack=3106950861 Win=24584 Len=0

Frame 47: 62 bytes on wire (496 bits), 62 bytes captured (496 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 251, Ack: 3106950861, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     48 6.645043837    192.168.1.78          192.168.1.81          TCP      62     [TCP Retransmission] 52432 → 1883 [SYN] Seq=0 Win=2144 Len=0 MSS=536

Frame 48: 62 bytes on wire (496 bits), 62 bytes captured (496 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 0, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     49 6.645080149    192.168.1.81          192.168.1.78          TCP      60     [TCP Previous segment not captured] [TCP Port numbers reused] 1883 → 52432 [SYN, ACK] Seq=0 Ack=1 Win=64240 Len=0 MSS=1460

Frame 49: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 0, Ack: 1, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     51 7.647390100    192.168.1.78          192.168.1.81          MQTT     82     [TCP ACKed unseen segment] , Connect Command

Frame 51: 82 bytes on wire (656 bits), 82 bytes captured (656 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 1, Ack: 1, Len: 26
MQ Telemetry Transport Protocol, Connect Command

No.     Time           Source                Destination           Protocol Length Info
     52 7.647423584    192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=1 Ack=27 Win=64214 Len=0

Frame 52: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 1, Ack: 27, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     53 7.647625668    192.168.1.81          192.168.1.78          MQTT     60     Connect Ack

Frame 53: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 1, Ack: 27, Len: 4
MQ Telemetry Transport Protocol, Connect Ack

No.     Time           Source                Destination           Protocol Length Info
     56 8.651540586    192.168.1.78          192.168.1.81          TCP      62     52432 → 1883 [ACK] Seq=27 Ack=5 Win=2140 Len=0

Frame 56: 62 bytes on wire (496 bits), 62 bytes captured (496 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 27, Ack: 5, Len: 0

No.     Time           Source                Destination           Protocol Length Info
     78 9.654208499    192.168.1.78          192.168.1.81          MQTT     72     Publish Message [topic]

Frame 78: 72 bytes on wire (576 bits), 72 bytes captured (576 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 27, Ack: 5, Len: 16
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
     79 9.696126183    192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=5 Ack=43 Win=64198 Len=0

Frame 79: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 43, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    105 14.661197830   192.168.1.78          192.168.1.81          MQTT     72     Publish Message [topic]

Frame 105: 72 bytes on wire (576 bits), 72 bytes captured (576 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 43, Ack: 5, Len: 16
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
    106 14.661222980   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=5 Ack=59 Win=64182 Len=0

Frame 106: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 59, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    136 19.668216630   192.168.1.78          192.168.1.81          MQTT     72     Publish Message [topic]

Frame 136: 72 bytes on wire (576 bits), 72 bytes captured (576 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 59, Ack: 5, Len: 16
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
    137 19.668240191   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=5 Ack=75 Win=64166 Len=0

Frame 137: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 75, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    159 24.675227333   192.168.1.78          192.168.1.81          MQTT     72     Publish Message [topic]

Frame 159: 72 bytes on wire (576 bits), 72 bytes captured (576 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 75, Ack: 5, Len: 16
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
    160 24.675258358   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=5 Ack=91 Win=64150 Len=0

Frame 160: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 91, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    187 29.682215271   192.168.1.78          192.168.1.81          MQTT     72     Publish Message [topic]

Frame 187: 72 bytes on wire (576 bits), 72 bytes captured (576 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 91, Ack: 5, Len: 16
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
    188 29.682241205   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=5 Ack=107 Win=64134 Len=0

Frame 188: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 107, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    211 34.689206046   192.168.1.78          192.168.1.81          MQTT     72     Publish Message [topic]

Frame 211: 72 bytes on wire (576 bits), 72 bytes captured (576 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 107, Ack: 5, Len: 16
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
    212 34.689232301   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=5 Ack=123 Win=64118 Len=0

Frame 212: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 123, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    279 39.696225423   192.168.1.78          192.168.1.81          MQTT     72     Publish Message [topic]

Frame 279: 72 bytes on wire (576 bits), 72 bytes captured (576 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 123, Ack: 5, Len: 16
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
    280 39.696253345   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=5 Ack=139 Win=64102 Len=0

Frame 280: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 139, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    375 44.703228893   192.168.1.78          192.168.1.81          MQTT     72     Publish Message [topic]

Frame 375: 72 bytes on wire (576 bits), 72 bytes captured (576 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 139, Ack: 5, Len: 16
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
    376 44.703254356   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=5 Ack=155 Win=64086 Len=0

Frame 376: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 155, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    423 49.710230686   192.168.1.78          192.168.1.81          MQTT     72     Publish Message [topic]

Frame 423: 72 bytes on wire (576 bits), 72 bytes captured (576 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 155, Ack: 5, Len: 16
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
    424 49.710254304   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=5 Ack=171 Win=64070 Len=0

Frame 424: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 171, Len: 0
```


## simple publisher & subscriber

### mosquitto_sub

```json 
» mosquitto_sub -h 192.168.1.81 -p 1883 -t "stm32/control"

```



### mosquitto broker

```json 
1740849506: New connection from 192.168.1.78:52432 on port 1883.
1740849506: New mqttc_client connected from 192.168.1.78:52432 as stm32_client (p2, c0, k30).
1740849506: No will message specified.
1740849506: Sending CONNACK to stm32_client (1, 0)



1740849508: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (7 bytes))
1740849509: Received SUBSCRIBE from stm32_client
1740849509: 	stm32/control (QoS 0)
1740849509: stm32_client 0 stm32/control
1740849509: Sending SUBACK to stm32_client
1740849513: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (7 bytes))
1740849514: Received SUBSCRIBE from stm32_client
1740849514: 	stm32/control (QoS 0)
1740849514: stm32_client 0 stm32/control
1740849514: Sending SUBACK to stm32_client
1740849518: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (7 bytes))
1740849519: Received SUBSCRIBE from stm32_client
1740849519: 	stm32/control (QoS 0)
1740849519: stm32_client 0 stm32/control
1740849519: Sending SUBACK to stm32_client
1740849523: Received PUBLISH from stm32_client (d0, q0, r0, m0, 'stm32/data', ... (7 bytes))
1740849524: Received SUBSCRIBE from stm32_client
1740849524: 	stm32/control (QoS 0)
1740849524: stm32_client 0 stm32/control
1740849524: Sending SUBACK to stm32_client
1740849525: Client stm32_client closed its connection.
```


### stm32 mqttc_client

```json
Waiting for network interface...
Waiting for link...

TCP connection initiated
MQTT mqttc_client initialized
MQTT CONNECT sent
TCP Connected
Sent 26 bytes
Received 4 bytes:  
Published message
Sent 21 bytes
Sent 20 bytes
Received 5 bytes: ��(
Published message
Sent 21 bytes
Sent 20 bytes
Received 5 bytes: �;�
Published message
Sent 21 bytes
Sent 20 bytes
Received 5 bytes: ���
Published message
Sent 21 bytes
Sent 20 bytes
Received 5 bytes: ���
Connection closed
```

### wireshark captures

```json 
» cat Downloads/stm32-mqtt-publish-subscriber.txt 
No.     Time           Source                Destination           Protocol Length Info
    162 22.348101533   192.168.1.78          192.168.1.81          TCP      62     [TCP Port numbers reused] 52432 → 1883 [SYN] Seq=0 Win=2144 Len=0 MSS=536

Frame 162: 62 bytes on wire (496 bits), 62 bytes captured (496 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 0, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    163 22.348118998   192.168.1.81          192.168.1.78          TCP      56     [TCP ACKed unseen segment] 1883 → 52432 [ACK] Seq=3505450335 Ack=396 Win=64070 Len=0

Frame 163: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 3505450335, Ack: 396, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    177 23.350364700   192.168.1.78          192.168.1.81          TCP      62     52432 → 1883 [RST, ACK] Seq=396 Ack=3505450335 Win=24584 Len=0

Frame 177: 62 bytes on wire (496 bits), 62 bytes captured (496 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 396, Ack: 3505450335, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    178 23.350406437   192.168.1.78          192.168.1.81          TCP      62     [TCP Retransmission] 52432 → 1883 [SYN] Seq=0 Win=2144 Len=0 MSS=536

Frame 178: 62 bytes on wire (496 bits), 62 bytes captured (496 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 0, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    179 23.350434025   192.168.1.81          192.168.1.78          TCP      60     [TCP Previous segment not captured] [TCP Port numbers reused] 1883 → 52432 [SYN, ACK] Seq=0 Ack=1 Win=64240 Len=0 MSS=1460

Frame 179: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 0, Ack: 1, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    182 24.352681283   192.168.1.78          192.168.1.81          MQTT     82     [TCP ACKed unseen segment] , Connect Command

Frame 182: 82 bytes on wire (656 bits), 82 bytes captured (656 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 1, Ack: 1, Len: 26
MQ Telemetry Transport Protocol, Connect Command

No.     Time           Source                Destination           Protocol Length Info
    183 24.352729918   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=1 Ack=27 Win=64214 Len=0

Frame 183: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 1, Ack: 27, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    184 24.352930525   192.168.1.81          192.168.1.78          MQTT     60     Connect Ack

Frame 184: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 1, Ack: 27, Len: 4
MQ Telemetry Transport Protocol, Connect Ack

No.     Time           Source                Destination           Protocol Length Info
    188 25.356794119   192.168.1.78          192.168.1.81          TCP      62     52432 → 1883 [ACK] Seq=27 Ack=5 Win=2140 Len=0

Frame 188: 62 bytes on wire (496 bits), 62 bytes captured (496 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 27, Ack: 5, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    190 26.359439105   192.168.1.78          192.168.1.81          MQTT     77     Publish Message [stm32/data]

Frame 190: 77 bytes on wire (616 bits), 77 bytes captured (616 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 27, Ack: 5, Len: 21
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
    191 26.400061663   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=5 Ack=48 Win=64193 Len=0

Frame 191: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 48, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    210 27.361587996   192.168.1.78          192.168.1.81          MQTT     76     Subscribe Request (id=60968) [stm32/control]

Frame 210: 76 bytes on wire (608 bits), 76 bytes captured (608 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 48, Ack: 5, Len: 20
MQ Telemetry Transport Protocol, Subscribe Request

No.     Time           Source                Destination           Protocol Length Info
    211 27.361621078   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=5 Ack=68 Win=64173 Len=0

Frame 211: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 68, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    212 27.361699613   192.168.1.81          192.168.1.78          MQTT     61     Subscribe Ack (id=60968)

Frame 212: 61 bytes on wire (488 bits), 61 bytes captured (488 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 5, Ack: 68, Len: 5
MQ Telemetry Transport Protocol, Subscribe Ack

No.     Time           Source                Destination           Protocol Length Info
    213 28.365786752   192.168.1.78          192.168.1.81          TCP      62     52432 → 1883 [ACK] Seq=68 Ack=10 Win=2135 Len=0

Frame 213: 62 bytes on wire (496 bits), 62 bytes captured (496 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 68, Ack: 10, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    218 31.369279481   192.168.1.78          192.168.1.81          MQTT     77     Publish Message [stm32/data]

Frame 218: 77 bytes on wire (616 bits), 77 bytes captured (616 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 68, Ack: 10, Len: 21
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
    219 31.410066566   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=10 Ack=89 Win=64152 Len=0

Frame 219: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 10, Ack: 89, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    237 32.371413909   192.168.1.78          192.168.1.81          MQTT     76     Subscribe Request (id=15242) [stm32/control]

Frame 237: 76 bytes on wire (608 bits), 76 bytes captured (608 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 89, Ack: 10, Len: 20
MQ Telemetry Transport Protocol, Subscribe Request

No.     Time           Source                Destination           Protocol Length Info
    238 32.371443606   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=10 Ack=109 Win=64132 Len=0

Frame 238: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 10, Ack: 109, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    239 32.371521310   192.168.1.81          192.168.1.78          MQTT     61     Subscribe Ack (id=15242)

Frame 239: 61 bytes on wire (488 bits), 61 bytes captured (488 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 10, Ack: 109, Len: 5
MQ Telemetry Transport Protocol, Subscribe Ack

No.     Time           Source                Destination           Protocol Length Info
    243 33.375629851   192.168.1.78          192.168.1.81          TCP      62     52432 → 1883 [ACK] Seq=109 Ack=15 Win=2130 Len=0

Frame 243: 62 bytes on wire (496 bits), 62 bytes captured (496 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 109, Ack: 15, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    252 36.379121535   192.168.1.78          192.168.1.81          MQTT     77     Publish Message [stm32/data]

Frame 252: 77 bytes on wire (616 bits), 77 bytes captured (616 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 109, Ack: 15, Len: 21
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
    253 36.420057394   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=15 Ack=130 Win=64111 Len=0

Frame 253: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 15, Ack: 130, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    270 37.381271913   192.168.1.78          192.168.1.81          MQTT     76     Subscribe Request (id=47842) [stm32/control]

Frame 270: 76 bytes on wire (608 bits), 76 bytes captured (608 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 130, Ack: 15, Len: 20
MQ Telemetry Transport Protocol, Subscribe Request

No.     Time           Source                Destination           Protocol Length Info
    271 37.381297145   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=15 Ack=150 Win=64091 Len=0

Frame 271: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 15, Ack: 150, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    272 37.381363872   192.168.1.81          192.168.1.78          MQTT     61     Subscribe Ack (id=47842)

Frame 272: 61 bytes on wire (488 bits), 61 bytes captured (488 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 15, Ack: 150, Len: 5
MQ Telemetry Transport Protocol, Subscribe Ack

No.     Time           Source                Destination           Protocol Length Info
    273 38.385459142   192.168.1.78          192.168.1.81          TCP      62     52432 → 1883 [ACK] Seq=150 Ack=20 Win=2125 Len=0

Frame 273: 62 bytes on wire (496 bits), 62 bytes captured (496 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 150, Ack: 20, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    276 41.388963968   192.168.1.78          192.168.1.81          MQTT     77     Publish Message [stm32/data]

Frame 276: 77 bytes on wire (616 bits), 77 bytes captured (616 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 150, Ack: 20, Len: 21
MQ Telemetry Transport Protocol, Publish Message

No.     Time           Source                Destination           Protocol Length Info
    277 41.429101837   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=20 Ack=171 Win=64070 Len=0

Frame 277: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 20, Ack: 171, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    305 42.391097273   192.168.1.78          192.168.1.81          MQTT     76     Subscribe Request (id=39608) [stm32/control]

Frame 305: 76 bytes on wire (608 bits), 76 bytes captured (608 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 171, Ack: 20, Len: 20
MQ Telemetry Transport Protocol, Subscribe Request

No.     Time           Source                Destination           Protocol Length Info
    306 42.391122333   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [ACK] Seq=20 Ack=191 Win=64070 Len=0

Frame 306: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 20, Ack: 191, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    307 42.391215423   192.168.1.81          192.168.1.78          MQTT     61     Subscribe Ack (id=39608)

Frame 307: 61 bytes on wire (488 bits), 61 bytes captured (488 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 20, Ack: 191, Len: 5
MQ Telemetry Transport Protocol, Subscribe Ack

No.     Time           Source                Destination           Protocol Length Info
    310 42.691759068   192.168.1.81          192.168.1.78          TCP      56     1883 → 52432 [FIN, ACK] Seq=25 Ack=191 Win=64070 Len=0

Frame 310: 56 bytes on wire (448 bits), 56 bytes captured (448 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.81, Dst: 192.168.1.78
Transmission Control Protocol, Src Port: 1883, Dst Port: 52432, Seq: 25, Ack: 191, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    313 43.397029166   192.168.1.78          192.168.1.81          TCP      62     52432 → 1883 [RST, ACK] Seq=191 Ack=26 Win=24584 Len=0

Frame 313: 62 bytes on wire (496 bits), 62 bytes captured (496 bits) on interface any, id 0
Linux cooked capture v1
Internet Protocol Version 4, Src: 192.168.1.78, Dst: 192.168.1.81
Transmission Control Protocol, Src Port: 52432, Dst Port: 1883, Seq: 191, Ack: 26, Len: 0
```
