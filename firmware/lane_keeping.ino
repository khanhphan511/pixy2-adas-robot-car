#include <Pixy2.h>
#include "Freenove_4WD_Car_for_Arduino.h"

Pixy2 pixy;

// ---------- Pixy & lane settings ----------
const uint8_t BLUE_SIGNATURE = 1;
const int PIXY_WIDTH         = 316;
const int PIXY_CENTER_X      = 180;

const float LANE_WIDTH_PIX   = 75.0;
const int MIN_LANE_WIDTH_PIX = 10;
const int MIN_Y              = 70;

// ---------- PID PARAMETERS ----------
int   baseSpeed = 70;
float Kp = 1;
float Ki = 0.04;   // <-- NEW: tune this
float Kd = 0.2;

const int MAX_TURN = 50;

// PID state
float prevError     = 0.0;
float integralError = 0.0;

// Anti-windup limit
const float INTEGRAL_LIMIT = 300.0;

// ---------- Drive helper ----------
void driveWithError(float error)
{
  // --- Compute PID terms ---
  integralError += error;
  
  // Anti-windup clamp
  if (integralError >  INTEGRAL_LIMIT) integralError =  INTEGRAL_LIMIT;
  if (integralError < -INTEGRAL_LIMIT) integralError = -INTEGRAL_LIMIT;

  float dError = error - prevError;
  prevError = error;

  float turn = (Kp * error) + (Ki * integralError) + (Kd * dError);

  // Limit steering
  if (turn >  MAX_TURN) turn =  MAX_TURN;
  if (turn < -MAX_TURN) turn = -MAX_TURN;

  int leftSpeed  = baseSpeed - turn;
  int rightSpeed = baseSpeed + turn;

  leftSpeed  = constrain(leftSpeed,  0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);

  motorRun(leftSpeed, rightSpeed);

  // Debug output
  Serial.print("err=");
  Serial.print(error);
  Serial.print("  I=");
  Serial.print(integralError);
  Serial.print("  turn=");
  Serial.print(turn);
  Serial.print("  L=");
  Serial.print(leftSpeed);
  Serial.print("  R=");
  Serial.println(rightSpeed);
}

void setup()
{
  Serial.begin(115200);
  pinsSetup();
  pixy.init();
  //pixy.setLamp(1, 1);

  Serial.println("Lane-centering with PID controller");
}

void loop()
{
  pixy.ccc.getBlocks();

  int leftX = -1, rightX = -1;
  int bestLeftY = -1, bestRightY = -1;

  // Find bottom-most blocks
  for (uint16_t i = 0; i < pixy.ccc.numBlocks; i++)
  {
    auto &b = pixy.ccc.blocks[i];
    
    if (b.m_signature != BLUE_SIGNATURE) continue;
    if (b.m_y < MIN_Y) continue;

    int x = b.m_x;
    int y = b.m_y;

    if (x < PIXY_CENTER_X) {
      if (y > bestLeftY) { bestLeftY = y; leftX = x; }
    } else {
      if (y > bestRightY) { bestRightY = y; rightX = x; }
    }
  }

  bool haveLeft  = (leftX  >= 0);
  bool haveRight = (rightX >= 0);

  float laneCenterX;
  bool  validLane = false;

  // ---- Case 1: both edges visible ----
  if (haveLeft && haveRight)
  {
    int width = rightX - leftX;

    if (width >= MIN_LANE_WIDTH_PIX) {
      laneCenterX = 0.5f * (leftX + rightX);
      validLane = true;
      
      Serial.print("BOTH L=");
      Serial.print(leftX);
      Serial.print(" R=");
      Serial.print(rightX);
      Serial.print(" width=");
      Serial.print(width);
      Serial.print(" centerX=");
      Serial.println(laneCenterX);
    }
  }

  // ---- Case 2: only left or right visible → use fixed lane width ----
  if (!validLane)
  {
    if (haveLeft && !haveRight) {
      laneCenterX = leftX + LANE_WIDTH_PIX / 2.0f;
      validLane = true;
      //delay(20);
      Serial.print("LEFT only L=");
      Serial.print(leftX);
      Serial.print(" → est center=");
      Serial.println(laneCenterX);
    }
    else if (!haveLeft && haveRight) {
      laneCenterX = rightX - LANE_WIDTH_PIX / 2.0f;
      validLane = true;
      //delay(20);
      Serial.print("RIGHT only R=");
      Serial.print(rightX);
      Serial.print(" → est center=");
      Serial.println(laneCenterX);
    }
    else {
      // No edges → go straight
      Serial.println("NO EDGES → straight");
      motorRun(baseSpeed, baseSpeed);
      delay(20);
      return;
    }
  }

  // ---- Drive via PID based on lane center ----
  float error = (float)PIXY_CENTER_X - laneCenterX;
  driveWithError(error);

  delay(20);
}
