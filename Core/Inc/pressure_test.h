#ifndef __SENSORS_H
#define __SENSORS_H

#include <stdint.h>

#define PRESSURE_DEBUG_MODE 1
#define APPLY_EMA_FILTER 1

#define SAMPLE_RATE_MS 50
#define TEST_DURATION_MS 5000
#define TOTAL_SAMPLES (TEST_DURATION_MS/SAMPLE_RATE_MS)



typedef struct {
	uint32_t max;
	uint32_t last;
#if PRESSURE_DEBUG_MODE
	uint32_t samples[TOTAL_SAMPLES];
#endif
}PressureResult_t;

void PressureTest(void);

#endif /* __SENSORS_H */
