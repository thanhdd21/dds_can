#pragma once
#include <cstdint>
#include <cstddef>

#ifndef CRC8_TABLE_SIZE
#define CRC8_TABLE_SIZE 256
#endif

constexpr uint8_t CRC8_J1850_POLY = 0x1D;
constexpr uint8_t CRC8_J1850_INIT = 0xFF;

namespace crc
{
#if CRC8_TABLE_SIZE == 16
    // 16-entry nibble table (fast, small ROM)
    extern const uint8_t CRC8_J1850_TABLE_16[16];
#elif CRC8_TABLE_SIZE == 256
    // Full 256-entry table (fastest)
    extern const uint8_t CRC8_J1850_TABLE[256];
#else
#error "CRC8_TABLE_SIZE must be 16 or 256"
#endif

    // CRC function
    uint8_t crc8_j1850(const uint8_t* data, uint32_t length);

} // namespace crc