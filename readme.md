# Snake on a 16x16 LED matrix

A while back, I came across a 16x16 LED matrix equipped with WS2812 LEDs and couldn't resist purchasing it. At first, I didn't have a specific purpose in mind for it, so I decided to experiment. I connected it to an ESP8266 microcontroller and integrated an old joystick from the Commodore C64 era. To give it a neat finish, I housed the entire setup in an IKEA Sannahed picture frame and then developed some software to bring it all to life.

![](docs/frame01.jpg)
![](docs/frameSnake.jpg)

## Shopping list
 - 16x16 LED matrix w/ WS2812B Leds like [this](https://de.aliexpress.com/item/1005005034320069.html)
  - any ESP8266 board 
  - some 5V power (I use a USB-C to 5V step down chip)
  - IKEA Sannahed 25 x 25cm
  - Sub D 9 pin connector for the joystick

## Wiring

The three cables from the LED matrix are connected as follows: one goes to GND, another to +5V, and the last one to GPIO06. That's all that's needed for the display setup.

These old joysticks are essentially composed of switches connected to a common signal. Each button simply shorts this common signal to one of the pins on the sub-D connector. To set it up, simply connect the common signal to GND and the other pins to some GPIOs configured as inputs with pull-up resistors enabled."

![](docs/wiring.png)

## Software

I  quickly hacked together some code to do something useful with these LEDs. It was never intended to be published, but it works.

Simply open it in the Arduino IDE, compile it, and then flash it to the ESP.

It depends only on the FastLED libaray. Tested with version 3.6.
