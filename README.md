# AT24CXX EEPROM Driver

Portable, multi-instance AT24CXX EEPROM driver.

## Overview

This HAL-mediated EEPROM driver permits ease of use that is designed to be platform-independent. It is designed specifically for the [AT24CXX series](https://ww1.microchip.com/downloads/en/devicedoc/doc0180.pdf) EEPROM chips (1kB to 256kB), with compatibility expected to extend to similar chips.

## Usage

The driver's header and source rely on an external user-defined hardware abstraction layer (HAL) called `hal.h` which defines the necessary calls in the `HAL` namespace. Namely, a GPIO object with `pinMode()` and `digitalWrite()` methods, and an I2C object with `init()`, `write()`, and `writeRead()` methods. I2C methods should return zero for success and nonzero for error. Finally, a `delay_ms()` function is also required in the `HAL` namespace, as during writes the EEPROM will be in a busy state inbetween page for 5ms while the previous page write operation completes.

The HAL GPIO pin object `pinMode()` method should set as output when supplied with a const value `GPIO_OUTPUT`, and the `digitalWrite()` method should take a single boolean argument of logic level to which the pin will be driven. The HAL I2C object `init()` method should perform any necessary initialization, if relevant. The `write()` method writes bytes from the specified buffer of the specified length, while the `writeRead()` method specifies a single 8- or 16-bit value to write as register access followed by a read into the given buffer to the given length. Each method takes the target address, as it is expected that the bus may be shared.

Most methods of this driver will return true for success, or false for I2C bus error or malformed parameters.

### Example

```cpp
#include "at24cxx.h"

...

// Instantiate I2C bus
HAL::I2C i2c_bus;

// Instantiate EEPROM
PeripheralIO::AT24CXX eeprom(i2c_bus, PeripheralIO::AT24C64, 0, PIN_A3);

...

int main()
{
    bool status;
    uint8_t data_o[4] = { 'T', 'E', 'S', 'T' };
    uint8_t data_i[4];

...
    // Init I2C bus
    i2c_bus.init();

    // Init EEPROM
    eeprom.init();

...
    // Write data to the EEPROM and record whether it succeeded
    status = eeprom.write(0, data_o, 4);

    // Write protect the EEPROM
    eeprom.setWriteProtect();

    // Read the data just written to the EEPROM
    if (status)
        status = eeprom.read(0, data_i, 4);
    else
        // Some error handling

    // On success, the data read from the EEPROM is now stored in data_i
...
}

...
```

## License

MIT © 2024 John Greenwell
