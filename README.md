# firmware

### Modelo de datos a publicar/suscribir datos

Estructura de datos propuesta en base a los siguientes archivos:

+ [logic.h](https://github.com/nahueespinosa/salt-firmware/blob/master/salt/logic/logic.h#L128)

+ [publisher.c](https://github.com/nahueespinosa/salt-firmware/blob/master/salt/publisher/publisher.c)


```json


typedef struct LogicCfg LogicCfg;
struct LogicCfg
{
    rui16_t publishTime;    /* in secs */
};

typedef enum VEL_SOURCE {
    VEL_SOURCE_TELOC,
    VEL_SOURCE_EXTERNAL,
    VEL_SOURCE_GPS,
    VEL_SOURCE_COUNT,
    VEL_SOURCE_NULL,
}VEL_SOURCE;

typedef struct VelEvt VelEvt;
struct VelEvt
{
    RKH_EVT_T evt;
    float vel;
    VEL_SOURCE source;
};

typedef struct CmdEvt CmdEvt;
struct CmdEvt
{
    RKH_EVT_T evt;
    SaltCmd cmd;
};


typedef struct FailSubsystemData {

  int port;
  char *name;
  char *serial_number;
  rbool_t ct_on;
  rbool_t fe_on;

} FailSubsystem;

typedef struct SpeedData {

    float value;
    VEL_SOURCE source;


} SpeedData;

typedef struct SaltData {

    rbool_t alMode;
    SALT_CMD_ORDER currentCmd;

    FailSubsystemData failSubsystem[];

} SaltData;


typedef struct Configuration {

    double velCtOn;
    double velCtOff;
    double velFeOn;
    double timeFeHold;
    rui16_t cmdTimeout;

    double timeBlinkEnable;
    double timeBlinkDisable;
    rui8_t blinkPeriod;

    rui16_t publishPeriod;


} Configuration;

typedef struct LogicData
{
    SaltData saltData;

    SpeedData speedData; // it could be a list of SpeedData with all the sources

    Configuration configuration;

} LogicData;
```


### paginaas a desarrollar

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


