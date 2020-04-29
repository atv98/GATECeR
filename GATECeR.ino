// Generic Arduino-based Thermo-Electric Cooler Regulator (GATECeR)
// 2020-04-29 - ver 1.0
//
// Built using:
// Arduino Nano Every
// Adafruit Peltier Thermo-Electric Cooler Module+Heatsink Assembly - 12V 5A (Part 1335)
// SparkFun MOSFET Power Control Kit (Part COM-12959) [includes Harris RFP30N06LE MOSFET]
// Analog Devices Low Voltage Temperature Sensor (TMP36)
// SparkFun Trimpot 10K Ohm with Knob (Part COM-09806)
// 4D Systems 1.44" Display Module (uLCD-144-G2)
// Generic 12V/5A power supply unit
// Generic 5V/1A power supply unit
// assorted electronics (e.g. breadboard, on/off switch, power connector, wires)
//


//
//  MOSFET control board (to TEC) pinout:    with flat side facing & leads pointing down,
//
//
//                            +------- - ----------------> TEC black wire
//                            |   +------ + -------------> TEC red wire  
//                            |   |
//                           -------
//                           [MOSFET]
//                           -------
//                           |  |  |
//                           |  |  +----- + -------------> +12V
//                           |  +----- - ----------------> GND
//                           +----- C -------------------> TECPin 
//
//
//
//MOSFET control board (to TEC) Pin Variables
//
int TECPin = 9;    // MOSFET to TEC connected to digital pin 9


//
//  tmp36 pinout:    with flat side facing & leads pointing down,
//
//                           -------
//                           [tmp36]
//                           -------
//                           |  |  |
//                           |  |  +----- Vin -----------> +5V
//                           |  +----- Vout (0 - 5v) ----> A/D
//                           |     |
//                           |     |
//                           |   =====  0.1uF capacitor, the key to accuracy
//                           |     |
//                           |     |
//                           +---------------------------> GND 
//
//
//
//TMP36 Pin Variables
//
int sensorPin = 0;  //the analog pin the TMP36's Vout (sense) pin is connected to
                         //the resolution is 10 mV / degree centigrade with a
                         //500 mV offset to allow for negative temperatures
float t36Vin = 5.0; // for tmp36 Vin of 3.3v use 3.3


//
//  Trimpot pinout:    horizontal orientation (which is up/down doesn't matter)
//
//
//                           -------
//                           [ POT ]
//                           -------
//                           |  |  |
//                           |  |  +----- (left) --------> +5V
//                           |  +----- (center) ---------> potPin
//                           +----- (right) -------------> GND 
//
//
//
//Trimpot Pin Variables
//
int potPin = 1;    // Potentiometer on A1
 

//
//  uLCD pinout:    lower row of five pins along bottom edge of board,
//
//                           -------------
//                           [    uLCD   ]
//                           -------------
//                           |  |  |  |  |
//                           |  |  |  |  +----- RES -----------> uLCDPin
//                           |  |  |  +----- GND --------------> GND
//                           |  |  +----- RX ------------------> TX
//                           |  +----- TX ---------------------> RX
//                           +----- Vin -----------------------> +5V
//
//
//uLCD libraries
//
#include <Goldelox_Serial_4DLib.h>
#include <Goldelox_Const4D.h>

//uLCD serial configuration
//
#define DisplaySerial Serial1
Goldelox_Serial_4DLib Display(&DisplaySerial);

//uLCD Pin Variables
//
int uLCDPin = 3;    // uLCD reset connected to pin 3 (as digital)

//uLCD definitions
//
#define TextColor SPRINGGREEN



/*
 * setup() - this function runs once when you turn your Arduino on
 */
void setup()
{
  //Initialize serial connection with the computer

  Serial.begin(9600);  //Start the serial connection with the computer
                       //to view the result open the serial monitor or plotter

  //
  //Initialize uLCD on second serial port
  //

  DisplaySerial.begin(9600);
  
  pinMode(uLCDPin, OUTPUT);  // Set uLCDPin on Arduino to Output (4D Arduino Adaptor V2 - Display Reset)
  digitalWrite(uLCDPin, 0);  // Reset the Display
  delay(100);
  digitalWrite(uLCDPin, 1);  // unReset the Display
  delay (8000);              // long delay to let the display start up
  Display.gfx_Cls();         // clear the screen
  delay(100);
}



//Global Variable Initializations
//get_raw_temp(), maintain_temp()
double temp_raw;
// adjust_temp(), maintain_temp()
float tempC, tempF;



/*
 * displayStats() - this function outputs temperature & TEC power level
 */
void displayStats(double tempSetIn, float cIn, float fIn, int PWMValIn)
{
  // update stats on the uLCD screen

  // display the temperature values
  // (some not very pretty code as the Display 
  Display.gfx_MoveTo(1,10);
  Display.print("Temperatures");
  // display the set temperature point
  Display.gfx_MoveTo(1,23);
  Display.print("Set to ( C): "); Display.print(tempSetIn);
  Display.gfx_Circle(61, 24, 1, TextColor); //display a degree symbol
  // display the detected temperatures (in degC & degF)
  Display.gfx_MoveTo(1,35);
  Display.print("Read ( C): "); Display.println(cIn);
  Display.gfx_Circle(48, 36, 1, TextColor); //display a degree symbol
  Display.print("Read ( F): "); Display.print(fIn);
  Display.gfx_Circle(48, 44, 1, TextColor); //display a degree symbol

  // display the output PWM level (0-255)
  Display.gfx_MoveTo(1,90);
  if (PWMValIn >= 100) {
    Display.print("PWM value: "); Display.print(PWMValIn);    //adjust spacing for 3 digits
  } else if (PWMValIn >= 10) {
    Display.print("PWM value:  "); Display.print(PWMValIn);   //adjust spacing for 2 digits
  } else {
    Display.print("PWM value:   "); Display.print(PWMValIn);  //adjust spacing for 1 digit
  }
  
  delay(100);
}



/*
 * computePID() - this function runs the proportional, integral, derivative (PID) control
 */
//PID Control Loop Variable Initializations
unsigned long currentTime, previousTime;
double elapsedTime;         // Current error[k], Error[k-1], Cumulative Error, Error Rate
double e_k, e_k_minus_1, e_cm, e_r;      // Control output, control output[k-1], setpoint
double u_k, u_k_plus_1, setPoint;
//PID Constants (TUNE THESE!) Negative gains because positive error (temp too high)
   // means increase control output (make it colder)
double kp = -100;
double ki = -0.001;
double kd = -0.01; 

double computePID(double inp) {
  currentTime = millis();
  elapsedTime = (double)(currentTime-previousTime);
  
  e_k = setPoint - inp; //proportional error
  e_cm += e_k * elapsedTime; //integral error
  e_r = (e_k - e_k_minus_1)/elapsedTime; //derivative error

  e_k_minus_1 = e_k;
  previousTime = currentTime;
  
  double out = kp*e_k + ki*e_cm + kd*e_r;
  //Constrain control output to within analogWrite() range
  if(out > 255) {
    out = 255;
  }
  if(out < 0) {
    out = 0;
  }
  return out;
}



/*
 * adjust_temp() - this function gets the temp target & sets the TEC power output level
 */
void adjust_temp()
{
  // get potentiometer reading
  int potRaw = analogRead(potPin);
  setPoint = potRaw/1023.0 * 15;

  // output temperature set point for serial reading/plotting
  Serial.print("setPoint:   "); Serial.print(setPoint); Serial.print(" C  "); Serial.println(tempC);
  
  // maintain TEC temperature by moderating voltage output with PWM
  // analogWrite values from 0 to 255
  double PIDout = round(computePID(tempC));
  analogWrite(TECPin, PIDout); // turn TEC on

  // output stats to the uLCD
  displayStats(setPoint, tempC, tempF, PIDout);

//Serial.print("PID Value: "); Serial.print(PIDout); Serial.println("  ");
}



/*
 * get_raw_temp() - this function gets a temp reading and converts it to voltage
 */
//Get Raw Temp Variable Initializations
double temp_sensor_raw;

void get_raw_temp()
{
  temp_sensor_raw = analogRead(sensorPin);      // read the analog value from the TMP36
  temp_raw = temp_sensor_raw * t36Vin / 1024.0; // convert the TMP36 value to voltage
}



/*
 * maintain_temp() - this function averages temp readings & starts adjust_temp()
 */
//Maintain Temp Variable Initializations
int IsLoopRun = 0;        // Test for if loop first run, for first run, just set 
                              // weighted = raw so that weighted doesn't start at 0
double temp_weighted;     // Weighted temp
float scaling = 0.005;    // Used to weight the impact of raw input, adjust as required
int LoopCounter = 0;      // Counter used to track input readings
int IsReadyToAdjust = 0;  // After 800 readings, begin any temperature adjustments 
                              // (This will allow temp value to stabilise)

double maintain_temp()
{
  get_raw_temp(); // Get raw temp
  
  if(IsLoopRun == 0)  // Check if initial run / first loop and account for if so
  {
    temp_weighted = temp_raw; // no weighting applied, so that we don't start at 0;
    IsLoopRun = 1;
    Display.println("First loop done.");
    return temp_weighted;
  }
  
  // scale the raw input and add/subtract to weighted (voltage) 
  temp_weighted = temp_weighted + (temp_raw - temp_weighted) * scaling;
  tempC = (temp_weighted - 0.5) * 100;    
  // now convert to Fahrenheit
  tempF = (tempC * 9.0 / 5.0) + 32.0;

  LoopCounter++;
  if(LoopCounter % 20 == 0)   // Make adjustments and print values every 20 readings
  {
    // display the current loop (0-800) during temperature stabilization phase
    if(IsReadyToAdjust==0) 
    {
      Display.gfx_MoveTo(1,23);
      Display.print("Loop [800]: "); Display.print(LoopCounter); // display current loop
    } else {
      adjust_temp();
    }
  }

  delay(10); // wait 10ms between each reading

  if(LoopCounter==800)  // Let temperature stabilise before making adjustments
  {
    IsReadyToAdjust=1;  // Begin making adjustments after next loop
    Display.println("");
    Display.println("");
    Display.println("Begin adjustments");
    Display.println("");
    Display.println("Have an ICE day!");
    delay(2000);
    Display.gfx_Cls();         // clear the screen of initialization items
  }
  return temp_weighted;
}



/*
 * loop() - this function loops continuously after setup() to run other functions
 */
void loop()          
{
  maintain_temp();
  delay(10);                //wait 10 milliseconds
}
