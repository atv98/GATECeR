#include <Goldelox_Serial_4DLib.h>
#include <Goldelox_Const4D.h>

 #define DisplaySerial Serial1
//#include <SoftwareSerial.h>
//SoftwareSerial DisplaySerial(16,17);
Goldelox_Serial_4DLib Display(&DisplaySerial);

#define TextColor SPRINGGREEN

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  DisplaySerial.begin(9600) ;
  
  pinMode(A3, OUTPUT);  // Set D17 on Arduino to Output (4D Arduino Adaptor V2 - Display Reset)
  digitalWrite(A3, 0);  // Reset the Display via D17
  delay(100);
  digitalWrite(A3, 1);  // unReset the Display via D17
  delay (8000);         // let the display start up  
  Display.gfx_Cls();    // clear the screen
  delay(100);

//  Works in the GoldieloxBigDemo, but isn't working here. Stick to 9600 for now.
//  int br = 19200;       // change the baud rate for the screen to 19200
//  DisplaySerial.flush();
//  DisplaySerial.end() ;
//  DisplaySerial.begin(19200);
//  delay(100);
//  DisplaySerial.flush();
}

void displayStats(double tempSetIn, float cIn, float fIn, int PWMValIn)
{
  // update stats on the uLCD screen
  
// used this for testing spacing with sprintf, below
//  char buffer[18];

  // display the temperature values
  Display.gfx_MoveTo(1,10);
  Display.print("Temperatures");
  // display the set temperature point
  Display.gfx_MoveTo(1,23);
  Display.print("Set to ( C): "); Display.print(tempSetIn);
  Display.gfx_Circle(61, 24, 1, TextColor); //display a degree symbol
  // display the detected temperatures (in degC & degF)
  Display.gfx_MoveTo(1,35);
  Display.print("Read ( C): "); Display.println(cIn, 2);
  Display.gfx_Circle(48, 36, 1, TextColor); //display a degree symbol
  Display.print("Read ( F): "); Display.print(fIn, 2);

// The following output the fIn as "  ?", likely due to Arduino not including float support in sprintf
//  sprintf(buffer,"Read ( F): %4.2f",fIn);
//  Display.println(buffer);

  Display.gfx_Circle(48, 44, 1, TextColor); //display a degree symbol

  // display the output PWM level (0-255)
  Display.gfx_MoveTo(1,90);

// Due to limited numerical formmating capabilites in the uLCD library, this line ends up displaying
// erroneous 'values' when when a digit is removed (e.g. 100->99 displays as "990").
//  Display.print("PWM value: "); Display.print(PWMValIn);

// Clearing the screen takes too long.

// Using sprintf here did add the leading spaces desired, 
// but also used about 4% more storage space
//  sprintf(buffer,"PWM value: %3d",PWMValIn);
//  Display.println(buffer);

// This is less elegant, but works perfectly well and saves the ~2k from loading sprintf
  if (PWMValIn >= 100) {
    Display.print("PWM value: "); Display.print(PWMValIn);    //adjust spacing for 3 digits
  } else if (PWMValIn >= 10) {
    Display.print("PWM value:  "); Display.print(PWMValIn);   //adjust spacing for 2 digits
  } else {
    Display.print("PWM value:   "); Display.print(PWMValIn);  //adjust spacing for 1 digit
  }

// The same issue happens above as well with the temperature displays, but they change less
// frequently and the display 'error' occurs in the decimals place.  Float handling in Arduino
// is even worse, the sprintf command displays a "?" when called for a float.  There is a way
// to load the full float handling, but again, that takes up more memory.

}

//dummy variables for display
double temp = 14.0;
float tempC = 8.01;
float tempF = 46.418;

void loop() {
// put your main code here, to run repeatedly:
// Display.gfx_Rectangle(3,25,33,55,RED);
// Display.gfx_BGcolour(WHITE) ; //change background color to white [not working?]
//  Display.println("This is String");

  for (int PWMOut = 1; PWMOut < 254; PWMOut++) {
    displayStats(temp, tempC, tempF, PWMOut);
    delay(250);
  }
  Display.gfx_Cls();    // clear the screen
  
  Serial.println("Beuller?");
  delay(2000);
}
