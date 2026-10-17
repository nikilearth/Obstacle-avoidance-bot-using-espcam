
#include <Servo.h>

const int TRIG_F = 2;
const int ECHO_F = 3;

const int TRIG_L = 4;
const int ECHO_L = 5;

const int TRIG_R = 6;
const int ECHO_R = 7;

const int IN1 = 8;
const int IN2 = 9;
const int IN3 = 10;
const int IN4 = 11;

const int SERVO_PIN = 12;
Servo scanServo;

const int SAFE_DISTANCE = 25;  
const int SIDE_CLEARANCE = 20; 

const int SERVO_CENTER = 90;
const int SERVO_LEFT = 150;
const int SERVO_RIGHT = 30;

const int REVERSE_TIME = 250;
const int TURN_TIME = 350;

long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(3);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration =
      pulseIn(echoPin, HIGH, 25000UL);

  if (duration == 0) {
    return 250;
  }

  long distance = duration * 0.0343 / 2.0;

  return distance;
}

void readSensors(long &front, long &left, long &right) {
  front = getDistance(TRIG_F, ECHO_F);
  delay(35);

  left = getDistance(TRIG_L, ECHO_L);
  delay(35);

  right = getDistance(TRIG_R, ECHO_R);
  delay(35);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void moveForward() {
  // Left motor
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Right motor
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  // Left motor backward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  // Right motor forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  // Left motor forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Right motor backward
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_F, OUTPUT);
  pinMode(ECHO_F, INPUT);

  pinMode(TRIG_L, OUTPUT);
  pinMode(ECHO_L, INPUT);

  pinMode(TRIG_R, OUTPUT);
  pinMode(ECHO_R, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  scanServo.attach(SERVO_PIN);
  scanServo.write(SERVO_CENTER);

  stopMotors();
  delay(500);

  Serial.println("Obstacle avoidance robot ready");
}


void loop() {
  long front, left, right;

  readSensors(front, left, right);

  Serial.println();
  Serial.println("========== ROBOT STATUS ==========");

  Serial.print("Front Distance: ");
  Serial.print(front);
  Serial.println(" cm");

  Serial.print("Left Distance : ");
  Serial.print(left);
  Serial.println(" cm");

  Serial.print("Right Distance: ");
  Serial.print(right);
  Serial.println(" cm");

  // CASE 1: All paths are clear
  if (front > SAFE_DISTANCE &&
      left > SIDE_CLEARANCE &&
      right > SIDE_CLEARANCE) {

    scanServo.write(SERVO_CENTER);
    moveForward();

    Serial.println("STATUS: NO OBSTACLE");
    Serial.println("ACTION: GOING FORWARD");
    Serial.println("REASON: All paths are clear.");

    delay(40);
    return;
  }

  // CASE 2: Obstacle detected
  stopMotors();

  Serial.println("STATUS: OBSTACLE DETECTED");
  Serial.println("ACTION: STOPPING MOTORS");

  if (front <= SAFE_DISTANCE) {
    Serial.println("REASON: Obstacle in front.");
  }
  else if (left <= SIDE_CLEARANCE) {
    Serial.println("REASON: Left side is blocked.");
  }
  else {
    Serial.println("REASON: Right side is blocked.");
  }

  delay(150);

  Serial.println("ACTION: REVERSING");
  moveBackward();
  delay(REVERSE_TIME);

  stopMotors();
  delay(150);

  // CASE 3: Turn left if left has more space
  if (left > right && left > SIDE_CLEARANCE) {

    Serial.println("DECISION: LEFT SIDE IS CLEARER");
    Serial.println("ACTION: SERVO SCANNING LEFT");

    scanServo.write(SERVO_LEFT);
    delay(250);

    Serial.println("ACTION: TURNING LEFT");
    turnLeft();
    delay(TURN_TIME);

  }

  // CASE 4: Turn right if right has more space
  else if (right >= left && right > SIDE_CLEARANCE) {

    Serial.println("DECISION: RIGHT SIDE IS CLEARER");
    Serial.println("ACTION: SERVO SCANNING RIGHT");

    scanServo.write(SERVO_RIGHT);
    delay(250);

    Serial.println("ACTION: TURNING RIGHT");
    turnRight();
    delay(TURN_TIME);

  }

  // CASE 5: Both sides blocked
  else {

    Serial.println("WARNING: BOTH SIDES BLOCKED");
    Serial.println("ACTION: TURNING RIGHT TO ESCAPE");

    turnRight();
    delay(TURN_TIME * 2);
  }

  // Return to the normal position
  stopMotors();
  scanServo.write(SERVO_CENTER);

  Serial.println("ACTION: STOPPED AFTER TURN");
  Serial.println("STATUS: CHECKING PATH AGAIN");
  Serial.println("==================================");

  delay(150);
}
