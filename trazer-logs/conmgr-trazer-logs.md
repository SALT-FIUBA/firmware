
# ConMgr trazer logs

## simulation until ConMgr_connecting sub state machine

```json
 
» ./trazer -c /dev/ttyACM0 115200 8N1

--- TRAZER V3.2 RC --- RKH trace event interpreter ---

          build date:	Dec 21 2024
          tested with:	RKH V3.0.00

Using local RKH configuration

   RKH_CFGPORT_TRC_SIZEOF_TSTAMP = 32
   RKH_CFGPORT_TRC_SIZEOF_PTR    = 32
   RKH_CFG_FWK_SIZEOF_EVT        = 8
   RKH_CFG_FWK_SIZEOF_EVT_SIZE   = 16
   RKH_CFG_FWK_MAX_EVT_POOL      = 4

   RKH_CFG_RQ_GET_LWMARK_EN      = 1
   RKH_CFG_RQ_SIZEOF_NELEM       = 8
   RKH_CFG_MP_GET_LWM_EN         = 1
   RKH_CFG_MP_SIZEOF_NBLOCK      = 8
   RKH_CFG_MP_SIZEOF_BSIZE       = 8
   RKH_CFG_SMA_TRC_SNDR_EN       = 1
   RKH_CFG_TMR_SIZEOF_NTIMER     = 16

   RKH_CFG_TRC_RTFIL_EN          = 1
   RKH_CFG_TRC_USER_TRACE_EN     = 1
   RKH_CFG_TRC_ALL_EN            = 1
   RKH_CFG_TRC_MP_EN             = 1
   RKH_CFG_TRC_RQ_EN             = 1
   RKH_CFG_TRC_SMA_EN            = 1
   RKH_CFG_TRC_TMR_EN            = 1
   RKH_CFG_TRC_SM_EN             = 1
   RKH_CFG_TRC_FWK_EN            = 1
   RKH_CFG_TRC_ASSERT_EN         = 1
   RKH_CFG_TRC_RTFIL_SMA_EN      = 1
   RKH_CFG_TRC_RTFIL_SIGNAL_EN   = 1
   RKH_CFG_TRC_NSEQ_EN           = 1
   RKH_CFG_TRC_TSTAMP_EN         = 0
   RKH_CFG_TRC_CHK_EN            = 0


-------- Parsing trace stream from /dev/ttyACM0 at 115200 8N1 --------

 [  0] | FWK | TCFG       : Update RKH Configuration from client
                                      | RKH_VERSION                   = 3.4.00
                                      | TRZ_RKH_CFGPORT_TRC_SIZEOF_TSTAMP= 32
                                      | TRZ_RKH_CFGPORT_TRC_SIZEOF_PTR= 32
                                      | TRZ_RKH_CFG_FWK_SIZEOF_EVT    = 8
                                      | TRZ_RKH_CFG_FWK_SIZEOF_EVT_SIZE= 16
                                      | TRZ_RKH_CFG_FWK_MAX_EVT_POOL  = 4
                                      | TRZ_RKH_CFG_RQ_GET_LWMARK_EN  = 1
                                      | TRZ_RKH_CFG_RQ_SIZEOF_NELEM   = 8
                                      | TRZ_RKH_CFG_MP_GET_LWM_EN     = 1
                                      | TRZ_RKH_CFG_MP_SIZEOF_NBLOCK  = 8
                                      | TRZ_RKH_CFG_MP_SIZEOF_BSIZE   = 8
                                      | TRZ_RKH_CFG_SMA_TRC_SNDR_EN   = 1
                                      | TRZ_RKH_CFG_TMR_SIZEOF_NTIMER = 16
                                      | TRZ_RKH_CFG_TRC_RTFIL_EN      = 1
                                      | TRZ_RKH_CFG_TRC_USER_TRACE_EN = 1
                                      | TRZ_RKH_CFG_TRC_ALL_EN        = 1
                                      | TRZ_RKH_CFG_TRC_MP_EN         = 1
                                      | TRZ_RKH_CFG_TRC_RQ_EN         = 1
                                      | TRZ_RKH_CFG_TRC_SMA_EN        = 1
                                      | TRZ_RKH_CFG_TRC_TMR_EN        = 1
                                      | TRZ_RKH_CFG_TRC_SM_EN         = 1
                                      | TRZ_RKH_CFG_TRC_FWK_EN        = 1
                                      | TRZ_RKH_CFG_TRC_ASSERT_EN     = 1
                                      | TRZ_RKH_CFG_TRC_RTFIL_SMA_EN  = 1
                                      | TRZ_RKH_CFG_TRC_RTFIL_SIGNAL_EN= 1
                                      | TRZ_RKH_CFG_TRC_NSEQ_EN       = 1
                                      | TRZ_RKH_CFG_TRC_TSTAMP_EN     = 0
                                      | TRZ_RKH_CFG_TRC_CHK_EN        = 0
                                      | tstamp_tick_hz                = 10

***** May be have lost trace info, sequence are not correlatives
 [  8] | FWK | STATE      : ao=0X200009D0, obj=0x08015B74, nm=ConMgr_inactive
 [  9] | FWK | STATE      : ao=0X200009D0, obj=0x08015508, nm=ConMgr_active
 [ 10] | FWK | STATE      : ao=0X200009D0, obj=0x08015C04, nm=ConMgr_initialize
 [ 11] | FWK | STATE      : ao=0X200009D0, obj=0x08015F88, nm=ConMgr_sync
 [ 12] | FWK | PSTATE     : ao=0X200009D0, obj=0x080156D4, nm=ConMgr_checkSyncTry
 [ 13] | FWK | STATE      : ao=0X200009D0, obj=0x08015BCC, nm=ConMgr_init
 [ 14] | FWK | STATE      : ao=0X200009D0, obj=0x08016070, nm=ConMgr_waitInit
 [ 15] | FWK | STATE      : ao=0X200009D0, obj=0x08015AE4, nm=ConMgr_gps
 [ 16] | FWK | STATE      : ao=0X200009D0, obj=0x08015CD8, nm=ConMgr_pin
 [ 17] | FWK | STATE      : ao=0X200009D0, obj=0x08015F50, nm=ConMgr_setPin
 [ 18] | FWK | STATE      : ao=0X200009D0, obj=0x0801599C, nm=ConMgr_enableNetTime
 [ 19] | FWK | STATE      : ao=0X200009D0, obj=0x08015A0C, nm=ConMgr_getImei
 [ 20] | FWK | STATE      : ao=0X200009D0, obj=0x08015710, nm=ConMgr_cipShutdown
 [ 21] | FWK | STATE      : ao=0X200009D0, obj=0x08015C28, nm=ConMgr_initializeFinal
 [ 22] | FWK | STATE      : ao=0X200009D0, obj=0x08015D88, nm=ConMgr_registered
 [ 23] | FWK | STATE      : ao=0X200009D0, obj=0x08015FD0, nm=ConMgr_unregistered
 [ 24] | FWK | STATE      : ao=0X200009D0, obj=0x080159D4, nm=ConMgr_failure
 [ 25] | FWK | STATE      : ao=0X200009D0, obj=0x080160A8, nm=ConMgr_waitNetClockSync
 [ 26] | FWK | STATE      : ao=0X200009D0, obj=0x08015C90, nm=ConMgr_localTime
 [ 27] | FWK | STATE      : ao=0X200009D0, obj=0x08015A44, nm=ConMgr_getOper
 [ 28] | FWK | STATE      : ao=0X200009D0, obj=0x08015748, nm=ConMgr_configure
 [ 29] | FWK | PSTATE     : ao=0X200009D0, obj=0x08015794, nm=ConMgr_configureHist
 [ 30] | FWK | STATE      : ao=0X200009D0, obj=0x08015F18, nm=ConMgr_setManualGet
 [ 31] | FWK | STATE      : ao=0X200009D0, obj=0x08015EE0, nm=ConMgr_setAPN
 [ 32] | FWK | STATE      : ao=0X200009D0, obj=0x08015964, nm=ConMgr_enableGPRS
 [ 33] | FWK | STATE      : ao=0X200009D0, obj=0x0801565C, nm=ConMgr_checkIP
 [ 34] | FWK | PSTATE     : ao=0X200009D0, obj=0x080155E4, nm=ConMgr_checkConfigTry
 [ 35] | FWK | STATE      : ao=0X200009D0, obj=0x08016160, nm=ConMgr_waitRetryConfig
 [ 36] | FWK | STATE      : ao=0X200009D0, obj=0x0801576C, nm=ConMgr_configureFinal
 [ 37] | FWK | STATE      : ao=0X200009D0, obj=0x08015848, nm=ConMgr_connecting
 [ 38] | FWK | STATE      : ao=0X200009D0, obj=0x080161D0, nm=ConMgr_waitingServer
 [ 39] | FWK | STATE      : ao=0X200009D0, obj=0x080157D4, nm=ConMgr_connected
 [ 40] | FWK | STATE      : ao=0X200009D0, obj=0x08015B1C, nm=ConMgr_idle
 [ 41] | FWK | STATE      : ao=0X200009D0, obj=0x08015A7C, nm=ConMgr_getStatus
 [ 42] | FWK | STATE      : ao=0X200009D0, obj=0x08015E34, nm=ConMgr_sending
 [ 43] | FWK | STATE      : ao=0X200009D0, obj=0x08016128, nm=ConMgr_waitPrompt
 [ 44] | FWK | STATE      : ao=0X200009D0, obj=0x080160F0, nm=ConMgr_waitOk
 [ 45] | FWK | STATE      : ao=0X200009D0, obj=0x08015E58, nm=ConMgr_sendingFinal
 [ 46] | FWK | STATE      : ao=0X200009D0, obj=0x08015D30, nm=ConMgr_receiving
 [ 47] | FWK | STATE      : ao=0X200009D0, obj=0x08015DEC, nm=ConMgr_restarting
 [ 48] | FWK | STATE      : ao=0X200009D0, obj=0x08016038, nm=ConMgr_wReopen
 [ 49] | FWK | STATE      : ao=0X200009D0, obj=0x08016198, nm=ConMgr_waitRetryConnect
 [ 50] | FWK | PSTATE     : ao=0X200009D0, obj=0x08015620, nm=ConMgr_checkConnectTry
 [ 51] | FWK | STATE      : ao=0X200009D0, obj=0x0801591C, nm=ConMgr_disconnecting
 [ 52] | FWK | TIMER      : obj=0x200009FC, nm=me->timer
 [ 53] | FWK | TIMER      : obj=0x20000A10, nm=me->timerReg
 [ 54] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_inactive
 [ 55] | SM  | TRN        : ao=0X20000A9C, sst=0X801666C, tst=0X801666C
 [ 56] | SM  | TS_STATE   : ao=0X20000A9C, st=0X801666C
 [ 57] | SM  | STATE      : ao=0X20000A9C, nxtst=0X801666C
 [ 58] | FWK | ACTOR      : obj=0x200024F4, nm=rkhtick
 [ 59] | SM  | TRN        : ao=0X200009D0, sst=ConMgr_inactive, tst=ConMgr_active
 [ 60] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_active
 [ 61] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_initialize
 [ 62] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_sync
 [ 63] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_sync
 [ 64] | SM  | TRN        : ao=0X200009D0, sst=ConMgr_sync, tst=ConMgr_init
 [ 65] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_init
 [ 66] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_init
 [ 67] | SM  | TRN        : ao=0X200009D0, sst=ConMgr_init, tst=ConMgr_gps
 [ 68] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_gps
 [ 69] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_gps
 [ 70] | SM  | TRN        : ao=0X200009D0, sst=ConMgr_gps, tst=ConMgr_pin
 [ 71] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_pin
 [ 72] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_pin
 [ 73] | SM  | TRN        : ao=0X200009D0, sst=ConMgr_pin, tst=ConMgr_enableNetTime
 [ 74] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_enableNetTime
 [ 75] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_enableNetTime
 [ 76] | SM  | TRN        : ao=0X200009D0, sst=ConMgr_enableNetTime, tst=ConMgr_getImei
 [ 77] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_getImei
 [ 78] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_getImei
 [ 79] | SM  | TRN        : ao=0X200009D0, sst=ConMgr_getImei, tst=ConMgr_cipShutdown
 [ 80] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_cipShutdown
 [ 81] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_cipShutdown
 [ 82] | SM  | TRN        : ao=0X200009D0, sst=ConMgr_cipShutdown, tst=ConMgr_setManualGet
 [ 83] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_setManualGet
 [ 84] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_setManualGet
 [ 85] | SM  | TRN        : ao=0X200009D0, sst=ConMgr_setManualGet, tst=ConMgr_initializeFinal
 [ 86] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_initializeFinal
 [ 87] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_initializeFinal
 [ 88] | SM  | DCH        : ao=0X200009D0, sig=0XFE, st=ConMgr_initializeFinal
 [ 89] | SM  | TRN        : ao=0X200009D0, sst=ConMgr_initialize, tst=ConMgr_unregistered
 [ 90] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_unregistered
 [ 91] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_unregistered
 [ 92] | SM  | TRN        : ao=0X200009D0, sst=ConMgr_unregistered, tst=ConMgr_registered
 [ 93] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_registered
 [ 94] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_configure
 [ 95] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_getOper
 [ 96] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_getOper
 [ 97] | SM  | TRN        : ao=0X200009D0, sst=ConMgr_getOper, tst=ConMgr_setAPN
 [ 98] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_setAPN
 [ 99] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_setAPN
 [100] | SM  | TRN        : ao=0X200009D0, sst=ConMgr_setAPN, tst=ConMgr_enableGPRS
 [101] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_enableGPRS
 [102] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_enableGPRS
 [103] | SM  | TRN        : ao=0X200009D0, sst=ConMgr_enableGPRS, tst=ConMgr_checkIP
 [104] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_checkIP
 [105] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_checkIP
 [106] | SM  | TRN        : ao=0X200009D0, sst=ConMgr_checkIP, tst=ConMgr_configureFinal
 [107] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_configureFinal
 [108] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_configureFinal
 [109] | SM  | DCH        : ao=0X200009D0, sig=0XFE, st=ConMgr_configureFinal
 [110] | SM  | TRN        : ao=0X200009D0, sst=ConMgr_configure, tst=ConMgr_connecting
 [111] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_connecting
 [112] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_waitingServer
 [113] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_waitingServer
 [114] | SM  | TRN        : ao=0X200009D0, sst=ConMgr_connecting, tst=ConMgr_checkConnectTry
 [115] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_checkConnectTry
 [116] | SM  | TS_STATE   : ao=0X200009D0, st=ConMgr_waitRetryConnect
 [117] | SM  | STATE      : ao=0X200009D0, nxtst=ConMgr_waitRetryConnect

```