# firmware

## stm32 wolfssl-tcp-conmgr + mqtt-prot + logic state machines working - SALT DISABLED

### stm32 huart port

```json
» ./STM32_Programmer_CLI -c port=ttyACM0 br=115200 console
      -------------------------------------------------------------------
                        STM32CubeProgrammer v2.17.0                  
      -------------------------------------------------------------------

Serial Port ttyACM0 is successfully opened.
 Port configuration: parity = even, baudrate = 115200, data-bit = 8,
                     stop-bit = 1,0, flow-control = off

---------------------------------------------------------
Waiting for network interface...
Waiting for link...
Link up - IP: 192.168.1.80
init
socketOpen
DNS resolution in progress...
dns_callback
tcp_conmgr_connect_attempt
tcp_connected_callback
TCP connected to 46.137.47.218:8883
startHandshake
enHandshaking
WolfSSL session created, starting handshake with 007f5e0286aa4c36ba410312d36d42f0.s1.eu.hivemq.cloud
lwip_send
Initial wolfSSL_connect ret: -1, err: 2
Initial handshake needs READ
Received 1412 bytes from broker
Received 1412 bytes from broker
Received 235 bytes from broker
processHandshake

lwip_recv: copied 5 bytes, requested 5, pbuf tot_len 3059, offset 0
lwip_recv: copied 90 bytes, requested 90, pbuf tot_len 3059, offset 5
lwip_recv: copied 5 bytes, requested 5, pbuf tot_len 3059, offset 95
lwip_recv: copied 2612 bytes, requested 2612, pbuf tot_len 3059, offset 100
lwip_recv: copied 5 bytes, requested 5, pbuf tot_len 3059, offset 2712
lwip_recv: copied 333 bytes, requested 333, pbuf tot_len 3059, offset 2717
lwip_recv: copied 5 bytes, requested 5, pbuf tot_len 3059, offset 3050
lwip_recv: copied 4 bytes, requested 4, pbuf tot_len 3059, offset 3055
lwip_recv: pbuf fully consumed
lwip_send
lwip_send
lwip_send
wolfSSL_connect ret: -1, err: 2
SSL handshake in progress: WANT_READ
processHandshake
wolfSSL_connect ret: -1, err: 2
SSL handshake in progress: WANT_READ
processHandshake
wolfSSL_connect ret: -1, err: 2
SSL handshake in progress: WANT_READ
Received 51 bytes from broker
processHandshake
lwip_recv: copied 5 bytes, requested 5, pbuf tot_len 51, offset 0
lwip_recv: copied 1 bytes, requested 1, pbuf tot_len 51, offset 5
lwip_recv: copied 5 bytes, requested 5, pbuf tot_len 51, offset 6
lwip_recv: copied 40 bytes, requested 40, pbuf tot_len 51, offset 11
lwip_recv: pbuf fully consumed
SSL handshake successful
socketConnected
rv < 0
lwip_send
lwip_send
Received 33 bytes from broker
Received 34 bytes from broker

 tcp-mqttprot | publish
lwip_recv: copied 5 bytes, requested 5, pbuf tot_len 67, offset 0
lwip_recv: copied 28 bytes, requested 28, pbuf tot_len 67, offset 5
lwip_recv: copied 5 bytes, requested 5, pbuf tot_len 67, offset 33
lwip_recv: copied 29 bytes, requested 29, pbuf tot_len 67, offset 38
lwip_recv: pbuf fully consumed
logic | logic_getData
logic | logic_getPublishPeriod
Published successful

 tcp-mqttprot | publish
lwip_send
logic | logic_getData
logic | logic_getPublishPeriod
Published successful
Received 33 bytes from broker

 tcp-mqttprot | publish
lwip_recv: copied 5 bytes, requested 5, pbuf tot_len 33, offset 0
lwip_recv: copied 28 bytes, requested 28, pbuf tot_len 33, offset 5
lwip_recv: pbuf fully consumed
lwip_send
logic | logic_getData
logic | logic_getPublishPeriod
Published successful
Received 33 bytes from broker

 tcp-mqttprot | publish
lwip_recv: copied 5 bytes, requested 5, pbuf tot_len 33, offset 0
lwip_recv: copied 28 bytes, requested 28, pbuf tot_len 33, offset 5
lwip_recv: pbuf fully consumed
lwip_send
logic | logic_getData
logic | logic_getPublishPeriod
Published successful
Received 33 bytes from broker
```

### wireshark captures


```txt 
No.     Time           Source                Destination           Protocol Length Info
    198 17.052581      192.168.1.80          8.8.8.8               DNS      111    Standard query 0x3129 A 29763578558a437bb804d48d7e8b4e01.s1.eu.hivemq.cloud

Frame 198: 111 bytes on wire (888 bits), 111 bytes captured (888 bits) on interface en5, id 0
Ethernet II, Src: STMicroelect_00:00:00 (00:80:e1:00:00:00), Dst: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c)
Internet Protocol Version 4, Src: 192.168.1.80, Dst: 8.8.8.8
User Datagram Protocol, Src Port: 63046, Dst Port: 53
Domain Name System (query)

No.     Time           Source                Destination           Protocol Length Info
    205 17.222540      8.8.8.8               192.168.1.80          DNS      159    Standard query response 0x3129 A 29763578558a437bb804d48d7e8b4e01.s1.eu.hivemq.cloud A 52.31.149.80 A 46.137.47.218 A 54.73.92.158

Frame 205: 159 bytes on wire (1272 bits), 159 bytes captured (1272 bits) on interface en5, id 0
Ethernet II, Src: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c), Dst: STMicroelect_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 8.8.8.8, Dst: 192.168.1.80
User Datagram Protocol, Src Port: 53, Dst Port: 63046
Domain Name System (response)

No.     Time           Source                Destination           Protocol Length Info
    206 17.226756      192.168.1.80          52.31.149.80          TCP      60     52432 → 8883 [SYN] Seq=0 Win=11680 Len=0 MSS=1460

Frame 206: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface en5, id 0
Ethernet II, Src: STMicroelect_00:00:00 (00:80:e1:00:00:00), Dst: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c)
Internet Protocol Version 4, Src: 192.168.1.80, Dst: 52.31.149.80
Transmission Control Protocol, Src Port: 52432, Dst Port: 8883, Seq: 0, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    212 17.466995      52.31.149.80          192.168.1.80          TCP      54     [TCP ACKed unseen segment] 8883 → 52432 [ACK] Seq=1 Ack=25070 Win=56385 Len=0

Frame 212: 54 bytes on wire (432 bits), 54 bytes captured (432 bits) on interface en5, id 0
Ethernet II, Src: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c), Dst: STMicroelect_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 52.31.149.80, Dst: 192.168.1.80
Transmission Control Protocol, Src Port: 8883, Dst Port: 52432, Seq: 1, Ack: 25070, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    213 17.467270      192.168.1.80          52.31.149.80          TCP      60     52432 → 8883 [RST, ACK] Seq=25070 Ack=1 Win=41005 Len=0

Frame 213: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface en5, id 0
Ethernet II, Src: STMicroelect_00:00:00 (00:80:e1:00:00:00), Dst: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c)
Internet Protocol Version 4, Src: 192.168.1.80, Dst: 52.31.149.80
Transmission Control Protocol, Src Port: 52432, Dst Port: 8883, Seq: 25070, Ack: 1, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    214 17.467271      192.168.1.80          52.31.149.80          TCP      60     [TCP Port numbers reused] 52432 → 8883 [SYN] Seq=0 Win=11680 Len=0 MSS=1460

Frame 214: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface en5, id 0
Ethernet II, Src: STMicroelect_00:00:00 (00:80:e1:00:00:00), Dst: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c)
Internet Protocol Version 4, Src: 192.168.1.80, Dst: 52.31.149.80
Transmission Control Protocol, Src Port: 52432, Dst Port: 8883, Seq: 0, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    220 17.694102      52.31.149.80          192.168.1.80          TCP      58     8883 → 52432 [SYN, ACK] Seq=0 Ack=1 Win=62727 Len=0 MSS=1412

Frame 220: 58 bytes on wire (464 bits), 58 bytes captured (464 bits) on interface en5, id 0
Ethernet II, Src: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c), Dst: STMicroelect_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 52.31.149.80, Dst: 192.168.1.80
Transmission Control Protocol, Src Port: 8883, Dst Port: 52432, Seq: 0, Ack: 1, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    221 17.700104      192.168.1.80          52.31.149.80          TCP      60     52432 → 8883 [ACK] Seq=1 Ack=1 Win=11680 Len=0

Frame 221: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface en5, id 0
Ethernet II, Src: STMicroelect_00:00:00 (00:80:e1:00:00:00), Dst: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c)
Internet Protocol Version 4, Src: 192.168.1.80, Dst: 52.31.149.80
Transmission Control Protocol, Src Port: 52432, Dst Port: 8883, Seq: 1, Ack: 1, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    227 17.841721      192.168.1.80          52.31.149.80          TLSv1.2  304    Client Hello (SNI=29763578558a437bb804d48d7e8b4e01.s1.eu.hivemq.cloud)

Frame 227: 304 bytes on wire (2432 bits), 304 bytes captured (2432 bits) on interface en5, id 0
Ethernet II, Src: STMicroelect_00:00:00 (00:80:e1:00:00:00), Dst: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c)
Internet Protocol Version 4, Src: 192.168.1.80, Dst: 52.31.149.80
Transmission Control Protocol, Src Port: 52432, Dst Port: 8883, Seq: 1, Ack: 1, Len: 250
Transport Layer Security

No.     Time           Source                Destination           Protocol Length Info
    238 18.076153      52.31.149.80          192.168.1.80          TCP      54     8883 → 52432 [ACK] Seq=1 Ack=251 Win=62477 Len=0

Frame 238: 54 bytes on wire (432 bits), 54 bytes captured (432 bits) on interface en5, id 0
Ethernet II, Src: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c), Dst: STMicroelect_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 52.31.149.80, Dst: 192.168.1.80
Transmission Control Protocol, Src Port: 8883, Dst Port: 52432, Seq: 1, Ack: 251, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    239 18.076171      52.31.149.80          192.168.1.80          TLSv1.2  1466   Server Hello

Frame 239: 1466 bytes on wire (11728 bits), 1466 bytes captured (11728 bits) on interface en5, id 0
Ethernet II, Src: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c), Dst: STMicroelect_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 52.31.149.80, Dst: 192.168.1.80
Transmission Control Protocol, Src Port: 8883, Dst Port: 52432, Seq: 1, Ack: 251, Len: 1412
Transport Layer Security

No.     Time           Source                Destination           Protocol Length Info
    240 18.076175      52.31.149.80          192.168.1.80          TLSv1.2  1466   Certificate

Frame 240: 1466 bytes on wire (11728 bits), 1466 bytes captured (11728 bits) on interface en5, id 0
Ethernet II, Src: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c), Dst: STMicroelect_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 52.31.149.80, Dst: 192.168.1.80
Transmission Control Protocol, Src Port: 8883, Dst Port: 52432, Seq: 1413, Ack: 251, Len: 1412
[2 Reassembled TCP Segments (2617 bytes): #239(1317), #240(1300)]
Transport Layer Security

No.     Time           Source                Destination           Protocol Length Info
    241 18.076179      52.31.149.80          192.168.1.80          TLSv1.2  289    Server Key Exchange, Server Hello Done

Frame 241: 289 bytes on wire (2312 bits), 289 bytes captured (2312 bits) on interface en5, id 0
Ethernet II, Src: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c), Dst: STMicroelect_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 52.31.149.80, Dst: 192.168.1.80
Transmission Control Protocol, Src Port: 8883, Dst Port: 52432, Seq: 2825, Ack: 251, Len: 235
[2 Reassembled TCP Segments (338 bytes): #240(112), #241(226)]
Transport Layer Security
Transport Layer Security

No.     Time           Source                Destination           Protocol Length Info
    242 18.082828      192.168.1.80          52.31.149.80          TCP      60     52432 → 8883 [ACK] Seq=251 Ack=2825 Win=8856 Len=0

Frame 242: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface en5, id 0
Ethernet II, Src: STMicroelect_00:00:00 (00:80:e1:00:00:00), Dst: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c)
Internet Protocol Version 4, Src: 192.168.1.80, Dst: 52.31.149.80
Transmission Control Protocol, Src Port: 52432, Dst Port: 8883, Seq: 251, Ack: 2825, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    243 18.114835      192.168.1.80          52.31.149.80          TCP      60     52432 → 8883 [ACK] Seq=251 Ack=3060 Win=11333 Len=0

Frame 243: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface en5, id 0
Ethernet II, Src: STMicroelect_00:00:00 (00:80:e1:00:00:00), Dst: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c)
Internet Protocol Version 4, Src: 192.168.1.80, Dst: 52.31.149.80
Transmission Control Protocol, Src Port: 52432, Dst Port: 8883, Seq: 251, Ack: 3060, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    251 18.426349      192.168.1.80          52.31.149.80          TLSv1.2  129    Client Key Exchange

Frame 251: 129 bytes on wire (1032 bits), 129 bytes captured (1032 bits) on interface en5, id 0
Ethernet II, Src: STMicroelect_00:00:00 (00:80:e1:00:00:00), Dst: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c)
Internet Protocol Version 4, Src: 192.168.1.80, Dst: 52.31.149.80
Transmission Control Protocol, Src Port: 52432, Dst Port: 8883, Seq: 251, Ack: 3060, Len: 75
Transport Layer Security

No.     Time           Source                Destination           Protocol Length Info
    258 18.710796      52.31.149.80          192.168.1.80          TCP      60     8883 → 52432 [ACK] Seq=3060 Ack=326 Win=62402 Len=0

Frame 258: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface en5, id 0
Ethernet II, Src: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c), Dst: STMicroelect_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 52.31.149.80, Dst: 192.168.1.80
Transmission Control Protocol, Src Port: 8883, Dst Port: 52432, Seq: 3060, Ack: 326, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    259 18.711137      192.168.1.80          52.31.149.80          TLSv1.2  105    Change Cipher Spec, Encrypted Handshake Message

Frame 259: 105 bytes on wire (840 bits), 105 bytes captured (840 bits) on interface en5, id 0
Ethernet II, Src: STMicroelect_00:00:00 (00:80:e1:00:00:00), Dst: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c)
Internet Protocol Version 4, Src: 192.168.1.80, Dst: 52.31.149.80
Transmission Control Protocol, Src Port: 52432, Dst Port: 8883, Seq: 326, Ack: 3060, Len: 51
Transport Layer Security

No.     Time           Source                Destination           Protocol Length Info
    265 18.937009      52.31.149.80          192.168.1.80          TCP      60     8883 → 52432 [ACK] Seq=3060 Ack=377 Win=62351 Len=0

Frame 265: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface en5, id 0
Ethernet II, Src: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c), Dst: STMicroelect_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 52.31.149.80, Dst: 192.168.1.80
Transmission Control Protocol, Src Port: 8883, Dst Port: 52432, Seq: 3060, Ack: 377, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    266 18.937030      52.31.149.80          192.168.1.80          TLSv1.2  105    Change Cipher Spec, Encrypted Handshake Message

Frame 266: 105 bytes on wire (840 bits), 105 bytes captured (840 bits) on interface en5, id 0
Ethernet II, Src: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c), Dst: STMicroelect_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 52.31.149.80, Dst: 192.168.1.80
Transmission Control Protocol, Src Port: 8883, Dst Port: 52432, Seq: 3060, Ack: 377, Len: 51
Transport Layer Security

No.     Time           Source                Destination           Protocol Length Info
    268 18.979481      192.168.1.80          52.31.149.80          TLSv1.2  138    Application Data

Frame 268: 138 bytes on wire (1104 bits), 138 bytes captured (1104 bits) on interface en5, id 0
Ethernet II, Src: STMicroelect_00:00:00 (00:80:e1:00:00:00), Dst: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c)
Internet Protocol Version 4, Src: 192.168.1.80, Dst: 52.31.149.80
Transmission Control Protocol, Src Port: 52432, Dst Port: 8883, Seq: 377, Ack: 3111, Len: 84
Transport Layer Security

No.     Time           Source                Destination           Protocol Length Info
    274 19.278159      52.31.149.80          192.168.1.80          TCP      60     8883 → 52432 [ACK] Seq=3111 Ack=461 Win=62267 Len=0

Frame 274: 60 bytes on wire (480 bits), 60 bytes captured (480 bits) on interface en5, id 0
Ethernet II, Src: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c), Dst: STMicroelect_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 52.31.149.80, Dst: 192.168.1.80
Transmission Control Protocol, Src Port: 8883, Dst Port: 52432, Seq: 3111, Ack: 461, Len: 0

No.     Time           Source                Destination           Protocol Length Info
    275 19.278485      192.168.1.80          52.31.149.80          TLSv1.2  103    Application Data

Frame 275: 103 bytes on wire (824 bits), 103 bytes captured (824 bits) on interface en5, id 0
Ethernet II, Src: STMicroelect_00:00:00 (00:80:e1:00:00:00), Dst: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c)
Internet Protocol Version 4, Src: 192.168.1.80, Dst: 52.31.149.80
Transmission Control Protocol, Src Port: 52432, Dst Port: 8883, Seq: 461, Ack: 3111, Len: 49
Transport Layer Security

No.     Time           Source                Destination           Protocol Length Info
    276 19.286116      52.31.149.80          192.168.1.80          TLSv1.2  87     Application Data

Frame 276: 87 bytes on wire (696 bits), 87 bytes captured (696 bits) on interface en5, id 0
Ethernet II, Src: HuaweiTechno_c3:4a:3c (9c:b2:e8:c3:4a:3c), Dst: STMicroelect_00:00:00 (00:80:e1:00:00:00)
Internet Protocol Version 4, Src: 52.31.149.80, Dst: 192.168.1.80
Transmission Control Protocol, Src Port: 8883, Dst Port: 52432, Seq: 3111, Ack: 461, Len: 33
Transport Layer Security

No.     Time           Source                Destination           Protocol Length Info
    278 19.358045      192.168.1.80          52.31.149.80          TCP      60     52432 → 8883 [ACK] Seq=510 Ack=3144 Win=11249 Len=0
```

## stm32 wolfssl-tcp-conmgr + mqtt-prot + logic state machines working - SALT ENABLED

## stm32 huart port
```json


```


