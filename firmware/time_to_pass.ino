#include <TimerOne.h>
#include <Pixy2.h>
#include "Freenove_4WD_Car_for_Arduino.h"




#define MOTOR_DIRECTION     0 //If the direction is reversed, change 0 to 1
#define PIN_DIRECTION_RIGHT 3
#define PIN_DIRECTION_LEFT  4
#define PIN_MOTOR_PWM_RIGHT 5
#define PIN_MOTOR_PWM_LEFT  6


#define STOP_AT_LIGHT 0
#define GO_THROUGH_LIGHT 1
#define NO_DECISION -1


// This is the main Pixy object
Pixy2 pixy;


const double diameter = 2.56; // wheel diameter in inches
int count;
int height;
double dist;
double vel;
int motorspeed = 70;
double time;
char actCode = 'N'; //If N the idle routine will run repeatedly until signature is detected.
int velInt = 1000000; //In  microseconds
int decision = NO_DECISION;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pixy.init();
  pinMode(PIN_DIRECTION_LEFT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_LEFT, OUTPUT);
  pinMode(PIN_DIRECTION_RIGHT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_RIGHT, OUTPUT);
  pinMode(2, INPUT);
  motorRun(motorspeed, motorspeed); // the default case is to go forward, do this once at the beginning
  delay(100); // do not want velocity to be zero -> divide by zero error for time calculation. Let the motors get some speed first.
  pixy.setLamp(1, 1); // turn on the lamp
  // configure interrupt
  // Disable interrupts while configuring
  cli();


  // Set Timer1 to CTC mode
  TCCR1A = 0;                 // Normal port operation
  TCCR1B = 0;
  TCCR1B |= (1 << WGM12);     // CTC mode


  // Set compare match value for 1 second
  // OCR1A = (16,000,000) / (prescaler * frequency) - 1
  // For 1 Hz interrupt with prescaler 1024:
  // OCR1A = 16,000,000 / (1024 * 1) - 1 = 15624
  OCR1A = 15624;


  // Enable Timer1 compare interrupt
  TIMSK1 |= (1 << OCIE1A);


  // Set prescaler to 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);


  // Enable interrupts globally
  sei();


  attachInterrupt(digitalPinToInterrupt(2), incrementCount, RISING);
}




void loop() {
  // put your main code here, to run repeatedly:
  idleRoutine();
}




void idleRoutine(){
  //Fetch signature and height
  pixy.ccc.getBlocks();
  if (pixy.ccc.numBlocks)
  {
    // Obtain color and height: (Ideally there is only one element in the buffer)
    for (int i=0; i<pixy.ccc.numBlocks; i++)
    {
      height = pixy.ccc.blocks[i].m_height;
      //Serial.println(height);
      switch (pixy.ccc.blocks[i].m_signature){
        case(1):
          actCode = 'R'; //Red Code
          break;
        case(2):
          actCode = 'Y'; //Yellow Code
          break;
        case(3):
          actCode = 'G'; //Green Code
          break;
        default:
          actCode = 'N'; //Repeat idleRoutine
          break;
      }  
    }
  }
  //Choose Action
  switch(actCode){
    case('G'):
      cruise();
      break;
    case('R'):
      stop();
      break;
    case('Y'):
      decide();
      break;
    default:
      //Do nothing
      break;
  }
}
void decide() {
  // mapping only:
  //Serial.println(height);
  //return;
  // remove the above lines when done with mapping


  // make a decision based on time. Always display the dist, time, and speed, regardless of the decision
  // we have velocity every 100 ms. Need to get distance -> mapped from height of the object
  dist = collectDist();
  //Calculate Time
  //time = dist/vel;
  //switch(decision){
    //case(NO_DECISION):
      // need to make a decision
      //if (time > 8){ // need more than 8 seconds to pass the light -> stop in front of the light
        //decision = STOP_AT_LIGHT;
      //} else { // 8 or less seconds -> we can go through the light
        //decision = GO_THROUGH_LIGHT;
      //}
    //break;
    //case(STOP_AT_LIGHT):
     
    //break;
    //case(GO_THROUGH_LIGHT):
      // just maintain speed, nothing to do here :)
    //break;
  //}
 
 
  return;
}


void cruise(){ // green light only -> just go forward without displaying anything to the serial monitor
  //Stall the processor
  while(true);
}




void stop(){ // red light only -> pays no mind to mapping height to distance, just having the height is enough for this part
  if (height > 28) {
    slowdown();
  }
}




void slowdown(){
  for(motorspeed; motorspeed > 0; motorspeed -= 10){
    motorRun(motorspeed, motorspeed);
    delay(100);
  }
  while(1){};
}




void incrementCount(){
 count++;
}




ISR(TIMER1_COMPA_vect){ // this occurs every 1s on a timer interrupt
  // count    1 rotation   d x pi [in]
  // ------ x ---------- x ----------- = speed [in/s]
  //   1s      20 count    1 rotation
  vel = ((count*diameter*PI)/(1*20.0));
  //Reset count
  count = 0;
  //Calculate Time
  time = dist/vel;
  // display the information
  Serial.println("_______________________________________");
  Serial.print("Distance: ");
  Serial.print(dist); // print dist to 2 decimal places
  Serial.println(" in");
  Serial.print("Speed: ");
  Serial.print(vel); // print dist to 2 decimal places
  Serial.println(" in/s");
  Serial.print("Calculated Time: ");
  Serial.print(time); // print dist to 2 decimal places
  Serial.println(" s");
  Serial.print("Action: ");
  switch(decision){
    case(NO_DECISION):
        Serial.println(" Undecided");
        // need to make a decision
      if (time > 8){ // need more than 8 seconds to pass the light -> stop in front of the light
        decision = STOP_AT_LIGHT;
      } else { // 8 or less seconds -> we can go through the light
        decision = GO_THROUGH_LIGHT;
      }
    break;
    case(STOP_AT_LIGHT):
      Serial.println(" Stop at light");
      if (dist < 7) { // less than 7 inches to the light -> slow to a stop
        slowdown();
      }
    break;
    case(GO_THROUGH_LIGHT):
      Serial.println(" Cruise");
      if (time > 8){ // need more than 8 seconds to pass the light -> stop in front of the light
        decision = STOP_AT_LIGHT;
      }
    break;
  }
}


int collectDist(){ // only used in the case of the yellow light
  // the distance to the traffic light is dependent on the height of the yellow signature
  // height [7,17]: dist = -.734(height) + 27.8
  // height [17,55]: dist = -.205(height) + 19.4
  // height [55,120]: dist = -.057(height) + 11.1
  if (height < 17){
    dist = (-0.734*height)+27.8;
  } else if (height < 55) {
    dist = (-0.205*height)+19.4;
  } else {
    dist = (-0.057*height)+11.1;
  }


  // return dist as inches
  return dist;
}













