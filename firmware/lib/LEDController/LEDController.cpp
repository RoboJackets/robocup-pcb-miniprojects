#include "LEDController.hpp"

PowerControl::PowerControl(): 
    software_shutdown(SSDMode::ENABLE), 
    resolution(PMSResolution::R_8BIT), 
    b14_enable(B14Mode::DISABLE), 
    hfp_select(HFPS::F_32kHZ), 
    channel_mode(RGBMode::M_24CH) {}

PowerControl::PowerControl(SSDMode ssd, PMSResolution pms, B14Mode b14, HFPS hfps, RGBMode rgb): 
    software_shutdown(ssd), 
    resolution(pms), 
    b14_enable(b14), 
    hfp_select(hfps), 
    channel_mode(rgb) {}

uint8_t PowerControl::toByte() {
    uint8_t byte = 0;
    byte |= (static_cast<uint8_t>(software_shutdown) & 0x01) << 0;
    byte |= (static_cast<uint8_t>(resolution) & 0x01) << 1;
    byte |= (static_cast<uint8_t>(b14_enable) & 0x01) << 2;
    byte |= (static_cast<uint8_t>(hfp_select) & 0x03) << 4;
    byte |= (static_cast<uint8_t>(channel_mode) & 0x01) << 6;
    return byte;
}

PhaseDelay::PhaseDelay(): 
    phase_1(PSn::DISABLE), 
    phase_2(PSn::DISABLE), 
    phase_3(PSn::DISABLE), 
    phase_4(PSn::DISABLE), 
    phase_5(PSn::DISABLE), 
    phase_6(PSn::DISABLE), 
    high_freq(HLF::LFP), 
    enable(PDE::DISABLE) {}

PhaseDelay::PhaseDelay(PDE enable, HLF high_freq, PSn phase_1, PSn phase_2, PSn phase_3, PSn phase_4, PSn phase_5, PSn phase_6):
    enable(enable),
    high_freq(high_freq),
    phase_1(phase_1),
    phase_2(phase_2),
    phase_3(phase_3),
    phase_4(phase_4),
    phase_5(phase_5),
    phase_6(phase_6) {}

uint8_t PhaseDelay::toByte() {
    uint8_t byte = 0;
    byte |= (static_cast<uint8_t>(phase_1) & 0x01) << 0;
    byte |= (static_cast<uint8_t>(phase_2) & 0x01) << 1;
    byte |= (static_cast<uint8_t>(phase_3) & 0x01) << 2;
    byte |= (static_cast<uint8_t>(phase_4) & 0x01) << 3;
    byte |= (static_cast<uint8_t>(phase_5) & 0x01) << 4;
    byte |= (static_cast<uint8_t>(phase_6) & 0x01) << 5;
    byte |= (static_cast<uint8_t>(high_freq) & 0x01) << 6;
    byte |= (static_cast<uint8_t>(enable) & 0x01) << 7;
    return byte;
}

SpreadSpectrum::SpreadSpectrum(): 
    cycle_time(CLT::t_500ms), 
    range(RNG::R_5), 
    spectrum_enable(SSP::DISABLE), 
    synch_enable(SSY::DISABLE) {}

SpreadSpectrum::SpreadSpectrum(SSY synch_enable, SSP spectrum_enable, RNG range, CLT cycle_time):
    synch_enable(synch_enable),
    spectrum_enable(spectrum_enable),
    range(range),
    cycle_time(cycle_time) {}

uint8_t SpreadSpectrum::toByte() {
    uint8_t byte = 0;
    byte |= (static_cast<uint8_t>(cycle_time) & 0x03) << 0;
    byte |= (static_cast<uint8_t>(range) & 0x03) << 2;
    byte |= (static_cast<uint8_t>(spectrum_enable) & 0x01) << 4;
    byte |= (static_cast<uint8_t>(synch_enable) & 0x01) << 5;
    return byte;
}

LEDController::LEDController(LEDControllerAddress address, i2c_inst_t* i2c): 
    address_(address), i2c_(i2c) {}

LEDControllerError LEDController::begin(PowerControl power_control, PhaseDelay phase_delay, SpreadSpectrum spread_spectrum) {
    write_byte(CONTROL_REG, power_control.toByte());
    
    pcontrol_ = power_control;
    pdelay_ = phase_delay;
    ss_ = spread_spectrum;
}

bool LEDController::setLFP(uint8_t channel, uint8_t value)
{
    switch (channel) {
        case 1:
            return write_byte(LFP1, value);
        case 2:
            return write_byte(LFP2, value);
        case 3:
            return write_byte(LFP3, value);
        case 4:
            return write_byte(LFP4, value);
        case 5:
            return write_byte(LFP5, value);
        case 6:
            return write_byte(LFP6, value);
        case 7:
            return write_byte(LFP7, value);
        case 8:
            return write_byte(LFP8, value);
        case 9:
            return write_byte(LFP9, value);
        case 10:
            return write_byte(LFP10, value);
        case 11:
            return write_byte(LFP11, value);
        case 12:
            return write_byte(LFP12, value);
        case 13:
            return write_byte(LFP13, value);
        case 14:
            return write_byte(LFP14, value);
        case 15:
            return write_byte(LFP15, value);
        case 16:
            return write_byte(LFP16, value);
        case 17:
            return write_byte(LFP17, value);
        case 18:
            return write_byte(LFP18, value);
        case 19:
            return write_byte(LFP19, value);
        case 20:
            return write_byte(LFP20, value);
        case 21:
            return write_byte(LFP21, value);
        case 22:
            return write_byte(LFP22, value);
        case 23:
            return write_byte(LFP23, value);
        case 24:
            return write_byte(LFP24, value);
    }
    return false;
}

bool LEDController::setHFP(uint8_t channel, uint16_t value)
{
    return false;
}

bool LEDController::update() {
    return write_byte(UPDATE_REG, 0);
}

bool LEDController::reset() {
    return write_byte(RESET_REG, 0);
}

bool LEDController::write_bytes(uint8_t reg, uint8_t length, uint8_t *data) {
    uint8_t buf[length + 1];
    buf[0] = reg;
    
    for (uint8_t i = 1; i < length; i++) {buf[i] = data[i - 1];}

    return (i2c_write_blocking(i2c_, address_, buf, length, false) != PICO_ERROR_GENERIC);
}

bool LEDController::write_byte(uint8_t reg, uint8_t data) {
    uint8_t buf[2];
    buf[0] = reg;  // First byte is the target register
    buf[1] = data; // Second byte is the data to write
    
    return (i2c_write_blocking(i2c_, address_, buf, 2, false) != PICO_ERROR_GENERIC);
}

bool LEDController::read_bytes(uint8_t reg, uint8_t length, uint8_t *data) {
    if (i2c_write_blocking(i2c_, address_, &reg, 1, true) == PICO_ERROR_GENERIC) return false;
    return (i2c_read_blocking(i2c_, address_, data, length, false)  != PICO_ERROR_GENERIC);
}

bool LEDController::read_byte(uint8_t reg, uint8_t *data) {
    if (i2c_write_blocking(i2c_, address_, &reg, 1, true) == PICO_ERROR_GENERIC) return false;
    return (i2c_read_blocking(i2c_, address_, data, 1, false)  != PICO_ERROR_GENERIC);
}

