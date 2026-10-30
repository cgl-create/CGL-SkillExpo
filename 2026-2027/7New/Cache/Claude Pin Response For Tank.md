Here are the connections, assuming an Arduino Uno or Nano.

**Ultrasonic sensor (HC-SR04)**

| Sensor | Arduino |
|---|---|
| VCC | 5V |
| Trig | Pin 7 |
| Echo | Pin 8 |
| GND | GND |

**Servos**

| Wire | Scan servo | Pusher servo |
|---|---|---|
| Signal (orange/yellow) | Pin 6 | Pin 5 |
| Power (red) | External 5 V supply + | External 5 V supply + |
| Ground (brown/black) | External supply − and Arduino GND | External supply − and Arduino GND |

Power both servos from their own 5 V supply (such as a 4×AA pack or a 5 V regulator). Don't power them from the Arduino's 5V pin.

**Arm switch and LED**

| Part | Connection |
|---|---|
| Arm switch | One side to Pin 4, other side to GND |
| Warning LED | Pin 13 → 220 Ω resistor → LED long leg; LED short leg → GND (the built-in LED on pin 13 also works) |

**Flywheel motors (MOSFET IRLZ44N or similar)**

With the flat printed face toward you and the legs pointing down, the legs are Gate, Drain, Source from left to right.

| From | To |
|---|---|
| Arduino Pin 3 | 100 Ω resistor, then MOSFET Gate |
| MOSFET Gate | 10 kΩ resistor to GND (pulldown) |
| MOSFET Source | GND |
| MOSFET Drain | Negative (−) wire of both motors, joined together |
| Both motors' positive (+) wires | Flywheel battery + (through the kill switch) |
| Diode (1N5819 or 1N4007) | Stripe end to battery +, other end to MOSFET Drain |

Wire the two motors in parallel. Mount them facing each other, and if the dart is pulled backward or jams when you test, swap the two wires on one motor only.

**Grounds (important)**
Connect these together at one point: Arduino GND, servo supply −, flywheel battery −, and MOSFET Source. Without a common ground, the MOSFET and servos won't work properly.

**Power summary**
- Arduino: USB or its own battery.
- Servos: separate 5 V supply.
- Flywheel motors: their own battery, with a kill switch in the positive line.

Please double-check the diode direction and the MOSFET pinout before powering on, and test with the arm switch open first.