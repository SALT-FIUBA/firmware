#ifndef __EPOCH_H__
#define __EPOCH_H__

#include "qpc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t Epoch;

typedef struct Time Time;
struct Time {
    uint8_t tm_sec;    /* Seconds after minute (0,59) */
    uint8_t tm_min;    /* Minutes after hour   (0,59) */
    uint8_t tm_hour;   /* Hours after midnight (0,23) */
    uint8_t tm_mday;   /* Month day            (1,31) */
    uint8_t tm_mon;    /* Months from January  (1,12) */
    int16_t tm_year;   /* From year 1900 */
    uint8_t tm_wday;   /* Days from Sunday     (1,7)  */
    uint8_t tm_isdst;  /* Daylight saving time flag   */
};

Epoch epoch_init(void);
Epoch epoch_get(void);
void epoch_updateNow(void);
Epoch epoch_updateByStep(void);
uint32_t epoch_mktime(Time *stime);

#ifdef __cplusplus
}
#endif

#endif
