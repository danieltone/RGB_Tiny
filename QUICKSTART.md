# Quick Start Guide

## Fastest Way to Get Started

### 1. Hardware Setup (5 minutes)

**Using USB-ESP01S Programmer (Recommended):**
1. Insert ESP-01S into programmer socket
2. Set switch towards the YELLOW breadboard portion
3. Plug programmer into computer via USB

**Wiring RGB LED to ESP-01S:**
```
RGB Module → ESP-01S
---------    -------
R        →   GPIO0
G        →   GPIO2
B        →   TX
GND      →   GND
```

### 2. Software Upload (2 minutes)

**Arduino IDE:**
1. Open `esp01s_rgb_led/esp01s_rgb_led.ino`
2. Select: Tools → Board → Generic ESP8266 Module
3. Select: Tools → Port → (Your COM port)
4. Click Upload ⬆️

**Arduino CLI:**
```bash
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp8266:esp8266:generic esp01s_rgb_led
```

### 3. Run the Program (1 minute)

1. Flip programmer switch away from YELLOW
2. Press reset or power cycle
3. Enjoy your color-changing RGB display! 🌈

## Power Options

Connect ESP-01S to any of these:
- USB-ESP01S programmer (easiest)
- USB to 3.3V adapter
- 3.3V voltage regulator (from 5V)
- 2x AA batteries (3V works okay)

**⚠️ Never use 5V directly - it will damage the ESP-01S!**

## Troubleshooting in 30 Seconds

| Problem | Fix |
|---------|-----|
| Won't upload | Switch to YELLOW position |
| Won't run after upload | Switch away from YELLOW, reset |
| No lights | Check CH_PD connected to 3.3V |
| Partial colors | Check R/G/B wiring |

## That's it! 🎉

For detailed documentation, see [README.md](README.md)
