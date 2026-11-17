/**
 * @file packet.hpp
 * @brief The 56-byte inter-board data packet (firmware README, "Inter-Board Data Packet").
 *
 * Every board receives the packet from the previous board, writes the
 * section of the device it is acting as, sets its Board ID bit, and
 * forwards the packet to the next board.
 *
 * All multi-byte fields are big-endian (high byte first), as in the README table.
 */
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace packet {

inline constexpr size_t kSize = 56;

struct Section {
    size_t offset;
    size_t length;
};

// byte layout straight from the README table
inline constexpr Section kImu{0, 12};             // accel xyz, gyro xyz
inline constexpr Section kTemperature{12, 2};
inline constexpr Section kHallEffect{14, 6};      // mag xyz
inline constexpr Section kGpioExpander{20, 1};    // ours
inline constexpr Section kColorSensor{21, 10};    // R G B C IR
inline constexpr Section kLedController{31, 24};  // 8 LEDs x RGB
inline constexpr Section kBoardIds{55, 1};

inline constexpr uint8_t kLedCount = 8;

// make sure the sections line up and add up to 56
static_assert(kImu.offset + kImu.length == kTemperature.offset);
static_assert(kTemperature.offset + kTemperature.length == kHallEffect.offset);
static_assert(kHallEffect.offset + kHallEffect.length == kGpioExpander.offset);
static_assert(kGpioExpander.offset + kGpioExpander.length == kColorSensor.offset);
static_assert(kColorSensor.offset + kColorSensor.length == kLedController.offset);
static_assert(kLedController.offset + kLedController.length == kBoardIds.offset);
static_assert(kBoardIds.offset + kBoardIds.length == kSize);
static_assert(kLedController.length == kLedCount * 3);

// bit number = position of the device in the packet, bit 0 is the debug board
enum class BoardId : uint8_t {
    DebugInput = 0,
    Imu = 1,
    Temperature = 2,
    HallEffect = 3,
    GpioExpander = 4,  // us
    ColorSensor = 5,
    LedController = 6,
};

struct ImuData {
    int16_t accel[3];
    int16_t gyro[3];
};

struct HallData {
    int16_t mag[3];
};

struct ColorData {
    uint16_t r, g, b, c, ir;
};

struct Rgb {
    uint8_t r, g, b;
};

class Packet {
public:
    using Bytes = std::array<uint8_t, kSize>;

    Packet() { bytes_.fill(0); }
    explicit Packet(const Bytes &raw) : bytes_(raw) {}

    const Bytes &bytes() const { return bytes_; }
    Bytes &bytes() { return bytes_; }

    // --- setters ---

    void setImu(const ImuData &d) {
        for (size_t i = 0; i < 3; ++i) putU16(kImu.offset + 2 * i, static_cast<uint16_t>(d.accel[i]));
        for (size_t i = 0; i < 3; ++i) putU16(kImu.offset + 6 + 2 * i, static_cast<uint16_t>(d.gyro[i]));
    }

    void setTemperature(int16_t raw) { putU16(kTemperature.offset, static_cast<uint16_t>(raw)); }

    void setHallEffect(const HallData &d) {
        for (size_t i = 0; i < 3; ++i) putU16(kHallEffect.offset + 2 * i, static_cast<uint16_t>(d.mag[i]));
    }

    void setGpioInputs(uint8_t inputs) { bytes_[kGpioExpander.offset] = inputs; }

    void setColor(const ColorData &d) {
        const uint16_t v[5] = {d.r, d.g, d.b, d.c, d.ir};
        for (size_t i = 0; i < 5; ++i) putU16(kColorSensor.offset + 2 * i, v[i]);
    }

    void setLed(uint8_t index, const Rgb &c) {
        if (index >= kLedCount) return;
        const size_t o = kLedController.offset + 3 * index;
        bytes_[o] = c.r;
        bytes_[o + 1] = c.g;
        bytes_[o + 2] = c.b;
    }

    // OR so we keep the bits the earlier boards already set
    void setBoardId(BoardId id) { bytes_[kBoardIds.offset] |= static_cast<uint8_t>(1u << static_cast<uint8_t>(id)); }

    // getters

    ImuData imu() const {
        ImuData d{};
        for (size_t i = 0; i < 3; ++i) d.accel[i] = static_cast<int16_t>(getU16(kImu.offset + 2 * i));
        for (size_t i = 0; i < 3; ++i) d.gyro[i] = static_cast<int16_t>(getU16(kImu.offset + 6 + 2 * i));
        return d;
    }

    int16_t temperature() const { return static_cast<int16_t>(getU16(kTemperature.offset)); }

    HallData hallEffect() const {
        HallData d{};
        for (size_t i = 0; i < 3; ++i) d.mag[i] = static_cast<int16_t>(getU16(kHallEffect.offset + 2 * i));
        return d;
    }

    uint8_t gpioInputs() const { return bytes_[kGpioExpander.offset]; }

    ColorData color() const {
        return {getU16(kColorSensor.offset), getU16(kColorSensor.offset + 2), getU16(kColorSensor.offset + 4),
                getU16(kColorSensor.offset + 6), getU16(kColorSensor.offset + 8)};
    }

    Rgb led(uint8_t index) const {
        if (index >= kLedCount) return {0, 0, 0};
        const size_t o = kLedController.offset + 3 * index;
        return {bytes_[o], bytes_[o + 1], bytes_[o + 2]};
    }

    uint8_t boardIds() const { return bytes_[kBoardIds.offset]; }
    bool hasBoardId(BoardId id) const { return boardIds() & (1u << static_cast<uint8_t>(id)); }

private:
    // big-endian: high byte goes first
    void putU16(size_t offset, uint16_t v) {
        bytes_[offset] = static_cast<uint8_t>(v >> 8);
        bytes_[offset + 1] = static_cast<uint8_t>(v & 0xFF);
    }

    uint16_t getU16(size_t offset) const {
        return static_cast<uint16_t>((bytes_[offset] << 8) | bytes_[offset + 1]);
    }

    Bytes bytes_;
};

static_assert(sizeof(Packet::Bytes) == kSize);

}  // namespace packet
