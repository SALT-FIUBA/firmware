# firmware

```json

» telnet 192.168.1.78 7
# Type messages to see them echoed back
Trying 192.168.1.78...
Connected to 192.168.1.78.
Escape character is '^]'.
1234567890
1234567890
12345678901
1234567890               

```

# lwip + mqttc connection

# nucleo-144 board output

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

Connecting to MQTT broker at 192.168.1.81:1883
MQTT TCP connect failed

=== Network Status ===
Interface: UP, Link: UP
IP: 192.168.1.78
MAC: 00:80:E1:00:00:00
===================

Starting MQTT connection attempt...
Connecting to MQTT broker at 192.168.1.81:1883
TCP connection initiated, waiting for callback...
TCP connect callback with status: 0
TCP connected successfully

=== Network Status ===
Interface: UP, Link: UP
IP: 192.168.1.78
MAC: 00:80:E1:00:00:00
===================

Starting MQTT connection attempt...
Connecting to MQTT broker at 192.168.1.81:1883
TCP connection initiated, waiting for callback...
TCP connect callback with status: 0
TCP connected successfully

=== Network Status ===
Interface: UP, Link: UP
IP: 192.168.1.78
MAC: 00:80:E1:00:00:00
===================

Starting MQTT connection attempt...
Connecting to MQTT broker at 192.168.1.81:1883
TCP connection initiated, waiting for callback...
TCP connect callback with status: 0
TCP connected successfully

=== Network Status ===
Interface: UP, Link: UP
IP: 192.168.1.78
MAC: 00:80:E1:00:00:00
===================

Starting MQTT connection attempt...
Connecting to MQTT broker at 192.168.1.81:1883
TCP connection initiated, waiting for callback...
TCP connect callback with status: 0
TCP connected successfully

=== Network Status ===
Interface: UP, Link: UP
IP: 192.168.1.78
MAC: 00:80:E1:00:00:00
===================

Starting MQTT connection attempt...
Connecting to MQTT broker at 192.168.1.81:1883
TCP connection initiated, waiting for callback...
TCP connect callback with status: 0
TCP connected successfully

=== Network Status ===
Interface: UP, Link: UP
IP: 192.168.1.78
MAC: 00:80:E1:00:00:00
===================

Starting MQTT connection attempt...
Failed to create TCP PCB

=== Network Status ===
Interface: UP, Link: UP
IP: 192.168.1.78
MAC: 00:80:E1:00:00:00
===================
```


## mosquitto broker 

```json

 » mosquitto -c mosquitto.conf -v
1737942500: mosquitto version 2.0.11 starting
1737942500: Config loaded from mosquitto.conf.
1737942500: Opening ipv4 listen socket on port 1883.
1737942500: mosquitto version 2.0.11 running

1737943875: New connection from 192.168.1.78:52432 on port 1883.
1737943879: New connection from 192.168.1.78:52433 on port 1883.
1737943885: New connection from 192.168.1.78:52434 on port 1883.
1737943890: New connection from 192.168.1.78:52435 on port 1883.
1737943896: New connection from 192.168.1.78:52436 on port 1883.
```
