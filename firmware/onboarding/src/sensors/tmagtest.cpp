#include <cstdio>
#include "pico/stdlib.h"
#include "hall_effect.hpp"

int main() {
    stdio_init_all();

    sleep_ms(2000);
    printf("TMAG GO BRRRR\n");

    Hall_Effect sensor(
        0x35,
        TMAG_TYPE::A2,
        0,
        16,
        17
    );

    if (!sensor.init_hall_effect()) {
        printf("FUCK FUCK FUCK FUCK\n");
        while (true) {
            sleep_ms(1000);
        }
    }
    printf("Sensor init.\n");

    if (!sensor.enableTemperatureSensor()) {
        printf("DAMN: Enabling Temp failed.\n");
    }

    while (true) {
        if (!sensor.read_sensor_stream()) {
            printf("SHIT: reading failed.\n");
            sleep_ms(500);
            continue;
        }

        float xyz[3] = {0.0f, 0.0f, 0.0f};
        float temp_c = 0.0f;

        bool ok_xyz = sensor.get_field_in_xyz_direction(xyz);
        bool ok_temp = sensor.get_temperature(&temp_c);

        if (ok_xyz && ok_temp) {
            printf("Mag Field : X=%.3f, Y=%.3f, Z=%.3f | Temp: %.2f C\n",
                   xyz[0], xyz[1], xyz[2], temp_c);
        } else if (ok_xyz) {
            printf("Mag Field : X=%.3f, Y=%.3f, Z=%.3f | Temp: <err>\n",
                   xyz[0], xyz[1], xyz[2]);
        } else {
            printf("SHIT: FAILURE FAILURE FAILURE\n");
        }

        sleep_ms(200);
    }
}