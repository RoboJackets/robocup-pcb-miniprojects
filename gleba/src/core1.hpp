#pragma once
// Core 1: read the IMU on a timer, store the latest sample in shared data,
// and run the button/LED UI. I2C and button setup happen here (on core 1),
// because only core 1 uses them and interrupts run on the core that enabled them.
// Owner: Yueqi

#define IMU_SAMPLE_PERIOD_MS 1

// Entry point for core 1. Must be void(void) for multicore_launch_core1().
// Never returns.
void core1_main(void);