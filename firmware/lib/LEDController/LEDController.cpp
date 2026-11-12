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
    
    //Move ssd to bit 0
    byte |= (static_cast<uint8_t>(software_shutdown) & 0x01) << 0;

    //Move resolution to bit 1
    byte |= (static_cast<uint8_t>(resolution) & 0x01) << 1;

    //Move b14_enable to bit 2
    byte |= (static_cast<uint8_t>(b14_enable) & 0x01) << 2;

    //Move hfp_select to bits 4-5
    byte |= (static_cast<uint8_t>(hfp_select) & 0x03) << 4;
    
    //Move channel_mode to bit 6
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

    //Move phase_1 to bit 0
    byte |= (static_cast<uint8_t>(phase_1) & 0x01) << 0;

    //Move phase_2 to bit 1
    byte |= (static_cast<uint8_t>(phase_2) & 0x01) << 1;

    //Move phase_3 to bit 2    
    byte |= (static_cast<uint8_t>(phase_3) & 0x01) << 2;

    //Move phase_4 to bit 3
    byte |= (static_cast<uint8_t>(phase_4) & 0x01) << 3;

    //Move phase_5 to bit 4
    byte |= (static_cast<uint8_t>(phase_5) & 0x01) << 4;

    //Move phase_6 to bit 5
    byte |= (static_cast<uint8_t>(phase_6) & 0x01) << 5;

    //Move high_freq to bit 6
    byte |= (static_cast<uint8_t>(high_freq) & 0x01) << 6;

    //Move phase delay enable to bit 7
    byte |= (static_cast<uint8_t>(enable) & 0x01) << 7;
    return byte;
}

SpreadSpectrum::SpreadSpectrum(): 
    cycle_time(CLT::t_500ms), 
    range(RNG::R_05), 
    spectrum_enable(SSP::DISABLE), 
    synch_enable(SSY::DISABLE) {}

SpreadSpectrum::SpreadSpectrum(SSY synch_enable, SSP spectrum_enable, RNG range, CLT cycle_time):
    synch_enable(synch_enable),
    spectrum_enable(spectrum_enable),
    range(range),
    cycle_time(cycle_time) {}

uint8_t SpreadSpectrum::toByte() {
    uint8_t byte = 0;

    //Move Cycle Time to bits 0-1
    byte |= (static_cast<uint8_t>(cycle_time) & 0x03) << 0;

    //Move Range to bits 2-3
    byte |= (static_cast<uint8_t>(range) & 0x03) << 2;

    //Move Spread Spectrum Enable to bit 4
    byte |= (static_cast<uint8_t>(spectrum_enable) & 0x01) << 4;

    // Move Synch Enable to bit 5
    byte |= (static_cast<uint8_t>(synch_enable) & 0x01) << 5;
    return byte;
}

LEDController::LEDController(i2c_inst_t* i2c, LEDControllerAddress address): 
    address_(address), i2c_(i2c) {}

bool LEDController::setConfigs(PowerControl power_control, PhaseDelay phase_delay, SpreadSpectrum spread_spectrum) {
    
    //Write the Power Control, Phase Delay, and Spread Spectrum configurations to their respective registers
    //Set value after each so that if one fails, the others are not set and the internal state is not updated
    if (!write_byte(CONTROL_REG, power_control.toByte())) return false;
    pcontrol_ = power_control;

    if (!write_byte(PHASE_REG, phase_delay.toByte())) return false;
    pdelay_ = phase_delay;

    if (!write_byte(SPREAD_SPECTRUM_REG, spread_spectrum.toByte())) return false;
    ss_ = spread_spectrum;

    return true;
}

bool LEDController::setLFP(uint8_t channel, uint8_t value)
{
    //Write value to respective channel register
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
    //Get the address of the channel register
    uint8_t address = 0;

    switch (pcontrol_.channel_mode) {
        case RGBMode::M_24CH:
            switch (channel) {
                case 1:
                    address = HFP1;
                    break;
                case 2:
                    address = HFP2;
                    break;
                case 3:
                    address = HFP3;
                    break;
                case 4:
                    address = HFP4;
                    break;
                case 5:
                    address = HFP5;
                    break;
                case 6:
                    address = HFP6;
                    break;
                case 7:
                    address = HFP7;
                    break;
                case 8:
                    address = HFP8;
                    break;
                case 9:
                    address = HFP9;
                    break;
                case 10:
                    address = HFP10;
                    break;
                case 11:
                    address = HFP11;
                    break;
                case 12:
                    address = HFP12;
                    break;
                case 13:
                    address = HFP13;
                    break;
                case 14:
                    address = HFP14;
                    break;
                case 15:
                    address = HFP15;
                    break;
                case 16:
                    address = HFP16;
                    break;
                case 17:
                    address = HFP17;
                    break;
                case 18:
                    address = HFP18;
                    break;
                case 19:
                    address = HFP19;
                    break;
                case 20:
                    address = HFP20;
                    break;
                case 21:
                    address = HFP21;
                    break;
                case 22:
                    address = HFP22;
                    break;
                case 23:
                    address = HFP23;
                    break; 
                case 24:
                    address = HFP24; 
            }
            break;
        case RGBMode::M_8RGB:
            switch (channel) {
                case 1:
                    address = HFP_RGB1;
                    break;
                case 2:
                    address = HFP_RGB2;
                    break;
                case 3:
                    address = HFP_RGB3;
                    break;
                case 4:
                    address = HFP_RGB4;
                    break;
                case 5:
                    address = HFP_RGB5;
                    break;
                case 6:
                    address = HFP_RGB6;
                    break;
                case 7:
                    address = HFP_RGB7;
                    break;
                case 8:
                    address = HFP_RGB8;
            }
            break;
    }

    //Retrieve the current byte to retain FMS mode and write value around it
    uint8_t data[2];
    if (!read_bytes(address, 2, data)) return false;

    //Cut off value to the maximum possible value based on the resolution and B14 mode
    data[0] = value & 0xFF;
    data[1] = (data[1] & ~(
        (pcontrol_.b14_enable == B14Mode::ENABLE)? 31 :
        (pcontrol_.resolution == PMSResolution::R_10BIT)? 3 : 0)) 
        | ((value >> 8) & 
            (pcontrol_.b14_enable == B14Mode::ENABLE)? 31 :
            (pcontrol_.resolution == PMSResolution::R_10BIT)? 3 : 0);

    return write_bytes(address, 2, data);
}

bool LEDController::setFMS(uint8_t channel, FMSMode fms)
{
    //Get the address of the channel register
    uint8_t address = 0;

    switch (pcontrol_.channel_mode) {
        case RGBMode::M_24CH:
            switch (channel) {
                case 1:
                    address = HFP1;
                    break;
                case 2:
                    address = HFP2;
                    break;
                case 3:
                    address = HFP3;
                    break;
                case 4:
                    address = HFP4;
                    break;
                case 5:
                    address = HFP5;
                    break;
                case 6:
                    address = HFP6;
                    break;
                case 7:
                    address = HFP7;
                    break;
                case 8:
                    address = HFP8;
                    break;
                case 9:
                    address = HFP9;
                    break;
                case 10:
                    address = HFP10;
                    break;
                case 11:
                    address = HFP11;
                    break;
                case 12:
                    address = HFP12;
                    break;
                case 13:
                    address = HFP13;
                    break;
                case 14:
                    address = HFP14;
                    break;
                case 15:
                    address = HFP15;
                    break;
                case 16:
                    address = HFP16;
                    break;
                case 17:
                    address = HFP17;
                    break;
                case 18:
                    address = HFP18;
                    break;
                case 19:
                    address = HFP19;
                    break;
                case 20:
                    address = HFP20;
                    break;
                case 21:
                    address = HFP21;
                    break;
                case 22:
                    address = HFP22;
                    break;
                case 23:
                    address = HFP23;
                    break; 
                case 24:
                    address = HFP24; 
            }
            break;
        case RGBMode::M_8RGB:
            switch (channel) {
                case 1:
                    address = HFP_RGB1;
                    break;
                case 2:
                    address = HFP_RGB2;
                    break;
                case 3:
                    address = HFP_RGB3;
                    break;
                case 4:
                    address = HFP_RGB4;
                    break;
                case 5:
                    address = HFP_RGB5;
                    break;
                case 6:
                    address = HFP_RGB6;
                    break;
                case 7:
                    address = HFP_RGB7;
                    break;
                case 8:
                    address = HFP_RGB8;
            }
            break;
    }

    //Retrieve the current byte to retain the HFP value and write the FMS mode around it
    uint8_t *data;
    if (!read_bytes(address + 1, 1, data)) return false;

    //FMS Mode bit placement is different based on B14 Mode
    *data = (*data & ~(3 <<
        (pcontrol_.b14_enable == B14Mode::ENABLE)? 6 : 2
    )) | (static_cast<uint8_t>(fms) & 0x03);

    return write_bytes(address + 1, 1, data);
}

bool LEDController::setGCC(GCC channel, uint8_t value)
{
    //Write value to respective channel register
    switch (channel) {
        case GCC::RED:
            return write_byte(GCCR, value);
        case GCC::GREEN:
            return write_byte(GCCG, value);
        case GCC::BLUE:
            return write_byte(GCCB, value);
    }
}

bool LEDController::update()
{
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
    buf[0] = reg;
    buf[1] = data;
    
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

