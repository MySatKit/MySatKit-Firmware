#pragma once

#define I2C_SDA 15
#define I2C_SCL 13

static TwoWire I2C = TwoWire(0);

TwoWire& getI2C()
{
    return I2C;
}

void i2cInit()
{
    I2C.begin(I2C_SDA, I2C_SCL, 400000);
}
