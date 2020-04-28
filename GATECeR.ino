//
//  uLCD pinout:    lower row of five pins along bottom edge of board,
//
//                           -------------
//                           [    uLCD   ]
//                           -------------
//                           |  |  |  |  |
//                           |  |  |  |  +----- RES -----------> digital pin
//                           |  |  |  +----- GND --------------> GND
//                           |  |  +----- RX ------------------> TX
//                           |  +----- TX ---------------------> RX
//                           +----- Vin -----------------------> +5Vtemp
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


//
//  tmp36 pinout:    with flat side facing & leads pointing down,
//
//                           -------
//                           [tmp36]
//                           -------
//                           |  |  |
//                           |  |  +----- Vin -----------> +5v)
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
int sensorPin = 0; //the analog pin the TMP36's Vout (sense) pin is connected to
                        //the resolution is 10 mV / degree centigrade with a
                        //500 mV offset to allow for negative temperatures

                        
int TECPin = 9;    // MOSFET to TEC connected to digital pin 5
int potPin = 1;    // Potentiometer on A1
 
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

double temp_sensor_raw, temp_raw, temp_weighted;
float scaling = 0.005; // Used to weight the impact of raw input, adjust as required
int LoopCounter = 0;  // Counter used to track input readings
int IsReadyToAdjust = 0;  // After 800 readings, begin any temperature adjustments (This will allow temp value to stabilise)
int IsLoopRun = 0;  // Test for if loop first run, for first run, just set weighted = raw so that weighted doesn't start at 0
float tempC, tempF;

double t36val;
float t36Vin = 5.0; // for tmp36 Vin of 3.3v use 3.3
float voltage = 0.0;
int PWMvalue = 70; // what PWM value to send out (0 to 255)
float setTemp = 14.0; // what temperature (in degC) to hold at (+/- offset)
float offset = 1.0;   // how many degC to allow +/- range around



void displayStats(double tempSetIn, float cIn, float fIn, int PWMValIn)
{
  // update stats on the uLCD screen

  // display the temperature values
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
  Display.print("PWM value: "); Display.print(PWMValIn);
  delay(100);
}



//PID Control Loop Variable Initializations
unsigned long currentTime, previousTime;
double elapsedTime;
// Current error[k], Error[k-1], Cumulative Error, Error Rate
double e_k, e_k_minus_1, e_cm, e_r;
// Control output, control output[k-1], setpoint
double u_k, u_k_plus_1, setPoint;
//Constants (TUNE THESE!) Negative gains because positive error (temp too high) means increase control output (make it colder)
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



void get_raw_temp()
{
  temp_sensor_raw = analogRead(sensorPin);
  temp_raw = temp_sensor_raw * t36Vin /1024.0; // should it be (1023) since 0 - 1023
}



double get_temp()
{
  get_raw_temp(); // Get raw temp
  if(IsLoopRun == 0)  // First loop
  {
    temp_weighted = temp_raw; // no weighting applied, so that we don't start at 0;
    IsLoopRun = 1;
    Serial.println("First loop done.");
    return temp_weighted;
  }

  temp_weighted = temp_weighted + (temp_raw - temp_weighted) * scaling;   // scale the raw input and add/subtract to weighted (voltage)
  tempC = (temp_weighted - 0.5) * 100;    
  // now convert to Fahrenheit
  tempF = (tempC * 9.0 / 5.0) + 32.0;

  LoopCounter++;
  if(LoopCounter % 20 == 0)   // Make adjustments and print values every 20 readings
  {
    //Serial.print(setTemp); Serial.print(" setpoint, ");
    //Serial.print(tempC); Serial.println(" C, ");
    //Serial.print(tempF); Serial.println("  F");

    if(IsReadyToAdjust==1)
    {
      adjust_temp();
    }
  }

  delay(10); // wait 10ms between each reading

  if(LoopCounter*10==8000)  // Let temperature stabilise before making adjustments
  {
    IsReadyToAdjust=1;
    Serial.println("Begin adjustments");
  }
  return temp_weighted;
}


 
void loop()          
{
  
  get_temp();
  
  delay(10);                                     //waiting 10 milliseconds
  
}