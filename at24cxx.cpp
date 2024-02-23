//--------------------------------------------------------------------------------------------------------------------
// Name        : at24cxx.cpp
// Purpose     : AT24CXX EEPROM Controller Class
// Description : This source file implements header file at24cxx.h.
// Language    : C++
// Platform    : Portable
// Framework   : Portable
// Copyright   : MIT License 2024, John Greenwell
//--------------------------------------------------------------------------------------------------------------------

#include "at24cxx.h"

namespace PeripheralIO
{

// Chip Selection (   chip size | page size   |addr bytes | addr overflow bits)
const uint32_t AT24C01  = 128   | (8 << 20)   | (1 << 28) | (0 << 30);
const uint32_t AT24C02  = 256   | (8 << 20)   | (1 << 28) | (0 << 30);
const uint32_t AT24C04  = 512   | (16 << 20)  | (1 << 28) | (1 << 30);
const uint32_t AT24C08  = 1024  | (16 << 20)  | (1 << 28) | (2 << 30);
const uint32_t AT24C16  = 2048  | (16 << 20)  | (1 << 28) | (3 << 30);
const uint32_t AT24C32  = 4096  | (32 << 20)  | (2 << 28) | (0 << 30);
const uint32_t AT24C64  = 8192  | (32 << 20)  | (2 << 28) | (0 << 30);
const uint32_t AT24C128 = 16384 | (64 << 20)  | (2 << 28) | (0 << 30);
const uint32_t AT24C256 = 32768 | (64 << 20)  | (2 << 28) | (0 << 30);
const uint32_t AT24C512 = 65536 | (128 << 20) | (2 << 28) | (0 << 30);

// Base Address and I2C Defines
const uint8_t AT24CXX_ADDR               = 0x50; // 7-bit addr
const uint8_t EEPROM_WRITE_CYCLE_TIME_MS = 5;    // datasheet: 5ms max


AT24CXX::AT24CXX(HAL::I2C& i2c_bus, uint32_t chip, uint8_t chip_addr, uint8_t wp_pin)
: _i2c(i2c_bus)
, _wp_pin(wp_pin)
, _chip_size(chip & 0x0001FFFF)
, _chip_addr((uint8_t)(AT24CXX_ADDR | (chip_addr & 0x07)))
, _page_size((uint8_t)((chip & 0x0FF00000) >> 20))
, _addr_bytes((uint8_t)((chip & 0x30000000) >> 28))
, _addr_ov_bits((uint8_t)((chip & 0xC0000000) >> 30))
, _addr_size(0)
, _mode(wp_pin)
{ }

void AT24CXX::init()
{
    _i2c.init();

    if (_mode != -1) {
        _wp_pin.pinMode(OUTPUT);
        _wp_pin.digitalWrite(false);

        _mode = 2; // Active mode, wp_pin used
    }
    else
    {
        _mode = 1; // Active mode, no wp_pin
    }
}

bool AT24CXX::write(uint16_t address, uint8_t val)
{
    uint8_t byte = val;
    return writeN(address, &byte, 1);
}

bool AT24CXX::write(uint16_t address, uint8_t * vals, uint16_t len)
{
    return writeN(address, vals, len);
}

bool AT24CXX::write(uint16_t address, const char * str, uint16_t len)
{
    return writeN(address, (uint8_t*)str, len);
}

uint8_t AT24CXX::read(uint16_t address)
{
    uint8_t byte;

    readN(address, &byte, 1);

    return byte;
}

bool AT24CXX::read(uint16_t address, uint8_t* vals, uint16_t len)
{
    return readN(address, vals, len);
}

bool AT24CXX::read(uint16_t address, char* str, uint16_t len)
{
    return readN(address, (uint8_t*)str, len);
}

void AT24CXX::setWriteProtect() const
{
    if (2 == _mode)
    {
        _wp_pin.digitalWrite(true);
    }
}

void AT24CXX::clearWriteProtect() const
{
    if (2 == _mode)
    {
        _wp_pin.digitalWrite(false);
    }
}

// Private: Hardware I2C Write Function
bool AT24CXX::writeN(uint16_t address, uint8_t* vals, uint16_t len)
{
    bool     result = false;
    uint16_t bytes_sent;
    uint8_t  addr;
    uint8_t  offset;
    uint8_t  page_size;
    uint16_t pages_req;
    uint8_t  chunk;

    if (_mode && ((uint32_t)(address + len) <= _chip_size))
    {
        page_size = _page_size;

        if ((_addr_bytes > 1) && (len > 30))
            page_size = 16; // AT24C32+ can only write 30 bytes at a time
        
        bytes_sent = 0;
        addr       = _chip_addr;
        offset     = address % page_size;
        pages_req  = (((len + offset - 1) / page_size) + 1);

        for (uint16_t i = 0; i < pages_req; i++)
        {
            if (_addr_ov_bits)
            {
                addr = ((uint8_t)((_chip_addr & 0xF8) | (((address + bytes_sent) & 0x0700) >> 8)));
                _i2c.setAddress(addr);
            }

            chunk = ((page_size - offset) < (len - bytes_sent)) ? (page_size - offset) : (len - bytes_sent);

            if (_addr_bytes > 1)
                _i2c.write((uint16_t)(address + bytes_sent), &vals[bytes_sent], chunk);
            else
                _i2c.write((uint8_t)(address + bytes_sent), &vals[bytes_sent], chunk);

            bytes_sent += chunk;
            offset = 0;
            HAL::delay_ms(EEPROM_WRITE_CYCLE_TIME_MS);
        }
        result = true;
    }

    return result;
}

// Private: Hardware I2C Read Function
bool AT24CXX::readN(uint16_t address, uint8_t* vals, uint16_t len)
{
    bool    result = false;
    uint8_t addr   = _chip_addr;

    if (_mode && ((uint32_t)(address + len) <= _chip_size))
    {
        if (_addr_ov_bits)
        {
            addr = ((uint8_t)((_chip_addr & 0xF8) | (((address + 0) & 0x0700) >> 8)));
            _i2c.setAddress(addr);
        }

        if (_addr_bytes > 1) 
            _i2c.writeRead((uint16_t)(address), vals, len);
        else
            _i2c.writeRead((uint8_t)(address), vals, len);

        result = true;
    }

    return result;
}

}

// EOF
