#include <cstdint>
#include <hardware/i2c.h>

//first power management register
#define PWR_MGMT_1 
// upper bytes of accelerator data x axis
#define ACC_X_HI 0x1F
// lower bytes of accelerator data x axis
#define ACC_X_LOW 0x20
// upper bytes of accelerator data y axis
#define ACC_Y_HI 0x21
// lower bytes of accelerator data y axis
#define ACC_Y_LOW 0x22
// upper bytes of acclerator data z axis
#define ACC_Z_HI 0x23
// lower bytes of accelerator data z axis
#define ACC_Z_LOW 0x24
// upper bytes of gyroscope data x axis
#define GYRO_X_HI 0x25
// lower bytes of gyroscope data x axis
#define GYRO_X_LOW 0x26
// upper bytes of gyroscope data y axis
#define GYRO_Y_HI 0x27
// lower bytes of gyroscope data y axis
#define GYRO_Y_LOW 0x28
// upper bytes of gyroscope data z axis
#define GYRO_Z_HI 0x29
// lower bytes of gyroscope data z axis
#define GYRO_Z_LOW 0x2A

#define CONFIGURATION 0x52
// check this one again
#define GYRO_CONFIG 0x51
// accelerator configuration, determines which full scale selection it is kept in
#define ACCEL_CONFIG 0x50

// regsiter where the who am i information is kept in. 
#define WHO_AM_I_REGISTER 0x75

// value in the Who am I register
#define WHO_AM_I 0x42 
// power management register
#define PWR_MGMT 0x4E
// struct with the acclerator data from IMU
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

enum FullScaleSelect_Acc {
    // 16g
    FS0 = 0b000,
    // 8g 
    FS1 = 0b001, 
    // 4g
    FS2 = 0b010, 
    // 2g
    FS3 = 0b011
};

enum ImuError {
    // No error occurred
    NO_ERROR = 0,
    // The whoami value returned from the IMU was incorrect
    INVALID_WHOAMI = 1,
    // I2C Communication Failed (likely something is unplugged)
    I2C_FAILED = 2,
    LOW_PASS_FILTER_ERR = 3,
    GYRO_CONFIG_ERR = 4,
    ACC_CONFIG_ERR = 5,
};


class Imu {
    public:

    Imu(uint8_t address);
    // sets the imu with the right filter and scale_select
    ImuError begin(LowPassFilter filter, FullScaleSelect scale_select, FullScaleSelect_Acc acc_scale_select);
    // stores the reading in here
    ImuError read_gyro(gyroReading *reading);

    ImuError read_acc(accReading *reading);

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
    FullScaleSelect_Acc scale_select_acc_;

};

