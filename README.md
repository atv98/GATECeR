# Generic Arduino-based ThermoElectric Cooler Regulator (GATECeR)

## Overview

Software control of a thermoelectric cooler (TEC), also know as a Peltier device, to maintain a desired temperature.  One or more temperature sensors will provide input to determine a variable power output level to the TEC to maintain the set temperature.  The TEC draws far more power than the controller boards (e.g. mbed, Arduino, etc.) can output and will be connected via a MOSFET, H-Bridge, or similar circuit.  Temperature readings and power levels will be displayed on an LCD screen for ease of use.

## Component List

* Arduino Nano Every
* sparkfun h-bridge motor driver
* Adafruit Peltier Thermo-Electric Cooler
* TMP36 temperature sensor
* uLCD-144-G2
* 12V Power Supply
* 5V Power Supply

## Pin Connections

### H-bridge 

<img src="https://user-images.githubusercontent.com/60458206/80298838-1491da80-875e-11ea-8c60-95a70aecbae4.jpg" width="250" height="250">

Arduino pin | H-bridge pin
--- | --- |
 3.3V |  Logic for motor driver for speed information
 GPIO |  Connect to H-bridge- Motor left -
 GPIO |  Connect to H-bridge- Motor left +
 GPIO |  Connect to H-bridge- Motor Right -
 GPIO |  Connect to H-bridge- Motor right -
 

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
### sparkfun h-bridge motor driver
https://www.sparkfun.com/products/14450
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
