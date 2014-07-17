// 360º Matchbox Pinhole Camera
// Arduino software to control the motors that controls the cameras
// by Potentiometer or by Automatic light measure
// Project Developed on Interactivos? Birmingham 2014
// Project leader: Rafael Ski
// Colaborators: Gustavo, Imma, Idris, Sonia, Bec, Anna, Pablo, Javier, Juan, Alberto.

// Code initialy based on Michal Rinott´s code to control
// servo position using potentiometer <http://people.interaction-ivrea.it/m.rinott> 


#include <Servo.h> 

Servo myservos[4];  // create servo object to control a servo 
int myservos_pin[] = {3, 5, 6, 11};

int pot_pin = 0;      // analog pin used to connect the potentiometer
int val;              // variable to read the value from the potentiometer´s analog pin 
int valmap = 70;
int val_photosensor;  // variable to read the value from the LDR photosensor
int eachCounter = 0;  // variable to count from 1 to 4 (among the motors)

boolean bAll = true;    // button all motors together
boolean bEach = false;  // button each motor, one by one
boolean bAuto = false;  // button automatic motors

int bAll_now = 0;   // actual state of the button
int bEach_now = 0;  // actual state of the button
int bAuto_now = 0;  // actual state of the button

int bAll_previous = 0;   // previous state of the button
int bEach_previous = 0;  // previous state of the button
int bAuto_previous = 0;  // previous state of the button

int bAll_pin = 2;
int bEach_pin = 4;
int bAuto_pin = 19;  //analog input A5

int shutOpen = 120;  // open the shutter
int shutClose = 70;  // close the shutter

//LEDs
int led_motors[] = {7, 8, 9, 10};  // LED corresponding to the motors

int green = 12;  // LED that means "free to use"
int red = 13;    // LED that means "busy"


void setup() 
{
  Serial.begin (9600);

  for (int i=0; i<4; i+=1){
    myservos[i].attach(myservos_pin[i]);  // attaches the servos on pins to the servo objects
    pinMode(led_motors[i], OUTPUT);
  }

  pinMode (bAll_pin, INPUT);
  pinMode (bEach_pin, INPUT);
  pinMode (bAuto_pin, INPUT);

  pinMode (green, OUTPUT);
  pinMode (red, OUTPUT);
}


void loop() 
{  
  bAll_now = digitalRead(bAll_pin);
  bEach_now = digitalRead(bEach_pin);
  bAuto_now = digitalRead(bAuto_pin);
  
  if (bAll_now != bAll_previous) {  // tests if the AllTogheter button was pressed
    if (bAll_now == HIGH) {
      bAll = true;
      bEach = false;
      bAuto = false;
    }
  }
  bAll_previous = bAll_now;

  if (bEach_now != bEach_previous) {  // tests if the One by One button was pressed
    if (bEach_now == HIGH) {
      for(int i=0; i<4; i++){
        digitalWrite(led_motors[i], LOW);
      }
      eachCounter += 1;
      eachCounter = eachCounter % 4;
      bEach = true;
      bAll = false;
      bAuto = false;
    }
  }
  bEach_previous = bEach_now;

  if (bAuto_now != bAuto_previous) {  // tests if the Automatic button was pressed
    if (bAuto_now == HIGH) {
      val_photosensor = analogRead(1);                            // reads the value of the potentiometer (value between 0 and 1023) 
      val_photosensor = map(val_photosensor, 0, 300, 50, 10000);  // scale it to use it with the servo (value between 0 and 1lMotor20)
      Serial.println (val_photosensor);
      bAuto = true;
      bAll = false;
      bEach = false;
    }
  }
  bAuto_previous = bAuto_now;


  if (bAll) {
    if (abs(analogRead(pot_pin) - val) > 15) {
      val = analogRead(pot_pin);                          // reads the value of the potentiometer (value between 0 and 1023) 
      valmap = map(val, 15, 1023, shutClose, shutOpen);  // scale it to use it with the servo (value between 0 and 1lMotor20) 
    }
    
    for (int i=0; i<4; i+=1){
      myservos[i].write(valmap);          // sets the servo position according to the scaled value 
      digitalWrite(led_motors[i], HIGH);  // sets high the 4 Leds 
    }
    
    if (valmap > shutClose + 3) {  //tests if the shutters are open
      digitalWrite (red, HIGH);
      digitalWrite (green, LOW);
    }
    else {                         //then, if the shutter is closed
      digitalWrite (green, HIGH);
      digitalWrite (red, LOW);
    }

    Serial.println (valmap);
    delay(15);  // waits for the servo to get there
  }
  
  
  if (bEach) {
    if (abs(analogRead(pot_pin) - val) > 15) {
      val = analogRead(pot_pin);                         // reads the value of the potentiometer (value between 0 and 1023) 
      valmap = map(val, 15, 1023, shutClose, shutOpen);  // scale it to use it with the servo (value between 0 and 1lMotor20) 
    }  
    digitalWrite(led_motors[eachCounter], HIGH);  // sets each Led depending on the eachCounter
    myservos[eachCounter].write(valmap);
    Serial.println(valmap);
   
    if (valmap > shutClose + 3) {  //tests if the shutters are open
      digitalWrite (red, HIGH);
      digitalWrite (green, LOW);
    }
    else {                         //then, if the shutter is closed
      digitalWrite (green, HIGH);
      digitalWrite (red, LOW);
    }
    delay(15);  // waits for the servo to get there
  }


  if (bAuto) {
    for (int i=0; i<4; i+=1){
      digitalWrite(led_motors[i], LOW);  // sets high the 4 Leds 
      myservos[i].write(shutOpen);       // sets the servo position according to this value 
    }
   
    digitalWrite (red, HIGH);   //led shutter Red
    digitalWrite (green, LOW);  //led shutter Green

    delay((4000 - val_photosensor)*10);
    
    for (int i=0; i<4; i+=1){
      myservos[i].write(shutClose);  // sets the servo position according to this value 
    }
    digitalWrite (green, HIGH);  //led shutter Green
    digitalWrite (red, LOW);     //led shutter Red

    delay(25);  // waits for the servo to get there
    bAuto = false;
  }
}
