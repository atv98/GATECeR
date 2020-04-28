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
int potPin = 1;
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
double kd = 0; 
/*
 * setup() - this function runs once when you turn your Arduino on
 * We initialize the serial connection with the computer
 */
void setup()
{
  Serial.begin(9600);  //Start the serial connection with the computer
                       //to view the result open the serial monitor 
  //setPoint = 9; // Main setpoint for PID Control
}

// -----------------------------
//
// doEma(newData, prevEma, points)
//  calculate Exp Moving Average (ema)
//  where: newData - current datapoint value
//         prevEma - previous ema value
//         points - number of dataPoints in moving average
//
//      e.g.:  emaVal = doEma(newVal, emaVal, 10);
//
float doEma(float newVal, float prevEma, int points){
  float smooth, ema;
  smooth = 2.0/(1.0 + points);
  ema = (smooth * (newVal - prevEma)) + prevEma;
  return(ema);
}

//  readTmp36() - read thermistor <count> times with 5ms between readings
//                and returns average voltage.  The two keys to relatively consistent
//                readings on the Arduino A/D pin are:
//
//                  1 - the 0.1uF decoupling capacitor from Vout to GND
//
//                  2 - taking an average of several readings, over a
//                      period of time - 5 readings, 1 ms apart seems toi
//                      work pretty well
//
//                Tests indicate that, without the 0.1uF capacitor, the ema of the range 
//                over 30 seconds runs about 5+ without touching the thermistor, and up
//                to 16+, when holding the thermistor.  With the 0.1uF capacitor, the
//                ema stays between 1.0 and 2.0, whether the thermistor is being held
//                or not. 

char buf[256];
float prevEma = 0;
float ema = 0;

unsigned long readTmp36(int count, int delayms){
  int n = 0;
  long reading = 0;
  long high = 0;
  long low = 1024;
  long range = 0;
  long sum = 0;
  for (n=0; n < count;n++) {
    reading = analogRead(sensorPin);  
    if ( reading < low ) {
      low = reading;
    }
    if ( reading > high ) {
      high = reading;
    }
    sum += reading;
    delay(delayms);
  }
  range = high - low;
  ema = doEma(range, prevEma,30);
  prevEma = ema;
  //sprintf(buf,"readTmp36() low:%ld high:%ld avg:%ld range:%ld ema:",
                       //low,high,sum/count,range, ema);
  //Serial.print(buf);
  //Serial.println(ema,3);
  return(sum/count);
}
 
unsigned long t36val;
float t36Vin = 5.0; // for tmp36 Vin of 3.3v use 3.3
float voltage = 0.0;

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

void loop()          
{
  unsigned long t36Val = readTmp36(5,1);  // take average of 5 readings, 1 ms apart
  voltage = t36Val * t36Vin; // adjust for Vin
  voltage /= 1024.0;  // convert to volts 
  //Serial.print(voltage, 5); Serial.print(" volts, ");
 
  //  reading == 10 mV per degree with 500 mV offset
  //  Convert to degrees C i.e. (voltage - 500mV) times 100
  float tempC = (voltage - 0.5) * 100;  
  Serial.print(tempC); Serial.print(" C, ");
 
  // now convert to Fahrenheit
  float tempF = (tempC * 9.0 / 5.0) + 32.0;
  //Serial.print(tempF); Serial.println("  F");

  //get potentiometer reading
  int potRaw = analogRead(potPin);
  setPoint = potRaw/1023.0 * 16-1;
  Serial.print("setPoint:   "); Serial.print(setPoint); Serial.println(" C  ");
  // maintain TEC temperature by moderating voltage output with PWM
  // analogWrite values from 0 to 255
  double PIDout = round(computePID(tempC));
  analogWrite(TECPin, PIDout); // turn TEC on
  //Serial.print("PID Value: "); Serial.print(PIDout); Serial.println("  ");
  
  delay(100);                                     //waiting a second
}
