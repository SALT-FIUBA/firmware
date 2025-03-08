# firmware

## mqttc protocol

1. mqttc_init <-> connect
2. mqttc_sync (wo keep_alive) <-> connack
3. mqttc_publish <-> publish
4. mqttc_sync <-> publish ack
5. mqttc_subscribe <-> subscribe
6. mqttc_sync <-> subscribe ack

