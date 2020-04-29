# Generic Arduino-based Thermo-Electric Cooler Regulator (GATECeR)

## Overview

Software control of a thermo-electric cooler (TEC), also know as a Peltier device, to maintain a desired temperature.  A temperature sensor provides input to determine a variable power output level to the TEC to maintain the set temperature.  The TEC draws far more power than the Arduino controller board can output and is connected via a MOSFET power controller (could use a H-Bridge, or similar circuit, with sufficent amperage handling).  Temperature readings and power levels are displayed on an LCD screen for ease of use.

## System Specification
### Thermo-electric cooler
### Temperature sensor
### Proportional, Integral, Derivative (PID) control

Arduino's PWM outputs are controlled using the 'analogWrite(pinName, value)' function, which accepts a value from 0 (0%) to 255 (100%). The TEC's PWM output is set by a Proportional-Integral-Derivative (PID) control loop using smoothed temperature feedback from the TMP36.

The PID control equation is as follows:

    error = Setpoint - Sensor Measurement
    ControlOutput = Kp * (error) + Ki * (cumulative error) + Kd * (error rate of change)

and is realized by the 'computePID(sensor reading)' function. Its principal parameters Kp, Ki, and Kd are manually tuned by trial and error according to the general guideline:

    1) Increase Kp until the response reaches an acceptable steady state neighborhood of the setpoint, either above or below it.
    2) Increase Ki until the response eliminates its steady state error, but before the output begins to steadily oscillate.
    3) Increase Kd if desired; it can sometimes mitigate over/undershoot severity. Be careful! It is susceptible to noisy sensors.

Using this process, the TEC PID loop was tuned to the following gains: Kp = -100, Ki = -0.001, Kd = -0.01. If you're recreating this project, your mileage with these gains may vary depending on your setup and power supply.

## Component List

* Arduino Nano Every
* Adafruit Peltier Thermo-Electric Cooler Module+Heatsink Assembly - 12V 5A (Part 1335)
* SparkFun MOSFET Power Control Kit (Part COM-12959)
* Analog Devices Low Voltage Temperature Sensor (TMP36)
* SparkFun Trimpot 10K Ohm with Knob (Part COM-09806)
* 4D Systems 1.44" Display Module (uLCD-144-G2)
* Generic 12V/5A Power Supply
* Generic 5V/1A Power Supply
* assorted electronics (e.g. breadboard, on/off switch, power connector, wires)

## Pin Connections
### Thermo-electric Cooler (TEC) Assembly

<img src="https://user-images.githubusercontent.com/60458206/80298839-15c30780-875e-11ea-9cac-0f7238d50f6e.jpg" width="300" height="250">

TEC Connectors | MOSFET Board | 12V Supply
:---:|:---:|:---:|
Cooler + | + |
Cooler - | - |
Fan + | | +
Fan - | | -

### MOSFET Power Control Kit

<img src="https://user-images.githubusercontent.com/60458206/80547479-4e452a00-8986-11ea-904b-b4f11e91551c.jpg" width="150" height="150">

MOSFET pin | Arduino pin | TEC | 12V Supply
:---:|:---:|:---:|:---:|
JP2-1 RAW ||| +
JP2-2 GND | GND || -
JP2-3 Control	| GPIO ||  
JP1-1 || Cooler + |
JP1-2 || Cooler - |
 

### Analog Devices Low Voltage Temperature Sensor (TMP36)
<img src="https://user-images.githubusercontent.com/60458206/80298841-165b9e00-875e-11ea-9aec-77fbebf6c321.jpg" width="150" height="150">

TMP36 pin| Arduino pin
:---:|:---:|
 GND |  GND
 Vs | Vout (3.3V)
 Vout | GPIO


### 4D Systems 1.44" Display Module (uLCD-144-G2)
<img src="https://user-images.githubusercontent.com/60458206/80299075-93d3de00-875f-11ea-8084-a9a89f5e7474.jpg" width="150" height="150">

uLCD | Arduino pin | 5V Supply
:---:|:---:|:---:|
5V || +
GND |  GND | -
TX |  RX |
RX |  TX |
RES| GPIO |

## Data Sheet & Product Pages
### Arduino Nano Every
https://www.sparkfun.com/products/15590
### Adafruit Peltier Thermo-Electric Cooler Assembly
https://www.adafruit.com/product/1335
### SparkFun MOSFET Power Control Kit
https://cdn.sparkfun.com/datasheets/Components/General/RFP30N06LE.pdf

https://www.sparkfun.com/products/12959
### Analog Devices Low Voltage Temperature Sensor (TMP36)
http://www.analog.com/media/en/technical-documentation/data-sheets/TMP35_36_37.pdf

https://www.sparkfun.com/products/10988
### SparkFun Trimpot 10K Ohm with Knob
http://www.sparkfun.com/datasheets/Components/General/TSR-3386.pdf

https://www.sparkfun.com/products/9806
### 4D Systems 1.44" Display Module (uLCD-144-G2)
https://4dsystems.com.au/mwdownloads/download/link/id/261/

https://4dsystems.com.au/ulcd-144-g2

https://www.sparkfun.com/products/11377


## Authors

* Ryan Khan
* David Thompson
* An Vo
