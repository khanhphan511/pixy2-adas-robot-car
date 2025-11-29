//
// begin license header
//
// This file is part of Pixy CMUcam5 or "Pixy" for short
//
// All Pixy source code is provided under the terms of the
// GNU General Public License v2 (http://www.gnu.org/licenses/gpl-2.0.html).
// Those wishing to use Pixy source code, software and/or
// technologies under different licensing terms should contact us at
// cmucam@cs.cmu.edu. Such licensing terms are available for
// all portions of the Pixy codebase presented here.
//
// end license header
//
// This sketch is a good place to start if you're just getting started with
// Pixy and Arduino.  This program simply prints the detected object blocks
// (including color codes) through the serial console.  It uses the Arduino's
// ICSP SPI port.  For more information go here:
//
// https://docs.pixycam.com/wiki/doku.php?id=wiki:v2:hooking_up_pixy_to_a_microcontroller_-28like_an_arduino-29
//
 
#include <Pixy2.h>
#include "Freenove_4WD_Car_for_Arduino.h"

#define MOTOR_DIRECTION     0 //If the direction is reversed, change 0 to 1
#define PIN_DIRECTION_RIGHT 3
#define PIN_DIRECTION_LEFT  4
#define PIN_MOTOR_PWM_RIGHT 5
#define PIN_MOTOR_PWM_LEFT  6

// This is the main Pixy object
Pixy2 pixy;

int x_val;
int dist;

void setup()
{
  Serial.begin(115200);
  pixy.init();
  pinMode(PIN_DIRECTION_LEFT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_LEFT, OUTPUT);
  pinMode(PIN_DIRECTION_RIGHT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_RIGHT, OUTPUT);
  pinMode(A3, INPUT);
}

void loop()
{
  int i;
  // grab blocks!
  pixy.ccc.getBlocks();
  // Check if there is a coordinate:
  if (pixy.ccc.numBlocks)
  {
    // Obtain X coordinate:
    for (i=0; i<pixy.ccc.numBlocks; i++)
    {
      if (pixy.ccc.blocks[i].m_signature == 2)
        Serial.print("Signature 2 X value: ");
        Serial.println(pixy.ccc.blocks[i].m_x);
        x_val = pixy.ccc.blocks[i].m_x;
    }
  } else {
    x_val = -1;
  }
  //Code to obtain distance here...
  dist = analogRead(A3);
  Serial.println(dist);
  //Actuate motors
  if (dist < 200 && x_val != -1) {//340 is Approx 6 inches, but stop quickly enough not to run into it. Also check if the object was found.
      if (x_val < 143) {
      // Turn Left
      Serial.println("Turning Left...");
      motorRun(-120, 120);
    } else if (x_val > 173){
      // Turn Right
      Serial.println("Turning Right...");
      motorRun(120, -120);
    } else {
      // Go Straight
      Serial.println("Going Straight...");
      motorRun(120, 120);
    }
  } else {
    Serial.println("Stopping...");
    motorRun(0, 0);
  }
  Serial.println("-------------------");
}






