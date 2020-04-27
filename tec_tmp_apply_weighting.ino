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

 
/*
 * setup() - this function runs once when you turn your Arduino on
 * We initialize the serial connection with the computer
 */
void setup()
{
  Serial.begin(9600);  //Start the serial connection with the computer
                       //to view the result open the serial monitor 
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
// float doEma(float newVal, float prevEma, int points){
//   float smooth, ema;
//   smooth = 2.0/(1.0 + points);
//   ema = (smooth * (newVal - prevEma)) + prevEma;
//   return(ema);
// }

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

// char buf[256];
// float prevEma = 0;
// float ema = 0;

// unsigned long readTmp36(int count, int delayms){
//   int n = 0;
//   long reading = 0;
//   long high = 0;
//   long low = 1024;
//   long range = 0;
//   long sum = 0;
//   for (n=0; n < count;n++) {
//     reading = analogRead(sensorPin);  
//     if ( reading < low ) {
//       low = reading;
//     }
//     if ( reading > high ) {
//       high = reading;
//     }
//     sum += reading;
//     delay(delayms);
//   }
//   range = high - low;
//   ema = doEma(range, prevEma,30);
//   prevEma = ema;
//   sprintf(buf,"readTmp36() low:%ld high:%ld avg:%ld range:%ld ema:",
//                        low,high,sum/count,range, ema);
//   Serial.print(buf);
//   Serial.println(ema,3);
//   return(sum/count);
// }

unsigned long temp_sensor_raw, temp_raw, temp_weighted;
float scaling = 0.005; // Used to weight the impact of raw input, adjust as required
int LoopCounter = 0;  // Counter used to track input readings
int IsReadyToAdjust = 0;  // After 800 readings, begin any temperature adjustments (This will allow temp value to stabilise)
int IsLoopRun = 0;  // Test for if loop first run, for first run, just set weighted = raw so that weighted doesn't start at 0
float tempC, tempF;

unsigned long t36val;
float t36Vin = 5.0; // for tmp36 Vin of 3.3v use 3.3
float voltage = 0.0;
int PWMvalue = 70; // what PWM value to send out (0 to 255)
float setTemp = 14.0; // what temperature (in degC) to hold at (+/- offset)
float offset = 1.0;   // how many degC to allow +/- range around


unsigned long get_raw_temp()
{
  temp_sensor_raw = analogRead(sensorPin);
  temp_raw = temp_sensor_raw * t36Vin /1024.0; // should it be (1023) since 0 - 1023
  
  return temp_raw;  // temp_raw is global so this line doesn't do anything

}

unsigned long get_temp()
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
    Serial.print(temp_weighted, 5); Serial.print(" volts, ");
    Serial.print(tempC); Serial.print(" C, ");
    Serial.print(tempF); Serial.println("  F");

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

void adjust_temp()
{
  // maintain TEC temperature by moderating voltage output with PWM
  // analogWrite values from 0 to 255, MOSFET responds >75
  analogWrite(TECPin, PWMvalue); // turn TEC on
  if (tempC >= (setTemp + offset) && PWMvalue < 255) {
    PWMvalue = PWMvalue + 1;
  } else if (tempC <= (setTemp - offset) && PWMvalue > 70) {
    PWMvalue = PWMvalue - 1;
  }
  Serial.print("PWM Value: "); Serial.print(PWMvalue); Serial.println("  ");
}
 
void loop()          
{
  
  get_temp();
  // unsigned long t36Val = readTmp36(5,1);  // take average of 5 readings, 1 ms apart
  // voltage = t36Val * t36Vin; // adjust for Vin
  // voltage /= 1024.0;  // convert to volts 
  // Serial.print(voltage, 5); Serial.print(" volts, ");
 
  //  reading == 10 mV per degree with 500 mV offset
  //  Convert to degrees C i.e. (voltage - 500mV) times 100
  // float tempC = (voltage - 0.5) * 100;  
  // Serial.print(tempC); Serial.print(" C, ");
 
  // // now convert to Fahrenheit
  // float tempF = (tempC * 9.0 / 5.0) + 32.0;
  // Serial.print(tempF); Serial.println("  F");

  // maintain TEC temperature by moderating voltage output with PWM
  // analogWrite values from 0 to 255, MOSFET responds >75
  // analogWrite(TECPin, PWMvalue); // turn TEC on
  // if (tempC >= (setTemp + offset) && PWMvalue < 255) {
  //   PWMvalue = PWMvalue + 1;
  // } else if (tempC <= (setTemp - offset) && PWMvalue > 70) {
  //   PWMvalue = PWMvalue - 1;
  // }
  // Serial.print("PWM Value: "); Serial.print(PWMvalue); Serial.println("  ");
  
  // delay(1000);                                     //waiting a second
}
