// Pin assignments
const int RED_PIN    = 2;   // Red LED on digital pin 2
const int YELLOW_PIN = 3;   // Yellow LED on digital pin 3
const int GREEN_PIN  = 4;   // Green LED on digital pin 4

// Times in milliseconds
const unsigned long RED_TIME    = 20000UL; // 20 seconds
const unsigned long GREEN_TIME  = 20000UL; // 20 seconds
const unsigned long YELLOW_TIME = 16000UL; // 16 seconds

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  // Start with all OFF
  digitalWrite(RED_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
}

void loop() {
  // 1) RED ON 20 s, others OFF
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  delay(RED_TIME);

  // 2) GREEN ON 20 s, others OFF
  digitalWrite(RED_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH);
  delay(GREEN_TIME);

  // 3) YELLOW ON 16 s, others OFF
  digitalWrite(RED_PIN, LOW);
  digitalWrite(YELLOW_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);
  delay(YELLOW_TIME);
}
