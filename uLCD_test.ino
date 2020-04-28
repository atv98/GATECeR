#include <Goldelox_Serial_4DLib.h>
#include <Goldelox_Const4D.h>

 #define DisplaySerial Serial1
//#include <SoftwareSerial.h>
//SoftwareSerial DisplaySerial(16,17);
Goldelox_Serial_4DLib Display(&DisplaySerial);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  DisplaySerial.begin(9600) ;
  
  pinMode(A3, OUTPUT);  // Set D17 on Arduino to Output (4D Arduino Adaptor V2 - Display Reset)
  digitalWrite(A3, 0);  // Reset the Display via D17
  delay(100);
  digitalWrite(A3, 1);  // unReset the Display via D17
  delay (8000); //let the display start up  
//  Display.gfx_Cls();            //clear the screen [not working?]

}

void loop() {
  // put your main code here, to run repeatedly:
  Display.gfx_Rectangle(3,25,33,55,RED);
  // Display.gfx_BGcolour(WHITE) ; //change background color to white [not working?]
  Display.println("This is String");
  
  Serial.println("Beuller?");
  delay(2000);
}
