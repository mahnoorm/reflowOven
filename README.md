# reflowOven
Reflow Oven Code.
reflowoven

Autonomous reflow oven for soldering PCBs

You need to download the MAX31855 library and arduino's PID library (by brett beauregard) for this to work

MAX31855 library: https://github.com/adafruit/Adafruit-MAX31855-library

PID library: https://github.com/br3ttb/Arduino-PID-Library/

Upload program to board
Open serial monitor so you can see current temperature, stage, and time
To start reflow stage, move the green wire (connected to pin 2 on the arduino) from ground to power (red bar)
Let the oven do its maggiccccc
When the current stage is COOL, open the oven door
Once it's done cooling, make sure you put the green wire back to ground and unplug the oven!
if the temperature isn't printing properly, make sure that the thermocouple is properly connected to the MAX31855 Amplifier
