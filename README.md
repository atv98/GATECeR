# Generic Arduino-based Thermo-Electric Cooler Regulator (GATECeR)

## Overview

Software control of a thermo-electric cooler (TEC), also know as a Peltier device, to maintain a desired temperature.  A temperature sensor provides input to determine a variable power output level to the TEC to maintain the set temperature.  The TEC draws far more power than the Arduino controller board can output and is connected via a MOSFET power controller.  Temperature readings and power levels are displayed on an LCD screen for ease of use.

<p align="center"> 
<img src="https://user-images.githubusercontent.com/60458206/80570445-c7657100-89c8-11ea-81e0-abb09efcbe2e.jpg" width="600" height="450">
</p>

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

## System Design Schematic

<p align="center"> 
<img src="https://user-images.githubusercontent.com/60458206/80570553-fe3b8700-89c8-11ea-8aed-c641bf1908ec.png" width="1000" height="650">
</p>

## System Specification

### Temperature Sensor

The TMP36 sensor is connected to one of the Arduino's analog input, ranging from 0 to 1023 which is converted to temperature values through the following equations:

    raw value = digital output * operating voltage / 1024
    tempC = (raw value - 0.5) * 100
    tempF = (tempC * 9.0 / 5.0) + 32.0
    
To reduce noise in the temperature readings, two methods in both hardware and software were utlilised. Firstly, a 0.1uF capacitor is connected to create a low pass filter, elimintating high frequency noise. Secondly, the raw input is scaled by a carefully callibrated factor and summed with the previous input value, decreasing the overall impact of the added noise but still maintaining accurate temperature determination.

The following equation illustrates this:

    new temp = prev temp + (raw temp - prev temp) * scaling
    
When recreating this, the scaling factor may vary depending on your setup.

### Proportional, Integral, Derivative (PID) Control

Arduino's PWM outputs are controlled using the analogWrite(pinName, value) function, which accepts a value from 0 (0%) to 255 (100%). The TEC's PWM output is set by a Proportional-Integral-Derivative (PID) control loop using smoothed temperature feedback from the TMP36.

The PID control equation is as follows:

    error = Setpoint - Sensor Measurement
    ControlOutput = Kp * (error) + Ki * (cumulative error) + Kd * (error rate of change)

and is realized by the computePID(sensor reading) function. Its principal parameters Kp, Ki, and Kd are manually tuned by trial and error according to the general guideline:

    1) Increase Kp until the response reaches an acceptable steady state neighborhood of the setpoint, either above or below it.
    2) Increase Ki until the response eliminates its steady state error, but before the output begins to steadily oscillate.
    3) Increase Kd if desired; it can sometimes mitigate over/undershoot severity. Be careful! It is susceptible to noisy sensors.

Using this process, the TEC PID loop was tuned to the following gains: Kp = -100, Ki = -0.001, Kd = -0.01. If you're recreating this project, your mileage with these gains may vary depending on your setup and power supply.

### Thermo-electric Cooler

The Adafruit Thermo-electric cooler consists of a peltier module connected to one of the Arduino's PWM enabled digital pins through a power MOSFET powering the module with 12V and up to 5A. The amount of cooling is controlled by the PID algorithm described earlier in order to maintain desired temperature. The heat sink module is also powered by the same 12V/5A power supply.

### uLCD Display

The uLCD display runs off a 5V supply and utilizes the only two RX and TX pins of the Arduino Nano Every board as well as any digital pin for the reset pin. It is used to display the desired temperature, current temperature as well as the PWM value currently set by the PID controller.

If you are recreating this project, the 4DGL-uLCD-SE zip folder found in this repository to be uploaded to the arduino compiler that you are using. This can be done from the top down menu following this specified path:

    sketch\ include library\ add .ZIP library
    
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
JP2-3 Control	| D9 ||  
JP1-1 || Cooler + |
JP1-2 || Cooler - |
 

### Analog Devices Low Voltage Temperature Sensor (TMP36)
<img src="https://user-images.githubusercontent.com/60458206/80298841-165b9e00-875e-11ea-9aec-77fbebf6c321.jpg" width="150" height="150">

TMP36 pin| Arduino pin
:---:|:---:|
 GND |  GND
 Vs | Vout (3.3V)
 Vout | A0


### 4D Systems 1.44" Display Module (uLCD-144-G2)
<img src="https://user-images.githubusercontent.com/60458206/80299075-93d3de00-875f-11ea-8084-a9a89f5e7474.jpg" width="150" height="150">

uLCD | Arduino pin | 5V Supply
:---:|:---:|:---:|
5V || +
GND |  GND | -
TX |  RX |
RX |  TX |
RES| D3 |

## Challenges Overcome

If you're recreating this project, make sure to look out for these potential problems that we had to solve:

The TMP36 is a finicky sensor. It can run off of 5 or 3.3V, but we were only able to get sensor values calibrated correctly at 5V, though 3.3 is theoretically less noisy. Additionally, if you wire V+ and GND backwards on the sensor, it will fry pretty quickly.

## Future Improvements

One potential improvement that could be implemented is to power the entire system using a single 12V power supply, which can be done through a number of voltage regulator ICs allowing the Arduino nano board and the uLCD display to power off 5V and the TEC off 12V.

Another improvement could be to refine the use of the delay() function calls used in the software that forces the entire program to halt for periods of time. This could be done using interrupts or through Arduino's RTOS capibilities whereby one thread is used for temperature aquistion, one for updating the uLCD display and a third for controlling the TEC.

## Video Links

### Presentation:
BlueJeans Presentation Recording: https://bluejeans.com/s/D8p4b

[Alternate] Youtube Presentation: https://youtu.be/ikx0H42VAZE


### Demonstration:
YouTube Demonstration: https://youtu.be/SU1SXLJOUXU


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

* Ryan Khan - EE
* David Thompson - EE
* An Vo - EE
