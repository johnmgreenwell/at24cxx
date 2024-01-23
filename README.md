# AT24CXX EEPROM Driver

Portable, multi-instance AT24CXX EEPROM driver.

## Overview

This HAL-mediated EEPROM driver permits ease of use that is portable across many platforms. It is designed specifically for the [AT24CXX series](https://ww1.microchip.com/downloads/en/devicedoc/doc0180.pdf) EEPROM chips, ranging in size from 1kB to 256kB.

## Usage

The driver's header and source rely on an external user-defined hardware abstraction layer (HAL) called `hal.h` which defines the necessary calls in the `HAL` namespace. Namely, a GPIO object with `pinMode()` and `digitalWrite()` methods, and an I2C object with `init()`, `setAddress()`, `write()`, and `writeRead()` methods.

The HAL GPIO pin object `pinMode()` method should set as output when supplied with a const value `GPIO_OUTPUT`, and the `digitalWrite()` method should take a single boolean argument of logic level to which the pin will be driven. The HAL I2C object `init()` method should perform any necessary initialization, and the `setAddress()` method should set the address that is stored within the object for transactions. The `write()` method writes bytes from the specified buffer of the specified length, while the `writeRead()` method specifies a single 8- or 16-bit value to write as register access followed by a read into the given buffer to the given length. 

### Example

```cpp
#include <at24cxx.h>

...

// Instantiate EEPROM
PeripheralIO::AT24CXX eeprom(PeripheralIO::AT24C64, 0, PIN_A3);

...

int main()
{
    uint8_t data_o[4] = { 'T', 'E', 'S', 'T' };
    uint8_t data_i[4];

...
    // Init EEPROM
    eeprom.init();

...
    // Write data to the EEPROM
    eeprom.write(0, data_o, 4);

    // Allow requisite time for write to complete
    delay_ms(5);

    // Write protect the EEPROM
    eeprom.setWriteProtect();

    // Read the data just written to the EEPROM
    eeprom.read(0, data_i, 4);
...
}

...
```

## License

MIT © 2024 John Greenwell
