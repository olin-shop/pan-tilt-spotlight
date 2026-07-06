# The Pan/Tilt Spotlight Guide

Coding motors and LEDs from scratch can be really messy and frustrating. To make things easier, we wrote this "library" (which is just a pre-written file of code). It hides all the complicated math so you can just focus on giving simple commands to make your spotlight move and light up.

---

## Where do the wires go?

Before you code, make sure your hardware is plugged into the right pins on your Arduino Nano:

| Part | Pin Number |
| :--- | :--- |
| **Left/Right Motor (Pan)** | Pin 10 |
| **Up/Down Motor (Tilt)** | Pin 9 |
| **Regular LED** | Pin 8 |
| **RGB LED - Red Wire** | Pin 4 |
| **RGB LED - Green Wire** | Pin 6 |
| **RGB LED - Blue Wire** | Pin 7 |
| **Flashing LED - One Second** | Pin 2 |
| **Flashing LED - Tenth Second** | Pin 3 |
| **Fading LED** | Pin 5 |
| **Constant LED** | Pin 13 |

---

## Your Starting Code

Every time you make a new program, start with this exact blank template. 

In Arduino, `setup()` runs exactly one time when you plug it in. `loop()` runs over and over forever.

```cpp
#include "Spotlight.h"

// This tells the Arduino that our spotlight exists
Spotlight spotlight;

void setup() {
  Serial.begin(115200);
  
  // 1. Turn on the hardware
  spotlight.begin();

  // --- Put your starting code here: ---
  
}

void loop() {
  // 2. Keep the motors and lights running smoothly
  spotlight.update();
  
  // --- Put your main code here: ---
  
}
```

# Command Dictionary

Here is every command you can type into your code, what it does, and an example of how to use it.
## The Required Stuff

These two commands make the whole thing work. You have to include them.

  - `spotlight.begin();`

       -  What it does: Turns on the pins and gets the motors ready.

       -  Example: `spotlight.begin();` (Put this in `setup()`)

 - `spotlight.update();`

      - What it does: This is the engine. It constantly checks the time to move the motors and fade the lights. If you delete this, your spotlight will freeze!

      - Example: `spotlight.update();` (Put this at the very top of `loop()`)

## Controlling the Lights
 - `spotlight.turnOnLED();`

      - What it does: Turns on the regular LED on Pin 8.

      - Example: `spotlight.turnOnLED();`

 - `spotlight.turnOffLED();`

      - What it does: Turns off the regular LED on Pin 8.

      - Example: `spotlight.turnOffLED();`

 - `spotlight.setRGB(red, green, blue);`

      - What it does: The RGB LED has three colors inside it. You turn them on using true and off using false.

      - Example: To make Purple (Red + Blue), you would type: `spotlight.setRGB(true, false, true);`

## Moving the Motors

Motors use angles from 0 to 180 degrees. 90 is dead center.

 - `spotlight.set_pan(angle);`

     - What it does: Tells the left/right motor to immediately point at a specific number.

     - Example: To point all the way left: `spotlight.set_pan(0);`
  
 - `spotlight.set_tilt(angle);`

     - What it does: Tells the up/down motor to immediately point at a specific number.

     - Example: To point straight ahead: `spotlight.set_tilt(90);`

 - `spotlight.pan_bounce(minimum, maximum);`

     - What it does: Tells the spotlight to automatically sweep left and right between two angles forever.

     - Example: Sweep between 45 and 135 degrees: `spotlight.pan_bounce(45, 135);`

 - `spotlight.tilt_bounce(minimum, maximum);`

     - What it does: Tells the spotlight to automatically nod up and down between two angles forever.

     - Example: Nod between 80 and 100 degrees: `spotlight.tilt_bounce(80, 100);`

 - `spotlight.stop_bounce();`

     - What it does: Immediately stops all automatic bouncing and freezes the motors where they are.

     - Example: `spotlight.stop_bounce();`

## Asking the Arduino Nano a Question

Sometimes your code needs to know where the motor is pointing so it can make a decision (like changing an LED color).

 - `spotlight.get_pan();`

     - What it does: Asks the Arduino what angle the left/right motor is at right now.

     - Example: `int currentPan = spotlight.get_pan();`

 - `spotlight.get_tilt();`

     - What it does: Asks the Arduino what angle the up/down motor is at right now.

     - Example: `int currentTilt = spotlight.get_tilt();`

### Two Quick Tips for Beginners

#### Don't use delay()!

You might see `delay(1000);` in other Arduino tutorials. Do not use it here! delay() literally freezes the entire Arduino. If you use it, your motors will stop moving and your LEDs will stop fading.

#### Why do the motors go to sleep?

If a motor stays in one spot for more than half a second, we programmed it to automatically go to sleep. We did this because cheap motors make a really annoying buzzing sound when they are holding still. As soon as you tell it to move again, it will wake right back up!
