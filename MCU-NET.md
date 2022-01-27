# MCU version of rotary decoder for Canon HJ14ex4.3B IRSE virtual output or standalone rotary encoders

[Previous](EZ-USB.md) version require a lot of CPU power to decode data, so we developing more simplified version that could also cover standalone encoders support

## Functional diagram
![Functional diagram](images/func2.jpg)

## Implementation

[Core board](HW/MCU_NET_OLED/) perform all task for receiving/processing/sending data, it accept [Canon lens cable](HW/Camera_Lens_Cable/) for connecting lens directly.

Standalone encoders:

![example 1](images/enc1.png)
![example 2](images/enc2.jpg) ![example 3](images/enc3.jpg)

could also be connection to that board (for Zoom/Focus/Iris)

