# firmware

## netcat 


```json
» nc -l 192.168.1.81 1883

hey there
hey there
```


## stm32 huart port

```json
» ./STM32_Programmer_CLI -c port=ttyACM1 br=115200 console
      -------------------------------------------------------------------
                        STM32CubeProgrammer v2.17.0                  
      -------------------------------------------------------------------

Serial Port ttyACM1 is successfully opened.
 Port configuration: parity = even, baudrate = 115200, data-bit = 8,
                     stop-bit = 1,0, flow-control = off

Entering console mode :
Press F to send a file
Press E to exit
Press W to write mode

Waiting for network interface...
Waiting for link...
Link up - IP: 192.168.1.78
init
open
tcp_connect_attempt
connecting_entry
tcp_connect_attempt
tcp_connect_callback
TCP Connected
tcp_recv_callback
Received 10 bytes
tcp_recv_callback
Received 10 bytes
tcp_recv_callback
Connection closed
```