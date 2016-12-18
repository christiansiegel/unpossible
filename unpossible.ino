#include <SparkFun_ADXL345.h>
#include <Keyboard.h>
#include <Gamepad.h>

constexpr float PLAY_ANGLE_FULL_LOCK = 60.0 / 180.0;  // ±60° is full lock 

const int LED_TOP_FAR_LEFT = 3;
const int LED_TOP_LEFT = 4;

const int LED_BOTTOM_FAR_LEFT = 5;
const int LED_BOTTOM_LEFT = 6;

const int LED_TOP_FAR_RIGHT = 7;
const int LED_TOP_RIGHT = 8;

const int LED_BOTTOM_FAR_RIGHT = 9;
const int LED_BOTTOM_RIGHT = A0;

const int LED_CENTER_LEFT = A2;
const int LED_CENTER_RIGHT = A3;

const int BUTTON_PIN = 2;

ADXL345 adxl = ADXL345(10);
Gamepad gp;

void setup() {
  //Serial.begin(9600);  
  //Keyboard.begin();  

  setup_gpios();
  setup_accelerometer();
}

void setup_accelerometer() {
  adxl.powerOn();
  adxl.setRangeSetting(2);  // 2g, 4g, 8g or 16g
  adxl.setSpiBit(0);  // use 4 wire SPI
}

void setup_gpios() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  pinMode(LED_TOP_FAR_LEFT, OUTPUT);
  pinMode(LED_TOP_LEFT, OUTPUT);
  pinMode(LED_BOTTOM_FAR_LEFT, OUTPUT);
  pinMode(LED_BOTTOM_LEFT, OUTPUT);
  pinMode(LED_TOP_FAR_RIGHT, OUTPUT);
  pinMode(LED_TOP_RIGHT, OUTPUT);
  pinMode(LED_BOTTOM_FAR_RIGHT, OUTPUT);
  pinMode(LED_BOTTOM_RIGHT, OUTPUT);
  pinMode(LED_CENTER_LEFT, OUTPUT);
  pinMode(LED_CENTER_RIGHT, OUTPUT);
}

void read_accelerometer_calibrated(float &x_val, float &y_val, float &z_val) {
  // Accelerometer Readings
  int x,y,z;   
  adxl.readAccel(&x, &y, &z);

  // Calibrate values
  x_val = (x - 2.5)/33.5;
  y_val = (y - 0.0)/34.0;
  z_val = (z + 4.0)/32.0;
}

float get_steering_angle() {
  // Get calibrated accelerometer values
  float x, y, z;
  read_accelerometer_calibrated(x, y, z);

  // Calculate angle about z-axis with four-quadrant inverse tangent
  float angle = atan2(x ,-y);

  // Limit steering angle
  angle /= PLAY_ANGLE_FULL_LOCK; 
  if(angle > PI) angle = PI;
  if(angle < -PI) angle = -PI;

  // Scale to [-1, 1]
  return angle / PI;
}

void loop() {
  // Get button state
  int buttonState = !digitalRead(BUTTON_PIN);

  // Get controller steering angle
  float angle = get_steering_angle(); 

  digitalWrite(LED_TOP_FAR_LEFT, buttonState);
  digitalWrite(LED_TOP_LEFT, buttonState);
  digitalWrite(LED_BOTTOM_FAR_LEFT, buttonState);
  digitalWrite(LED_BOTTOM_LEFT, buttonState);
  digitalWrite(LED_TOP_FAR_RIGHT, buttonState);
  digitalWrite(LED_TOP_RIGHT, buttonState);
  digitalWrite(LED_BOTTOM_FAR_RIGHT, buttonState);
  digitalWrite(LED_BOTTOM_RIGHT, buttonState);
  digitalWrite(LED_CENTER_LEFT, buttonState);
  digitalWrite(LED_CENTER_RIGHT, buttonState);

  // Set gamepad values:
  gp.setLeftXaxis(angle * 127.0);
  gp.setButtonState(0, buttonState);
}
