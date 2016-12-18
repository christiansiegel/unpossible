#include <SparkFun_ADXL345.h>
#include <Keyboard.h>
#include <Gamepad.h>

constexpr float PLAY_ANGLE_FULL_LOCK = 60.0 / 180.0;  // ±60° is full lock 

const int LED_TOP_FAR_LEFT = A3;
const int LED_TOP_LEFT = 7;

const int LED_BOTTOM_FAR_LEFT = 6;
const int LED_BOTTOM_LEFT = 9;

const int LED_TOP_FAR_RIGHT = A0;
const int LED_TOP_RIGHT = 4;

const int LED_BOTTOM_FAR_RIGHT = A2;
const int LED_BOTTOM_RIGHT = 3;

const int LED_CENTER_LEFT = 8;
const int LED_CENTER_RIGHT = 5;

const int BUTTON_PIN = 2;

ADXL345 adxl = ADXL345(10);
Gamepad gp;

void setup() {
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
  x_val = (x - 20)/33.5; //-2.5
  y_val = (y - 0.0)/34.0;
  z_val = (z + 4.0)/32.0;
}

float get_steering_angle(float x, float y) {
  // Calculate angle about z-axis with four-quadrant inverse tangent
  float angle = atan2(x ,-y);

  // Limit steering angle
  angle /= PLAY_ANGLE_FULL_LOCK; 
  if(angle > PI) angle = PI;
  if(angle < -PI) angle = -PI;

  // Scale to [-1, 1]
  return angle / PI;
}

bool is_in_playing_position(float y, float z) {
  // Calculate angle about x-axis with four-quadrant inverse tangent
  float angle = atan2(y ,z);

  // Not flat on table
  return abs(angle) > PI/6;
}

void blink_in_direction(float angle) {
  static long lastIntervalMillis = 0;
  if(lastIntervalMillis + 75 < millis()) {
    lastIntervalMillis = millis();

    static int row = 2;
    if(angle < 0) {
      if(--row < 0) row = 4;
    }
    else {
      if(++row > 4) row = 0;
    }
  
    digitalWrite(LED_TOP_FAR_LEFT, row == 0);
    digitalWrite(LED_BOTTOM_FAR_LEFT, row == 0);
    
    digitalWrite(LED_TOP_LEFT, row == 1);
    digitalWrite(LED_BOTTOM_LEFT, row == 1);
  
    digitalWrite(LED_CENTER_LEFT, row == 2);
    digitalWrite(LED_CENTER_RIGHT, row == 2);
    
    digitalWrite(LED_BOTTOM_RIGHT, row == 3);
    digitalWrite(LED_TOP_RIGHT, row == 3);
    
    digitalWrite(LED_TOP_FAR_RIGHT, row == 4);
    digitalWrite(LED_BOTTOM_FAR_RIGHT, row == 4);
  }
}

void set_all_leds(boolean value) {
  digitalWrite(LED_TOP_FAR_LEFT, value);
  digitalWrite(LED_BOTTOM_FAR_LEFT, value);
  
  digitalWrite(LED_TOP_LEFT, value);
  digitalWrite(LED_BOTTOM_LEFT, value);

  digitalWrite(LED_CENTER_LEFT, value);
  digitalWrite(LED_CENTER_RIGHT, value);
  
  digitalWrite(LED_BOTTOM_RIGHT, value);
  digitalWrite(LED_TOP_RIGHT, value);

  digitalWrite(LED_TOP_FAR_RIGHT, value);
  digitalWrite(LED_BOTTOM_FAR_RIGHT, value);
}

void loop() {
  // Get button state
  int buttonState = !digitalRead(BUTTON_PIN);

  // Get calibrated accelerometer values
  float x, y, z;
  read_accelerometer_calibrated(x, y, z);

  // Get controller steering angle
  float angle = get_steering_angle(x, y); 

  // Get if controller is in playing position and not just lying around
  bool playing = is_in_playing_position(y, z);

  if(playing) {
    if(abs(angle) > 0.2) {
      blink_in_direction(angle);
    } else {
      set_all_leds(HIGH);
    }
  } else {
    set_all_leds(LOW);
  }

  // Set gamepad values:
  gp.setLeftXaxis(playing ? angle * -127.0 : 0);
  gp.setButtonState(0, buttonState);
}
