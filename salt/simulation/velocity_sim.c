#include "velocity_sim.h"
#include <stdlib.h>  // For rand(), srand()
#include <stdio.h>   // For printf debug

#include "logic.h"
#include "salt-signals.h"

//#include <math.h>  // Uncomment for optional Gaussian noise

// Helper for random float in [min, max]
static float rand_float(float min, float max) {
    return min + (float)rand() / RAND_MAX * (max - min);
}

// Simulate evVelHasler with realistic random vel (Teloc 1500/Hasler: 0.2-200 km/h, 1% invalid)
void simulate_velHasler(void) {

    float vel = rand_float(32.0f, 34.0f);
    if (rand() % 100 < 1) vel = -vel;  // Rare invalid (<0)

    // Optional: Add Gaussian noise (mean=0, std=0.5 km/h for accuracy)
    // vel += rand_float(-0.5f, 0.5f);  // Or use normal dist: vel += sqrt(-2*log(rand_float(0,1)))*cos(2*M_PI*rand_float(0,1))*0.5;

    VelEvt *evt = RKH_ALLOC_EVT(VelEvt, evVelHasler, NULL);
    evt->vel = vel;
    evt->source = VEL_SOURCE_TELOC;

    RKH_SMA_POST_FIFO(logic, RKH_UPCAST(RKH_EVT_T, evt), NULL);
    printf("evVelHasler: vel=%.2f, source=TELOC\n", vel);
}

// Simulate evVelExternal with realistic random vel (Tachometer: 0-150 km/h, 5% invalid)
void simulate_velExternal(void) {

    float vel = rand_float(40.0f, 60.0f);
    if (rand() % 100 < 5) vel = -vel;  // Moderate invalid

    // Optional noise (std=1 km/h for pulse variability)
    // vel += rand_float(-1.0f, 1.0f);

    VelEvt *evt = RKH_ALLOC_EVT(VelEvt, evVelExternal, NULL);
    evt->vel = vel;
    evt->source = VEL_SOURCE_EXTERNAL;

    RKH_SMA_POST_FIFO(logic, RKH_UPCAST(RKH_EVT_T, evt), NULL);
    printf("Simulated evVelExternal: vel=%.2f, source=EXTERNAL\n", vel);
}

// Simulate evVelGPS with realistic random vel (SIM900/808 GPS: 0-200 km/h, 20% invalid as 0 or -1)
void simulate_velGps(void) {

    float vel;
    if (rand() % 100 < 20) {
        vel = (rand() % 2 == 0) ? 0.0f : -1.0f;  // No fix: 0 or -1
    } else {
        vel = rand_float(40.0f, 60.0f);
    }

    // Optional noise (std=0.5 km/h for GPS accuracy)
    // if (vel > 0) vel += rand_float(-0.5f, 0.5f);

    VelEvt *evt = RKH_ALLOC_EVT(VelEvt, evVelGPS, NULL);
    evt->vel = vel;
    evt->source = VEL_SOURCE_GPS;

    RKH_SMA_POST_FIFO(logic, RKH_UPCAST(RKH_EVT_T, evt), NULL);
    printf("Simulated evVelGPS: vel=%.2f, source=GPS\n", vel);
}




