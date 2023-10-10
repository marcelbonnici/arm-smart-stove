# Hard Boiled Egg Smart Stove - Powered by ARM
---
This hardware engineering project cooks an egg to various doneness levels (soft/medium/hard boiled) powered by components programmed at a low level, versus with libraries. The hardware is accomplished by an [ARM development board](https://www.st.com/en/evaluation-tools/nucleo-f401re.html) coordinating a [soldering plate](https://www.amazon.com/Remover-Heating-Soldering-Welding-Station/dp/B07W1ZZH8T/ref=sr_1_7?crid=2AFZ1592ZUTRB&keywords=soldering+plate&qid=1696189254&sprefix=soldering+plat%2Caps%2C165&sr=8-7), [LCD Character Display](https://www.digikey.com/en/products/detail/newhaven-display-intl/NHD-0420H1Z-FSW-GBW-33V3/2773595), [temperature sensor](https://www.digikey.com/en/products/detail/analog-devices-inc-maxim-integrated/DS1631/680597), and other small components to cook an egg in a user-friendly process.

## Motivation
Wanting to grow my microcontroller skills even outside work hours, I completed an online course on ARM processors before self-appointing a project to manifest all I learned. Having used all the above components throughout the course minus the soldering plate, I embraced my inner foodie (and past as a DIY food automation YouTuber youtube.com/astonishingstudios) to make a stove that automates the cooking process, while being flexible enough to acknowledge users’ preferred egg donenesses.

## Video Demonstration
[Click Here to Watch](https://youtu.be/pxwZWfczoWY) TODO

[![Turntable Fringe Projection](README-images/low_phase_map.png)](https://youtu.be/pxwZWfczoWY) TODO

The STM32 electronic schematic can be followed below:

![Schematic](README-images/schem.png) TODO

### Requirements
The code files require: TODO
1. [Arduino IDE](https://www.arduino.cc/en/main/software)
2. [Python 3](https://www.python.org/downloads/)
3. [Numpy](https://scipy.org/install.html)
4. [OpenCV](https://opencv.org/releases/)
5. [Serial](https://pypi.python.org/pypi/pyserial)
6. [Matplotlib](https://matplotlib.org/users/installing.html)
7. [The Python Standard Library (for time, subprocess, csv, threading, os, shutil)](https://docs.python.org/3/library/)

## How to Run the Code
TODO

## Future Possibilities
The soldering plate, which only has on and off modes, makes for an adequate stove because it gets hot enough to boil an egg, but not hotter. An alternative that features temperature control would enable this project to show my PID skills, too.

Similarly, the temperature sensor can read up to 125 degrees Celsius, or 257 degrees Fahrenheit, meaning choosing another sensor would enable the stove to cook other items, especially cuts of meat.

Until then, the project suffices as the best tool in my morning routine arsenal.
