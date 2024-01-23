//--------------------------------------------------------------------------------------------------------------------
// Name        : at24cxx.h
// Purpose     : AT24CXX EEPROM Driver
// Description : 
//               This driver intended for control of AT24CXX EEPROM chips. Though the AT24CXX family of EEPROM chips
//               are internally organized according to varied page sizes, use of this class abstracts away this
//               arrangement so that extended strings and arrays of arbitrary length may be written to any random
//               address within the memory. Attempts to overwrite the end boundary of the chip's memory space will be
//               ignored and the write()/read() method will return false.
//
//               Calls to any operational methods will perform no action if an initial call to init() has not yet
//               been performed.
//
//               Use of write protect pin WP is optional, and calls to methods setWriteProtect() and
//               clearWriteProtect() will only execute properly if wp_pin was included at call to init().
//
// Language    : C++
// Platform    : Portable
// Framework   : Portable
// Copyright   : MIT License 2024, John Greenwell
// Requires    : External : N/A
//               Custom   : hal.h - Custom implementation-defined Hardware Abstraction Layer
//--------------------------------------------------------------------------------------------------------------------
#ifndef _AT24CXX_H
#define _AT24CXX_H

#include "hal.h"

namespace PeripheralIO
{

// Chip Selection Options
extern const uint32_t AT24C01;
extern const uint32_t AT24C02;
extern const uint32_t AT24C04;
extern const uint32_t AT24C08;
extern const uint32_t AT24C16;
extern const uint32_t AT24C32;
extern const uint32_t AT24C64;
extern const uint32_t AT24C128;
extern const uint32_t AT24C256;
extern const uint32_t AT24C512;

class AT24CXX
{
    public:
        /**
         * @brief Constructor for AT24CXX object
         * @param chip Defined const value for chip (e.g. PeripheralIO::AT23C02)
         * @param chip_addr Externally biased address of chip
         * @param wp_pin Pin ID number for GPIO connected to WP pin; value -1 for unused
         * @param i2c_channel I2C channel the AT24Cxx is connected to
        */
        AT24CXX(uint32_t chip, uint8_t chip_addr=0, uint8_t wp_pin=-1, uint8_t i2c_channel=0);

        /**
         * @brief Initialize the IO for AT24CXX object; must be called prior to use of member functions
        */
        void init();

        /**
         * @brief Write single byte to EEPROM address
         * @param address Address to which value should be written
         * @param val Value to write to EEPROM address
         * @return False for invalid request, true otherwise
        */
        bool write(uint16_t address, uint8_t val);

        /**
         * @brief Write value to EEPROM address
         * @param address Starting address to which values should be written
         * @param vals Pointer to array of values to write to EEPROM
         * @param len Number of bytes to write to EEPROM
         * @return False for invalid request, true otherwise
        */
        bool write(uint16_t address, uint8_t * vals, uint16_t len);

        /**
         * @brief Write string to EEPROM address
         * @param address Starting address where string will be written
         * @param str Pointer to string of character to write
         * @param len Number of characters to write to EEPROM
         * @return False for invalid request, true otherwise
        */
        bool write(uint16_t address, const char * str, uint16_t len);

        /**
         * @brief Read single bytefrom EEPROM address
         * @param address Address from which value should be read
         * @return Value read from address
        */
        uint8_t read(uint16_t address);

        /**
         * @brief Read from EEPROM address
         * @param address Address from which values should be read
         * @param vals Pointer to array into which read values will be placed
         * @param len Number of bytes to read from EEPROM
        */
        bool read(uint16_t address, uint8_t* vals, uint16_t len);

        /**
         * @brief Read from EEPROM address
         * @param address Address from which values should be read
         * @param str Pointer to array into which read values will be placed
         * @param len Number of bytes to read from EEPROM
        */
        bool read(uint16_t address, char* str, uint16_t len);

        /**
         * @brief Assert write protect pin such that write operations may not be applied
        */
        void setWriteProtect() const;

        /**
         * @brief Release write protect pin such that write operations may be applied
        */
        void clearWriteProtect() const;

    private:
        bool writeN(uint16_t, uint8_t*, uint16_t);
        bool readN(uint16_t, uint8_t*, uint16_t);

        HAL::I2C  _i2c;
        HAL::GPIO _wp_pin;
        uint32_t  _chip_size;
        uint8_t   _chip_addr;
        uint8_t   _page_size;
        uint8_t   _addr_bytes;
        uint8_t   _addr_ov_bits;
        uint8_t   _addr_size;
        uint8_t   _mode;
};

}

#endif // _AT24CXX_H

// EOF
