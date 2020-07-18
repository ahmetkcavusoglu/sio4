# SiO4 - A WatchX Firmware

For the fabulous [WatchX](https://watchx.io/) Arduino smart watch.

## About

This is just my personal firmware for my WatchX.  It does only what I want it to do and is as much a learning experience as anything.

Indebted to and borrowing from [N|Watch](https://github.com/zkemble/NWatch) and [TapClock](https://github.com/venice1200/TapClock).

It's currently very basic.  You can press the lower right button to show the watch face, otherwise it's in a fairly deep sleep to save power.

### Features

 - Tells the time, date, temp and battery level on a faux `ps -ax` watch face.
 - Easily lasts all day, probably two, on a single charge.
 - Allows setting the time over the serial connection.

### Future planned features

 - Simple stuff like showing the day of the week and glowing a LED when connected and/or charging.
 - Gestures to show the time (rather than a button press).
 - BLE support for synchronising useful data (like the time, weather, sunrise/sunset or moon phase).
 - Better watch faces.

## Dependencies

Install these via the Arduino library manager:

 - [SSD1306Ascii](https://github.com/greiman/SSD1306Ascii) - for writing text to the display.
 - [uRTCLib](https://github.com/Naguissa/uRTCLib) - for accessing the RTC.
 - [YetAnotherPcInt](https://github.com/paulo-raca/YetAnotherArduinoPcIntLibrary) - for button interrupts.

## Setting The Time

On reset the firmware will check if there's a current USB/serial connection.  If so it will ask for the current date and time via a series of questions.  **This is currently the only way to set the time.**

I use [PuTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty) on Windows to connect to the COM port attached to the watch to do this.  You can't connect while the Arduino programmer is running (and vice-versa) so you need to connect just after writing the firmware.  The watch will wait 10 seconds after reset for you to connect and then prompt you for the values it needs.

## The `ps -ax` Watch Face

Here's a [gif on Giphy](https://media.giphy.com/media/UtP27vEWcgTxiksHSQ/giphy.gif) showing my watch with a Sugru 'case' and the `ps -ax` watch face.


