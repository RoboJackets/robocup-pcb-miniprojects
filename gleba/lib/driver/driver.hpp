#include <cstdint>
#include <hardware/i2c.h>


#define ACC_X_HI 0x1F

#define ACC_X_LOW 0x20

#define ACC_Y_HI 0x21

#define ACC_Y_LOW 0x22

#define ACC_Z_HI 0x23

#define ACC_Z_LOW 0x24

#define GYRO_X_HI 0x25

#define GYRO_X_LOW 0x26

#define GYRO_Y_HI 0x27

#define GYRO_Y_LOW 0x28

#define GYRO_Z_HI 0x29

#define GYRO_Z_LOW 0x2A

#define CONFIGURATION 0x52
// check this one again
#define GYRO_CONFIG 0x51

#define WHO_AM_I 0x75

struct accReading {
    double x;
    double y;
    double z;
};

struct gyroReading {
    double x;
    double y;
    double z;
};

enum LowPassFilter {
    // default
    CFG0 = 0,
    // 4
    CFG1 = 1,
    // 5
    CFG2 = 2,
    // 8
    CFG3 = 3,
    // 10
    CFG4 = 4,
    // 16
    CFG5 = 5,
    // 20
    CFG6 = 6,
    // 40
    CFG7 = 7,
    // low latency option1
    CFG14 = 14,
    // low latency option2
    CFG15 = 15
};

enum FullScaleSelect {
    // 2000 dps
    FS0 = 0b000, 
    // 1000 dps
    FS1 = 0b001, 
    // 500 dps
    FS2 = 0b010, 
    // 250 dps
    FS3 = 0b011,
    // 125 dps 
    FS4 = 0b100, 
    // 62.5 dps
    FS5 = 0b101, 
    // 31.25 dps
    FS6 = 0b110,
    // 15.625 dps
    FS7 = 0b111
};

enum ImuError {
    // No error occurred
    NO_ERROR = 0,
    // The whoami value returned from the IMU was incorrect
    INVALID_WHOAMI = 1,
    // I2C Communication Failed (likely something is unplugged)
    I2C_FAILED = 2,
};


class Imu {
    public:

    Imu(i2c_inst_t *i2c, uint8_t address)
    // sets the imu with the right filter and scale_select
    ImuError begin(LowPassFilter filter, FullScaleSelect scale_select);
    // stores the reading in here
    ImuError read_gyro(gyroReading *reading);
    private:
    bool exit_sleep();
    bool read_byte(uint8_t address, uint8_t *data);
    bool read_bytes(uint8_t address, uint8_t length, uint8_t *data);
    bool write_byte(uint8_t address, uint8_t data);
    bool write_bytes(uint8_t address, uint8_t length, uint8_t *data);
    bool write_bit(uint8_t address, uint8_t bit, uint8_t data);

    uint8_t address_;
    LowPassFilter low_pass_;
    FullScaleSelect scale_select_;

};

