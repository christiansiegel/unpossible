#include <SparkFun_ADXL345.h>
#include <Keyboard.h>
#include <Gamepad.h>

const float PLAY_ANGLE_FULL_LOCK = 60.0 / 180.0;  // ±60° is full lock 

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

void get_angles(float &x_angle, float &y_angle, float &z_angle) {
  // Accelerometer Readings
  int x,y,z;   
  adxl.readAccel(&x, &y, &z);

  // Calibrate values
  float x_val = (x - 2.5)/33.5;
  float y_val = (y - 0.0)/34.0;
  float z_val = (z + 4.0)/32.0;

  // Calculate angles
  x_angle = atan(y_val / z_val) * 57.2958;
  if(z_val < 0) 
    x_angle -= 90;
  else 
    x_angle += 90;

  y_angle = atan(-z_val / x_val) * 57.2958;
  if(x_val < 0) 
    y_angle -= 90;
  else 
    y_angle += 90;

  z_angle = atan(y_val / x_val) * 57.2958;
  if(x_val < 0) 
    z_angle -= 90;
  else 
    z_angle += 90;

  // Scale to -1..1
  x_angle /= 180.0;
  y_angle /= 180.0;
  z_angle /= 180.0;

  // Debug print
  /*Serial.print(x_angle);
  Serial.print(", ");
  Serial.print(y_angle);
  Serial.print(", ");
  Serial.println(z_angle);*/
}

void loop() {
  // Read button state
  int buttonState = !digitalRead(BUTTON_PIN);
  
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

  // Read accelerometer angles
  float x, y, z;
  get_angles(x, y, z);

  // Scale play angle
  float play_angle = z / PLAY_ANGLE_FULL_LOCK;
  if(play_angle > 1) {
    play_angle = 1;
  } else if(play_angle < -1) {
    play_angle = -1;
  }

  // Set gamepad values:
  gp.setLeftXaxis(play_angle * 127.0);
  gp.setButtonState(0, buttonState);
}
