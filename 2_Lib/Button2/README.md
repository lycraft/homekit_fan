# Button2

Arduino/ESP library to simplify working with buttons.

* Author: Lennart Hennigs (<https://www.lennarthennigs.de>)
* Copyright (C) 2017-2022 Lennart Hennigs.
* Released under the MIT license.

## Description

This library allows you to use callback functions to track single, double, triple and long clicks. Alternatively, it provides function to use in your main `loop()`.
The library also takes care of debouncing. Using this lib will reduce and simplify your source code significantly.

It has been tested with Arduino, ESP8266 and ESP32 devices.

To see the latest changes to the library please take a look at the [Changelog](https://github.com/LennartHennigs/Button2/blob/master/CHANGELOG.md).

## How To Use

This library allows you to define a button and uses callback functions to detect different types of button interactions.
If you don't want to use callback there are also functions available for using it in your code's main `loop()`.

### Definition

* Define the button either using the `constructor` or the `begin()` function.

```c++
  void begin(byte attachTo, byte buttonMode = INPUT_PULLUP, boolean isCapacitive = false , boolean activeLow  = true);
```

* You can also `reset()` a button definition if needed.

### Button Types

* You can use the class for "real" buttons (*pullup*, *pulldown*, and *active low*) and capacative buttons that come with the ESP32.
* Per default the button pins are defined as `INPUT_PULLUP`. You can override this upon creation.
* To use it for the ESPs' built-on capacitive button pins do something like this:

```c++
   Button2 button;
   button.begin(BUTTON_PIN,, true);
```

* You can also add a custom handler for other types of buttons, e.g. handled via I2C or touch. See the section on defining custom handler below.

### Callback Handler

* Instead of frequently checking the button state in your main `loop` this class allows you to assign callback functions.
* You can define callback functions to track various types of clicks:
  * `setTapHandler()` will be be called when any click occurs.
  * `setChangedHandler()`, `setPressedHandler()` and `setReleasedHandler()` allow to detect basic interactions.
  * `setLongClickDetectedHandler()` will be called as soon as the long click timeout has passed.
  * `setLongClickHandler()` will be called after the button has released.
  * `setDoubleClickHandler()` and `setTripleClickHandler()` detect complex interactions.
* All callback function need a `Button2` reference parameter. There the reference to the triggered button is stored. This can used to call status fuctions, e.g. `wasPressedFor()`.

* You can use handlers for single or for multiple buttons.
* You can track individual or multiple events with a single handler.
* Please take a look at the included examples (see below) to get an overview over the different callback handlers and their usage.

### Longpress Handling

* There are two possible callback functions: `setLongClickDetectedHandler()` and `setLongClickHandler()`.
* `setLongClickDetectedHandler()` will be called as soon as the defined timeout has passed.
* `setLongClickDetectedRetriggerable(bool retriggerable)` allows you to define if want to get multiple norifications for a single long click depeding on the timeout.

### Timeouts

* The default timeouts for events are (in ms):

```c++
  #define DEBOUNCE_MS 50
  #define LONGCLICK_MS 200
  #define DOUBLECLICK_MS 300
```

* You can define your own timeouts by using these setter functions:
  * `void setDebounceTime(unsigned int ms)`
  * `void setLongClickTime(unsigned int ms)`
  * `void setDoubleClickTime(unsigned int ms)`
  
### Using Button2 in the main `loop()`

* Even though I suggest to use handlers for tracking events, you can also use Button2 to check button's state in the main loop
* `bool wasPressed()` allows you to check whether the button was pressed
* `clickType read(bool keepState = false)` gives you the type of click that took place
* `clickType wait(bool keepState = false)` combines this and halts execution until a button click was detected
* The `clickType` is an enum defined as...

```c++
    enum clickType {
      single_click, 
      double_click, 
      triple_click, 
      long_click,
      empty
    };
```

* There are also dedicated waits (`waitForClick()`, `waitForDouble()`, `waitForTriple()` and `waitForLong()`) to detect a sepcific type
* The `read()` and the *wait* fuctions will reset the state of `wasPressed()` unless specified otherwise (via a `bool` parameter)

* Check out the [ButtonLoop](https://github.com/LennartHennigs/Button2/blob/master/examples/ButtonLoop/ButtonLoop.ino) example to see it in action

### IDs for Button Instances

* Each button instance gets a unique (auto incremented) ID upon creation.
* You can get a buttons' ID via `getID()`.
* Alternatively, you can use `setID(int newID)` to set a new one. But then you need to make sure that they are unique.

### Adding A Custom Button State Handler

* Out of the box *Button2* supports regular hardware buttons as well as the built-in capacative touch ones of the ESP32.
* If you want to add other button type you need to define your own `byte _getState()` function
* Use `setButtonStateFunction()` to assign it to your *Button2* instance
* Don't forget to initalize the button as this cannot be handled by *Button2*
* See [CustomButtonStateHandler.ino](https://github.com/LennartHennigs/Button2/blob/master/examples/CustomButtonStateHandler/CustomButtonStateHandler.ino) and [M5StackCore2CustomHandler](https://github.com/LennartHennigs/Button2/blob/master/examples/M5StackCore2CustomHandler/M5StackCore2CustomHandler.ino) examples for more details

### The Loop
* For the class to work, you need to call the button's `loop()` member function in your sketch's `loop()` function.
* Please see the *examples* below for more details.

The button class offers a few additional functions, please take a look at the *Class Definition* below.

## Examples

- [SingleButtonSimple](https://github.com/LennartHennigs/Button2/blob/master/examples/SingleButtonSimple/SingleButtonSimple.ino) – the most basic example, shows how to assign event handlers
* [LongpressHandler](https://github.com/LennartHennigs/Button2/blob/master/examples/LongpressHandler/LongpressHandler.ino) – shows how determine the time of a button press
* [SingleButton](https://github.com/LennartHennigs/Button2/blob/master/examples/SingleButton/SingleButton.ino) – shows the different event handlers
* [MultipleButtons](https://github.com/LennartHennigs/Button2/blob/master/examples/MultipleButtons/MultipleButtons.ino) – how to use two buttons
* [MultiHandler](https://github.com/LennartHennigs/Button2/blob/master/examples/MultiHandler/MultiHandler.ino) – how to use a single handler for multiple events
* [MultiHandlerTwoButtons](https://github.com/LennartHennigs/Button2/blob/master/examples/MultiHandlerTwoButtons/MultiHandlerTwoButtons.ino) – a single handler for multiple buttons
* [TrackDualButtonClick](https://github.com/LennartHennigs/Button2/blob/master/examples/TrackDualButtonClick/TrackDualButtonClick.ino) – how to detect when two buttons are clicked at the same time
* [ButtonLoop](https://github.com/LennartHennigs/Button2/blob/master/examples/ButtonLoop/ButtonLoop.ino) – how to use the button class in the main loop (I recommend using handlers, but well...)
* [CustomButtonStateHandler.ino](https://github.com/LennartHennigs/Button2/blob/master/examples/CustomButtonStateHandler/CustomButtonStateHandler.ino) - how to assign your own button handler
* [M5StackCore2CustomHandler](https://github.com/LennartHennigs/Button2/blob/master/examples/M5StackCore2CustomHandler/M5StackCore2CustomHandler.ino) - example for the M5Stack Core2 touch buttons

## Notes

* To see the latest changes to the library please take a look at the [Changelog](https://github.com/LennartHennigs/Button2/blob/master/CHANGELOG.md).
* And if you find this library helpful, please consider giving it a star at [GitHub](https://github.com/LennartHennigs/Button2). Thanks!

## Class Definition

See below the constructors and member functions the library provides:

```c++
Button2();
Button2(byte attachTo, byte buttonMode = INPUT_PULLUP, boolean isCapacitive = false, boolean activeLow = true);

void begin(byte attachTo, byte buttonMode = INPUT_PULLUP, boolean isCapacitive = false , boolean activeLow  = true);

void setDebounceTime(unsigned int ms);
void setLongClickTime(unsigned int ms);
void setDoubleClickTime(unsigned int ms);

unsigned int getDebounceTime();
unsigned int getLongClickTime();
unsigned int getDoubleClickTime();
byte getPin();

void reset();

void setButtonStateFunction(StateCallbackFunction f);

void setChangedHandler(CallbackFunction f);
void setPressedHandler(CallbackFunction f);
void setReleasedHandler(CallbackFunction f);

void setTapHandler(CallbackFunction f);
void setClickHandler(CallbackFunction f);
void setDoubleClickHandler(CallbackFunction f);
void setTripleClickHandler(CallbackFunction f);

void setLongClickHandler(CallbackFunction f);
void setLongClickDetectedHandler(CallbackFunction f);
void setLongClickDetectedRetriggerable(bool retriggerable);

unsigned int wasPressedFor() const;
boolean isPressed() const;
boolean isPressedRaw() const;

bool wasPressed() const;
clickType read(bool keepState = false);
clickType wait(bool keepState = false);
void waitForClick(bool keepState = false);
void waitForDouble(bool keepState = false);
void waitForTriple(bool keepState = false);
void waitForLong(bool keepState = false);

byte getNumberOfClicks() const;
byte getType() const;
String clickToString(clickType type) const;

int getID() const;
void setID(int newID);

bool operator == (Button2 &rhs);

void loop();
```

## Installation

Open the Arduino IDE choose "Sketch > Include Library" and search for "Button2".
Or download the ZIP archive (<https://github.com/lennarthennigs/Button2/zipball/master>), and choose "Sketch > Include Library > Add .ZIP Library..." and select the downloaded file.

## License

MIT License

Copyright (c) 2017-2022 Lennart Hennigs

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
