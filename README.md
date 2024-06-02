# firmware

### Modelo de datos a publicar/suscribir datos

Estructura de datos propuesta en base a los siguientes archivos:

+ [logic.h](https://github.com/nahueespinosa/salt-firmware/blob/master/salt/logic/logic.h#L128)

+ [publisher.c](https://github.com/nahueespinosa/salt-firmware/blob/master/salt/publisher/publisher.c)

+ [saltCmd.h](https://github.com/nahueespinosa/salt-firmware/blob/6ace7d5886d3d5555b6e407cb1322ef2c9facf9d/salt/parser/saltCmd/saltCmd.h)

```json

{
  "SALT_CMD_TYPE": [
    "SALT_CMD_TYPE_CMD",
    "SALT_CMD_TYPE_CONFIG",
    "SALT_CMD_TYPE_COUNT",
    "SALT_CMD_TYPE_NULL"
  ],
  "SALT_CMD_ORDER": [
    "SALT_CMD_ORDER_STOP",
    "SALT_CMD_ORDER_DRIFT",
    "SALT_CMD_ORDER_ISOLATED",
    "SALT_CMD_ORDER_AUTOMATIC",
    "SALT_CMD_ORDER_COUNT",
    "SALT_CMD_ORDER_NULL"
  ],
  "SALT_PARAMETER": [
    "SALT_PARAMETER_CMD_TIMEOUT",
    "SALT_PARAMETER_VEL_CT_ON",
    "SALT_PARAMETER_VEL_CT_OFF",
    "SALT_PARAMETER_VEL_FE_ON",
    "SALT_PARAMETER_TIME_FE_HOLD",
    "SALT_PARAMETER_TIME_BLINK_ENABLE",
    "SALT_PARAMETER_TIME_BLINK_DISABLE",
    "SALT_PARAMETER_PERIOD_BLINK",
    "SALT_PARAMETER_PUBLISH_PERIOD",
    "SALT_PARAMETER_COUNT",
    "SALT_PARAMETER_NULL"
  ],
  "SaltCmd": {
    "type": "SALT_CMD_TYPE",
    "cmd": "SALT_CMD_ORDER",
    "parameter": "SALT_PARAMETER",
    "parameterValueString": "string",
    "parameterValueStringSize": "number",
    "parameterValueDouble": "number",
    "parameterValueBool": "boolean"
  },
  "LogicCfg": {
    "publishTime": "number"
  },
  "VEL_SOURCE": [
    "VEL_SOURCE_TELOC",
    "VEL_SOURCE_EXTERNAL",
    "VEL_SOURCE_GPS",
    "VEL_SOURCE_COUNT",
    "VEL_SOURCE_NULL"
  ],
  "VelEvt": {
    "evt": "RKH_EVT_T",
    "vel": "number",
    "source": "VEL_SOURCE"
  },
  "CmdEvt": {
    "evt": "RKH_EVT_T",
    "cmd": {
      "type": "SALT_CMD_TYPE",
      "cmd": "SALT_CMD_ORDER",
      "parameter": "SALT_PARAMETER",
      "parameterValueString": "string",
      "parameterValueStringSize": "number",
      "parameterValueDouble": "number",
      "parameterValueBool": "boolean"
    }
  },
  "FailSubsystemData": {
    "port": "number",
    "name": "string",
    "serial_number": "string",
    "ct_on": "boolean",
    "fe_on": "boolean"
  },
  "SpeedData": {
    "value": "number",
    "source": "VEL_SOURCE"
  },
  "SaltData": {
    "alMode": "boolean",
    "currentCmd": "SALT_CMD_ORDER",
    "failSubsystem": [
      {
        "port": "number",
        "name": "string",
        "serial_number": "string",
        "ct_on": "boolean",
        "fe_on": "boolean"
      }
    ]
  },
  "Configuration": {
    "velCtOn": "number",
    "velCtOff": "number",
    "velFeOn": "number",
    "timeFeHold": "number",
    "cmdTimeout": "number",
    "timeBlinkEnable": "number",
    "timeBlinkDisable": "number",
    "blinkPeriod": "number",
    "publishPeriod": "number"
  },
  "LogicData": {
    "saltData": {
      "alMode": "boolean",
      "currentCmd": "SALT_CMD_ORDER",
      "failSubsystem": [
        {
          "port": "number",
          "name": "string",
          "serial_number": "string",
          "ct_on": "boolean",
          "fe_on": "boolean"
        }
      ]
    },
    "speedData": {
      "value": "number",
      "source": "VEL_SOURCE"
    },
    "configuration": {
      "velCtOn": "number",
      "velCtOff": "number",
      "velFeOn": "number",
      "timeFeHold": "number",
      "cmdTimeout": "number",
      "timeBlinkEnable": "number",
      "timeBlinkDisable": "number",
      "blinkPeriod": "number",
      "publishPeriod": "number"
    }
  }
}

```


### paginas a desarrollar

- [ ] MQTT Client integration from Device service
- [ ] POST /device/status<br>data class StatusRequest
- [ ] POST /device/config<br>data class ConfigRequest

- [ ] Device list page
- [ ] Device detail page
- [ ] Device dashboard page

- [ ] Organism list page
- [ ] Organism detail page

- [ ] Users list page
- [ ] User detail page

- [ ] Trains list page
- [ ] Train detail page

- [ ] Settings page

- [ ] MQTT actions & settings
- [ ] Error handler

- [x] Register side page
- [x] Login side page


