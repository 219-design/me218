# ME218 PCB Ruler - Digital Level 

This sketch creates a digital level, utilizing the IMU on the XIAO-nRF52840 board. Gyroscope and accelerometer readings are displayed on the OLED screen, along with roll and pitch angles. A complementary filter is used for approximation of pitch/roll angles. The LEDs will indicate white for levelness in the appropriate direction, and a graduated scale of color (purple to red) for the direction of positive slope.

![level](https://github.com/219-design/me218/blob/main/pcb-ruler/firmware/ruler_lib/examples/digital_level/photos/PXL_20251009_213600901.jpg)
