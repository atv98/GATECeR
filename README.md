# Generic Arduino-based ThermoElectric Cooler Regulator (GATECeR)

## Overview

Software control of a thermoelectric cooler (TEC), also know as a Peltier device, to maintain a desired temperature.  A temperature sensor provides input to determine a variable power output level to the TEC to maintain the set temperature.  The TEC draws far more power than the Arduino controller board can output and is connected via a MOSFET power controller (could use a H-Bridge, or similar circuit, with sufficent amperage handling).  Temperature readings and power levels are displayed on an LCD screen for ease of use.
## System Specification
### Thermoelectric cooler
### Temperature sensor
### Proportional, Integral, Derivative (PID) control
## Component List

* Arduino Nano Every
* SparkFun MOSFET Power Control Kit (Part COM-12959)
* Adafruit Peltier Thermo-Electric Cooler Module+Heatsink Assembly - 12V 5A (Part 1335)
* Analog Devices Low Voltage Temperature Sensor (TMP36)
* 4D Systems 1.44" Display Module (uLCD-144-G2)
* SparkFun Trimpot 10K Ohm with Knob (Part COM-09806)
* Generic 12V/5A Power Supply
* Generic 5V/1A Power Supply
* assorted electronics (e.g. breadboard, on/off switch, power connector, wires)

## Pin Connections
### Thermoelectric Cooler (TEC) Assembly

<img src="https://user-images.githubusercontent.com/60458206/80298839-15c30780-875e-11ea-9cac-0f7238d50f6e.jpg" width="300" height="250">

TEC Connectors | MOSFET Board | 12V Supply
--- | --- |
Cooler + | + 
Cooler - | - 
Fan + | | -
Fan - | | +
### MOSFET Power Control Kit

<img src="https://user-images.githubusercontent.com/60458206/80547479-4e452a00-8986-11ea-904b-b4f11e91551c.jpg" width="150" height="150">

MOSFET pin | Arduino pin | TEC | 12V Supply
--- | --- | --- | --- |
JP2-2 GND ||| +
JP2-1 RAW ||| -
JP2-3 Control	| GPIO ||  
JP1-1 || Cooler + |
JP1-2 || Cooler - |
 

### TMP36
<img src="https://user-images.githubusercontent.com/60458206/80298841-165b9e00-875e-11ea-9aec-77fbebf6c321.jpg" width="150" height="150">

Arduino pin | TMP36 pin
--- | --- |
 GND |  GND
 Vout (3.3V) |  Vs
 GPIO |  Vout


### uLCD 
<img src="https://user-images.githubusercontent.com/60458206/80299075-93d3de00-875f-11ea-8084-a9a89f5e7474.jpg" width="150" height="150">

Arduino pin | uLCD Header | uLCD Cable
--- | --- | --- |
Vu (5V) |  5V | 5V
GND |  GND | GND
 TX = P9 |  RX | TX
RX = P10 |  TX | RX
P11 | Reset | Reset

## Data Sheet & Product Pages
### Arduino Nano Every
https://www.sparkfun.com/products/15590
### SparkFun MOSFET
https://www.sparkfun.com/products/12959
### Adafruit Peltier Thermo-Electric Cooler
https://www.adafruit.com/product/1335
### TMP36
http://www.analog.com/media/en/technical-documentation/data-sheets/TMP35_36_37.pdf
### uLCD-144-G2
https://www.sparkfun.com/products/11377


## Authors

* Ryan Khan
* David Thompson
* An Vo
